#include "pch.h"

#include "ActorProcessorTask.h"

#include "Controller.h"
#include "IED/Inventory.h"
#include "IObjectManager.h"

namespace IED
{
	ActorProcessorTask::ActorProcessorTask(
		Controller& a_controller) :
		m_controller(a_controller)
	{
	}

	void ActorProcessorTask::UpdateNode(
		const ActorObjectHolder& a_record,
		objectEntryBase_t&       a_entry)
	{
		auto state = a_entry.state.get();

		if (!state)
		{
			return;
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

			Controller::actorInfo_t info;
			if (m_controller.LookupCachedActorInfo(a_record, info))
			{
				if (result = m_controller.AttachNodeImpl(
						info.npcRoot,
						state->nodeDesc,
						state->atmReference,
						a_entry))
				{
					m_controller.UpdateRootPaused(info.root);
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
		    nodes.obj->IsVisible())
		{
			if (state->transform.scale)
			{
				nodes.obj->m_localTransform.scale =
					nodes.ref->m_localTransform.scale * *state->transform.scale;
			}
			else
			{
				nodes.obj->m_localTransform.scale = nodes.ref->m_localTransform.scale;
			}

			if (state->transform.rotation)
			{
				nodes.obj->m_localTransform.rot =
					nodes.ref->m_localTransform.rot * *state->transform.rotation;
			}
			else
			{
				nodes.obj->m_localTransform.rot = nodes.ref->m_localTransform.rot;
			}

			if (state->transform.position)
			{
				nodes.obj->m_localTransform.pos =
					nodes.ref->m_localTransform * *state->transform.position;
			}
			else
			{
				nodes.obj->m_localTransform.pos = nodes.ref->m_localTransform.pos;
			}
		}
	}

	void ActorProcessorTask::ProcessTransformUpdateRequest(
		const ActorObjectHolder& a_data)
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

	void ActorProcessorTask::Run()
	{
		IScopedLock lock(m_controller.m_lock);

		m_timer.Begin();

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
			}

			ProcessEvalRequest(e);

			if (CheckMonitorNodes(e))
			{
				e.RequestTransformUpdateDeferNoSkip();
			}

			ProcessTransformUpdateRequest(e);

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
							update |= f.state->nodes.obj->IsVisible();

							f.state->nodes.obj->SetVisible(false);
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