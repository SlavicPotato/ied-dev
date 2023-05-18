#include "pch.h"

#include "ObjectEntryBase.h"

#include "IObjectManager.h"

#include "IED/AnimationUpdateController.h"
#include "IED/EngineExtensions.h"
#include "IED/ReferenceLightController.h"
#include "IED/StringHolder.h"

namespace IED
{
	bool ObjectEntryBase::reset(
		Game::ObjectRefHandle    a_handle,
		const NiPointer<NiNode>& a_root,
		const NiPointer<NiNode>& a_root1p,
		IObjectManager&          a_db,
		bool                     a_defer,
		bool                     a_removeCloningTask) noexcept
	{
		if (!data)
		{
			return false;
		}

		const bool result = static_cast<bool>(data.state);

		if (a_defer)
		{
			struct DisposeStateTask :
				public TaskDelegate
			{
			public:
				DisposeStateTask(
					ObjectEntryBase::ObjectEntryData&& a_data,
					Game::ObjectRefHandle              a_handle,
					const NiPointer<NiNode>&           a_root,
					const NiPointer<NiNode>&           a_root1p,
					IObjectManager&                    a_db) noexcept :
					m_data(std::move(a_data)),
					m_handle(a_handle),
					m_root(a_root),
					m_root1p(a_root1p),
					m_db(a_db)
				{
				}

				virtual void Run() override
				{
					if (m_handle)
					{
						NiPointer<TESObjectREFR> ref;
						(void)m_handle.LookupZH(ref);
					}

					stl::lock_guard lock(m_db.GetLock());

					m_data.Cleanup(m_handle, m_root, m_root1p, m_db, true);
				}

				virtual void Dispose() override
				{
					delete this;
				}

			private:
				ObjectEntryBase::ObjectEntryData m_data;
				Game::ObjectRefHandle            m_handle;
				NiPointer<NiNode>                m_root;
				NiPointer<NiNode>                m_root1p;
				IObjectManager&                  m_db;
			};

			if (data.cloningTask)
			{
				data.cloningTask->try_cancel_task();
			}

			ITaskPool::AddPriorityTask<DisposeStateTask>(
				std::move(data),
				a_handle,
				a_root,
				a_root1p,
				a_db);
		}
		else
		{
			assert(!EngineExtensions::ShouldDefer3DTask());

			data.Cleanup(a_handle, a_root, a_root1p, a_db, a_removeCloningTask);
		}

		return result;
	}

	bool ObjectEntryBase::SetObjectVisible(bool a_switch) const noexcept
	{
		auto& state = data.state;

		if (!state)
		{
			return false;
		}

		a_switch = !a_switch;

		if (!state->hideCountdown &&
		    a_switch == state->flags.test(ObjectEntryFlags::kInvisible))
		{
			return false;
		}

		state->hideCountdown = 0;
		state->SetVisible(!a_switch);

		state->flags.set(ObjectEntryFlags::kInvisible, a_switch);

		return true;
	}

	bool ObjectEntryBase::DeferredHideObject(
		std::uint8_t a_delay) const noexcept
	{
		assert(a_delay != 0);

		auto& state = data.state;

		if (!state)
		{
			return false;
		}

		if (state->flags.test(ObjectEntryFlags::kInvisible))
		{
			return false;
		}

		if (state->hideCountdown == 0)
		{
			state->hideCountdown = a_delay;
		}

		state->flags.set(ObjectEntryFlags::kInvisible);

		/*if (a_unloadAfterHide)
		{
			state->flags.set(ObjectEntryFlags::kWantUnloadAfterHide);
		}*/

		return true;
	}

	void ObjectEntryBase::ResetDeferredHide() const noexcept
	{
		if (auto& state = data.state)
		{
			if (state->flags.test(ObjectEntryFlags::kInvisible) && state->hideCountdown != 0)
			{
				state->flags.clear(ObjectEntryFlags::kInvisible);
				state->hideCountdown = 0;
			}
		}
	}

	void ObjectEntryBase::State::UpdateArrows(std::int32_t a_count) noexcept
	{
		if (arrowState)
		{
			arrowState->Update(a_count);
		}
	}

	void ObjectEntryBase::State::Cleanup(
		Game::ObjectRefHandle a_handle) noexcept
	{
		for (auto& e : groupObjects)
		{
			e.second.CleanupObject(a_handle);
		}

		refNode.reset();
		physicsNode.reset();
		arrowState.reset();

		CleanupObject(a_handle);
	}

	void ObjectEntryBase::State::GroupObject::PlayAnimation(
		Actor*                   a_actor,
		const stl::fixed_string& a_sequence) noexcept
	{
		if (a_sequence.empty())
		{
			return;
		}

		if (!commonNodes.object)
		{
			return;
		}

		if (auto controller = commonNodes.object->GetControllers())
		{
			if (auto manager = controller->AsNiControllerManager())
			{
				if (auto nseq = manager->GetSequenceByName(a_sequence.c_str()))
				{
					a_actor->PlayAnimation(
						manager,
						nseq,
						nseq);
				}
			}
		}
	}

