#include "pch.h"

#include "QueuedModel.h"

#include "ObjectDatabase.h"

namespace IED
{
	QueuedModel::QueuedModel(
		const ObjectDatabaseEntry& a_entry,
		const char*                a_path,
		ObjectDatabase&            a_owner,
		std::uint8_t               a_priority) :
		_entry(a_entry),
		_path(a_path),
		_owner(a_owner)
	{
		SetPriority(a_priority);
	}

	void QueuedModel::Unk_01()
	{
	}

	void QueuedModel::Unk_02()
	{
	}

	bool QueuedModel::Run()
	{
		if (_entry->try_acquire_queued_for_load())
		{
			if (_owner.LoadModel(_path.c_str(), _entry))
			{
				_entry->accessed = IPerfCounter::Query();

				_entry->loadState.store(ODBEntryLoadState::kLoaded);

				//_DMESSAGE("%s", _path.c_str());
			}
			else
			{
				_entry->loadState.store(ODBEntryLoadState::kError);

				//_DMESSAGE("ERROR: %s", _path.c_str());
			}

			_owner.RequestCleanup();
		}

		return false;
	}

}