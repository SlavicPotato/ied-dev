#include "pch.h"

#include "ObjectEntryBase.h"

#include "IED/EngineExtensions.h"

namespace IED
{
	bool ObjectEntryBase::reset(
		Game::ObjectRefHandle    a_handle,
		const NiPointer<NiNode>& a_root,
		const NiPointer<NiNode>& a_root1p)
	{
		bool result;

		if (state)
		{
			for (auto& e : state->dbEntries)
			{
				e->accessed = IPerfCounter::Query();
			}

			result = true;
		}
		else
		{
			result = false;
		}

		if (EngineExtensions::SceneRendering() ||
		    !ITaskPool::IsRunningOnCurrentThread())
		{
			if (state || effectShaderData)
			{
				struct DisposeStateTask :
					public TaskDelegate
				{
				public:
					DisposeStateTask(
						std::unique_ptr<State>&&            a_state,
						std::unique_ptr<EffectShaderData>&& a_effectShaderData,
						const Game::ObjectRefHandle         a_handle,
						const NiPointer<NiNode>&            a_root,
						const NiPointer<NiNode>&            a_root1p) :
						m_state(std::move(a_state)),
						m_effectShaderData(std::move(a_effectShaderData)),
						m_handle(a_handle),
						m_root(a_root),
						m_root1p(a_root1p)
					{
					}

					virtual void Run() override
					{
						if (m_effectShaderData)
						{
							m_effectShaderData->ClearEffectShaderDataFromTree(m_root);
							m_effectShaderData->ClearEffectShaderDataFromTree(m_root1p);

							m_effectShaderData.reset();
						}

						if (m_state)
						{
							m_state->Cleanup(m_handle);

							m_state.reset();
						}
					}

					virtual void Dispose() override
					{
						delete this;
					}

				private:
					std::unique_ptr<State>            m_state;
					std::unique_ptr<EffectShaderData> m_effectShaderData;
					const Game::ObjectRefHandle       m_handle;
					NiPointer<NiNode>                 m_root;
					NiPointer<NiNode>                 m_root1p;
				};

				ITaskPool::AddPriorityTask<DisposeStateTask>(
					std::move(state),
					std::move(effectShaderData),
					a_handle,
					a_root,
					a_root1p);
			}
		}
		else
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

				state.reset();
			}
		}

		return result;
	}

	bool ObjectEntryBase::SetNodeVisible(bool a_switch) const noexcept
	{
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
		state->nodes.rootNode->SetHidden(a_switch);

		state->flags.set(ObjectEntryFlags::kInvisible, a_switch);

		return true;
	}

	bool ObjectEntryBase::DeferredHideNode(std::uint8_t a_delay) const noexcept
	{
		if (!a_delay)
		{
			return false;
		}

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

		return true;
	}

	void ObjectEntryBase::ResetDeferredHide() const noexcept
	{
		if (state)
		{
			if (state->flags.test(ObjectEntryFlags::kInvisible) && state->hideCountdown != 0)
			{
				state->flags.clear(ObjectEntryFlags::kInvisible);
				state->hideCountdown = 0;
			}
		}
	}

	void ObjectEntryBase::State::UpdateAnimationGraphs(
		const BSAnimationUpdateData& a_data)
	{
		for (auto& e : groupObjects)
		{
			if (e.second.weapAnimGraphManagerHolder)
			{
				EngineExtensions::UpdateAnimationGraph(
					e.second.weapAnimGraphManagerHolder.get(),
					a_data);
			}
		}

		if (weapAnimGraphManagerHolder)
		{
			EngineExtensions::UpdateAnimationGraph(
				weapAnimGraphManagerHolder.get(),
				a_data);
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

}