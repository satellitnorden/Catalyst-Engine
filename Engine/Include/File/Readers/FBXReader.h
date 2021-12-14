#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File.
#include <File/Types/ModelFile.h>

//Constants.
#define USE_ASSIMP (0)

#if USE_ASSIMP
//Third party.
#include <ThirdParty/assimp/Importer.hpp>
#include <ThirdParty/assimp/scene.h>
#include <ThirdParty/assimp/postprocess.h>

class FBXReader final
{

public:

	/*
	*	Reads the model at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char* const RESTRICT file_path, ModelFile *const RESTRICT model_file) NOEXCEPT
	{
		//Initialize the Assimp scene.
		Assimp::Importer importer;
		const aiScene *const RESTRICT scene{ importer.ReadFile(	file_path,	
																aiPostProcessSteps::aiProcess_CalcTangentSpace
																| aiPostProcessSteps::aiProcess_JoinIdenticalVertices
																| aiPostProcessSteps::aiProcess_Triangulate
																| aiPostProcessSteps::aiProcess_GenNormals
																| aiPostProcessSteps::aiProcess_ImproveCacheLocality
																| aiPostProcessSteps::aiProcess_FlipUVs) };

		if (!scene)
		{
			ASSERT(false, "Couldn't read model!");

			return false;
		}

		//Read the meta data.
		class MetaData meta_data;
		
		if (scene->mMetaData)
		{
			GatherMetaData(scene->mMetaData, &meta_data);
		}

		//Process the node(s).
		ProcessNode(scene->mRootNode, scene, model_file);

		//Apply the transformation to the vertices to the Catalyst coordinate space.
		for (ModelFile::Mesh &mesh : model_file->_Meshes)
		{
			for (Vertex &vertex : mesh._Vertices)
			{
				vertex._Position = Vector3<float32>(vertex._Position[meta_data._AxesSwizzle[0]] * meta_data._AxesCoefficients[0],
													vertex._Position[meta_data._AxesSwizzle[1]] * meta_data._AxesCoefficients[1],
													vertex._Position[meta_data._AxesSwizzle[2]] * meta_data._AxesCoefficients[2]);

				vertex._Normal = Vector3<float32>(	vertex._Normal[meta_data._AxesSwizzle[0]] * meta_data._AxesCoefficients[0],
													vertex._Normal[meta_data._AxesSwizzle[1]] * meta_data._AxesCoefficients[1],
													vertex._Normal[meta_data._AxesSwizzle[2]] * meta_data._AxesCoefficients[2]);

				vertex._Tangent = Vector3<float32>(	vertex._Tangent[meta_data._AxesSwizzle[0]] * meta_data._AxesCoefficients[0],
													vertex._Tangent[meta_data._AxesSwizzle[1]] * meta_data._AxesCoefficients[1],
													vertex._Tangent[meta_data._AxesSwizzle[2]] * meta_data._AxesCoefficients[2]);
			}
		}

		return true;
	}

private:

	/*
	*	Meta data class definition.
	*/
	class MetaData final
	{

	public:

		//The axes swizzle.
		Vector3<uint8> _AxesSwizzle{ 0, 1, 2 };

		//The axes coefficients.
		Vector3<float32> _AxesCoefficients{ 1.0f, 1.0f, 1.0f };

	};

