#include "pch.h"

#include "ActorProcessorTask.h"

#include "Controller.h"
#include "IObjectManager.h"

#include "IED/EngineExtensions.h"
#include "IED/Inventory.h"
#include "IED/StringHolder.h"

#include <ext/BSAnimationUpdateData.h>
#include <ext/Sky.h>

namespace IED
{
	ActorProcessorTask::ActorProcessorTask(
		Controller& a_controller) :
		m_state{ IPerfCounter::Query() - STATE_CHECK_INTERVAL_LOW },
		m_controller(a_controller)
	{
	}

	void ActorProcessorTask::UpdateNode(
		ActorObjectHolder& a_record,
		ObjectEntryBase&   a_entry)
	{
		auto state = a_entry.state.get();

		if (!state)
		{
			return;
		}

		const auto& nodes = state->nodes;

		const bool visible = nodes.rootNode->IsVisible();

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
					m_controller.UpdateNodeIfGamePaused(info->root);
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
		    visible)
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
		if (!a_data.m_flags.test(ActorObjectHolderFlags::kWantEval))
		{
			return;
		}

		if (a_data.m_flagsbf.evalCountdown > 0)
		{
			a_data.m_flagsbf.evalCountdown--;
		}

		if (a_data.m_flags.test(ActorObjectHolderFlags::kImmediateEval) ||
		    a_data.m_flagsbf.evalCountdown == 0)
		{
			a_data.m_flags.clear(ActorObjectHolderFlags::kRequestEvalMask);

			m_controller.EvaluateImpl(
				a_data,
				ControllerUpdateFlags::kPlaySound |
					ControllerUpdateFlags::kUseCachedParams);
		}
	}

	bool ActorProcessorTask::CheckMonitorNodes(ActorObjectHolder& a_data)
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

#if defined(IED_ENABLE_1D10T_SAFEGUARDS)
	void ActorProcessorTask::WriteCMETransforms(
		ActorObjectHolder& a_data)
	{
		for (auto& e : a_data.m_cmeNodes)
		{
			e.second.node->m_localTransform = e.second.current;
		}
	}
#endif

	void ActorProcessorTask::UpdateState()
	{
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
				m_timer.GetStartTime()) < STATE_CHECK_INTERVAL_LOW)
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
			for (auto& e : m_controller.m_objects)
			{
				e.second.m_wantLFUpdate = true;
			}
		}
	}

	static void UpdateActorGearAnimations(
		const ActorObjectHolder& a_holder,
		float                    a_step)
	{
		NiPointer<TESObjectREFR> refr;

		if (!a_holder.GetHandle().Lookup(refr))
		{
			return;
		}

		struct TLSData
		{
			std::uint8_t  unk000[0x768];  // 000
			std::uint32_t unk768;         // 768
		};

		auto tlsData = reinterpret_cast<TLSData**>(__readgsqword(0x58));

		auto& tlsUnk768 = tlsData[*EngineExtensions::tlsIndex]->unk768;

		auto oldUnk768 = tlsUnk768;
		tlsUnk768      = 0x3A;

		BSAnimationUpdateData data{ a_step };
		data.reference    = refr;
		data.shouldUpdate = refr->GetMustUpdate();

		refr->ModifyAnimationUpdateData(data);

		a_holder.UpdateAllAnimationGraphs(data);

		tlsUnk768 = oldUnk768;
	}

	void ActorProcessorTask::Run()
	{
		stl::scoped_lock lock(m_controller.m_lock);

		if (!m_run)
		{
			return;
		}

		m_timer.Begin();

		UpdateState();

		std::optional<animUpdateData_t> animUpdateData;

		if (!EngineExtensions::ParallelAnimationUpdatesEnabled() &&
		    m_runAnimationUpdates &&
		    !Game::IsPaused())
		{
			animUpdateData.emplace(Game::Unk2f6b948::GetSteps());
		}

		for (auto& [i, e] : m_controller.m_objects)
		{
			auto actor = e.m_actor.get();

			if (!actor->formID)
			{
				continue;
			}

			auto& state = e.m_state;

			auto cell = actor->GetParentCell();
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

			if (state.UpdateState(actor))
			{
				e.RequestEvalDefer();
			}

			e.m_wantLFUpdate |= state.UpdateStateLF(actor);

			if (e.UpdateNodeMonitorEntries())
			{
				e.RequestEvalDefer();
			}

			if (IPerfCounter::delta_us(
					e.m_lastLFStateCheck,
					m_timer.GetStartTime()) >= STATE_CHECK_INTERVAL_LOW)
			{
				e.m_lastLFStateCheck = m_timer.GetStartTime();

				/*PerfTimer pt;
				pt.Start();*/

				e.m_wantLFUpdate |= state.UpdateFactions(e.m_actor.get());

				//_DMESSAGE("%.8X: %f | %zu", e.m_formid, pt.Stop(), state.GetNumFactions());

				if (e.m_wantLFUpdate)
				{
					e.m_wantLFUpdate = false;
					e.RequestEval();
				}

#if defined(IED_ENABLE_1D10T_SAFEGUARDS)
				if (m_activeWriteCMETransforms)
				{
					WriteCMETransforms(e);
				}
#endif
			}

			ProcessEvalRequest(e);

			if (CheckMonitorNodes(e))
			{
				e.RequestTransformUpdate();
			}

			ProcessTransformUpdateRequest(e);

			e.ClearCurrentParams();

			if (animUpdateData)
			{
				float step =
					e.m_formid == Data::IData::GetPlayerRefID() ?
						animUpdateData->steps.player :
                        animUpdateData->steps.npc;

				UpdateActorGearAnimations(e, step);
			}

			bool update = false;

			for (auto& f : e.m_entriesSlot)
			{
				UpdateNode(e, f);

				if (f.hideCountdown)
				{
					if (!f.state)
					{
						f.hideCountdown = 0;
					}
					else
					{
						f.hideCountdown--;

						if (f.hideCountdown == 0)
						{
							update |= f.state->nodes.rootNode->IsVisible();

							f.state->nodes.rootNode->SetVisible(false);
						}
					}
				}
			}

			for (auto& f : e.m_entriesCustom)
			{
				for (auto& g : f)
				{
					for (auto& h : g.second)
					{
						UpdateNode(e, h.second);
					}
				}
			}

			if (update)
			{
				e.RequestTransformUpdateDeferNoSkip();
			}
		}

		m_controller.RunObjectCleanup();

		m_timer.End(m_currentTime);
	}

}