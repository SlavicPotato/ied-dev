#include "pch.h"

#include "ActorProcessorTask.h"

#include "Controller.h"
#include "IConditionalVariableProcessor.h"
#include "IObjectManager.h"

#include "IED/EngineExtensions.h"
#include "IED/Inventory.h"
#include "IED/StringHolder.h"
#include "IED/Util/Common.h"

#include <ext/BSAnimationUpdateData.h>
#include <ext/Sky.h>

namespace IED
{
	ActorProcessorTask::ActorProcessorTask(
		Controller& a_controller) :
		m_state{ IPerfCounter::Query() - COMMON_STATE_CHECK_INTERVAL },
		m_controller(a_controller)
	{
	}

	void ActorProcessorTask::UpdateNode(
		ActorObjectHolder& a_record,
		ObjectEntryBase&   a_entry)
	{
		auto state = a_entry.data.state.get();

		const auto& nodes = state->nodes;

		if (!nodes.ref)
		{
			return;
		}

		if (nodes.IsReferenceMovedOrOphaned())
		{
			if (state->flags.test(ObjectEntryFlags::kRefSyncDisableFailedOrphan))
			{
				return;
			}

			bool result = false;

			if (auto info = m_controller.LookupCachedActorInfo(a_record))
			{
				result = m_controller.AttachNodeImpl(
					info->npcRoot,
					state->nodeDesc,
					state->atmReference,
					a_entry);

				if (result)
				{
					INode::UpdateRootIfGamePaused(info->root);
					a_record.RequestEvalDefer();
				}
			}

			if (!result)
			{
				state->flags.set(ObjectEntryFlags::kRefSyncDisableFailedOrphan);
				return;
			}

			if (!nodes.ref)
			{
				return;
			}
		}

		if (state->flags.test(ObjectEntryFlags::kSyncReferenceTransform) &&
		    nodes.rootNode->IsVisible())
		{
			if (state->transform.scale)
			{
				nodes.rootNode->m_localTransform.scale =
					nodes.ref->m_localTransform.scale * *state->transform.scale;
			}
			else
			{
				nodes.rootNode->m_localTransform.scale = nodes.ref->m_localTransform.scale;
			}

			if (state->transform.rotation)
			{
				nodes.rootNode->m_localTransform.rot =
					nodes.ref->m_localTransform.rot * *state->transform.rotation;
			}
			else
			{
				nodes.rootNode->m_localTransform.rot = nodes.ref->m_localTransform.rot;
			}

			if (state->transform.position)
			{
				nodes.rootNode->m_localTransform.pos =
					nodes.ref->m_localTransform * *state->transform.position;
			}
			else
			{
				nodes.rootNode->m_localTransform.pos = nodes.ref->m_localTransform.pos;
			}
		}
	}

	void ActorProcessorTask::ProcessTransformUpdateRequest(
		ActorObjectHolder& a_data)
	{
		if (!a_data.m_flags.test(ActorObjectHolderFlags::kWantTransformUpdate))
		{
			return;
		}

		if (!a_data.m_flags.test(ActorObjectHolderFlags::kImmediateTransformUpdate) &&
		    a_data.m_flags.test(ActorObjectHolderFlags::kSkipNextTransformUpdate))
		{
			a_data.m_flags.clear(ActorObjectHolderFlags::kSkipNextTransformUpdate);
		}
		else
		{
			a_data.m_flags.clear(ActorObjectHolderFlags::kRequestTransformUpdateMask);

			m_controller.EvaluateTransformsImpl(
				a_data,
				ControllerUpdateFlags::kUseCachedParams);
		}
	}

	void ActorProcessorTask::ProcessEvalRequest(ActorObjectHolder& a_data)
	{
		if (a_data.m_flags.consume(ActorObjectHolderFlags::kEvalThisFrame))
		{
			if (auto& params = a_data.GetCurrentProcessParams())
			{
				m_controller.EvaluateImpl(
					*params,
					a_data,
					ControllerUpdateFlags::kPlaySound |
						ControllerUpdateFlags::kFromProcessorTask |
						ControllerUpdateFlags::kUseCachedParams);
			}
			else
			{
				m_controller.EvaluateImpl(
					a_data,
					ControllerUpdateFlags::kPlaySound |
						ControllerUpdateFlags::kFromProcessorTask |
						ControllerUpdateFlags::kUseCachedParams);
			}
		}
	}

	constexpr bool ActorProcessorTask::CheckMonitorNodes(ActorObjectHolder& a_data) noexcept
	{
		bool result = false;

		for (auto& f : a_data.m_monitorNodes)
		{
			if (f.parent != f.node->m_parent)
			{
				f.parent = f.node->m_parent;

				result = true;
			}

			if (f.size != f.node->m_children.m_size)
			{
				f.size = f.node->m_children.m_size;

				result = true;
			}

			if (bool visible = f.node->IsVisible();
			    visible != f.visible)
			{
				f.visible = visible;

				result = true;
			}
		}

		return result;
	}

