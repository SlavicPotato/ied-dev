#include "pch.h"

#include "QueuedModel.h"

#include "IObjectManager.h"

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
		_owner.RequestCleanup();  // must be called after entry decref
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

		ITaskPool::AddPriorityTask<PostRunTask>(this);

		return false;
	}

	QueuedModel::PostRunTask::PostRunTask(QueuedModel* a_task) :
		task(a_task)
	{
	}

	void QueuedModel::PostRunTask::Run()
	{
		task->_owner.OnAsyncModelLoad(task);
	}

	void QueuedModel::PostRunTask::Dispose()
	{
		delete this;
	}

}