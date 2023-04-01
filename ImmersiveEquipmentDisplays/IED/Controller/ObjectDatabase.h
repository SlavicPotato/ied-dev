#pragma once

#include "BackgroundLoaderThreadPool.h"
#include "ObjectDatabaseEntry.h"
#include "ObjectDatabaseLevel.h"

#include <ext/BSThread.h>
#include <ext/stl_queue.h>

namespace IED
{
	class ObjectDatabase :
		public virtual ILog
	{
		static constexpr long long CLEANUP_DELAY = 1000000;

	private:
		using container_type = stl::vectormap<stl::fixed_string, ObjectDatabaseEntry>;

	public:
		static constexpr auto DEFAULT_LEVEL = ObjectDatabaseLevel::kNone;

		enum class ObjectLoadResult
		{
			kPending,
			kFailed,
			kSuccess
		};

		[[nodiscard]] ObjectLoadResult GetModel(
			const char*          a_path,
			ObjectDatabaseEntry& a_outEntry,
			NiPointer<NiNode>*   a_cloneResult = nullptr,
			float                a_colliderScale      = 1.0f,
			bool                 a_forceImmediateLoad = false) noexcept;

		static bool ValidateObject(const char* a_path, NiAVObject* a_object) noexcept;
		static bool HasBSDismemberSkinInstance(NiAVObject* a_object) noexcept;

		void PreODBCleanup() noexcept;
		void RunObjectCleanup() noexcept;
		void QueueDatabaseCleanup() noexcept;

		[[nodiscard]] constexpr auto GetODBLevel() const noexcept
		{
			return m_level;
		}

		[[nodiscard]] inline auto GetODBObjectCount() const noexcept
		{
			return m_data.size();
		}

		[[nodiscard]] std::size_t GetODBUnusedObjectCount() const noexcept;

		void ClearObjectDatabase();

		void StartObjectLoaderWorkerThreads(std::uint32_t a_numThreads);

		inline void RequestCleanup() noexcept
		{
			bool expected = false;
			m_wantCleanup.compare_exchange_strong(expected, true);
		}

		FN_NAMEPROC("ObjectDatabase");

	protected:
		constexpr void SetODBLevel(ObjectDatabaseLevel a_level) noexcept
		{
			if (a_level == ObjectDatabaseLevel::kDisabled)
			{
				m_level = ObjectDatabaseLevel::kNone;
			}
			else
			{
				m_level = std::clamp(
					a_level,
					ObjectDatabaseLevel::kNone,
					ObjectDatabaseLevel::kMax);
			}
		}

	public:
		static NiPointer<NiNode> LoadImpl(const char* a_path);
		static NiNode*           CreateClone(NiNode* a_object, float a_collisionObjectScale) noexcept;

	private:
		ObjectDatabaseLevel      m_level{ DEFAULT_LEVEL };
		std::optional<long long> m_cleanupDeadline;
		container_type           m_data;

		std::unique_ptr<BackgroundLoaderThreadPool> m_threadPool;
		std::atomic_bool                            m_wantCleanup{ false };

		//stl::vector<std::pair<stl::fixed_string, long long>> m_scc;
	};
}