/**
 * @file methods/ann/layer/softmin_impl.hpp
 * @author Aakash Kaushik
 *
 * Implementation of the Softmin class.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LAYER_SOFTMIN_IMPL_HPP
#define MLPACK_METHODS_ANN_LAYER_SOFTMIN_IMPL_HPP

// In case it hasn't yet been included.
#include "softmin.hpp"

namespace mlpack {

template<typename InputType, typename OutputType>
SoftminType<InputType, OutputType>::SoftminType()
{
  // Nothing to do here.
}

template<typename InputType, typename OutputType>
void SoftminType<InputType, OutputType>::Forward(
    const InputType& input,
    OutputType& output)
{
  InputType softminInput = arma::exp(-(input.each_row() -
      arma::min(input, 0)));
  output = softminInput.each_row() / sum(softminInput, 0);
}

template<typename InputType, typename OutputType>
void SoftminType<InputType, OutputType>::Backward(
    const InputType& input,
    const OutputType& gy,
    OutputType& g)
{
  g = input % (gy - arma::repmat(arma::sum(gy % input), input.n_rows, 1));
}

template<typename InputType, typename OutputType>
template<typename Archive>
void SoftminType<InputType, OutputType>::serialize(
    Archive& ar,
    const uint32_t /* version */)
{
  ar(cereal::base_class<Layer<InputType, OutputType>>(this));
}

} // namespace mlpack

#endif
