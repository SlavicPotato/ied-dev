#include "pch.h"

#include "ActorProcessorTask.h"

#include "Controller.h"
#include "IED/Inventory.h"
#include "IObjectManager.h"

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
		ActorObjectHolder&                          a_record,
		objectEntryBase_t&                          a_entry,
		const std::optional<BSAnimationUpdateData>& a_animUpdateData)
	{
		auto state = a_entry.state.get();

		if (!state)
		{
			return;
		}

		if (a_animUpdateData)
		{
			if (state->weapAnimGraphManagerHolder)
			{
				//data.reference = a_record.m_actor.get();
				state->weapAnimGraphManagerHolder->Update(*a_animUpdateData);
			}

			for (auto& e : state->groupObjects)
			{
				if (e.second.weapAnimGraphManagerHolder)
				{
					e.second.weapAnimGraphManagerHolder->Update(*a_animUpdateData);
				}
			}
		}

		auto& nodes = state->nodes;

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
					m_controller.UpdateIfPaused(info->root);
					a_record.RequestTransformUpdateDefer();
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

			m_controller.EvaluateTransformsImpl(a_data);
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

			m_controller.EvaluateImpl(a_data, ControllerUpdateFlags::kAll);
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
		if (auto fpstate = IsInFirstPerson();
		    fpstate != m_state.inFirstPerson)
		{
			m_state.inFirstPerson = fpstate;

			if (auto it = m_controller.m_objects.find(Data::IData::GetPlayerRefID());
			    it != m_controller.m_objects.end())
			{
				it->second.RequestEvalDefer();
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

		if (auto current = (sky ? sky->currentWeather : nullptr);
		    current != m_state.currentWeather)
		{
			m_state.currentWeather = current;
			changed                = true;
		}

		if (auto tod = Data::GetTimeOfDay(sky);
		    tod != m_state.timeOfDay)
		{
			m_state.timeOfDay = tod;
			changed           = true;
		}

		if (changed)
		{
			for (auto& e : m_controller.m_objects)
			{
				e.second.m_wantLFUpdate = true;
			}
		}
	}

	void ActorProcessorTask::Run()
	{
		stl::scoped_lock lock(m_controller.m_lock);

		m_timer.Begin();

		UpdateState();

		auto steps  = Game::Unk2f6b948::GetSteps();
		bool paused = Game::IsPaused();

		for (auto& [i, e] : m_controller.m_objects)
		{
			if (!e.m_actor->formID)
			{
				continue;
			}

			auto cell = e.m_actor->GetParentCell();
			if (cell && cell->IsAttached())
			{
				e.m_cellAttached = true;
			}
			else
			{
				e.m_cellAttached = false;
				continue;
			}

			if (bool interior = cell->IsInterior();
			    interior != e.m_locData.inInterior)
			{
				e.m_locData.inInterior = interior;
				e.RequestEvalDefer();
			}

			if (auto ws = cell->GetWorldSpace();
			    ws != e.m_locData.worldspace)
			{
				e.m_locData.worldspace = ws;
				e.RequestEvalDefer();
			}

			if (auto cstate = Game::GetActorInCombat(e.m_actor);
			    cstate != e.m_inCombat)
			{
				e.m_inCombat = cstate;
				e.RequestEvalDefer();
			}

			if (IPerfCounter::delta_us(
					e.m_lastLFStateCheck,
					m_timer.GetStartTime()) >= STATE_CHECK_INTERVAL_LOW)
			{
				e.m_lastLFStateCheck = m_timer.GetStartTime();

				if (auto n = e.m_actor->IsPlayerTeammate();
				    n != e.m_isPlayerTeammate)
				{
					e.m_isPlayerTeammate = n;
					e.RequestEvalDefer();
				}

				if (auto n = e.m_actor->GetCurrentPackage();
				    n != e.m_currentPackage)
				{
					e.m_currentPackage = n;
					e.RequestEvalDefer();
				}

				if (e.m_wantLFUpdate)
				{
					e.m_wantLFUpdate = false;
					e.RequestEvalDefer();
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
				e.RequestTransformUpdateDeferNoSkip();
			}

			ProcessTransformUpdateRequest(e);

			bool update = false;

			std::optional<BSAnimationUpdateData> animUpdateData;

			if (!paused)
			{
				animUpdateData.emplace(e.m_actor == *g_thePlayer ? steps.player : steps.npc);
				//animUpdateData->reference = e.m_actor.get();
				//animUpdateData->forceUpdate = true;
			}

			for (auto& f : e.m_entriesSlot)
			{
				UpdateNode(e, f, animUpdateData);

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
						UpdateNode(e, h.second, animUpdateData);
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