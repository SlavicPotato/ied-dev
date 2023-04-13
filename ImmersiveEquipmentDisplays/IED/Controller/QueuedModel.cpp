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

	QueuedModel::~QueuedModel()
	{
		_entry.reset();
		_owner.RequestCleanup(); // must be called after entry decref
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
			}
			else
			{
				_entry->loadState.store(ODBEntryLoadState::kError);
			}
		}

		return false;
	}

}