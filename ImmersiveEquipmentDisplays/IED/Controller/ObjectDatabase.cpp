#include "pch.h"

#include "ObjectDatabase.h"

namespace IED
{
	bool ObjectDatabase::GetUniqueObject(
		const char*          a_path,
		ObjectDatabaseEntry& a_outEntry,
		NiPointer<NiNode>&   a_outObject,
		float                a_colliderScale) noexcept
	{
		using namespace ::Util::Model;

		char        path_buffer[MAX_PATH];
		const char* path;

		if (!MakePath("meshes", a_path, path_buffer, path))
		{
			return false;
		}

		stl::fixed_string spath(path);

		auto it = m_data.find(spath);
		if (it == m_data.end())
		{
			NiPointer<NiNode> tmp;

			ModelLoader loader;
			if (!loader.LoadObject(path, tmp))
			{
				return false;
			}

			if (!ValidateObject(path, tmp))
			{
				return false;
			}

			it = m_data.emplace(
						   spath,
						   new entry_t(std::move(tmp)))
			         .first;

			QueueDatabaseCleanup();
		}

		it->second->accessed = IPerfCounter::Query();

		a_outEntry  = it->second;
		a_outObject = CreateClone(it->second->object.get(), a_colliderScale);

		return true;
	}

	bool ObjectDatabase::ValidateObject(
		const char* a_path,
		NiAVObject* a_object) noexcept
	{
		const bool result = HasBSDismemberSkinInstance(a_object);

		if (result)
		{
			Debug("[%s] meshes with BSDismemberSkinInstance objects are not supported", a_path);
		}

		return !result;
	}

	bool ObjectDatabase::HasBSDismemberSkinInstance(NiAVObject* a_object) noexcept
	{
		using namespace Util::Node;

		const auto r = TraverseGeometry(a_object, [](BSGeometry* a_geometry) noexcept {
			if (const auto* const skin = a_geometry->m_spSkinInstance.get())
			{
				if (::NRTTI<BSDismemberSkinInstance>::IsType(skin->GetRTTI()))
				{
					return VisitorControl::kStop;
				}
			}

			return VisitorControl::kContinue;
		});

		return r == VisitorControl::kStop;
	}

	void ObjectDatabase::RunObjectCleanup() noexcept
	{
		if (!m_cleanupDeadline)
		{
			return;
		}

		if (IPerfCounter::Query() < *m_cleanupDeadline)
		{
			return;
		}

		m_cleanupDeadline.reset();

		if (m_level == ObjectDatabaseLevel::kNone)
		{
			std::erase_if(
				m_data,
				[](auto& a_v) noexcept [[msvc::forceinline]] {
					return a_v.second.use_count() <= 1;
				});

			return;
		}

		const auto level = stl::underlying(m_level);

		if (m_data.size() <= level)
		{
			return;
		}

		std::size_t numCandidates = 0;

		for (auto& e : m_data)
		{
			if (e.second.use_count() <= 1)
			{
				numCandidates++;
			}
		}

		if (numCandidates <= level)
		{
			return;
		}

		m_scc.reserve(numCandidates);

		for (const auto& [i, e] : m_data)
		{
			if (e.use_count() <= 1)
			{
				m_scc.emplace_back(i, e->accessed);
			}
		}

		std::sort(
			m_scc.begin(),
			m_scc.end(),
			[](const auto& a_lhs,
		       const auto& a_rhs) noexcept [[msvc::forceinline]] {
				return a_lhs.second < a_rhs.second;
			});

		for (const auto& e : m_scc)
		{
			if (m_data.size() <= level)
			{
				break;
			}

			m_data.erase(e.first);
		}

		m_scc.clear();
	}

	void ObjectDatabase::QueueDatabaseCleanup() noexcept
	{
		if (!m_cleanupDeadline)
		{
			m_cleanupDeadline = IPerfCounter::get_tp(CLEANUP_DELAY);
		}
	}

	std::size_t ObjectDatabase::GetODBUnusedObjectCount() const noexcept
	{
		std::size_t total = 0;

		for (auto& e : m_data)
		{
			if (e.second.use_count() <= 1)
			{
				total++;
			}
		}

		return total;
	}

	void ObjectDatabase::ClearObjectDatabase()
	{
		m_data.clear();
		m_cleanupDeadline.reset();
	}

	NiNode* ObjectDatabase::CreateClone(NiNode* a_object, float a_collisionObjectScale) noexcept
	{
		NiCloningProcess process(NiObjectNET::CopyType::COPY_EXACT);

		process.SetCollisionObjectScaleUniform(a_collisionObjectScale);

		auto result = a_object->CreateClone(process);
		a_object->ProcessClone(process);

		return static_cast<NiNode*>(result);
	}

}