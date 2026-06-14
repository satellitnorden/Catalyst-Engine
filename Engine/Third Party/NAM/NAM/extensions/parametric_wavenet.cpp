#include <algorithm>
#include <cstring>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdexcept>

#include "../Eigen/Dense"

#include "../get_dsp.h"
#include "../registry.h"
#include "parametric_slimmable_wavenet.h"
#include "parametric_wavenet.h"

#if defined(NAM_ENABLE_A2_FAST)
  #include "parametric_a2_fast.h"
#endif

namespace
{
int wave_net_output_channels(const std::vector<nam::wavenet::LayerArrayParams>& layer_array_params,
                             const bool with_head, const std::optional<nam::wavenet::HeadParams>& head_params)
{
  if (layer_array_params.empty())
    throw std::runtime_error("WaveNet requires at least one layer array");
  if (with_head && head_params.has_value())
    return head_params->out_channels;
  return layer_array_params.back().head_size;
}
} // namespace

// WaveNet ====================================================================

nam::parametric_wavenet::ParametricWaveNet::ParametricWaveNet(const int in_channels,
                               const std::vector<nam::wavenet::LayerArrayParams>& layer_array_params,
                               const float head_scale, const bool with_head, std::optional<nam::wavenet::HeadParams> head_params,
                               std::vector<float> weights, std::unique_ptr<DSP> condition_dsp,
                               const double expected_sample_rate)
: DSP(in_channels, wave_net_output_channels(layer_array_params, with_head, head_params), expected_sample_rate)
, _condition_dsp(std::move(condition_dsp))
, _head_scale(head_scale)
{
    _condition_size = layer_array_params[0].condition_size;

  // Assert that if there's a condition DSP, its input is compatible with what it'll get from this WaveNet:
  if (this->_condition_dsp != nullptr)
  {
    if (this->_get_condition_dim() != this->_condition_dsp->NumInputChannels())
    {
      std::stringstream ss;
      ss << "input channels of WaveNet (" << in_channels << ") don't match input channels of condition DSP ("
         << this->_condition_dsp->NumInputChannels() << "!\n";
      throw std::runtime_error(ss.str().c_str());
    }
  }
  if (with_head)
  {
    if (!head_params.has_value())
      throw std::runtime_error("WaveNet: with_head is true but head configuration is missing");
    if (head_params->in_channels != layer_array_params.back().head_size)
    {
      std::stringstream ss;
      ss << "WaveNet head in_channels (" << head_params->in_channels << ") must match last layer array head_size ("
         << layer_array_params.back().head_size << ")";
      throw std::runtime_error(ss.str());
    }
    this->_post_stack_head = std::make_unique<nam::wavenet::detail::Head>(*head_params);
  }
  else if (head_params.has_value())
    throw std::runtime_error("WaveNet: head configuration provided but with_head is false");

  for (size_t i = 0; i < layer_array_params.size(); i++)
  {
    // Quick assert that the condition_dsp will output compatibly with this layer array
    if (this->_condition_dsp != nullptr)
    {
      if (layer_array_params[i].condition_size != this->_condition_dsp->NumOutputChannels())
      {
        std::stringstream ss;
        ss << "condition_size of layer " << i << " (" << layer_array_params[i].condition_size
           << ") doesn't match output channels of condition DSP (" << this->_condition_dsp->NumOutputChannels()
           << "!\n";
        throw std::runtime_error(ss.str().c_str());
      }
    }
    this->_layer_arrays.push_back(nam::wavenet::detail::LayerArray(layer_array_params[i]));
    if (i > 0)
      if (layer_array_params[i].channels != layer_array_params[i - 1].head_size)
      {
        std::stringstream ss;
        ss << "channels of layer " << i << " (" << layer_array_params[i].channels
           << ") doesn't match head_size of preceding layer (" << layer_array_params[i - 1].head_size << "!\n";
        throw std::runtime_error(ss.str().c_str());
      }
  }
  this->set_weights_(weights);

  // Finally, figure out how much pre-warming is needed for this model.
  //mPrewarmSamples = this->_condition_dsp != nullptr ? this->_condition_dsp->PrewarmSamples() : 1;
  mPrewarmSamples = 1;
  for (size_t i = 0; i < this->_layer_arrays.size(); i++)
    mPrewarmSamples += this->_layer_arrays[i].get_receptive_field();
  if (this->_post_stack_head != nullptr)
    mPrewarmSamples += this->_post_stack_head->receptive_field() - 1;
}

