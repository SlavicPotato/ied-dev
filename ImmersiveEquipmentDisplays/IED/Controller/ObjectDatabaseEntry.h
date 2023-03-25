#pragma once

namespace IED
{
	class ObjectDatabase;

	enum class ODBEntryLoadState
	{
		kPending    = 0,
		kProcessing = 1,
		kError      = 2,
		kLoaded     = 3
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
			return loadState.compare_exchange_strong(expected, ODBEntryLoadState::kProcessing);
		}

		NiPointer<NiNode>              object;
		volatile long long             accessed{ 0 };
		std::atomic<ODBEntryLoadState> loadState{ ODBEntryLoadState::kPending };
	};

	using ObjectDatabaseEntry = stl::smart_ptr<ObjectDatabaseEntryData>;
}