	void ActorProcessorTask::UpdateState()
	{
		if (const auto lrhandle = (*g_thePlayer)->lastRiddenHorseHandle;
		    lrhandle != m_state.playerLastRidden)
		{
			m_state.playerLastRidden = lrhandle;

			m_controller.RequestLFEvaluateAll();
		}

		if (const auto fpstate = IsInFirstPerson();
		    fpstate != m_state.inFirstPerson)
		{
			m_state.inFirstPerson = fpstate;

			if (auto it = m_controller.m_objects.find(Data::IData::GetPlayerRefID());
			    it != m_controller.m_objects.end())
			{
				it->second.RequestEval();
			}
		}

		if (IPerfCounter::delta_us(
				m_state.lastRun,
				m_timer.GetStartTime()) < COMMON_STATE_CHECK_INTERVAL)
		{
			return;
		}

		m_state.lastRun = m_timer.GetStartTime();

		bool changed = false;

		auto sky = RE::Sky::GetSingleton();
		assert(sky);

		if (const auto current = (sky ? sky->currentWeather : nullptr);
		    current != m_state.currentWeather)
		{
			m_state.currentWeather = current;
			changed                = true;
		}

		if (const auto tod = Data::GetTimeOfDay(sky);
		    tod != m_state.timeOfDay)
		{
			m_state.timeOfDay = tod;
			changed           = true;
		}

#if defined(IED_ENABLE_CONDITION_EN)

		auto player = *g_thePlayer;
		assert(player);

		if (player->loadedState)
		{
			auto pl = Game::ProcessLists::GetSingleton();
			assert(pl);

			if (bool n = pl->PlayerHasEnemiesNearby(0);
			    n != m_state.playerEnemiesNearby)
			{
				m_state.playerEnemiesNearby = n;

				if (auto it = m_controller.m_objects.find(Data::IData::GetPlayerRefID());
				    it != m_controller.m_objects.end())
				{
					it->second.RequestEval();
				}
			}
		}

#endif

		if (changed)
		{
			m_controller.RequestLFEvaluateAll();
		}
	}

	static void UpdateActorGearAnimations(
		TESObjectREFR*           a_actor,
		const ActorObjectHolder& a_holder,
		float                    a_step)
	{
		struct TLSData
		{
			std::uint8_t  unk000[0x768];  // 000
			std::uint32_t unk768;         // 768
		};

		auto tlsData = reinterpret_cast<TLSData**>(__readgsqword(0x58));

		auto& tlsUnk768 = tlsData[*g_TlsIndexPtr]->unk768;

		const auto oldUnk768 = tlsUnk768;
		tlsUnk768            = 0x3A;

		BSAnimationUpdateData data{ a_step };
		data.reference    = a_actor;
		data.shouldUpdate = a_actor->GetMustUpdate();

		a_actor->ModifyAnimationUpdateData(data);

		a_holder.UpdateAllAnimationGraphs(data);

		tlsUnk768 = oldUnk768;
	}