void nam::parametric_wavenet::ParametricWaveNet::set_weights_(std::vector<float>& weights)
{
  std::vector<float>::iterator it = weights.begin();
  // Note: condition_dsp already has its own weights from construction,
  // so we don't need to set its weights here.
  for (size_t i = 0; i < this->_layer_arrays.size(); i++)
    this->_layer_arrays[i].set_weights_(it);
  if (this->_post_stack_head != nullptr)
    this->_post_stack_head->set_weights_(it);
  this->_head_scale = *(it++); // TODO `LayerArray.absorb_head_scale()`
  if (it != weights.end())
  {
    std::stringstream ss;
    for (size_t i = 0; i < weights.size(); i++)
      if (weights[i] == *it)
      {
        ss << "Weight mismatch: assigned " << i + 1 << " weights, but " << weights.size() << " were provided.";
        throw std::runtime_error(ss.str().c_str());
      }
    ss << "Weight mismatch: provided " << weights.size() << " weights, but the model expects more.";
    throw std::runtime_error(ss.str().c_str());
  }
}

void nam::parametric_wavenet::ParametricWaveNet::SetMaxBufferSize(const int maxBufferSize)
{
  DSP::SetMaxBufferSize(maxBufferSize);
  this->_condition_input.resize(NumInputChannels(), maxBufferSize);
  // Resize condition output
  if (this->_condition_dsp == nullptr)
  {
    this->_condition_output.resize(this->_get_condition_dim(), maxBufferSize);
  }
  else
  {
    //this->_condition_dsp->SetMaxBufferSize(maxBufferSize);
    const int condition_output_channels = this->_condition_dsp->NumOutputChannels();
    this->_condition_output.resize(condition_output_channels, maxBufferSize);

    // Resize temporary buffers for condition DSP processing
    const int condition_dim = this->_get_condition_dim();
    this->_condition_dsp_input_buffers.resize(condition_dim);
    this->_condition_dsp_output_buffers.resize(condition_output_channels);
    this->_condition_dsp_input_ptrs.resize(condition_dim);
    this->_condition_dsp_output_ptrs.resize(condition_output_channels);

    for (int ch = 0; ch < condition_dim; ch++)
    {
      this->_condition_dsp_input_buffers[ch].resize(maxBufferSize);
      this->_condition_dsp_input_ptrs[ch] = this->_condition_dsp_input_buffers[ch].data();
    }

    for (int ch = 0; ch < condition_output_channels; ch++)
    {
      this->_condition_dsp_output_buffers[ch].resize(maxBufferSize);
      this->_condition_dsp_output_ptrs[ch] = this->_condition_dsp_output_buffers[ch].data();
    }
  }

  for (size_t i = 0; i < this->_layer_arrays.size(); i++)
    this->_layer_arrays[i].SetMaxBufferSize(maxBufferSize);

  if (this->_post_stack_head != nullptr)
  {
    this->_post_stack_head->SetMaxBufferSize(maxBufferSize);
    this->_scaled_head_scratch.resize(this->_post_stack_head->in_channels(), maxBufferSize);
  }
}

void nam::parametric_wavenet::ParametricWaveNet::_process_condition(NAM_SAMPLE** input, const int num_frames)
{
    for (int ch = 0; ch < _condition_size; ch++)
    {
        for (int j = 0; j < num_frames; j++)
        {
            this->_condition_output(ch, j) = input[ch + 1][j];
        }
    }
}

void nam::parametric_wavenet::ParametricWaveNet::_set_condition_array(NAM_SAMPLE** input, const int num_frames)
{
  const int in_channels = NumInputChannels();
  // Fill condition array with input channels
  for (int ch = 0; ch < in_channels; ch++)
  {
    for (int j = 0; j < num_frames; j++)
    {
      this->_condition_input(ch, j) = input[ch][j];
    }
  }
}

