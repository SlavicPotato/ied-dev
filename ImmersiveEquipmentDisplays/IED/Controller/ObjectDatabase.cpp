#include "pch.h"

#include "ObjectDatabase.h"

namespace IED
{

	auto ObjectDatabase::GetUniqueObject(
		const char*          a_path,
		ObjectDatabaseEntry& a_outEntry,
		NiPointer<NiNode>&   a_outObject,
		float                a_colliderScale) noexcept
		-> ObjectLoadResult
	{
		using namespace ::Util::Model;

		char        path_buffer[MAX_PATH];
		const char* path;

		if (!MakePath("meshes", a_path, path_buffer, path))
		{
			return ObjectLoadResult::kFailed;
		}

		stl::fixed_string spath(path);

		const auto r = m_data.try_emplace(spath);

		auto& entry = r.first->second;

		if (m_threadPool)
		{
			if (r.second)
			{
				entry.reset(new ObjectDatabaseEntryData);
				m_threadPool->Push(path, entry);
			}
		}
		else
		{
			if (r.second)
			{
				auto object = LoadImpl(path);

				if (object)
				{
					entry.reset(new ObjectDatabaseEntryData(std::move(object)));
					entry->loadState.store(ODBEntryLoadState::kLoaded);
				}
				else
				{
					entry->loadState.store(ODBEntryLoadState::kError);
				}

				QueueDatabaseCleanup();
			}
		}

		switch (entry->loadState.load())
		{
		case ODBEntryLoadState::kPending:

			a_outEntry = entry;

			return ObjectLoadResult::kPending;

		case ODBEntryLoadState::kLoaded:

			entry->accessed = IPerfCounter::Query();

			a_outEntry  = entry;
			a_outObject = CreateClone(entry->object.get(), a_colliderScale);

			return ObjectLoadResult::kSuccess;

		default:

			return ObjectLoadResult::kFailed;
		}
	}

	bool ObjectDatabase::ValidateObject(
		const char* a_path,
		NiAVObject* a_object) noexcept
	{
		const bool result = HasBSDismemberSkinInstance(a_object);

		if (result)
		{
			gLog.Debug("[%s] meshes with BSDismemberSkinInstance objects are not supported", a_path);
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

	void ObjectDatabase::StartObjectLoaderWorkerThreads(std::uint32_t a_numThreads)
	{
		if (!a_numThreads)
		{
			return;
		}

		Message("Spinning up %u object loader thread(s)..", a_numThreads);

		m_threadPool = std::make_unique_for_overwrite<BackgroundLoaderThreadPool>();
		m_threadPool->Start(*this, a_numThreads);
	}

	NiPointer<NiNode> ObjectDatabase::LoadImpl(const char* a_path)
	{
		NiPointer<NiNode> result;

		::Util::Model::ModelLoader loader;

		if (!loader.LoadObject(a_path, result))
		{
			return {};
		}

		if (!ValidateObject(a_path, result))
		{
			return {};
		}

		return result;
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