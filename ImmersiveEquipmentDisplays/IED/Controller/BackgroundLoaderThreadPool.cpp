#include "pch.h"

#include "BackgroundLoaderThreadPool.h"

#include "ObjectDatabase.h"

namespace IED
{
	BackgroundLoaderThreadPool::~BackgroundLoaderThreadPool()
	{
		std::for_each(m_workers.begin(), m_workers.end(), [&](auto&) {
			m_queue.push();
		});

		m_workers.clear();
	}

	void BackgroundLoaderThreadPool::Start(
		ObjectDatabase& a_db,
		std::uint32_t   a_numThreads)
	{
		if (!m_workers.empty())
		{
			return;
		}

		for (std::uint32_t i = 0; i < a_numThreads; ++i)
		{
			auto& thread = m_workers.emplace_front(a_db, m_queue);
			thread.StartThread();
		}
	}

	DWORD BackgroundLoaderThreadPool::Thread::RunThread()
	{
		//_DMESSAGE("%u: starting", GetCurrentThreadId());

		for (;;)
		{
			QueuedFile queued;

			m_queue.pop(queued);

			if (!queued.entry)
			{
				break;
			}

			auto expected = ODBEntryLoadState::kPending;

			auto obj = ObjectDatabase::LoadImpl(queued.path.c_str());

			if (obj)
			{
				queued.entry->object   = std::move(obj);
				queued.entry->accessed = IPerfCounter::Query();

				ASSERT(queued.entry->loadState.compare_exchange_strong(expected, ODBEntryLoadState::kLoaded));

				//_DMESSAGE("loaded %s", tmp.path.c_str());
			}
			else
			{
				ASSERT(queued.entry->loadState.compare_exchange_strong(expected, ODBEntryLoadState::kError));
			}

			m_db.RequestCleanup();
		}

		//_DMESSAGE("%u: stopping", GetCurrentThreadId());

		return 0;
	}

}