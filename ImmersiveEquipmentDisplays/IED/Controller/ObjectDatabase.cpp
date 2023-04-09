#include "pch.h"

#include "ObjectDatabase.h"

#include "QueuedModel.h"

#include <ext/BackgroundProcessThread.h>

namespace IED
{
	using namespace ::Util::Model;

	auto ObjectDatabase::GetModel(
		const char*          a_path,
		ObjectDatabaseEntry& a_outEntry,
		NiPointer<NiNode>*   a_cloneResult,
		float                a_colliderScale,
		bool                 a_forceImmediateLoad) noexcept
		-> ObjectLoadResult
	{
		char path_buffer[MAX_PATH];

		const auto path = MakePath("meshes", a_path, path_buffer);

		const auto r = m_data.try_emplace(path);

		auto& entry = r.first->second.entry;

		if (ODBGetBackgroundLoadingEnabled() && !a_forceImmediateLoad)
		{
			if (entry->try_acquire_for_queuing())
			{
				if (const auto thrd = RE::BackgroundProcessThread::GetSingleton())
				{
					thrd->QueueTask<QueuedModel>(entry, path, *this);
				}
				else
				{
					entry->loadState.store(ODBEntryLoadState::kError);
				}
			}
		}
		else
		{
			if (entry->try_acquire_for_load())
			{
				const bool result = LoadModel(path, entry);

				entry->loadState.store(
					result ?
						ODBEntryLoadState::kLoaded :
						ODBEntryLoadState::kError);

				QueueDatabaseCleanup();
			}
		}

		switch (entry->loadState.load())
		{
		case ODBEntryLoadState::kPending:
		case ODBEntryLoadState::kQueued:
		case ODBEntryLoadState::kLoading:

			a_outEntry = entry;

			return ObjectLoadResult::kPending;

		case ODBEntryLoadState::kLoaded:

			entry->accessed = IPerfCounter::Query();

			a_outEntry = entry;

			if (a_cloneResult)
			{
				*a_cloneResult = CreateClone(entry->object.get(), a_colliderScale);
			}

			return ObjectLoadResult::kSuccess;

		default:

			return ObjectLoadResult::kFailed;
		}
	}

	bool ObjectDatabase::ValidateObject(
		NiAVObject* a_object) noexcept
	{
		return !HasBSDismemberSkinInstance(a_object);
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

	void ObjectDatabase::PreODBCleanup() noexcept
	{
		bool e = true;
		if (m_wantCleanup.compare_exchange_strong(e, false))
		{
			QueueDatabaseCleanup();
		}
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
			for (auto it = m_data.begin(); it != m_data.end();)
			{
				if (it->second.entry.use_count() <= 1)
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

		const auto level = stl::underlying(m_level);

		if (m_data.size() <= level)
		{
			return;
		}

		m_data.sortvec([](auto& a_lhs, auto& a_rhs) noexcept {
			return a_lhs->second->accessed < a_rhs->second->accessed;
		});

		auto& vec = m_data.getvec();

		for (auto it = vec.begin(); it != vec.end();)
		{
			if ((*it)->second.entry.use_count() > 1)
			{
				++it;
			}
			else
			{
				it = m_data.erase(it);

				if (m_data.size() <= level)
				{
					break;
				}
			}
		}
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
			if (e.second.entry.use_count() <= 1)
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

	bool ObjectDatabase::LoadImpl(
		const char*        a_path,
		NiPointer<NiNode>& a_nodeOut)
	{
		NiPointer<NiAVObject> result;

		RE::BSModelDB::ModelLoadParams params(
			3,
			false,
			true);

		if (!ModelLoader::Load(a_path, params, result))
		{
			return false;
		}

		const auto node = result->AsNode();

		if (!node)
		{
			return false;
		}

		if (!ValidateObject(result))
		{
			return false;
		}

		a_nodeOut = node;

		return true;
	}

	bool ObjectDatabase::LoadImpl(
		const char*                    a_path,
		NiPointer<NiNode>&             a_nodeOut,
		RE::BSModelDB::ModelEntryAuto& a_entryOut)
	{
		RE::BSModelDB::ModelEntryAuto result;

		RE::BSModelDB::ModelLoadParams params(
			3,
			false,
			true);

		if (!ModelLoader::NativeLoad(a_path, params, result))
		{
			return false;
		}

		const auto node = result->object->AsNode();

		if (!node)
		{
			return false;
		}

		if (!ValidateObject(node))
		{
			return false;
		}

		a_entryOut = std::move(result);
		a_nodeOut  = node;

		return true;
	}

	NiNode* ObjectDatabase::CreateClone(
		NiNode* a_object,
		float   a_collisionObjectScale) noexcept
	{
		NiCloningProcess process(
			NiObjectNET::CopyType::COPY_EXACT,
			a_collisionObjectScale);

		auto result = a_object->CreateClone(process);
		a_object->ProcessClone(process);

		return static_cast<NiNode*>(result);
	}

	bool ObjectDatabase::LoadModel(
		const char*                a_path,
		const ObjectDatabaseEntry& a_entry)
	{
		bool result;

		if (ODBGetUseNativeModelDB())
		{
			result = LoadImpl(a_path, a_entry->object, a_entry->holder);
		}
		else
		{
			result = LoadImpl(a_path, a_entry->object);
		}

		return result;
	}
}