	void ActorProcessorTask::Run()
	{
		const boost::lock_guard lock(m_controller.m_lock);

		if (!m_run)
		{
			return;
		}

		m_timer.Begin();

		UpdateState();

		std::optional<animUpdateData_t> animUpdateData;

		if (!EngineExtensions::ParallelAnimationUpdatesEnabled() &&
		    m_runAnimationUpdates &&
		    !m_controller.m_objects.empty() &&
		    !Game::IsPaused())
		{
			animUpdateData.emplace(Game::Unk2f6b948::GetSteps());
		}

		for (auto& [i, e] : m_controller.m_objects)
		{
			auto& state = e.m_state;

			NiPointer<TESObjectREFR> refr;
			if (!e.GetHandle().Lookup(refr))
			{
				state.cellAttached = false;
				continue;
			}

			const auto actor = refr->As<Actor>();
			if (!Util::Common::IsREFRValid(actor))
			{
				state.cellAttached = false;
				continue;
			}

			const auto cell = actor->GetParentCell();
			if (cell && cell->IsAttached())
			{
				if (!state.cellAttached)
				{
					e.RequestEvalDefer();
					state.cellAttached = true;
				}
			}
			else
			{
				state.cellAttached = false;
				continue;
			}

			if (state.UpdateState(actor, cell))
			{
				e.RequestEvalDefer();
			}

			e.m_wantLFUpdate |= state.UpdateStateLF(actor);
			//e.m_wantHFUpdate |= state.UpdateStateHF(actor);

			if (e.UpdateNodeMonitorEntries())
			{
				e.RequestEvalDefer();
			}

			if (m_timer.GetStartTime() >= e.m_nextLFStateCheck)
			{
				e.m_nextLFStateCheck =
					m_timer.GetStartTime() +
					IPerfCounter::T(ActorObjectHolder::STATE_CHECK_INTERVAL_LOW);

				/*PerfTimer pt;
				pt.Start();*/

				e.m_wantLFUpdate |= state.UpdateFactions(actor);

				//_DMESSAGE("%.8X: %f  %d", actor->formID, pt.Stop(), r);

				if (e.m_wantLFUpdate)
				{
					e.m_wantLFUpdate = false;
					e.RequestEval();
				}

				/*if (e.m_wantLFVarUpdate)
				{
					e.m_wantLFVarUpdate = false;
					e.m_flags.set(ActorObjectHolderFlags::kWantVarUpdate);
				}*/
			}

			if (m_timer.GetStartTime() >= e.m_nextMFStateCheck)
			{
				e.m_nextMFStateCheck =
					m_timer.GetStartTime() +
					IPerfCounter::T(ActorObjectHolder::STATE_CHECK_INTERVAL_MED);

				if (state.UpdateEffects(actor))
				{
					e.RequestEval();
				}
			}

			if (m_timer.GetStartTime() >= e.m_nextHFStateCheck)
			{
				e.m_nextHFStateCheck =
					m_timer.GetStartTime() +
					IPerfCounter::T(ActorObjectHolder::STATE_CHECK_INTERVAL_HIGH);

				if (e.m_wantHFUpdate)
				{
					e.m_wantHFUpdate = false;
					e.RequestEval();
				}
			}

			if (animUpdateData)
			{
				const auto step =
					e.m_actorid == Data::IData::GetPlayerRefID() ?
						animUpdateData->steps.player :
						animUpdateData->steps.npc;

				UpdateActorGearAnimations(actor, e, step);
			}

			if (e.m_flags.test(ActorObjectHolderFlags::kWantEval))
			{
				if (e.m_flagsbf.evalCountdown > 0)
				{
					e.m_flagsbf.evalCountdown--;
				}

				if (e.m_flags.test(ActorObjectHolderFlags::kImmediateEval) ||
				    e.m_flagsbf.evalCountdown == 0)
				{
					e.m_flags.clear(ActorObjectHolderFlags::kRequestEvalMask);
					e.m_flags.set(ActorObjectHolderFlags::kEvalThisFrame);
				}
			}

			if (e.m_flags.consume(ActorObjectHolderFlags::kWantVarUpdate) ||
			    e.m_flags.test(ActorObjectHolderFlags::kEvalThisFrame))
			{
				auto& cvdata = m_controller.m_config.active.condvars;

				if (!cvdata.empty())
				{
					if (auto info = m_controller.LookupCachedActorInfo(actor, e))
					{
						auto& params = e.CreateProcessParams(
							info->sex,
							ControllerUpdateFlags::kPlaySound |
								ControllerUpdateFlags::kFailVariableCondition,
							info->actor.get(),
							info->handle,
							m_controller.m_temp.sr,
							e.m_temp.idt,
							e.m_temp.eqt,
							m_controller.m_temp.uc,
							actor,
							info->npc,
							info->npcOrTemplate,
							info->race,
							info->root,
							info->npcRoot,
							e,
							m_controller);

						if (IConditionalVariableProcessor::UpdateVariableMap(
								params,
								cvdata,
								e.GetVariables()))
						{
							m_controller.RequestHFEvaluateAll(i);
							e.m_flags.set(ActorObjectHolderFlags::kEvalThisFrame);
						}

						params.flags.clear(ControllerUpdateFlags::kFailVariableCondition);
					}
				}
			}
		}

		for (auto& [i, e] : m_controller.m_objects)
		{
			if (!e.m_state.cellAttached)
			{
				e.ClearCurrentProcessParams();
				continue;
			}

			ProcessEvalRequest(e);

			if (CheckMonitorNodes(e))
			{
				e.RequestTransformUpdate();
			}

			ProcessTransformUpdateRequest(e);

			e.ClearCurrentProcessParams();

			bool update = false;

			e.visit([&](auto& a_v) [[msvc::forceinline]] {

				auto& state = a_v.data.state;

				if (!state)
				{
					return;
				}

				UpdateNode(e, a_v);

				if constexpr (
					std::is_same_v<
						std::remove_cvref_t<decltype(a_v)>,
						ObjectEntrySlot>)
				{
					if (state->hideCountdown)
					{
						if (--state->hideCountdown == 0)
						{
							if (state->flags.test(ObjectEntryFlags::kInvisible))
							{
								state->SetVisible(false);

								if (state->nodes.HasPhysicsNode())
								{
									if (auto& simComponent = state->simComponent)
									{
										e.RemoveAndDestroySimComponent(simComponent);
									}
								}

								update = true;
							}
						}
					}
				}
			});

			if (update)
			{
				e.RequestTransformUpdateDeferNoSkip();
			}
		}

		m_controller.RunObjectCleanup();

		m_timer.End(m_currentTime);
	}

}