#include "pch.h"

#include "D3DAssets.h"

namespace IED
{
	bool ModelProfile::Save()
	{
		m_lastExcept = "unsupported";

		return false;
	}

	bool ModelProfile::Load()
	{
		using namespace Serialization;

		try
		{
			if (m_path.empty())
			{
				throw parser_exception("bad path");
			}

			Assimp::Importer importer;

			importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, IMPORT_RVC_FLAGS);

			const auto scene = importer.ReadFile(m_pathStr, IMPORT_FLAGS);

			if (!scene || !scene->mRootNode)
			{
				throw parser_exception("no mesh was loaded");
			}

			if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			{
				throw parser_exception("incomplete scene");
			}

			if (!scene->mMeshes || scene->mNumMeshes < 1)
			{
				throw parser_exception("no data");
			}

			const auto* const mesh = scene->mMeshes[0];

			if (!mesh->HasPositions())
			{
				throw parser_exception("no vertices");
			}

			if (!mesh->HasFaces())
			{
				throw parser_exception("no faces");
			}

			const auto numVertices = mesh->mNumVertices;
			const auto numFaces    = mesh->mNumFaces;

			if (numVertices > std::numeric_limits<std::uint16_t>::max())
			{
				throw parser_exception("too many vertices (max. 2^16)");
			}

			auto tmp = std::make_shared<ModelData>();

			tmp->vertices.resize(numVertices);

			const bool hasNormals = mesh->HasNormals();
			const bool hasColors  = mesh->GetNumColorChannels() > 0 && mesh->HasVertexColors(0);

			tmp->m_hasVertexColors = hasColors;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				const auto& e             = mesh->mVertices[i];
				tmp->vertices[i].position = DirectX::XMFLOAT3(e.x, e.y, e.z);

				if (hasNormals)
				{
					const auto& f           = mesh->mNormals[i];
					tmp->vertices[i].normal = DirectX::XMFLOAT3(f.x, f.y, f.z);
				}
				else
				{
					tmp->vertices[i].normal = DirectX::XMFLOAT3(0, 0, 0);
				}

				if (hasColors)
				{
					const auto& f          = mesh->mColors[0][i];
					tmp->vertices[i].color = DirectX::XMFLOAT4(f.r, f.g, f.b, 1.0f);
				}
				else
				{
					tmp->vertices[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				}
			}

			std::uint32_t numIndices = 0;

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				const auto n = mesh->mFaces[i].mNumIndices;

				if (n != 3)
				{
					throw parser_exception("aiFace.mNumIndices != 3");
				}

				numIndices += n;
			}

			if (!numIndices)
			{
				throw parser_exception("no indices");
			}

			tmp->indices.reserve(numIndices);

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				const auto& e = mesh->mFaces[i];

				for (unsigned int j = 0; j < e.mNumIndices; j++)
				{
					tmp->indices.emplace_back(static_cast<std::uint16_t>(e.mIndices[j]));
				}
			}

			tmp->numVertices  = numVertices;
			tmp->numTriangles = numFaces;
			tmp->numIndices   = numIndices;

			if (const auto name = mesh->mName.C_Str())
			{
				SetDescription(name);
			}
			else
			{
				ClearDescription();
			}

			m_data     = std::move(tmp);
			m_modified = false;

			return true;
		}
		catch (const std::exception& e)
		{
			m_lastExcept = e;
			return false;
		}
	}

	D3DBoundingOrientedBox ModelData::CreateBoundingBox() const
	{
		D3DBoundingOrientedBox result;

		D3DBoundingOrientedBox::CreateFromPoints(
			result,
			vertices.size(),
			std::addressof(vertices.data()->position),
			sizeof(decltype(vertices)::value_type));

		return result;
	}

	D3DBoundingSphere ModelData::CreateBoundingSphere() const
	{
		D3DBoundingSphere result;

		D3DBoundingSphere::CreateFromPoints(
			result,
			vertices.size(),
			std::addressof(vertices.data()->position),
			sizeof(decltype(vertices)::value_type));

		return result;
	}

	void D3DShaderDataIO::Load(
		const char*          a_path,
		D3DShaderDataHolder& a_out) noexcept(false)
	{
		std::ifstream ifs;

		ifs.open(a_path, std::ifstream::in | std::ifstream::binary);

		if (!ifs || !ifs.is_open())
		{
			throw std::system_error(
				errno,
				std::system_category(),
				a_path);
		}

		boost::archive::binary_iarchive ia(ifs);

		ia >> a_out;
	}

	void D3DShaderDataIO::Save(
		const char*                a_path,
		const D3DShaderDataHolder& a_in) noexcept(false)
	{
		std::ofstream ofs;

		ofs.open(
			a_path,
			std::ofstream::out |
				std::ofstream::binary |
				std::ofstream::trunc,
			_SH_DENYWR);

		if (!ofs || !ofs.is_open())
		{
			throw std::system_error(
				errno,
				std::system_category(),
				a_path);
		}

		boost::archive::binary_oarchive oa(ofs);

		oa << a_in;
	}
}