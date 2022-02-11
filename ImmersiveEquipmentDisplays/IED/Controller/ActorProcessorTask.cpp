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
	{}

	void ActorProcessorTask::UpdateRef(
		const ActorObjectHolder& a_record,
		objectEntryBase_t& a_entry)
	{
		if (!a_entry.state)
		{
			return;
		}

		auto& nodes = a_entry.state->nodes;

		if (!nodes.ref)
		{
			return;
		}

		if (nodes.IsReferenceMovedOrOphaned())
		{
			if (a_entry.state->flags.test(ObjectEntryFlags::kRefSyncDisableFailedOrphan))
			{
				return;
			}

			bool result = false;

			Controller::actorInfo_t info;
			if (m_controller.LookupCachedActorInfo(a_record, info))
			{
				if (result = m_controller.AttachNodeImpl(
						info.npcRoot,
						a_entry.state->nodeDesc,
						a_entry.state->atmReference,
						a_entry))
				{
					m_controller.UpdateRootPaused(info.root);
					a_record.RequestTransformUpdateDefer();
				}
			}

			if (!result)
			{
				a_entry.state->flags.set(ObjectEntryFlags::kRefSyncDisableFailedOrphan);
				return;
			}

			if (!nodes.ref)
			{
				return;
			}
		}

		if (a_entry.state->flags.test(ObjectEntryFlags::kSyncReferenceTransform) &&
		    nodes.obj->IsVisible())
		{
			if (a_entry.state->transform.scale)
			{
				nodes.obj->m_localTransform.scale =
					nodes.ref->m_localTransform.scale * *a_entry.state->transform.scale;
			}
			else
			{
				nodes.obj->m_localTransform.scale = nodes.ref->m_localTransform.scale;
			}

			if (a_entry.state->transform.rotation)
			{
				nodes.obj->m_localTransform.rot =
					nodes.ref->m_localTransform.rot * *a_entry.state->transform.rotation;
			}
			else
			{
				nodes.obj->m_localTransform.rot = nodes.ref->m_localTransform.rot;
			}

			if (a_entry.state->transform.position)
			{
				nodes.obj->m_localTransform.pos =
					nodes.ref->m_localTransform * *a_entry.state->transform.position;
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

		for (auto& e : m_controller.m_objects)
		{
			if (!e.second.m_actor->formID)
			{
				continue;
			}

			auto cell = e.second.m_actor->parentCell;

			if (bool interior = cell && cell->IsInterior();
			    interior != e.second.m_inInterior)
			{
				e.second.m_inInterior = interior;
				e.second.RequestEvalDefer();
			}

			if (!(e.second.m_cellAttached = cell && cell->IsAttached()))
			{
				continue;
			}

			ProcessEvalRequest(e.second);

			if (CheckMonitorNodes(e.second))
			{
				e.second.RequestTransformUpdateDeferNoSkip();
			}

			ProcessTransformUpdateRequest(e.second);

			bool update = false;

			for (auto& f : e.second.m_entriesSlot)
			{
				UpdateRef(e.second, f);

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

			for (auto& f : e.second.m_entriesCustom)
			{
				for (auto& g : f)
				{
					for (auto& h : g.second)
					{
						UpdateRef(e.second, h.second);
					}
				}
			}

			if (update)
			{
				e.second.RequestTransformUpdateDeferNoSkip();
			}
		}

		m_controller.RunObjectCleanup();

		m_timer.End(m_currentTime);
	}

}