	/*
	*	Gathers the meta data.
	*/
	FORCE_INLINE static void GatherMetaData(const aiMetadata *const RESTRICT in_meta_data, MetaData *const RESTRICT out_meta_data) NOEXCEPT
	{
		for (uint32 i{ 0 }; i < in_meta_data->mNumProperties; ++i)
		{
			const aiString &key{ in_meta_data->mKeys[i] };;
			const aiMetadataEntry &value{ in_meta_data->mValues[i] };

			if (StringUtilities::IsEqual(key.C_Str(), "CoordAxis"))
			{
				if (value.mType == AI_INT32)
				{
					const int32 value_int32{ *static_cast<const int32 *const RESTRICT>(value.mData) };

					switch (value_int32)
					{
						case 0:
						{
							out_meta_data->_AxesSwizzle[0] = 0;

							break;
						}

						case 1:
						{
							out_meta_data->_AxesSwizzle[0] = 1;

							break;
						}

						case 2:
						{
							out_meta_data->_AxesSwizzle[0] = 2;

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}
				}
			}

			if (StringUtilities::IsEqual(key.C_Str(), "CoordAxisSign"))
			{
				if (value.mType == AI_INT32)
				{
					const int32 value_int32{ *static_cast<const int32 *const RESTRICT>(value.mData) };

					switch (value_int32)
					{
						case 0:
						{
							out_meta_data->_AxesCoefficients[0] = -1.0f;

							break;
						}

						case 1:
						{
							out_meta_data->_AxesCoefficients[0] = 1.0f;

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}
				}
			}

			if (StringUtilities::IsEqual(key.C_Str(), "UpAxis"))
			{
				if (value.mType == AI_INT32)
				{
					const int32 value_int32{ *static_cast<const int32 *const RESTRICT>(value.mData) };

					switch (value_int32)
					{
						case 0:
						{
							out_meta_data->_AxesSwizzle[1] = 0;

							break;
						}

						case 1:
						{
							out_meta_data->_AxesSwizzle[1] = 1;

							break;
						}

						case 2:
						{
							out_meta_data->_AxesSwizzle[1] = 2;

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}
				}
			}

			if (StringUtilities::IsEqual(key.C_Str(), "UpAxisSign"))
			{
				if (value.mType == AI_INT32)
				{
					const int32 value_int32{ *static_cast<const int32 *const RESTRICT>(value.mData) };

					switch (value_int32)
					{
						case 0:
						{
							out_meta_data->_AxesCoefficients[1] = -1.0f;

							break;
						}

						case 1:
						{
							out_meta_data->_AxesCoefficients[1] = 1.0f;

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}
				}
			}

			if (StringUtilities::IsEqual(key.C_Str(), "FrontAxis"))
			{
				if (value.mType == AI_INT32)
				{
					const int32 value_int32{ *static_cast<const int32 *const RESTRICT>(value.mData) };

					switch (value_int32)
					{
						case 0:
						{
							out_meta_data->_AxesSwizzle[2] = 0;

							break;
						}

						case 1:
						{
							out_meta_data->_AxesSwizzle[2] = 1;

							break;
						}

						case 2:
						{
							out_meta_data->_AxesSwizzle[2] = 2;

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}
				}
			}

			if (StringUtilities::IsEqual(key.C_Str(), "FrontAxisSign"))
			{
				if (value.mType == AI_INT32)
				{
					const int32 value_int32{ *static_cast<const int32 *const RESTRICT>(value.mData) };

					switch (value_int32)
					{
						case 0:
						{
							out_meta_data->_AxesCoefficients[2] = -1.0f;

							break;
						}

						case 1:
						{
							out_meta_data->_AxesCoefficients[2] = 1.0f;

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}
				}
			}
		}
	}

	/*
	*	Processes a single node.
	*/
	FORCE_INLINE static void ProcessNode(aiNode *RESTRICT node, const aiScene *RESTRICT scene, ModelFile *const RESTRICT model_file) NOEXCEPT
	{
		//Process all meshes.
		for (uint32 i{ 0 }; i < node->mNumMeshes; ++i)
		{
			//Gather the meta data.
			MetaData meta_data;

			if (node->mMetaData)
			{
				GatherMetaData(node->mMetaData, &meta_data);
			}

			//Add a new mesh.
			model_file->_Meshes.Emplace();

			ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, &model_file->_Meshes.Back());
		}

		//Process all nodes.
		for (uint32 i{ 0 }; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, model_file);
		}
	}

	/*
	*	Processes a single mesh.
	*/
	FORCE_INLINE static void ProcessMesh(aiMesh *RESTRICT mesh, const aiScene *RESTRICT scene, ModelFile::Mesh *const RESTRICT out_mesh) NOEXCEPT
	{
		//Process the vertices.
		for (uint32 i{ 0 }; i < mesh->mNumVertices; ++i)
		{
			out_mesh->_Vertices.Emplace(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Position._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Position._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Position._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Normal._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Normal._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Normal._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Tangent._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Tangent._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Tangent._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._TextureCoordinate._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._TextureCoordinate._Y), "NaN detected!");
		}

		//Process the indices.
		for (uint32 i{ 0 }; i < mesh->mNumFaces; ++i)
		{
			aiFace &face = mesh->mFaces[i];

			for (uint32 j{ 0 }; j < face.mNumIndices; ++j)
			{
				out_mesh->_Indices.Emplace(face.mIndices[j]);
			}
		}
	}

};
#else
//Macros.
#define FBX_READER_PRINT_TO_OUTPUT(MESSAGE) /*PRINT_TO_OUTPUT(MESSAGE)*/

//File.
#include <File/Core/BinaryFile.h>

//Third party.
#include <ThirdParty/zlib/zlib.h>

class FBXReader final
{

public:

	/*
	*	Reads the model at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char* const RESTRICT file_path, ModelFile *const RESTRICT model_file) NOEXCEPT
	{
		//Open the binary file.
		BinaryFile<BinaryFileMode::IN> binary_file{ file_path };

		//Read the header title.
		char header_title[21];
		binary_file.Read(header_title, 21);

		//Read the header unknown.
		char header_unknown[2];
		binary_file.Read(header_unknown, 2);

		//Read the header version.
		uint32 header_version;
		binary_file.Read(&header_version, sizeof(uint32));

		//Read the nodes.
		TemporaryData temporary_data;

		while (ReadNode(&binary_file, &temporary_data, model_file));

		ASSERT(temporary_data._FoundTangents, "No tangents were found, please export " << file_path << " with tangents.");

		//Close the binary file.
		binary_file.Close();

		//Post process the model file.
		PostProcess(model_file);

		//Return that the read succeeded!
		return true;
	}

private:

	/*
	*	Temporary data class definition.
	*/
	class TemporaryData final
	{

	public:

		//Enumeration covering all mapping information types.
		enum class MappingInformationType : uint8
		{
			BY_POLYGON_VERTEX,

			UNKNOWN
		};

		//Enumeration covering all reference information types.
		enum class ReferenceInformationType : uint8
		{
			DIRECT,
			INDEX_TO_DIRECT,

			UNKNOWN
		};

		//The mapping information type.
		MappingInformationType _MappingInformationType{ MappingInformationType::UNKNOWN };

		//The mapping information type.
		ReferenceInformationType _ReferenceInformationType{ ReferenceInformationType::UNKNOWN };

		//Denotes if tangents were found.
		bool _FoundTangents{ false };

		//The vertex position lookup.
		DynamicArray<Vector3<float32>> _VertexPositionLookup;

		//The vertex index lookup.
		DynamicArray<uint32> _VertexIndexLookup;

		//The texture coordinate lookup.
		DynamicArray<Vector2<float32>> _TextureCoordinateLookup;

	};

	/*
	*	Reads a node.
	*/
	FORCE_INLINE static bool ReadNode(BinaryFile<BinaryFileMode::IN> *const RESTRICT binary_file, TemporaryData *const RESTRICT temporary_data, ModelFile *const RESTRICT model_file) NOEXCEPT
	{
		//Read the end offset.
		uint32 end_offset;
		binary_file->Read(&end_offset, sizeof(uint32));

		//Read the number of properties.
		uint32 number_of_properties;
		binary_file->Read(&number_of_properties, sizeof(uint32));

		//The the property list length.
		uint32 property_list_length;
		binary_file->Read(&property_list_length, sizeof(uint32));

		//Read the name length.
		uint8 name_length;
		binary_file->Read(&name_length, sizeof(uint8));

		//Read the name.
		DynamicString name;
		name.SetLength(name_length);
		binary_file->Read(name.Data(), name_length);

		//Is this the start of a new geometry definition?
		if (name == "Geometry")
		{
			//Emplace the new mesh.
			model_file->_Meshes.Emplace();

			//Reset the vertex position lookup.
			temporary_data->_VertexPositionLookup.Clear();

			//Reset the vertex index lookup.
			temporary_data->_VertexIndexLookup.Clear();

			//Reset the texture coordinate lookup.
			temporary_data->_TextureCoordinateLookup.Clear();
		}

		//Read the properties.
		for (uint32 i{ 0 }; i < number_of_properties; ++i)
		{
			//Read the type code.
			char type_code;
			binary_file->Read(&type_code, sizeof(char));

			switch (type_code)
			{
				case 'C':
				{
					//Read the bool.
					bool value;
					binary_file->Read(&value, sizeof(bool));

					FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value);

					break;
				}

				case 'D':
				{
					//Read the float64.
					float64 value;
					binary_file->Read(&value, sizeof(float64));

					FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value);

					break;
				}

				case 'I':
				{
					//Read the int32.
					int32 value;
					binary_file->Read(&value, sizeof(int32));

					FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value);

					break;
				}

				case 'L':
				{
					//Read the int64.
					int64 value;
					binary_file->Read(&value, sizeof(int64));

					FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value);

					break;
				}

				case 'R':
				{
					//Read the raw data length.
					uint32 raw_data_length;
					binary_file->Read(&raw_data_length, sizeof(uint32));

					//Read the raw data.
					DynamicArray<char> raw_data;
					raw_data.Upsize<false>(raw_data_length);

					binary_file->Read(raw_data.Data(), raw_data_length);

					FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << raw_data.Data());

					break;
				}

				case 'S':
				{
					//Read the string length.
					uint32 string_length;
					binary_file->Read(&string_length, sizeof(uint32));

					//Read the value.
					DynamicString value;
					value.SetLength(string_length);

					binary_file->Read(value.Data(), string_length);

					FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value.Data());

					//Is this the creator value?
					if (name == "Creator")
					{
						if (value.Find("Blender"))
						{
							model_file->_Creator = ModelFile::Creator::BLENDER;
						}
					}

					//Is this the mapping information type value?
					if (name == "MappingInformationType")
					{
						if (value == "ByPolygonVertex")
						{
							temporary_data->_MappingInformationType = TemporaryData::MappingInformationType::BY_POLYGON_VERTEX;
						}
					}

					//Is this the reference information type value?
					if (name == "ReferenceInformationType")
					{
						if (value == "Direct")
						{
							temporary_data->_ReferenceInformationType = TemporaryData::ReferenceInformationType::DIRECT;
						}

						if (value == "IndexToDirect")
						{
							temporary_data->_ReferenceInformationType = TemporaryData::ReferenceInformationType::INDEX_TO_DIRECT;
						}
					}

					break;
				}

