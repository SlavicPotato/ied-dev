#include "pch.h"

#include "ObjectDatabase.h"

namespace IED
{
	bool ObjectDatabase::GetUniqueObject(
		const char*          a_path,
		ObjectDatabaseEntry& a_entry,
		NiPointer<NiNode>&   a_object)
	{
		using namespace ::Util::Model;

		char        path_buffer[MAX_PATH];
		const char* path;

		if (!MakePath("meshes", a_path, path_buffer, path))
		{
			return false;
		}

		if (m_level == ObjectDatabaseLevel::kDisabled)
		{
			ModelLoader loader;
			if (!loader.LoadObject(path, a_object))
			{
				return false;
			}

			if (!ValidateObject(path, a_object))
			{
				return false;
			}
		}
		else
		{
			stl::fixed_string spath(path);

			auto it = m_data.find(spath);
			if (it == m_data.end())
			{
				auto entry = std::make_shared<entry_t>();

				ModelLoader loader;
				if (!loader.LoadObject(path, entry->object))
				{
					return false;
				}

				if (!ValidateObject(path, entry->object))
				{
					return false;
				}

				it = m_data.emplace(std::move(spath), std::move(entry)).first;

				QueueDatabaseCleanup();
			}

			it->second->accessed = IPerfCounter::Query();

			a_entry  = it->second;
			a_object = CreateClone(*it->second);
		}

		return true;
	}

	bool ObjectDatabase::ValidateObject(
		const char* a_path,
		NiAVObject* a_object)
	{
		bool result = HasBSDismemberSkinInstance(a_object);

		if (result)
		{
			Debug("[%s] objects with BSDismemberSkinInstance are not supported", a_path);
		}

		return !result;
	}

	bool ObjectDatabase::HasBSDismemberSkinInstance(NiAVObject* a_object)
	{
		auto r = Util::Node::TraverseGeometry(a_object, [&](BSGeometry* a_geometry) {
			if (a_geometry->m_spSkinInstance)
			{
				if (ni_is_type(a_geometry->m_spSkinInstance->GetRTTI(), BSDismemberSkinInstance))
				{
					return Util::Node::VisitorControl::kStop;
				}
			}

			return Util::Node::VisitorControl::kContinue;
		});

		return r == Util::Node::VisitorControl::kStop;
	}

	void ObjectDatabase::RunObjectCleanup()
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

		if (m_level == ObjectDatabaseLevel::kDisabled)
		{
			return;
		}
		else if (m_level == ObjectDatabaseLevel::kNone)
		{
			for (auto it = m_data.begin(); it != m_data.end();)
			{
				if (it->second.use_count() <= 1)
				{
					it = m_data.erase(it);
				}
				else
				{
					++it;
				}
			}
			return;
		}

		auto level = stl::underlying(m_level);

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

		std::vector<container_type::const_iterator> candidates;

		for (auto it = m_data.begin(); it != m_data.end(); ++it)
		{
			if (it->second.use_count() <= 1)
			{
				candidates.emplace_back(it);
			}
		}

		std::sort(
			candidates.begin(),
			candidates.end(),
			[](const auto& a_lhs,
		       const auto& a_rhs) {
				return a_lhs->second->accessed <
			           a_rhs->second->accessed;
			});

		for (const auto& e : candidates)
		{
			if (m_data.size() <= level)
			{
				break;
			}

			// spec: only iterator to the erased element is invalidated, this should be safe
			m_data.erase(e);
		}
	}

	void ObjectDatabase::QueueDatabaseCleanup() noexcept
	{
		if (m_level != ObjectDatabaseLevel::kDisabled &&
		    !m_cleanupDeadline)
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

	NiNode* ObjectDatabase::CreateClone(const entry_t& a_entry)
	{
		NiCloningProcess process(NiObjectNET::CopyType::COPY_EXACT);

		auto result = a_entry.object->CreateClone(process);
		a_entry.object->ProcessClone(process);

		return static_cast<NiNode*>(result);
	}

}