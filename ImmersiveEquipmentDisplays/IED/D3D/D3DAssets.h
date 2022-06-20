#pragma once

#include "IED/Profile/Manager.h"
#include "IED/Profile/Profile.h"

#include <DirectXTK/VertexTypes.h>

namespace IED
{
	struct ModelData
	{
		DirectX::BoundingOrientedBox CreateBoundingBox() const;

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
		virtual bool Save(const std::shared_ptr<ModelData>& a_data, bool a_store) override
		{
			return SaveImpl(a_data, a_store);
		}

		virtual bool Save(std::shared_ptr<ModelData>&& a_data, bool a_store) override
		{
			return SaveImpl(a_data, a_store);
		}

		inline constexpr bool HasParserErrors() const noexcept
		{
			return false;
		}

	private:
		bool SaveImpl(const std::shared_ptr<ModelData>& a_data, bool a_store);
	};

}