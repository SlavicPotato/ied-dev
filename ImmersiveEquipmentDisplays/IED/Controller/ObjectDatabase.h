#pragma once

#include "ObjectDatabaseEntry.h"
#include "ObjectDatabaseLevel.h"

#include <ext/BSThread.h>
#include <ext/stl_queue.h>

namespace IED
{
	class ObjectDatabase :
		public virtual ILog
	{
		static constexpr long long CLEANUP_RUN_DEADLINE_DELTA = 1000000;
		static constexpr long long CLEANUP_DELAY              = 2000000;

	private:
		struct Entry
		{
			constexpr auto operator->() const noexcept
			{
				return entry.get();
			}

			constexpr operator auto &() noexcept
			{
				return entry;
			}

			constexpr operator auto &() const noexcept
			{
				return entry;
			}

			ObjectDatabaseEntry entry{
				new ObjectDatabaseEntryData
			};
		};

		using container_type = stl::vectormap<stl::fixed_string, Entry>;

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
			NiPointer<NiNode>*   a_cloneResult        = nullptr,
			float                a_colliderScale      = 1.0f,
			bool                 a_forceImmediateLoad = false) noexcept;

		static bool ValidateObject(NiAVObject* a_object) noexcept;
		static bool HasBSDismemberSkinInstance(NiAVObject* a_object) noexcept;

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

		/*inline void ODBSetUseNativeModelDB(bool a_switch) noexcept
		{
			m_odbNativeLoader.store(a_switch, std::memory_order_relaxed);
		}*/

		inline void ODBEnableBackgroundLoading(bool a_switch) noexcept
		{
			m_odbBackgroundLoading.store(a_switch, std::memory_order_relaxed);
		}

		/*[[nodiscard]] inline bool ODBGetUseNativeModelDB() const noexcept
		{
			return m_odbNativeLoader.load(std::memory_order_relaxed);
		}*/

		[[nodiscard]] inline bool ODBGetBackgroundLoadingEnabled() const noexcept
		{
			return m_odbBackgroundLoading.load(std::memory_order_relaxed);
		}

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
		bool           LoadModel(const char* a_path, const ObjectDatabaseEntry& a_entry);
		static NiNode* CreateClone(NiNode* a_object, float a_collisionObjectScale) noexcept;

	private:
		void PreODBCleanup() noexcept;
		void ODBCleanupPass1() noexcept;
		void ODBCleanupPass2() noexcept;

		bool LoadImpl(const char* a_path, NiPointer<NiNode>& a_nodeOut);
		bool LoadImpl(const char* a_path, NiPointer<NiNode>& a_nodeOut, RE::BSModelDB::ModelEntryAuto& a_entryOut);

		ObjectDatabaseLevel      m_level{ DEFAULT_LEVEL };
		//std::atomic_bool         m_odbNativeLoader{ false };
		std::atomic_bool         m_odbBackgroundLoading{ true };
		std::atomic_bool         m_wantCleanup{ false };
		std::optional<long long> m_cleanupDeadline;
		container_type           m_data;
	};
}