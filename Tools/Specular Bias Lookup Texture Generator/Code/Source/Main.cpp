//Core.
#include <Core/Essential/CatalystEssential.h>

//File.
#include <File/Core/BinaryFile.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//STL.
#include <iostream>

#if !defined(EXIT_SUCCESS)
#define EXIT_SUCCES (0)
#endif

float32 RadicalInverse_VdC(uint32 bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

	return float32(bits) * 2.3283064365386963e-10f;
}

Vector2<float32> Hammersley(unsigned int i, unsigned int N)
{
	return Vector2<float32>(float(i) / float(N), RadicalInverse_VdC(i));
}

Vector3<float32> ImportanceSampleGGX(Vector2<float32> Xi, float roughness, Vector3<float32> N)
{
	float a = roughness*roughness;

	float phi = 2.0f * CatalystBaseMathConstants::PI * Xi._X;
	float cosTheta = sqrt((1.0f - Xi._Y) / (1.0f + (a*a - 1.0f) * Xi._Y));
	float sinTheta = sqrt(1.0f - cosTheta*cosTheta);

	// from spherical coordinates to cartesian coordinates
	Vector3<float32> H;
	H._X = cos(phi) * sinTheta;
	H._Y = sin(phi) * sinTheta;
	H._Z = cosTheta;

	// from tangent-space vector to world-space sample vector
	Vector3<float32> up = abs(N._Z) < 0.999f ? Vector3<float32>(0.0f, 0.0f, 1.0f) : Vector3<float32>(1.0f, 0.0f, 0.0f);
	Vector3<float32> tangent = Vector3<float32>::Normalize(Vector3<float32>::CrossProduct(up, N));
	Vector3<float32> bitangent = Vector3<float32>::CrossProduct(N, tangent);

	Vector3<float32> sampleVec = tangent * H._X + bitangent * H._Y + N * H._Z;
	
	return Vector3<float32>::Normalize(sampleVec);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float a = roughness;
	float k = (a * a) / 2.0f;

	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return nom / denom;
}

float GeometrySmith(float roughness, float NoV, float NoL)
{
	float ggx2 = GeometrySchlickGGX(NoV, roughness);
	float ggx1 = GeometrySchlickGGX(NoL, roughness);

	return ggx1 * ggx2;
}

Vector2<float32> IntegrateBRDF(float NdotV, float roughness, unsigned int samples)
{
	Vector3<float32> V;
	V._X = sqrt(1.0f - NdotV * NdotV);
	V._Y = 0.0f;
	V._Z = NdotV;

	float A = 0.0f;
	float B = 0.0f;

	Vector3<float32> N = Vector3<float32>(0.0f, 0.0f, 1.0f);

	for (unsigned int i = 0u; i < samples; ++i)
	{
		Vector2<float32> Xi = Hammersley(i, samples);
		Vector3<float32> H = ImportanceSampleGGX(Xi, roughness, N);
		Vector3<float32> L = Vector3<float32>::Normalize(2.0f * Vector3<float32>::DotProduct(V, H) * H - V);

		float NoL = CatalystBaseMath::Maximum(L._Z, 0.0f);
		float NoH = CatalystBaseMath::Maximum(H._Z, 0.0f);
		float VoH = CatalystBaseMath::Maximum(Vector3<float32>::DotProduct(V, H), 0.0f);
		float NoV = CatalystBaseMath::Maximum(Vector3<float32>::DotProduct(N, V), 0.0f);

		if (NoL > 0.0)
		{
			float G = GeometrySmith(roughness, NoV, NoL);

			float G_Vis = (G * VoH) / (NoH * NoV);
			float Fc = pow(1.0f - VoH, 5.0f);

			A += (1.0f - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	return Vector2<float32>(A / float(samples), B / float(samples));
}

int32 main() NOEXCEPT
{
	//Define constants.
	constexpr uint32 SIZE{ 512 };
	constexpr uint32 SAMPLES{ 1'024 };

	//Create the texture.
	Texture2D<Vector2<float32>> texture{ SIZE };

	for (uint32 Y{ 0 }; Y < SIZE; ++Y)
	{
		for (uint32 X{ 0 }; X < SIZE; ++X)
		{
			float NoV = (Y + 0.5f) * (1.0f / SIZE);
			float roughness = (X + 0.5f) * (1.0f / SIZE);

			texture.At(X, Y) = IntegrateBRDF(NoV, roughness, SAMPLES);
		}

		std::cout << float32(float32(Y) / float32(SIZE)) * 100.0f << "% done." << std::endl;
	}

	//Write the texture to file.
	BinaryFile<IOMode::Out> file{ "SpecularBiasLookupTextureData.cr" };

	file.Write(texture.Data(), SIZE * SIZE * sizeof(Vector2<float32>));

	file.Close();

	return EXIT_SUCCESS;
}