void nam::parametric_wavenet::ParametricWaveNet::process(NAM_SAMPLE** input, NAM_SAMPLE** output, const int num_frames)
{
  assert(num_frames <= mMaxBufferSize);
  const int out_channels = NumOutputChannels();

  this->_set_condition_array(input, num_frames);
  this->_process_condition(input, num_frames);

  // Main layer arrays:
  // Layer-to-layer
  for (size_t i = 0; i < this->_layer_arrays.size(); i++)
  {
    if (i == 0)
    {
      // First layer array - no head input
      // layer_inputs should be the original input (before condition_dsp processing),
      // condition should be the processed condition output (after condition_dsp)
      this->_layer_arrays[i].Process(this->_condition_input, this->_condition_output, num_frames);
    }
    else
    {
      // Subsequent layer arrays - use outputs from previous layer array.
      // Pass full buffers and slice inside the callee to avoid passing Blocks
      // across API boundaries (which can cause Eigen to allocate temporaries).
      Eigen::MatrixXf& prev_layer_outputs = this->_layer_arrays[i - 1].GetLayerOutputs();
      Eigen::MatrixXf& prev_head_outputs = this->_layer_arrays[i - 1].GetHeadOutputs();
      this->_layer_arrays[i].Process(prev_layer_outputs, this->_condition_output, prev_head_outputs, num_frames);
    }
  }

  auto& final_head_outputs = this->_layer_arrays.back().GetHeadOutputs();

  if (this->_post_stack_head != nullptr)
  {
    assert(final_head_outputs.rows() == this->_post_stack_head->in_channels());
    const int head_in = this->_post_stack_head->in_channels();
    for (int ch = 0; ch < head_in; ch++)
    {
      for (int s = 0; s < num_frames; s++)
        this->_scaled_head_scratch(ch, s) = this->_head_scale * final_head_outputs(ch, s);
    }
    this->_post_stack_head->process(this->_scaled_head_scratch, num_frames);
    const Eigen::MatrixXf& head_out = this->_post_stack_head->get_last_output();
    assert(head_out.rows() == out_channels);

    if (out_channels == 1)
    {
      const float* __restrict__ src = head_out.data();
      NAM_SAMPLE* __restrict__ dst = output[0];
      for (int s = 0; s < num_frames; s++)
        dst[s] = (NAM_SAMPLE)src[s];
    }
    else
    {
      for (int ch = 0; ch < out_channels; ch++)
      {
        for (int s = 0; s < num_frames; s++)
          output[ch][s] = (NAM_SAMPLE)head_out(ch, s);
      }
    }
    return;
  }

  assert(final_head_outputs.rows() == out_channels);

  // Optimized output copy with head_scale multiplication
  if (out_channels == 1)
  {
    // Single channel: data is contiguous
    const float scale = this->_head_scale;
    const float* __restrict__ src = final_head_outputs.data();
    NAM_SAMPLE* __restrict__ dst = output[0];
    for (int s = 0; s < num_frames; s++)
    {
      dst[s] = scale * src[s];
    }
  }
  else
  {
    // Multi-channel: rows are not contiguous in column-major
    for (int ch = 0; ch < out_channels; ch++)
    {
      for (int s = 0; s < num_frames; s++)
      {
        output[ch][s] = this->_head_scale * final_head_outputs(ch, s);
      }
    }
  }
}

