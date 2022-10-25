#include "pch.h"

#include "ObjectEntryBase.h"

namespace IED
{
	bool ObjectEntryBase::reset(
		Game::ObjectRefHandle    a_handle,
		const NiPointer<NiNode>& a_root,
		const NiPointer<NiNode>& a_root1p,
		ObjectDatabase&          a_db)
	{
		if (!data)
		{
			return false;
		}

		const bool result = static_cast<bool>(data.state);

		if (EngineExtensions::ShouldDefer3DTask())
		{
			struct DisposeStateTask :
				public TaskDelegate
			{
			public:
				DisposeStateTask(
					ObjectEntryBase::ActiveData&& a_data,
					Game::ObjectRefHandle         a_handle,
					const NiPointer<NiNode>&      a_root,
					const NiPointer<NiNode>&      a_root1p,
					ObjectDatabase&               a_db) :
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

					m_data.Cleanup(m_handle, m_root, m_root1p, m_db);
				}

				virtual void Dispose() override
				{
					delete this;
				}

			private:
				ObjectEntryBase::ActiveData m_data;
				Game::ObjectRefHandle       m_handle;
				NiPointer<NiNode>           m_root;
				NiPointer<NiNode>           m_root1p;
				ObjectDatabase&             m_db;
			};

			ITaskPool::AddPriorityTask<DisposeStateTask>(
				std::move(data),
				a_handle,
				a_root,
				a_root1p,
				a_db);
		}
		else
		{
			data.Cleanup(a_handle, a_root, a_root1p, a_db);
		}

		return result;
	}

	bool ObjectEntryBase::SetNodeVisible(bool a_switch) const noexcept
	{
		if (!data.state)
		{
			return false;
		}

		a_switch = !a_switch;

		if (!data.state->hideCountdown &&
		    a_switch == data.state->flags.test(ObjectEntryFlags::kInvisible))
		{
			return false;
		}

		data.state->hideCountdown = 0;
		data.state->nodes.rootNode->SetHidden(a_switch);

		data.state->flags.set(ObjectEntryFlags::kInvisible, a_switch);

		return true;
	}

	bool ObjectEntryBase::DeferredHideNode(std::uint8_t a_delay) const noexcept
	{
		if (!a_delay)
		{
			return false;
		}

		if (!data.state)
		{
			return false;
		}

		if (data.state->flags.test(ObjectEntryFlags::kInvisible))
		{
			return false;
		}

		if (data.state->hideCountdown == 0)
		{
			data.state->hideCountdown = a_delay;
		}

		data.state->flags.set(ObjectEntryFlags::kInvisible);

		return true;
	}

	void ObjectEntryBase::ResetDeferredHide() const noexcept
	{
		if (data.state)
		{
			if (data.state->flags.test(ObjectEntryFlags::kInvisible) && data.state->hideCountdown != 0)
			{
				data.state->flags.clear(ObjectEntryFlags::kInvisible);
				data.state->hideCountdown = 0;
			}
		}
	}

	void ObjectEntryBase::State::Cleanup(
		Game::ObjectRefHandle a_handle)
	{
		for (auto& e : groupObjects)
		{
			EngineExtensions::CleanupObjectImpl(
				a_handle,
				e.second.rootNode);

			if (e.second.weapAnimGraphManagerHolder)
			{
				EngineExtensions::CleanupWeaponBehaviorGraph(
					e.second.weapAnimGraphManagerHolder);
			}
		}

		EngineExtensions::CleanupObjectImpl(
			a_handle,
			nodes.rootNode);

		if (weapAnimGraphManagerHolder)
		{
			EngineExtensions::CleanupWeaponBehaviorGraph(
				weapAnimGraphManagerHolder);
		}
	}

	void ObjectEntryBase::State::GroupObject::PlayAnimation(
		Actor*                   a_actor,
		const stl::fixed_string& a_sequence)
	{
		if (a_sequence.empty())
		{
			return;
		}

		if (!object)
		{
			return;
		}

		if (auto controller = object->GetControllers())
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

	void ObjectEntryBase::State::UpdateAndPlayAnimation(
		Actor*                   a_actor,
		const stl::fixed_string& a_sequence)
	{
		if (a_sequence.empty())
		{
			return;
		}

		if (a_sequence == currentSequence)
		{
			return;
		}

		if (!nodes.object)
		{
			return;
		}

		if (auto controller = nodes.object->GetControllers())
		{
			if (auto manager = controller->AsNiControllerManager())
			{
				if (auto nseq = manager->GetSequenceByName(a_sequence.c_str()))
				{
					auto cseq = !currentSequence.empty() ?
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

	void ObjectEntryBase::AnimationState::UpdateAndSendAnimationEvent(
		const stl::fixed_string& a_event)
	{
		assert(weapAnimGraphManagerHolder != nullptr);

		if (a_event.empty())
		{
			return;
		}

		if (a_event != currentAnimationEvent)
		{
			currentAnimationEvent = a_event;
			weapAnimGraphManagerHolder->NotifyAnimationGraph(a_event.c_str());
		}
	}

	void ObjectEntryBase::ActiveData::Cleanup(
		Game::ObjectRefHandle    a_handle,
		const NiPointer<NiNode>& a_root,
		const NiPointer<NiNode>& a_root1p,
		ObjectDatabase&          a_db)
	{
		if (effectShaderData)
		{
			effectShaderData->ClearEffectShaderDataFromTree(a_root);
			effectShaderData->ClearEffectShaderDataFromTree(a_root1p);

			effectShaderData.reset();
		}

		if (state)
		{
			state->Cleanup(a_handle);

			if (!state->dbEntries.empty())
			{
				const auto ts = IPerfCounter::Query();

				for (auto& e : state->dbEntries)
				{
					e->accessed = ts;
				}

				a_db.QueueDatabaseCleanup();
			}

			state.reset();
		}
	}

}