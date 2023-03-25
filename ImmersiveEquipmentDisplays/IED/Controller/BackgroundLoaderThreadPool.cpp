#include "pch.h"

#include "BackgroundLoaderThreadPool.h"

#include "ObjectDatabase.h"

namespace IED
{
	BackgroundLoaderThreadPool::~BackgroundLoaderThreadPool()
	{
		std::for_each(
			m_workers.begin(),
			m_workers.end(),
			[&](auto&) {
				m_queue.push();
			});

		m_workers.clear();
	}

	void BackgroundLoaderThreadPool::Start(
		std::uint32_t a_numThreads)
	{
		if (!m_workers.empty())
		{
			return;
		}

		decltype(m_workers) tmp;

		for (std::uint32_t i = 0; i < a_numThreads; ++i)
		{
			tmp.emplace_front(*this);
		}

		for (auto& e : tmp)
		{
			ASSERT(e.StartThread());
		}

		//ASSERT(m_wts.WaitForThreads(a_numThreads, 5000));

		m_workers = std::move(tmp);
	}

	DWORD BackgroundLoaderThreadPool::Thread::Run()
	{
		//m_owner.m_wts.NotifyThreadStart();

		gLog.Debug(__FUNCTION__ " [%u] starting", threadID);

		for (;;)
		{
			const auto file = m_owner.m_queue.pop();

			if (!file)
			{
				break;
			}

			auto& entry = file->entry;

			if (entry->try_acquire_for_load())
			{
				auto obj = ObjectDatabase::LoadImpl(file->path.c_str());

				if (obj)
				{
					entry->object   = std::move(obj);
					entry->accessed = IPerfCounter::Query();

					entry->loadState.store(ODBEntryLoadState::kLoaded);

					//_DMESSAGE("loaded %s", tmp.path.c_str());
				}
				else
				{
					entry->loadState.store(ODBEntryLoadState::kError);
				}

				m_owner.m_owner.RequestCleanup();
			}
		}

		gLog.Debug(__FUNCTION__ " [%u] stopping", threadID);

		return 0;
	}

	bool BackgroundLoaderThreadPool::WaitThreadStartHelper::WaitForThreads(
		std::uint32_t a_expectedNum,
		long long     a_timeout)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		return m_cond.wait_for(
			lock,
			std::chrono::milliseconds(a_timeout),
			[&] { return m_count == a_expectedNum; });
	}

	void BackgroundLoaderThreadPool::WaitThreadStartHelper::NotifyThreadStart()
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_count++;
		}
		m_cond.notify_one();
	}

}