// Config parser - extracts all configuration from JSON without constructing the DSP
nam::parametric_wavenet::WaveNetConfig nam::parametric_wavenet::parse_config_json(const nlohmann::json& config,
                                                            const double expectedSampleRate)
{
  WaveNetConfig wc;

  // Condition DSP (eagerly built via get_dsp)
  if ((config.find("condition_dsp") != config.end()) && !config["condition_dsp"].is_null())
  {
    const nlohmann::json& condition_dsp_json = config["condition_dsp"];
    wc.condition_dsp = nam::get_dsp(condition_dsp_json);
    if (wc.condition_dsp->GetExpectedSampleRate() != expectedSampleRate)
    {
      std::stringstream ss;
      ss << "Condition DSP expected sample rate (" << wc.condition_dsp->GetExpectedSampleRate()
         << ") doesn't match WaveNet expected sample rate (" << expectedSampleRate << "!\n";
      throw std::runtime_error(ss.str().c_str());
    }
  }

  int first_layer_input_size = 1;
  int first_layer_condition_size = 1;

  for (size_t i = 0; i < config["layers"].size(); i++)
  {
    nlohmann::json layer_config = config["layers"][i];

    const int groups = layer_config.value("groups_input", 1); // defaults to 1
    const int groups_input_mixin = layer_config.value("groups_input_mixin", 1); // defaults to 1

    const int channels = layer_config["channels"];
    const int bottleneck = layer_config.value("bottleneck", channels); // defaults to channels if not present

    // Parse layer1x1 parameters
    bool layer1x1_active = true;
    int layer1x1_groups = 1;
    if (layer_config.find("layer1x1") != layer_config.end())
    {
      const auto& layer1x1_config = layer_config["layer1x1"];
      layer1x1_active = layer1x1_config["active"];
      layer1x1_groups = layer1x1_config["groups"];
    }
    nam::wavenet::Layer1x1Params layer1x1_params(layer1x1_active, layer1x1_groups);

    const int input_size = layer_config["input_size"];
    const int condition_size = layer_config["condition_size"];

    if (i == 0)
    {
        first_layer_input_size = input_size;
        first_layer_condition_size = condition_size;
    }

    int head_size = 0;
    int head_kernel_size = 1;
    bool head_bias = false;

    // Prefer nested "head" (matches trainer export). Legacy .nam uses head_size + head_bias (implicit kernel 1).
    if (layer_config.find("head") != layer_config.end() && !layer_config["head"].is_null())
    {
      const auto& head_json = layer_config["head"];
      if (!head_json.is_object())
      {
        throw std::runtime_error("Layer array " + std::to_string(i) + ": 'head' must be a JSON object");
      }
      head_size = head_json.at("out_channels").get<int>();
      head_kernel_size = head_json.at("kernel_size").get<int>();
      head_bias = head_json.at("bias").get<bool>();
    }
    else if (layer_config.find("head_size") != layer_config.end())
    {
      head_size = layer_config["head_size"].get<int>();
      head_kernel_size = 1;
      head_bias = layer_config.at("head_bias").get<bool>();
    }
    else
    {
      throw std::runtime_error("Layer array " + std::to_string(i)
                               + ": expected 'head' object with out_channels, kernel_size, and bias, "
                                 "or legacy 'head_size' and 'head_bias'");
    }

    if (head_kernel_size < 1)
    {
      throw std::runtime_error("Layer array " + std::to_string(i) + ": head.kernel_size must be >= 1");
    }

    const auto dilations = layer_config["dilations"];
    const size_t num_layers = dilations.size();

    // Parse kernel sizes - support legacy single-value kernel_size or new per-layer kernel_sizes
    const bool has_kernel_size = layer_config.find("kernel_size") != layer_config.end();
    const bool has_kernel_sizes = layer_config.find("kernel_sizes") != layer_config.end();
    std::vector<int> kernel_sizes;
    if (has_kernel_size && has_kernel_sizes)
    {
      throw std::runtime_error("Layer array " + std::to_string(i)
                               + ": only one of kernel_size (int) or kernel_sizes (array) may be provided");
    }
    else if (has_kernel_sizes)
    {
      const auto& kernel_sizes_json = layer_config["kernel_sizes"];
      if (!kernel_sizes_json.is_array())
      {
        throw std::runtime_error("Layer array " + std::to_string(i) + ": kernel_sizes must be an array");
      }
      for (const auto& ks_json : kernel_sizes_json)
      {
        kernel_sizes.push_back(ks_json.get<int>());
      }
      if (kernel_sizes.size() != num_layers)
      {
        throw std::runtime_error("Layer array " + std::to_string(i) + ": kernel_sizes array size ("
                                 + std::to_string(kernel_sizes.size()) + ") must match dilations size ("
                                 + std::to_string(num_layers) + ")");
      }
    }
    else if (has_kernel_size)
    {
      const int kernel_size = layer_config["kernel_size"].get<int>();
      kernel_sizes.resize(num_layers, kernel_size);
    }
    else
    {
      throw std::runtime_error("Layer array " + std::to_string(i)
                               + ": either kernel_size (int) or kernel_sizes (array) must be provided");
    }

    // Parse activation config(s) - support both single config and array
    std::vector<activations::ActivationConfig> activation_configs;
    if (layer_config["activation"].is_array())
    {
      for (const auto& activation_json : layer_config["activation"])
      {
        activation_configs.push_back(activations::ActivationConfig::from_json(activation_json));
      }
      if (activation_configs.size() != num_layers)
      {
        throw std::runtime_error("Layer array " + std::to_string(i) + ": activation array size ("
                                 + std::to_string(activation_configs.size()) + ") must match dilations size ("
                                 + std::to_string(num_layers) + ")");
      }
    }
    else
    {
      // Single activation config - duplicate it for all layers
      const activations::ActivationConfig activation_config =
        activations::ActivationConfig::from_json(layer_config["activation"]);
      activation_configs.resize(num_layers, activation_config);
    }

    // Parse gating mode(s) - support both single value and array, and old "gated" boolean
    std::vector<nam::wavenet::GatingMode> gating_modes;
    std::vector<activations::ActivationConfig> secondary_activation_configs;

    auto parse_gating_mode_str = [](const std::string& gating_mode_str) -> nam::wavenet::GatingMode {
      if (gating_mode_str == "gated")
        return nam::wavenet::GatingMode::GATED;
      else if (gating_mode_str == "blended")
        return nam::wavenet::GatingMode::BLENDED;
      else if (gating_mode_str == "none")
        return nam::wavenet::GatingMode::NONE;
      else
        throw std::runtime_error("Invalid gating_mode: " + gating_mode_str);
    };

    if (layer_config.find("gating_mode") != layer_config.end())
    {
      if (layer_config["gating_mode"].is_array())
      {
        for (const auto& gating_mode_json : layer_config["gating_mode"])
        {
          std::string gating_mode_str = gating_mode_json.get<std::string>();
          nam::wavenet::GatingMode mode = parse_gating_mode_str(gating_mode_str);
          gating_modes.push_back(mode);

          // Parse corresponding secondary activation if gating is enabled
          if (mode != nam::wavenet::GatingMode::NONE)
          {
            if (layer_config.find("secondary_activation") != layer_config.end())
            {
              if (layer_config["secondary_activation"].is_array())
              {
                if (gating_modes.size() > layer_config["secondary_activation"].size())
                {
                  throw std::runtime_error("Layer array " + std::to_string(i)
                                           + ": secondary_activation array size must be at least "
                                           + std::to_string(gating_modes.size()));
                }
                secondary_activation_configs.push_back(activations::ActivationConfig::from_json(
                  layer_config["secondary_activation"][gating_modes.size() - 1]));
              }
              else
              {
                // Single secondary activation - use for all gated layers
                secondary_activation_configs.push_back(
                  activations::ActivationConfig::from_json(layer_config["secondary_activation"]));
              }
            }
            else
            {
              // Default to Sigmoid for backward compatibility
              secondary_activation_configs.push_back(
                activations::ActivationConfig::simple(activations::ActivationType::Sigmoid));
            }
          }
          else
          {
            secondary_activation_configs.push_back(activations::ActivationConfig{});
          }
        }
        if (gating_modes.size() != num_layers)
        {
          throw std::runtime_error("Layer array " + std::to_string(i) + ": gating_mode array size ("
                                   + std::to_string(gating_modes.size()) + ") must match dilations size ("
                                   + std::to_string(num_layers) + ")");
        }
        // Validate secondary_activation array size if it's an array
        if (layer_config.find("secondary_activation") != layer_config.end()
            && layer_config["secondary_activation"].is_array())
        {
          if (layer_config["secondary_activation"].size() != num_layers)
          {
            throw std::runtime_error("Layer array " + std::to_string(i) + ": secondary_activation array size ("
                                     + std::to_string(layer_config["secondary_activation"].size())
                                     + ") must match dilations size (" + std::to_string(num_layers) + ")");
          }
        }
      }
      else
      {
        // Single gating mode - duplicate for all layers
        std::string gating_mode_str = layer_config["gating_mode"].get<std::string>();
        nam::wavenet::GatingMode gating_mode = parse_gating_mode_str(gating_mode_str);
        gating_modes.resize(num_layers, gating_mode);

        activations::ActivationConfig secondary_activation_config;
        if (gating_mode != nam::wavenet::GatingMode::NONE)
        {
          if (layer_config.find("secondary_activation") != layer_config.end())
          {
            secondary_activation_config =
              activations::ActivationConfig::from_json(layer_config["secondary_activation"]);
          }
          else
          {
            // Default to Sigmoid for backward compatibility
            secondary_activation_config = activations::ActivationConfig::simple(activations::ActivationType::Sigmoid);
          }
        }
        secondary_activation_configs.resize(num_layers, secondary_activation_config);
      }
    }
    // Backward compatibility: convert old "gated" boolean to new enum
    else if (layer_config.find("gated") != layer_config.end())
    {
      bool gated = layer_config["gated"];
      nam::wavenet::GatingMode gating_mode = gated ? nam::wavenet::GatingMode::GATED : nam::wavenet::GatingMode::NONE;
      gating_modes.resize(num_layers, gating_mode);

      if (gated)
      {
        activations::ActivationConfig secondary_config =
          activations::ActivationConfig::simple(activations::ActivationType::Sigmoid);
        secondary_activation_configs.resize(num_layers, secondary_config);
      }
      else
      {
        secondary_activation_configs.resize(num_layers, activations::ActivationConfig{});
      }
    }
    else
    {
      // Default to NONE for all layers
      gating_modes.resize(num_layers, nam::wavenet::GatingMode::NONE);
      secondary_activation_configs.resize(num_layers, activations::ActivationConfig{});
    }

    // Parse head1x1 parameters
    bool head1x1_active = false;
    int head1x1_out_channels = channels;
    int head1x1_groups = 1;
    if (layer_config.find("head1x1") != layer_config.end())
    {
      const auto& head1x1_config = layer_config["head1x1"];
      head1x1_active = head1x1_config["active"];
      head1x1_out_channels = head1x1_config["out_channels"];
      head1x1_groups = head1x1_config["groups"];
    }
    nam::wavenet::Head1x1Params head1x1_params(head1x1_active, head1x1_out_channels, head1x1_groups);

    // Helper function to parse FiLM parameters
    auto parse_film_params = [&layer_config](const std::string& key) -> nam::wavenet::_FiLMParams {
      if (layer_config.find(key) == layer_config.end() || layer_config[key] == false)
      {
        return nam::wavenet::_FiLMParams(false, false);
      }
      const nlohmann::json& film_config = layer_config[key];
      bool active = film_config.value("active", true);
      bool shift = film_config.value("shift", true);
      int film_groups = film_config.value("groups", 1);
      return nam::wavenet::_FiLMParams(active, shift, film_groups);
    };

    // Parse FiLM parameters
    nam::wavenet::_FiLMParams conv_pre_film_params = parse_film_params("conv_pre_film");
    nam::wavenet::_FiLMParams conv_post_film_params = parse_film_params("conv_post_film");
    nam::wavenet::_FiLMParams input_mixin_pre_film_params = parse_film_params("input_mixin_pre_film");
    nam::wavenet::_FiLMParams input_mixin_post_film_params = parse_film_params("input_mixin_post_film");
    nam::wavenet::_FiLMParams activation_pre_film_params = parse_film_params("activation_pre_film");
    nam::wavenet::_FiLMParams activation_post_film_params = parse_film_params("activation_post_film");
    nam::wavenet::_FiLMParams _layer1x1_post_film_params = parse_film_params("layer1x1_post_film");
    nam::wavenet::_FiLMParams head1x1_post_film_params = parse_film_params("head1x1_post_film");

    // Validation: if layer1x1_post_film is active, layer1x1 must also be active
    if (_layer1x1_post_film_params.active && !layer1x1_active)
    {
      throw std::runtime_error("Layer array " + std::to_string(i)
                               + ": layer1x1_post_film cannot be active when layer1x1.active is false");
    }

    wc.layer_array_params.push_back(nam::wavenet::LayerArrayParams(
      input_size, condition_size, head_size, head_kernel_size, channels, bottleneck, std::move(kernel_sizes), dilations,
      std::move(activation_configs), std::move(gating_modes), head_bias, groups, groups_input_mixin, layer1x1_params,
      head1x1_params, std::move(secondary_activation_configs), conv_pre_film_params, conv_post_film_params,
      input_mixin_pre_film_params, input_mixin_post_film_params, activation_pre_film_params,
      activation_post_film_params, _layer1x1_post_film_params, head1x1_post_film_params));
  }

  wc.with_head = config.find("head") != config.end() && !config["head"].is_null();
  wc.head_scale = config["head_scale"];
  wc.in_channels = config.value("in_channels", 1);

  if (wc.layer_array_params.empty())
    throw std::runtime_error("WaveNet config requires at least one layer array");

  if (wc.with_head)
  {
    const nlohmann::json& hj = config["head"];
    nam::wavenet::HeadParams hp;
    const int implied_in = wc.layer_array_params.back().head_size;
    // New trainer export omits in_channels (single source: last layer head_size). Legacy .nam may include it.
    if (hj.find("in_channels") != hj.end() && !hj["in_channels"].is_null())
    {
      const int legacy_in = hj["in_channels"].get<int>();
      if (legacy_in != implied_in)
      {
        std::stringstream ss;
        ss << "WaveNet config: head.in_channels (" << legacy_in << ") must equal last layer's head_size (" << implied_in
           << ")";
        throw std::runtime_error(ss.str());
      }
    }
    hp.in_channels = implied_in;
    hp.channels = hj.at("channels").get<int>();
    hp.out_channels = hj.at("out_channels").get<int>();
    hp.kernel_sizes = hj.at("kernel_sizes").get<std::vector<int>>();
    hp.activation_config = nam::activations::ActivationConfig::from_json(hj.at("activation"));
    if (hp.kernel_sizes.empty())
      throw std::runtime_error("WaveNet config: head.kernel_sizes must be non-empty");
    wc.head_params = std::move(hp);
  }
  else
    wc.head_params = std::nullopt;

  return wc;
}