				case 'd':
				{
					//Read the array length.
					uint32 array_length;
					binary_file->Read(&array_length, sizeof(uint32));

					//Read the encoding.
					uint32 encoding;
					binary_file->Read(&encoding, sizeof(uint32));

					//Read the compressed length.
					uint32 compressed_length;
					binary_file->Read(&compressed_length, sizeof(uint32));

					//Read the values.
					DynamicArray<float64> values;
					values.Upsize<false>(array_length);

					if (encoding)
					{
						//Calculate the uncompressed length.
						const uint64 uncompressed_length{ sizeof(float64) * array_length };

						//Allocate the decompressed buffer.
						uint8 *const RESTRICT decompressed_buffer{ static_cast<uint8 *const RESTRICT>(Memory::Allocate(uncompressed_length)) };

						//Allocate and read the compressed buffer.
						uint8 *const RESTRICT compressed_buffer{ static_cast<uint8 *const RESTRICT>(Memory::Allocate(compressed_length)) };
						binary_file->Read(compressed_buffer, compressed_length);

						//Uncompress!
						uLongf destination_length{ static_cast<uLongf>(uncompressed_length) };
						uLong source_length{ static_cast<uLong>(compressed_length) };

						uncompress2(decompressed_buffer, &destination_length, compressed_buffer, &source_length);

						ASSERT(destination_length == uncompressed_length, "uncompressed_length does not match data!");
						ASSERT(source_length == compressed_length, "compressed_length does not match data!");

						//Read the values.
						Memory::Copy(values.Data(), decompressed_buffer, sizeof(float64) * array_length);

						//Free the buffers.
						Memory::Free(decompressed_buffer);
						Memory::Free(compressed_buffer);
					}

					else
					{
						binary_file->Read(values.Data(), sizeof(float64) * array_length);
					}

					//Is this the vertices?
					if (name == "Vertices")
					{
						for (uint64 i{ 0 }; i < values.Size() / 3; ++i)
						{
							//Write to the vertex position lookup.
							temporary_data->_VertexPositionLookup.Emplace(static_cast<float32>(values[i * 3 + 0]), static_cast<float32>(values[i * 3 + 1]), static_cast<float32>(values[i * 3 + 2]));
						}
					}

					//Is this the normals?
					if (name == "Normals")
					{
						//Cache the mesh.
						ModelFile::Mesh &mesh{ model_file->_Meshes.Back() };

						switch (temporary_data->_MappingInformationType)
						{
							case TemporaryData::MappingInformationType::BY_POLYGON_VERTEX:
							{
								switch (temporary_data->_ReferenceInformationType)
								{
									case TemporaryData::ReferenceInformationType::DIRECT:
									{
										for (uint64 i{ 0 }; i < values.Size(); i += 3)
										{
											//Create the new vertex.
											mesh._Vertices.Emplace();
											Vertex &new_vertex{ mesh._Vertices.Back() };

											//Create the new index.
											mesh._Indices.Emplace(mesh._Indices.Empty() ? 0 : mesh._Indices.Back() + 1);

											//Retrieve the vertex index.
											const uint32 vertex_index{ temporary_data->_VertexIndexLookup[i / 3] };

											//Retrieve the vertex position.
											const Vector3<float32> vertex_position{ temporary_data->_VertexPositionLookup[vertex_index] };

											//Set the position.
											new_vertex._Position = vertex_position;

											//Set the normal.
											new_vertex._Normal = Vector3<float32>(static_cast<float32>(values[i + 0]), static_cast<float32>(values[i + 1]), static_cast<float32>(values[i + 2]));
										}

										break;
									}

									default:
									{
										ASSERT(false, "Invalid case!");

										break;
									}
								}

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}
					}

					//Is this the tangents?
					if (name == "Tangents")
					{
						//Report that tangents were found.
						temporary_data->_FoundTangents = true;

						//Cache the mesh.
						ModelFile::Mesh &mesh{ model_file->_Meshes.Back() };

						switch (temporary_data->_MappingInformationType)
						{
							case TemporaryData::MappingInformationType::BY_POLYGON_VERTEX:
							{
								switch (temporary_data->_ReferenceInformationType)
								{
									case TemporaryData::ReferenceInformationType::DIRECT:
									{
										for (uint64 i{ 0 }; i < values.Size(); i += 3)
										{
											//Retrieve the vertex.
											Vertex &vertex{ mesh._Vertices[i / 3] };

											//Set the tangent.
											vertex._Tangent = Vector3<float32>(static_cast<float32>(values[i + 0]), static_cast<float32>(values[i + 1]), static_cast<float32>(values[i + 2]));
										
											ASSERT(vertex._Normal != vertex._Tangent, "Normal and tangent are the same, this is bad. /: Maybe the model isn't UV-wrapped properly?");
										}

										break;
									}

									default:
									{
										ASSERT(false, "Invalid case!");

										break;
									}
								}

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}
					}

					//Is this the UV's?
					if (name == "UV")
					{
						switch (temporary_data->_MappingInformationType)
						{
							case TemporaryData::MappingInformationType::BY_POLYGON_VERTEX:
							{
								switch (temporary_data->_ReferenceInformationType)
								{
									case TemporaryData::ReferenceInformationType::INDEX_TO_DIRECT:
									{
										for (uint64 i{ 0 }; i < values.Size(); i += 2)
										{
											//Write into the texture coordinate lookup.
											temporary_data->_TextureCoordinateLookup.Emplace(static_cast<float32>(values[i + 0]), static_cast<float32>(values[i + 1]));
										}

										break;
									}

									default:
									{
										ASSERT(false, "Invalid case!");

										break;
									}
								}

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}
					}

					for (uint64 i{ 0 }; i < values.Size(); ++i)
					{
						FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value #" << i << ": " << values[i]);
					}

					break;
				}

				case 'i':
				{
					//Read the array length.
					uint32 array_length;
					binary_file->Read(&array_length, sizeof(uint32));

					//Read the encoding.
					uint32 encoding;
					binary_file->Read(&encoding, sizeof(uint32));

					//Read the compressed length.
					uint32 compressed_length;
					binary_file->Read(&compressed_length, sizeof(uint32));

					//Read the values.
					DynamicArray<int32> values;
					values.Upsize<false>(array_length);

					if (encoding)
					{
						//Calculate the uncompressed length.
						const uint64 uncompressed_length{ sizeof(int32) * array_length };

						//Allocate the decompressed buffer.
						uint8 *const RESTRICT decompressed_buffer{ static_cast<uint8 *const RESTRICT>(Memory::Allocate(uncompressed_length)) };

						//Allocate and read the compressed buffer.
						uint8 *const RESTRICT compressed_buffer{ static_cast<uint8 *const RESTRICT>(Memory::Allocate(compressed_length)) };
						binary_file->Read(compressed_buffer, compressed_length);

						//Uncompress!
						uLongf destination_length{ static_cast<uLongf>(uncompressed_length) };
						uLong source_length{ static_cast<uLong>(compressed_length) };

						uncompress2(decompressed_buffer, &destination_length, compressed_buffer, &source_length);

						ASSERT(destination_length == uncompressed_length, "uncompressed_length does not match data!");
						ASSERT(source_length == compressed_length, "compressed_length does not match data!");

						//Read the values.
						Memory::Copy(values.Data(), decompressed_buffer, sizeof(int32) * array_length);

						//Free the buffers.
						Memory::Free(decompressed_buffer);
						Memory::Free(compressed_buffer);
					}

					else
					{
						binary_file->Read(values.Data(), sizeof(int32) * array_length);
					}

					//Is this the polygon vertex indices?
					if (name == "PolygonVertexIndex")
					{
						//Cache the mesh.
						ModelFile::Mesh &mesh{ model_file->_Meshes.Back() };

						for (uint64 i{ 0 }; i < values.Size(); ++i)
						{
							//Calculate the actual index.
							const uint32 actual_index{ static_cast<uint32>(values[i] >= 0 ? values[i] : (~values[i])) };

							//Write to the vertex index lookup.
							temporary_data->_VertexIndexLookup.Emplace(actual_index);
						}
					}

					//Is this the UV indices?
					if (name == "UVIndex")
					{
						//Cache the mesh.
						ModelFile::Mesh &mesh{ model_file->_Meshes.Back() };

						switch (temporary_data->_MappingInformationType)
						{
							case TemporaryData::MappingInformationType::BY_POLYGON_VERTEX:
							{
								switch (temporary_data->_ReferenceInformationType)
								{
									case TemporaryData::ReferenceInformationType::INDEX_TO_DIRECT:
									{
										for (uint64 i{ 0 }; i < values.Size(); ++i)
										{
											//Retrieve the vertex.
											Vertex &vertex{ mesh._Vertices[i] };

											//Retrieve the texture coordinate index.
											const uint32 texture_coordinate_index{ static_cast<uint32>(values[i]) };

											//Write the texture coordinate.
											vertex._TextureCoordinate = temporary_data->_TextureCoordinateLookup[texture_coordinate_index];
										}

										break;
									}

									default:
									{
										ASSERT(false, "Invalid case!");

										break;
									}
								}

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}
					}

					for (uint64 i{ 0 }; i < values.Size(); ++i)
					{
						FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value #" << i << ": " << values[i]);
					}

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		//Is there a nested list in this node?
		uint32 number_of_children{ 0 };

		if (binary_file->GetCurrentPosition() < end_offset)
		{
			while (binary_file->GetCurrentPosition() + 13 < end_offset)
			{
				//Read the new node.
				ReadNode(binary_file, temporary_data, model_file);

				//Update the number of children.
				++number_of_children;
			}

			//Read the null record.
			uint8 null_record[13];
			binary_file->Read(null_record, 13);

			ASSERT(binary_file->GetCurrentPosition() == end_offset, "oh no");
		}

		//Return whether or not this node is valid.
		return !(	number_of_children == 0
					&& number_of_properties == 0
					&& name_length == 0);
	}

	/*
	*	Post processes the model file.
	*/
	FORCE_INLINE static void PostProcess(ModelFile *const RESTRICT model_file) NOEXCEPT
	{
		//Check for NaN's/invalid values.
		for (ModelFile::Mesh &mesh : model_file->_Meshes)
		{
			//Go over all vertices, storing the indices of invalid vertices.
			DynamicArray<uint64> invalid_indices;

			for (uint64 vertex_index{ 0 }; vertex_index < mesh._Vertices.Size(); ++vertex_index)
			{
				if (mesh._Vertices[vertex_index]._Normal.IsZero())
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (mesh._Vertices[vertex_index]._Tangent.IsZero())
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Position[0])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Position[1])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Position[2]))
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Normal[0])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Normal[1])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Normal[2]))
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Tangent[0])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Tangent[1])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Tangent[2]))
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._TextureCoordinate[0])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._TextureCoordinate[1]))
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}
			}

			//Go over the indices, removing invalid triangles.
			for (uint64 index_index{ 0 }; index_index < mesh._Indices.Size();)
			{
				if (invalid_indices.Exists(mesh._Indices[index_index + 0])
					|| invalid_indices.Exists(mesh._Indices[index_index + 1])
					|| invalid_indices.Exists(mesh._Indices[index_index + 2]))
				{
					mesh._Indices.EraseAt<true>(index_index + 0);
					mesh._Indices.EraseAt<true>(index_index + 1);
					mesh._Indices.EraseAt<true>(index_index + 2);
				}

				else
				{
					index_index += 3;
				}
			}
		}

		//Transform the vertices based on the creator.
		switch (model_file->_Creator)
		{
			case ModelFile::Creator::BLENDER:
			{
				const Matrix4x4 TRANSFORMATION{ VectorConstants::ZERO, EulerAngles(CatalystBaseMath::DegreesToRadians(90.0f), CatalystBaseMath::DegreesToRadians(180.0f), 0.0f), VectorConstants::ONE };

				for (ModelFile::Mesh &mesh : model_file->_Meshes)
				{
					for (Vertex &vertex : mesh._Vertices)
					{
						vertex.Transform(TRANSFORMATION, 0.0f);
					}
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Join identical vertices.
		for (ModelFile::Mesh &mesh : model_file->_Meshes)
		{
			for (uint32 master_index{ 0 }; master_index < static_cast<uint32>(mesh._Vertices.Size()); ++master_index)
			{
				//Iterate over all the other vertices see if there's a duplicate.
				for (uint32 sub_index{ 0 }; sub_index < static_cast<uint32>(mesh._Vertices.Size());)
				{
					if (master_index != sub_index
						&& mesh._Vertices[master_index] == mesh._Vertices[sub_index])
					{
						//Find the indices that points to the sub index, and redirect it to the master index.
						for (uint32 &index : mesh._Indices)
						{
							if (index == sub_index)
							{
								index = master_index;
							}
						}

						//Find the indices that points to the last index, and redirect it to the sub index.
						for (uint32 &index : mesh._Indices)
						{
							if (index == mesh._Vertices.LastIndex())
							{
								index = sub_index;
							}
						}

						//Erase the offending index.
						mesh._Vertices.EraseAt<false>(sub_index);
					}

					else
					{
						++sub_index;
					}
				}
			}
		}
	}
};
#endif
#endif