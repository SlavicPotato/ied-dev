#pragma once

namespace IED
{
	class ObjectDatabase;
	class QueuedModel;

	enum class ODBEntryLoadState : std::uint32_t
	{
		kPending = 0,
		kQueued  = 1,
		kLoading = 2,
		kError   = 3,
		kLoaded  = 4
	};

	struct ObjectDatabaseEntryData :
		stl::intrusive_ref_counted
	{
		SKMP_REDEFINE_NEW_PREF();

		ObjectDatabaseEntryData() = default;

		ObjectDatabaseEntryData(const ObjectDatabaseEntryData&)            = delete;
		ObjectDatabaseEntryData& operator=(const ObjectDatabaseEntryData&) = delete;

		explicit ObjectDatabaseEntryData(
			const NiPointer<NiNode>& a_object) :
			object(a_object)
		{
		}

		explicit ObjectDatabaseEntryData(
			NiPointer<NiNode>&& a_object) :
			object(std::move(a_object))
		{
		}

		[[nodiscard]] inline bool try_acquire_for_load() noexcept
		{
			auto expected = ODBEntryLoadState::kPending;
			if (loadState.compare_exchange_strong(expected, ODBEntryLoadState::kLoading))
			{
				return true;
			}

			return expected == ODBEntryLoadState::kQueued ?
			           loadState.compare_exchange_strong(expected, ODBEntryLoadState::kLoading) :
			           false;
		}

		[[nodiscard]] inline bool try_acquire_queued_for_load() noexcept
		{
			auto expected = ODBEntryLoadState::kQueued;
			return loadState.compare_exchange_strong(expected, ODBEntryLoadState::kLoading);
		}

		[[nodiscard]] inline bool try_acquire_for_queuing() noexcept
		{
			auto expected = ODBEntryLoadState::kPending;
			return loadState.compare_exchange_strong(expected, ODBEntryLoadState::kQueued);
		}

		NiPointer<NiNode>              object;
		RE::BSModelDB::ModelEntryAuto  holder;
		volatile long long             accessed{ 0 };
		std::atomic<ODBEntryLoadState> loadState{ ODBEntryLoadState::kPending };
	};

	using ObjectDatabaseEntry = stl::smart_ptr<ObjectDatabaseEntryData>;
}