// WaveNetConfig::create()
std::unique_ptr<nam::DSP> nam::parametric_wavenet::WaveNetConfig::create(std::vector<float> weights, double sampleRate)
{
  return std::make_unique<nam::parametric_wavenet::ParametricWaveNet>(in_channels, layer_array_params, head_scale, with_head,
                                                 std::move(head_params), std::move(weights), std::move(condition_dsp),
                                                 sampleRate);
}

namespace
{
const std::string SLIMMABLE_METHOD = "slice_channels_uniform";

bool config_is_slimmable_wavenet(const nlohmann::json& config)
{
  if (config.find("layers") == config.end() || !config["layers"].is_array())
    return false;
  for (const auto& lc : config["layers"])
  {
    if (lc.find("slimmable") == lc.end() || !lc["slimmable"].is_object())
      continue;
    const std::string method = lc["slimmable"].value("method", "");
    if (method != SLIMMABLE_METHOD)
    {
      if (!method.empty())
        throw std::runtime_error("SlimmableWavenet: unsupported slimmable method '" + method + "'");
      continue;
    }
    return true;
  }
  return false;
}
} // namespace

// Config parser for ConfigParserRegistry
std::unique_ptr<nam::ModelConfig> nam::parametric_wavenet::create_config(const nlohmann::json& config, double sampleRate)
{
  if (config_is_slimmable_wavenet(config))
    return nam::parametric_slimmable_wavenet::create_config(config, sampleRate);

#if defined(NAM_ENABLE_A2_FAST)
  if ( nam::wavenet::parametric_a2_fast::is_a2_shape(config, nullptr, nullptr))
    return nam::wavenet::parametric_a2_fast::create_a2_fast_config(config, sampleRate);
#endif

  auto wc = std::make_unique<WaveNetConfig>();
  auto parsed = parse_config_json(config, sampleRate);
  *wc = std::move(parsed);
  return wc;
}

void nam::parametric_wavenet::register_parser()
{
    static bool ONCE{ false };

    if (!ONCE)
    {
        ConfigParserRegistry::instance().registerParser("ParametricWaveNet", nam::parametric_wavenet::create_config);

        ONCE = true;
    }
}

// Register the config parser
/*
namespace
{
static nam::ConfigParserHelper _register_WaveNet("WaveNet", nam::wavenet::create_config);
}
*/
