#include "pch.h"

#include "D3DAssets.h"

namespace IED
{
	bool ModelProfile::SaveImpl(const std::shared_ptr<ModelData>& a_data, bool a_store)
	{
		m_lastExcept = "unsupported";

		return false;
	}

	bool ModelProfile::Load()
	{
		try
		{
			if (m_path.empty())
			{
				throw std::exception("bad path");
			}

			Assimp::Importer importer;

			importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, IMPORT_RVC_FLAGS);

			const auto scene = importer.ReadFile(m_pathStr, IMPORT_FLAGS);

			if (!scene || !scene->mRootNode)
			{
				throw std::exception("no mesh was loaded");
			}

			if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			{
				throw std::exception("incomplete scene");
			}

			if (!scene->mMeshes || scene->mNumMeshes < 1)
			{
				throw std::exception("no data");
			}

			const auto mesh = scene->mMeshes[0];

			if (!mesh->HasPositions())
			{
				throw std::exception("no vertices");
			}

			if (!mesh->HasFaces())
			{
				throw std::exception("no faces");
			}

			auto numVertices = mesh->mNumVertices;
			auto numFaces    = mesh->mNumFaces;

			if (numVertices > std::numeric_limits<std::uint16_t>::max())
			{
				throw std::exception("too many vertices (max. 2^16)");
			}

			auto tmp = std::make_unique<ModelData>();

			tmp->vertices.resize(numVertices);

			const bool hasNormals = mesh->HasNormals();
			const bool hasColors  = mesh->GetNumColorChannels() > 0 && mesh->HasVertexColors(0);

			tmp->m_hasVertexColors = hasColors;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				auto& e                   = mesh->mVertices[i];
				tmp->vertices[i].position = DirectX::XMFLOAT3(e.x, e.y, e.z);

				if (hasNormals)
				{
					auto& f                 = mesh->mNormals[i];
					tmp->vertices[i].normal = DirectX::XMFLOAT3(f.x, f.y, f.z);
				}
				else
				{
					tmp->vertices[i].normal = DirectX::XMFLOAT3(0, 0, 0);
				}

				/*if (hasColors)
				{
					auto& f                = mesh->mColors[0][i];
					tmp->vertices[i].color = DirectX::XMFLOAT4(f.r, f.g, f.b, f.a);
				}
				else
				{
					tmp->vertices[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				}*/

				tmp->vertices[i].color = DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
				tmp->m_hasVertexColors = true;
			}

			std::uint32_t numIndices = 0;

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				auto n = mesh->mFaces[i].mNumIndices;

				if (n != 3)
				{
					throw std::exception("aiFace.mNumIndices != 3");
				}

				numIndices += n;
			}

			if (!numIndices)
			{
				throw std::exception("no indices");
			}

			tmp->indices.reserve(numIndices);

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				auto& e = mesh->mFaces[i];

				for (unsigned int j = 0; j < e.mNumIndices; j++)
				{
					tmp->indices.emplace_back(static_cast<std::uint16_t>(e.mIndices[j]));
				}
			}

			tmp->numVertices  = numVertices;
			tmp->numTriangles = numFaces;
			tmp->numIndices   = numIndices;

			m_data = std::move(tmp);

			SetDescription(mesh->mName.C_Str());

			return true;
		}
		catch (const std::exception& e)
		{
			m_lastExcept = e;
			return false;
		}
	}

	DirectX::BoundingOrientedBox ModelData::CreateBoundingBox() const
	{
		DirectX::BoundingOrientedBox result;

		DirectX::BoundingOrientedBox::CreateFromPoints(
			result,
			vertices.size(),
			std::addressof(vertices.data()->position),
			sizeof(decltype(vertices)::value_type));

		return result;
	}
}