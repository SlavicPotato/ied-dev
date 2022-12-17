#include "pch.h"

#include "ObjectEntryBase.h"

#include "IObjectManager.h"

#include "IED/StringHolder.h"

namespace IED
{
	bool ObjectEntryBase::reset(
		Game::ObjectRefHandle    a_handle,
		const NiPointer<NiNode>& a_root,
		const NiPointer<NiNode>& a_root1p,
		IObjectManager&          a_db,
		bool                     a_defer)
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
					ObjectEntryBase::ActiveData&& a_data,
					Game::ObjectRefHandle         a_handle,
					const NiPointer<NiNode>&      a_root,
					const NiPointer<NiNode>&      a_root1p,
					IObjectManager&               a_db) :
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

					boost::lock_guard lock(m_db.GetLock());

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
				IObjectManager&             m_db;
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
			assert(!EngineExtensions::ShouldDefer3DTask());

			data.Cleanup(a_handle, a_root, a_root1p, a_db);
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

	bool ObjectEntryBase::DeferredHideObject(std::uint8_t a_delay) const noexcept
	{
		if (!a_delay)
		{
			return false;
		}

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

	void ObjectEntryBase::State::UpdateArrows(std::int32_t a_count) noexcept
	{
		if (arrowState)
		{
			arrowState->Update(a_count);
		}
	}

	void ObjectEntryBase::State::Cleanup(
		Game::ObjectRefHandle a_handle)
	{
		nodes.physics.reset();

		const auto ts = IPerfCounter::Query();

		for (auto& e : groupObjects)
		{
			/*if (e.second.light)
			{
				EngineExtensions::CleanupLights(e.second.object.get());
				e.second.light.reset();
			}*/

			EngineExtensions::CleanupObjectImpl(
				a_handle,
				e.second.rootNode);

			if (auto& d = e.second.weapAnimGraphManagerHolder)
			{
				EngineExtensions::CleanupWeaponBehaviorGraph(d);
			}

			if (auto& d = e.second.dbEntry)
			{
				d->accessed = ts;
			}
		}

		arrowState.reset();
		nodes.object.reset();

		/*if (light)
		{
			EngineExtensions::CleanupLights(nodes.object.get());
			light.reset();
		}*/

		EngineExtensions::CleanupObjectImpl(
			a_handle,
			nodes.rootNode);

		if (auto& d = weapAnimGraphManagerHolder)
		{
			EngineExtensions::CleanupWeaponBehaviorGraph(d);
		}

		if (auto& d = dbEntry)
		{
			d->accessed = ts;
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

		if (const auto controller = nodes.object->GetControllers())
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

	void ObjectEntryBase::State::SetVisible(bool a_switch)
	{
		/*for (auto& e : groupObjects)
		{
			if (e.second.light)
			{
				e.second.light->SetVisible(a_switch && !flags.test(ObjectEntryFlags::kHideLight));
			}
		}

		if (light)
		{
			light->SetVisible(a_switch && !flags.test(ObjectEntryFlags::kHideLight));
		}*/

		nodes.rootNode->SetVisible(a_switch);
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
			state.reset();

			a_db.QueueDatabaseCleanup();
		}
	}

	namespace detail
	{
		inline auto make_arrow_array(NiNode* a_root) noexcept
		{
			const auto& arrowStrings = BSStringHolder::GetSingleton()->m_arrows;

			return stl::make_array<
				NiPointer<NiAVObject>,
				5>([&]<std::size_t I>() {
				return Util::Node::FindChildObject(a_root, arrowStrings[I]);
			});
		}
	}

	ObjectEntryBase::QuiverArrowState::QuiverArrowState(NiNode* a_arrowQuiver) :
		arrows{ detail::make_arrow_array(a_arrowQuiver) }
	{
	}

	void ObjectEntryBase::QuiverArrowState::Update(std::int32_t a_count) noexcept
	{
		a_count = std::min(a_count, 6);

		if (inventoryCount == a_count)
		{
			return;
		}

		inventoryCount = a_count;

		auto c = static_cast<std::int64_t>(a_count) - 1;

		for (auto& e : arrows)
		{
			if (e)
			{
				e->SetVisible(--c >= 0);
			}
		}
	}

}