#pragma once

#include <ext/BSThread.h>
#include <ext/stl_queue.h>

#include "ObjectDatabaseEntry.h"

namespace IED
{
	class ObjectDatabase;

	class BackgroundLoaderThreadPool
	{
	private:
		struct QueuedFile
		{
			std::string         path;
			ObjectDatabaseEntry entry;
		};

		using queue_type = stl::producer_consumer_queue<std::unique_ptr<QueuedFile>>;

		class Thread :
			public RE::BSThread
		{
		public:
			inline Thread(
				BackgroundLoaderThreadPool& a_owner) :
				RE::BSThread(),
				m_owner(a_owner)
			{
			}

			Thread(const Thread&)            = delete;
			Thread& operator=(const Thread&) = delete;

		private:
			DWORD Run() override;

			BackgroundLoaderThreadPool& m_owner;
		};

		class WaitThreadStartHelper
		{
		public:
			bool WaitForThreads(
				std::uint32_t a_expectedNum,
				long long     a_timeout);

			void NotifyThreadStart();

		private:
			std::mutex              m_mutex;
			std::condition_variable m_cond;
			std::uint32_t           m_count{ 0 };
		};

	public:
		inline BackgroundLoaderThreadPool(
			ObjectDatabase& a_owner) :
			m_owner(a_owner)
		{
		}

		~BackgroundLoaderThreadPool();

		void Start(std::uint32_t a_numThreads);

		template <class... Args>
		void Push(Args&&... a_args)
		{
			m_queue.push(std::make_unique<QueuedFile>(std::forward<Args>(a_args)...));
		}

		[[nodiscard]] inline bool IsEnabled() const noexcept
		{
			return !m_workers.empty();
		}

	private:
		stl::forward_list<Thread> m_workers;
		queue_type                m_queue;
		ObjectDatabase&           m_owner;

		//WaitThreadStartHelper m_wts;
	};
}