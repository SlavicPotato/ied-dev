#pragma once

#include "IED/Profile/Manager.h"
#include "IED/Profile/Profile.h"

#include "D3DBoundingOrientedBox.h"
#include "D3DBoundingSphere.h"

#include <DirectXTK/VertexTypes.h>

namespace IED
{
	struct ModelData
	{
		D3DBoundingOrientedBox CreateBoundingBox() const;
		D3DBoundingSphere      CreateBoundingSphere() const;

		stl::vector<DirectX::VertexPositionNormalColor> vertices;
		stl::vector<std::uint16_t>                      indices;

		std::uint32_t numVertices{ 0 };
		std::uint32_t numTriangles{ 0 };
		std::uint32_t numIndices{ 0 };

		bool m_hasVertexColors{ false };
	};

	class ModelProfile :
		public ProfileBase<std::shared_ptr<ModelData>>
	{
		static constexpr int IMPORT_RVC_FLAGS =
			//aiComponent_COLORS |
			aiComponent_TANGENTS_AND_BITANGENTS |
			aiComponent_TEXCOORDS |
			aiComponent_BONEWEIGHTS |
			aiComponent_ANIMATIONS |
			aiComponent_TEXTURES |
			aiComponent_LIGHTS |
			aiComponent_CAMERAS |
			aiComponent_MATERIALS;

		static constexpr int IMPORT_FLAGS =
			aiProcess_FindInvalidData |
			aiProcess_RemoveComponent |
			aiProcess_Triangulate |
			aiProcess_ValidateDataStructure |
			aiProcess_JoinIdenticalVertices;

	public:
		using ProfileBase<std::shared_ptr<ModelData>>::ProfileBase;
		using ProfileBase<std::shared_ptr<ModelData>>::Save;

		virtual ~ModelProfile() noexcept = default;

		virtual bool Load() override;
		virtual bool Save() override;

		constexpr bool HasParserErrors() const noexcept
		{
			return false;
		}

	};

	struct D3DShaderData
	{
		friend class boost::serialization::access;

	public:
		enum Serialization : unsigned int
		{
			DataVersion1 = 1
		};

		stl::boost_vector<std::uint8_t> shaderData;

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& shaderData;
		}
	};

	struct D3DShaderDataHolder
	{
		friend class boost::serialization::access;

	public:
		enum Serialization : unsigned int
		{
			DataVersion1 = 1
		};

		D3DShaderData vertexColor;
		D3DShaderData vertexColorLighting;
		D3DShaderData pixelBasic;
		D3DShaderData pixelVertexLighting;

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& vertexColor;
			a_ar& vertexColorLighting;
			a_ar& pixelBasic;
			a_ar& pixelVertexLighting;
		}
	};

	class D3DShaderDataIO
	{
	public:
		static void Load(const char* a_path, D3DShaderDataHolder& a_out) noexcept(false);
		static void Save(const char* a_path, const D3DShaderDataHolder& a_in) noexcept(false);
	};

}

BOOST_CLASS_VERSION(
	IED::D3DShaderData,
	IED::D3DShaderData::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::D3DShaderDataHolder,
	IED::D3DShaderDataHolder::Serialization::DataVersion1);
