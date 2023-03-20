#pragma once

namespace IED
{
	class ObjectDatabase;

	enum class ODBEntryLoadState
	{
		kPending = 0,
		kError   = 1,
		kLoaded  = 2
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

		volatile long long             accessed{ 0 };
		std::atomic<ODBEntryLoadState> loadState{ ODBEntryLoadState::kPending };
		NiPointer<NiNode>              object;
	};

	using ObjectDatabaseEntry = stl::smart_ptr<ObjectDatabaseEntryData>;
}