	void ObjectEntryBase::State::UpdateAndPlayAnimationSequence(
		Actor*                   a_actor,
		const stl::fixed_string& a_sequence) noexcept
	{
		if (a_sequence == currentSequence ||
		    a_sequence.empty())
		{
			return;
		}

		if (!commonNodes.object)
		{
			return;
		}

		if (const auto controller = commonNodes.object->GetControllers())
		{
			if (const auto manager = controller->AsNiControllerManager())
			{
				if (const auto nseq = manager->GetSequenceByName(a_sequence.c_str()))
				{
					const auto cseq = !currentSequence.empty() ?
					                      manager->GetSequenceByName(currentSequence.c_str()) :
					                      nullptr;

					a_actor->PlayAnimation(
						manager,
						nseq,
						cseq ? cseq : nseq);

					currentSequence = a_sequence;
				}
			}
		}
	}

	void ObjectEntryBase::State::SetLightsVisible(bool a_switch) noexcept
	{
		if (flags.test(ObjectEntryFlags::kHideLight))
		{
			a_switch = false;
		}

		for (auto& e : groupObjects)
		{
			if (auto& l = e.second.light)
			{
				l->niObject->SetVisible(a_switch);
			}
		}

		if (light)
		{
			light->niObject->SetVisible(a_switch);
		}
	}

	void ObjectEntryBase::State::SetVisible(bool a_switch) noexcept
	{
		SetLightsVisible(a_switch);
		commonNodes.rootNode->SetVisible(a_switch);
	}

	/*EventResult ObjectEntryBase::ObjectAnim::ReceiveEvent(
		const BSAnimationGraphEvent*                            a_event,
		[[maybe_unused]] BSTEventSource<BSAnimationGraphEvent>* a_sink)
	{
		if (a_event)
		{
			_DMESSAGE("%s -> %s", a_event->tag.c_str(), a_event->payload.c_str());
		}

		return EventResult::kContinue;
	}*/

	void ObjectEntryBase::ObjectAnim::UpdateAndSendAnimationEvent(
		const stl::fixed_string& a_event) noexcept
	{
		assert(holder != nullptr);

		if (a_event == currentEvent ||
		    a_event.empty())
		{
			return;
		}

		currentEvent = a_event;
		holder->NotifyAnimationGraph(a_event.c_str());
	}

	void ObjectEntryBase::ObjectAnim::Cleanup(Game::ObjectRefHandle a_handle)
	{
		if (holder && subGraphsAttached && a_handle)
		{
			if (auto ref = a_handle.get_ptr())
			{
				holder->DetachSubGraphs(*ref);
			}
		}

		AnimationUpdateController::CleanupWeaponBehaviorGraph(holder);
	}

	void ObjectEntryBase::ObjectEntryData::Cleanup(
		Game::ObjectRefHandle    a_handle,
		const NiPointer<NiNode>& a_root,
		const NiPointer<NiNode>& a_root1p,
		ObjectDatabase&          a_db,
		bool                     a_removeCloningTask) noexcept
	{
		if (a_removeCloningTask && cloningTask)
		{
			cloningTask->try_cancel_task();
			cloningTask.reset();
		}

		if (effectShaderData)
		{
			effectShaderData->ClearEffectShaderDataFromTree(a_root);
			effectShaderData->ClearEffectShaderDataFromTree(a_root1p);

			effectShaderData.reset();
		}

		if (state)
		{
			state->Cleanup(a_handle);
			state.reset();

			a_db.RequestCleanup();
		}
	}

	void ObjectEntryBase::ObjectEntryData::TryCancelAndReleaseCloningTask() noexcept
	{
		if (auto& ct = cloningTask)
		{
			ct->try_cancel_task();
			ct.reset();
		}
	}

	ObjectEntryBase::QuiverArrowState::QuiverArrowState(NiNode* a_arrowQuiver) noexcept
	{
		const auto& arrowStrings = BSStringHolder::GetSingleton()->m_arrows;

		for (auto& e : arrowStrings)
		{
			if (auto object = a_arrowQuiver->GetObjectByName(e))
			{
				arrows.emplace_back(object);
			}
		}
	}

	void ObjectEntryBase::QuiverArrowState::Update(std::int32_t a_count) noexcept
	{
		a_count = std::min(a_count, static_cast<std::int32_t>(BSStringHolder::NUM_DYN_ARROWS) + 1);

		if (inventoryCount == a_count)
		{
			return;
		}

		inventoryCount = a_count;

		auto c = static_cast<std::int64_t>(a_count) - 1;

		for (const auto& e : arrows)
		{
			e->SetVisible(c-- > 0);
		}
	}

	void ObjectEntryBase::Object::UnregisterFromControllers(Game::FormID a_owner) noexcept
	{
		if (light)
		{
			ReferenceLightController::GetSingleton().RemoveLight(a_owner, light->niObject);
		}

		if (anim.holder)
		{
			AnimationUpdateController::GetSingleton().RemoveObject(a_owner, anim.holder);

			/*RE::BSAnimationGraphManagerPtr agm;
			if (anim.holder->GetAnimationGraphManagerImpl(agm))
			{
				for (auto &e : agm->graphs)
				{
					if (e)
					{
						e->RemoveEventSink(std::addressof(anim));
					}
				}
			}*/
		}
	}

	void ObjectEntryBase::Object::CleanupObject(
		Game::ObjectRefHandle a_handle) noexcept
	{
		if (sound.handle.IsValid())
		{
			sound.handle.StopAndReleaseNow();
		}

		if (light)
		{
			ReferenceLightController::QueueRemoveAllLightsFromNode(commonNodes.rootNode.get());
			light.reset();
		}

		IObjectManager::CleanupObjectImpl(
			a_handle,
			commonNodes.rootNode.get());

		anim.Cleanup(a_handle);

		if (dbEntry)
		{
			dbEntry->accessed = IPerfCounter::Query();
		}
	}

}