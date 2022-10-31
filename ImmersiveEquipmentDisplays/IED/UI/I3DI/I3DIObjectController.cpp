#include "pch.h"

#include "I3DIObjectController.h"

#include "I3DICommonData.h"
#include "I3DIInputHelpers.h"
#include "I3DIModelObject.h"
#include "I3DIObject.h"

#include "I3DIActorObject.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		I3DIObjectController::I3DIObjectController() :
			m_runPT(1000000)
		{
		}

		void I3DIObjectController::RegisterObject(
			const std::shared_ptr<I3DIObject>& a_object)
		{
			assert(a_object != nullptr);

			m_data.emplace_back(a_object);
		}

		void I3DIObjectController::UnregisterObject(
			const std::shared_ptr<I3DIObject>& a_object)
		{
			assert(a_object != nullptr);

			std::erase_if(
				m_data,
				[&](auto& a_v) {
					return a_object == a_v.object;
				});

			if (a_object == m_selected)
			{
				m_selected.reset();
			}

			if (a_object == m_hovered)
			{
				m_hovered.reset();
			}

			if (a_object == m_dragObject)
			{
				if (auto draggable = a_object->GetAsDraggable())
				{
					draggable->OnDragEnd(I3DIDragDropResult::kCancelled, nullptr);
					draggable->SetDragging(false);
				}
				m_dragObject.reset();
			}
		}

		void I3DIObjectController::Run(
			I3DICommonData& a_data)
		{
			m_runPT.Begin();

			if (auto dragObject = m_dragObject.get())
			{
				UpdateDragObjectPosition(a_data, dragObject);
			}

			auto hoveredObject = GetHovered(a_data);

			if (hoveredObject != m_hovered)
			{
				if (auto current = m_hovered.get())
				{
					current->m_objectFlags.clear(I3DIObjectFlags::kHovered);
					current->OnMouseMoveOut(a_data);
				}

				if (hoveredObject)
				{
					hoveredObject->m_objectFlags.set(I3DIObjectFlags::kHovered);
					hoveredObject->OnMouseMoveOver(a_data);
				}

				m_hovered = hoveredObject;
			}

			if (m_dragObject)
			{
				if (hoveredObject)
				{
					if (auto dropTarget = hoveredObject->GetAsDropTarget())
					{
						dropTarget->OnDraggableMovingOver(*m_dragObject->GetAsDraggable());
					}
				}

				if (!I3DI::IsMouseDown())
				{
					auto draggable = m_dragObject->GetAsDraggable();

					const auto result = HandleDragEnd(
						a_data,
						draggable,
						hoveredObject);

					//_DMESSAGE("drag end: %p (%u)", draggable, result.first);

					draggable->OnDragEnd(result.first, result.second);
					draggable->SetDragging(false);

					_DMESSAGE("res %u %p", result.first, hoveredObject.get());

					m_dragObject.reset();
					m_lastClickPos = { -FLT_MAX, -FLT_MAX };
				}
			}
			else
			{
				if (hoveredObject)
				{
					if (I3DI::IsMouseClicked())
					{
						m_lastClickPos = ImGui::GetIO().MousePos;
					}
					else if (I3DI::IsMouseReleased())
					{
						hoveredObject->OnClick(a_data);

						SelectObject(a_data, hoveredObject);
					}

					if (I3DI::IsMouseDown())
					{
						TryBeginDrag(a_data, hoveredObject);
					}
				}
			}

			for (auto& e : m_data)
			{
				if (ShouldProcessObject(a_data, e.object.get()))
				{
					e.object->DrawObjectExtra(a_data);
				}
			}

			m_runPT.End(m_lastRunTime);
		}

		void I3DIObjectController::DrawObjects(I3DICommonData& a_data)
		{
			m_drawQueueOpaque.clear();
			m_drawQueueAlpha.clear();

			auto p = a_data.scene.GetCameraPosition();

			for (auto& e : m_data)
			{
				if (!e.object->HasWorldData())
				{
					continue;
				}

				if (!ShouldProcessObject(a_data, e.object.get()))
				{
					continue;
				}

				if (e.object != m_dragObject &&
				    e.object->IsGeometryHidden())
				{
					continue;
				}

				if (auto model = e.object->GetAsModelObject())
				{
					if (model->IsOpaque())
					{
						m_drawQueueOpaque.emplace_back(
							model->GetCenterDistanceSq(p),
							model);
					}
					else
					{
						m_drawQueueAlpha.emplace_back(
							model->GetCenterDistanceSq(p),
							model);
					}
				}
			}

			std::sort(
				m_drawQueueOpaque.begin(),
				m_drawQueueOpaque.end(),
				[&](auto& a_lhs, auto& a_rhs) {
					return a_lhs.first < a_rhs.first;
				});

			for (auto& e : m_drawQueueOpaque)
			{
				e.second->Draw(a_data.scene);
			}

			std::sort(
				m_drawQueueAlpha.begin(),
				m_drawQueueAlpha.end(),
				[&](auto& a_lhs, auto& a_rhs) {
					return a_lhs.first > a_rhs.first;
				});

			for (auto& e : m_drawQueueAlpha)
			{
				e.second->Draw(a_data.scene);
			}
		}

		auto I3DIObjectController::GetHovered(
			I3DICommonData& a_data)
			-> std::shared_ptr<I3DIObject>
		{
			auto& io = ImGui::GetIO();

			if (io.WantCaptureMouse)
			{
				return {};
			}

			for (auto& e : m_data)
			{
				float dist;

				if (e.object->ObjectIntersects(
						a_data,
						a_data.cursorRay,
						dist))
				{
					e.dist.emplace(dist);
				}
				else
				{
					e.dist.reset();
				}

				e.object->SetLastDistance(e.dist);
			}

			std::sort(
				m_data.begin(),
				m_data.end(),
				[](auto& a_lhs, auto& a_rhs) {
					return a_lhs.dist < a_rhs.dist;
				});

			auto it = std::find_if(
				m_data.begin(),
				m_data.end(),
				[&](auto& a_v) {
					return a_v.dist &&
				           a_v.object != m_dragObject &&
				           ShouldProcessObject(
							   a_data,
							   a_v.object.get());
				});

			return it != m_data.end() ?
			           it->object :
                       nullptr;
		}

		bool I3DIObjectController::ShouldProcessObject(
			I3DICommonData& a_data,
			I3DIObject*     a_object)
		{
			if (!a_object->ShouldProcess(a_data))
			{
				return false;
			}

			if (m_dragObject)
			{
				if (m_dragObject.get() == a_object)
				{
					return true;
				}

				auto dropTarget = a_object->GetAsDropTarget();
				if (!dropTarget)
				{
					return false;
				}

				if (!dropTarget->AcceptsDraggable(
						*m_dragObject->GetAsDraggable()))
				{
					return false;
				}
			}

			return true;
		}

		void I3DIObjectController::SelectObject(
			I3DICommonData&                    a_data,
			const std::shared_ptr<I3DIObject>& a_object)
		{
			if (a_object->IsSelectable() && a_object != m_selected)
			{
				if (a_object->OnSelectInt(a_data))
				{
					if (m_selected)
					{
						m_selected->OnUnselectInt(a_data);
					}

					m_selected = a_object;
				}
			}
		}

		void I3DIObjectController::TryBeginDrag(
			I3DICommonData&                    a_data,
			const std::shared_ptr<I3DIObject>& a_object)
		{
			auto draggable = a_object->GetAsDraggable();
			if (!draggable)
			{
				return;
			}

			if (!I3DI::IsMouseInputValid() ||
			    m_lastClickPos.x == -FLT_MAX ||
			    m_lastClickPos.y == -FLT_MAX)
			{
				return;
			}

			auto ld = a_object->GetLastDistance();
			if (!ld)
			{
				return;
			}

			const auto& io = ImGui::GetIO();

			constexpr auto DRAG_MD_THRESHOLD = 4.0f;

			const auto delta = io.MousePos - m_lastClickPos;

			const auto p = delta.x * delta.x + delta.y * delta.y;

			if (p < DRAG_MD_THRESHOLD * DRAG_MD_THRESHOLD)
			{
				return;
			}

			if (draggable->OnDragBegin())
			{
				//_DMESSAGE("drag start: %p", draggable);

				const auto dist = XMVectorReplicate(*ld);

				m_dragStartDist = dist;

				if (auto model = a_object->GetAsModelObject())
				{
					const auto intersectionPt =
						a_data.cursorRay.origin +
						a_data.cursorRay.dir * dist;

					m_dragObjectPositionOffset =
						model->GetWorldPosition() -
						intersectionPt;
				}
				else
				{
					m_dragObjectPositionOffset = g_XMZero.v;
				}

				SelectObject(a_data, a_object);

				m_dragObject = a_object;

				draggable->SetDragging(true);
			}
		}

		std::pair<I3DIDragDropResult, I3DIDropTarget*> I3DIObjectController::HandleDragEnd(
			I3DICommonData&                    a_data,
			I3DIDraggable*                     a_object,
			const std::shared_ptr<I3DIObject>& a_dropObject)
		{
			if (!I3DI::IsMouseReleased() ||
			    !a_dropObject)
			{
				return { I3DIDragDropResult::kCancelled, nullptr };
			}

			auto dropTarget = a_dropObject->GetAsDropTarget();

			if (!dropTarget)
			{
				return { I3DIDragDropResult::kCancelled, nullptr };
			}

			if (dropTarget->ProcessDropRequest(*a_object))
			{
				return { I3DIDragDropResult::kAccepted, dropTarget };
			}
			else
			{
				return { I3DIDragDropResult::kRejected, dropTarget };
			}
		}

		void I3DIObjectController::UpdateDragObjectPosition(
			I3DICommonData& a_data,
			I3DIObject*     a_object)
		{
			auto model = a_object->GetAsModelObject();
			if (!model)
			{
				return;
			}

			if (auto parent = model->GetParentObject())
			{
				float dist;

				if (parent->Intersects(
						a_data.cursorRay.origin,
						a_data.cursorRay.dir,
						dist))
				{
					m_dragStartDist = XMVectorReplicate(dist);
				}
				else
				{
					const auto dir = XMVector3Normalize(
						parent->GetBoundingShapeCenter() -
						a_data.cursorRay.origin);

					if (parent->Intersects(
							a_data.cursorRay.origin,
							dir,
							dist))
					{
						m_dragStartDist = XMVectorReplicate(dist);
					}
				}
			}

			const auto pos =
				a_data.cursorRay.origin +
				a_data.cursorRay.dir * m_dragStartDist;

			model->SetWorldPosition(pos + m_dragObjectPositionOffset);
			model->UpdateBound();

			a_object->GetAsDraggable()->OnDragPositionUpdate(a_data);
		}

	}
}