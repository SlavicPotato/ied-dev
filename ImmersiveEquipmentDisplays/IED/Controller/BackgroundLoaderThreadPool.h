#pragma once

#include <ext/BSThread.h>
#include <ext/stl_queue.h>

#include "ObjectDatabaseEntry.h"

namespace IED
{
	class ObjectDatabase;

	class BackgroundLoaderThreadPool
	{
	public:
		struct QueuedFile
		{
			std::string         path;
			ObjectDatabaseEntry entry;
		};

		using queue_type = stl::producer_consumer_queue<QueuedFile>;

		class Thread :
			public RE::BSThread
		{
		public:
			Thread(
				ObjectDatabase& a_db,
				queue_type&     a_queue) :
				RE::BSThread(),
				m_db(a_db),
				m_queue(a_queue)
			{
			}

		private:
			DWORD RunThread() override;

			ObjectDatabase& m_db;
			queue_type&     m_queue;
		};

	public:
		~BackgroundLoaderThreadPool();

		void Start(
			ObjectDatabase& a_db,
			std::uint32_t   a_numThreads = 1);

		template <class... Args>
		void Push(Args&&... a_args)
		{
			m_queue.push(std::forward<Args>(a_args)...);
		}

		inline bool IsEnabled() const noexcept
		{
			return !m_workers.empty();
		}

	private:
		stl::forward_list<Thread> m_workers;
		queue_type                m_queue;
	};
}