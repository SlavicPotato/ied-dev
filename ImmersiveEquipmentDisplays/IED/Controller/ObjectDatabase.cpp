#include "pch.h"

#include "ObjectDatabase.h"

namespace IED
{
	bool ObjectDatabase::GetUniqueObject(
		const char* a_path,
		ObjectDatabaseEntry& a_entry,
		NiPointer<NiNode>& a_object)
	{
		using namespace ::Util::Model;

		char path_buffer[MAX_PATH];
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
		}
		else
		{
			stl::fixed_string spath(path);

			auto it = m_data.find(spath);
			if (it == m_data.end())
			{
				auto entry = std::make_shared<entry_t>();

				ModelLoader loader;
				if (!loader.LoadObject(spath.c_str(), entry->object))
				{
					return false;
				}

				it = m_data.emplace(std::move(spath), std::move(entry)).first;

				QueueDatabaseCleanup();
			}
			/*else
			{
				_DMESSAGE("db hit: %s", path);
			}*/

			it->second->accessed = IPerfCounter::Query();

			//a_entry.db = this;
			a_entry = it->second;
			a_object = CreateClone(*it->second);
		}

		return true;
	}

	void ObjectDatabase::RunObjectCleanup()
	{
		if (!m_runCleanup)
		{
			return;
		}

		m_runCleanup = false;

		if (m_level == ObjectDatabaseLevel::kDisabled)
		{
			return;
		}
		else if (m_level == ObjectDatabaseLevel::kNone)
		{
			for (auto it = m_data.begin(); it != m_data.end();)
			{
				if (it->second.use_count() < 2)
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

		if (m_data.size() <= stl::underlying(m_level))
		{
			return;
		}

		std::vector<container_type::const_iterator> candidates;

		for (auto it = m_data.begin(); it != m_data.end(); ++it)
		{
			if (it->second.use_count() < 2)
			{
				candidates.emplace_back(it);
			}
		}

		auto level = stl::underlying(m_level);

		if (candidates.size() <= level)
		{
			return;
		}

		std::sort(
			candidates.begin(),
			candidates.end(),
			[](const auto& a_lhs, const auto& a_rhs) {
				return a_lhs->second->accessed <
			           a_rhs->second->accessed;
			});

		auto num_erase = candidates.size() - level;

		for (const auto& e : candidates)
		{
			m_data.erase(e);

			if (!(--num_erase))
			{
				break;
			}
		}
	}

	std::size_t ObjectDatabase::GetODBUnusedObjectCount() const noexcept
	{
		std::size_t total = 0;
		for (auto& e : m_data)
		{
			if (e.second.use_count() < 2)
			{
				total++;
			}
		}
		return total;
	}

	void ObjectDatabase::ClearObjectDatabase()
	{
		m_data.clear();
		m_runCleanup = false;
	}

	NiNode* ObjectDatabase::CreateClone(const entry_t& a_entry)
	{
		NiCloningProcess process(NiObjectNET::CopyType::COPY_EXACT);

		auto result = a_entry.object->CreateClone(process);
		a_entry.object->ProcessClone(process);

		return static_cast<NiNode*>(result);
	}

}