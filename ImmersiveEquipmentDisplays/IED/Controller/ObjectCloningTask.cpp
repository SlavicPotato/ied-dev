#include "pch.h"

#include "ObjectCloningTask.h"

#include "Controller.h"
#include "IED/Util/Common.h"
#include "QueuedModel.h"

namespace IED
{
	ObjectCloningTask::ObjectCloningTask(
		IObjectManager&            a_owner,
		Game::FormID               a_actor,
		const ObjectDatabaseEntry& a_entry,
		TESModelTextureSwap*       a_textureSwap,
		float                      a_colliderScale,
		std::uint8_t               a_priority) :
		_owner(a_owner),
		_actor(a_actor),
		_entry(a_entry),
		_colliderScale(a_colliderScale),
		_texSwap(a_textureSwap)
	{
		SetPriority(a_priority);
	}

	ObjectCloningTask::~ObjectCloningTask()
	{
		_entry.reset();
		_owner.RequestCleanup();
	}

	void ObjectCloningTask::Unk_01()
	{
	}

	void ObjectCloningTask::Unk_02()
	{
	}

	bool ObjectCloningTask::Run()
	{
		if (try_acquire_for_processing())
		{
			if (_entry->loadState.load() == ODBEntryLoadState::kLoaded)
			{
				CloneAndApplyTexSwap(_entry, _texSwap, _colliderScale, _clone);
			}
			else
			{
				gLog.Error(__FUNCTION__ ": source object not loaded");
			}

			_taskState.store(State::kCompleted);

			ITaskPool::AddTask<PostRunTask>(this);
		}

		return false;
	}

	void ObjectCloningTask::CloneAndApplyTexSwap(
		const ObjectDatabaseEntry& a_dbentry,
		TESModelTextureSwap*       a_texSwap,
		float                      a_colliderScale,
		NiPointer<NiNode>&         a_out)
	{
		const auto result = ObjectDatabase::CreateClone(a_dbentry->object.get(), a_colliderScale);

		if (a_texSwap && a_texSwap->numAlternateTextures > 0)
		{
			IObjectManager::ApplyTextureSwap(a_texSwap, result);
		}

		a_out = result;
	}

	ObjectCloningTask::PostRunTask::PostRunTask(
		ObjectCloningTask* a_task) :
		task(a_task)
	{
	}

	void ObjectCloningTask::PostRunTask::Run()
	{
		if (task->try_acquire_for_use())
		{
			auto& om = task->_owner;

			const stl::lock_guard lock(om.GetLock());

			const auto& actorMap = om.GetActorMap();

			if (const auto it = actorMap.find(task->_actor);
			    it != actorMap.end())
			{
				const auto handle = it->second.GetHandle();

				if (const auto refr = handle.get_ptr())
				{
					const auto actor = refr->As<Actor>();

					if (Util::Common::IsREFRValid(actor) &&
					    it->second.GetActor().get() == actor)
					{
						const auto cell = actor->GetParentCell();
						if (cell && cell->IsAttached())
						{
							it->second.RequestEval();
						}
						else
						{
							auto& controller = static_cast<Controller&>(om);

							controller.EvaluateImpl(
								actor,
								handle,
								ControllerUpdateFlags::kPlayEquipSound |
									ControllerUpdateFlags::kImmediateTransformUpdate);
						}
					}
				}
			}
		}
	}

	void ObjectCloningTask::PostRunTask::Dispose()
	{
		delete this;
	}

}