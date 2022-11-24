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

			a_object->OnObjectRegistered(*this);
		}

		void I3DIObjectController::UnregisterObject(
			const std::shared_ptr<I3DIObject>& a_object)
		{
			assert(a_object != nullptr);

			a_object->OnObjectUnregistered(*this);

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

			for (auto& e : m_mouseState)
			{
				if (a_object == e.lastClickedObject)
				{
					e = {};
				}
			}

			if (auto& dragContext = m_dragContext)
			{
				if (a_object == dragContext)
				{
					auto draggable = a_object->AsDraggable();

					draggable->OnDragEnd(I3DIDragDropResult::kCancelled, nullptr);
					draggable->SetDragging(false);

					dragContext.reset();
				}
			}
		}

		void I3DIObjectController::Run(
			I3DICommonData& a_data)
		{
			m_runPT.Begin();

			if (auto& dragContext = m_dragContext)
			{
				UpdateDragObjectPosition(a_data, dragContext->object.get());

				dragContext->object->AsDraggable()->OnDragUpdate(a_data);
			}

			const auto hoveredObject = GetHovered(a_data);

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

			if (auto& dragContext = m_dragContext)
			{
				if (hoveredObject)
				{
					if (auto dropTarget = hoveredObject->AsDropTarget())
					{
						dropTarget->OnDraggableMovingOver(*dragContext->object->AsDraggable());
					}
				}

				if (!I3DI::IsMouseDown(dragContext->button))
				{
					const auto result = HandleDragEnd(
						a_data,
						hoveredObject);

					//_DMESSAGE("drag end: %p (%u)", draggable, result.first);

					auto draggable = dragContext->object->AsDraggable();

					draggable->OnDragEnd(result.first, result.second);
					draggable->SetDragging(false);

					//_DMESSAGE("res %u %p", result.first, hoveredObject.get());

					dragContext.reset();
				}
			}
			else
			{
				for (std::uint32_t i = 0; i < m_mouseState.size(); i++)
				{
					ObjectControlInputHandler(
						a_data,
						hoveredObject,
						static_cast<ImGuiMouseButton>(i));
				}
			}

			for (auto& e : m_data)
			{
				if (ShouldProcessObject(a_data, e.object.get(), true))
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

				if (!ShouldProcessObject(a_data, e.object.get(), true))
				{
					continue;
				}

				if (e.object != m_dragContext &&
				    e.object->IsGeometryHidden())
				{
					continue;
				}

				if (auto model = e.object->AsModelObject())
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

		void I3DIObjectController::ObjectControlInputHandler(
			I3DICommonData&                    a_data,
			const std::shared_ptr<I3DIObject>& a_hoveredObject,
			ImGuiMouseButton                   a_button)
		{
			assert(a_button < m_mouseState.size());

			auto& e = m_mouseState[a_button];

			const auto& io = ImGui::GetIO();

			if (e.lastClickedObject)
			{
				if (io.WantCaptureMouse ||
				    io.MouseDownOwned[a_button])
				{
					e = {};
				}
				else if (io.MouseReleased[a_button])
				{
					if (e.lastClickedObject == a_hoveredObject)
					{
						a_hoveredObject->OnMouseUp(a_data, a_button);

						switch (a_button)
						{
						case ImGuiMouseButton_Left:
							SelectObject(a_data, a_hoveredObject);
							break;
						case ImGuiMouseButton_Right:
							UnSelectObject(a_data, a_hoveredObject);
							break;
						}
					}

					e = {};
				}
				else if (io.MouseDown[a_button])
				{
					if (TryBeginDrag(a_data, a_button))
					{
						e = {};
					}
				}
				else
				{
					e = {};
				}
			}
			else if (a_hoveredObject)
			{
				if (I3DI::IsMouseClicked(a_button))
				{
					e = {
						a_hoveredObject,
						ImGui::GetIO().MousePos
					};

					a_hoveredObject->OnMouseDown(a_data, a_button);
				}
			}
		}

		auto I3DIObjectController::GetHovered(
			I3DICommonData& a_data)
			-> std::shared_ptr<I3DIObject>
		{
			if (I3DI::ShouldBlockInput())
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
					e.object->SetLastDistance(e.dist);
				}
				else
				{
					e.dist.reset();
				}
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
				           a_v.object != m_dragContext &&
				           ShouldProcessObject(
							   a_data,
							   a_v.object.get(),
							   false);
				});

			return it != m_data.end() ?
			           it->object :
                       nullptr;
		}

		bool I3DIObjectController::ShouldProcessObject(
			I3DICommonData& a_data,
			I3DIObject*     a_object,
			bool            a_allowParent)
		{
			if (!a_object->ShouldProcess(a_data) ||
				a_object->IsDisabled())
			{
				return false;
			}

			if (auto& dragContext = m_dragContext)
			{
				if (dragContext->object.get() == a_object)
				{
					return true;
				}

				if (a_allowParent)
				{
					if (auto model = a_object->AsModelObject())
					{
						if (auto parent = model->GetParentObject())
						{
							if (parent == dragContext->object.get())
							{
								return true;
							}
						}
					}
				}

				auto dropTarget = a_object->AsDropTarget();
				if (!dropTarget)
				{
					return false;
				}

				if (!dropTarget->AcceptsDraggable(
						*dragContext->object->AsDraggable()))
				{
					return false;
				}
			}

			if (auto& selected = m_selected)
			{
				if (selected->HideOtherWhenSelected() && selected.get() != a_object)
				{
					if (a_allowParent)
					{
						if (auto model = a_object->AsModelObject())
						{
							if (auto parent = model->GetParentObject())
							{
								if (parent == selected.get())
								{
									return true;
								}
							}
						}
					}

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

		void I3DIObjectController::UnSelectObject(
			I3DICommonData&                    a_data,
			const std::shared_ptr<I3DIObject>& a_object)
		{
			if (a_object == m_selected)
			{
				a_object->OnUnselectInt(a_data);

				m_selected.reset();
			}
		}

		bool I3DIObjectController::TryBeginDrag(
			I3DICommonData&  a_data,
			ImGuiMouseButton a_button)
		{
			assert(a_button < m_mouseState.size());

			const auto& e = m_mouseState[a_button];

			auto& object = e.lastClickedObject;

			auto draggable = e.lastClickedObject->AsDraggable();
			if (!draggable)
			{
				return false;
			}

			if (!I3DI::IsMouseInputValid() ||
			    e.lastClickPos.x == -FLT_MAX ||
			    e.lastClickPos.y == -FLT_MAX)
			{
				return false;
			}

			auto ld = object->GetLastDistance();
			if (!ld)
			{
				return false;
			}

			const auto& io = ImGui::GetIO();

			constexpr auto DRAG_MD_THRESHOLD = 5.0f;

			const auto delta = io.MousePos - e.lastClickPos;

			const auto m = delta.x * delta.x + delta.y * delta.y;

			if (m < DRAG_MD_THRESHOLD * DRAG_MD_THRESHOLD)
			{
				return false;
			}

			if (draggable->OnDragBegin(a_data, a_button))
			{
				//_DMESSAGE("drag start: %p", draggable);

				m_dragContext.emplace(a_button, object);

				if (draggable->GetDraggableType() == I3DIDraggableType::Dynamic)
				{
					if (auto model = object->AsModelObject())
					{
						m_dragContext->startDist = *ld;

						const auto intersectionPt =
							a_data.cursorRay.origin +
							a_data.cursorRay.dir * *ld;

						m_dragContext->objectPositionOffset =
							model->GetWorldPosition() -
							intersectionPt;
					}
				}

				SelectObject(a_data, object);

				draggable->SetDragging(true);

				return true;
			}
			else
			{
				return false;
			}
		}

		std::pair<I3DIDragDropResult, I3DIDropTarget*> I3DIObjectController::HandleDragEnd(
			I3DICommonData&                    a_data,
			const std::shared_ptr<I3DIObject>& a_dropObject)
		{
			auto& dragContext = m_dragContext;

			assert(dragContext);

			if (!I3DI::IsMouseReleased(dragContext->button) ||
			    !a_dropObject)
			{
				return { I3DIDragDropResult::kCancelled, nullptr };
			}

			auto dropTarget = a_dropObject->AsDropTarget();

			if (!dropTarget)
			{
				return { I3DIDragDropResult::kCancelled, nullptr };
			}

			if (auto model = a_dropObject->AsModelObject())
			{
				if (auto parent = model->GetParentObject())
				{
					if (parent == dragContext->object.get())
					{
						return { I3DIDragDropResult::kRejected, nullptr };
					}
				}
			}

			if (dropTarget->ProcessDropRequest(*dragContext->object->AsDraggable()))
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
			auto draggable = a_object->AsDraggable();

			if (draggable->GetDraggableType() != I3DIDraggableType::Dynamic)
			{
				return;
			}

			auto model = a_object->AsModelObject();
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
					m_dragContext->startDist = dist;
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
						m_dragContext->startDist = dist;
					}
				}
			}

			const auto pos =
				a_data.cursorRay.origin +
				a_data.cursorRay.dir * m_dragContext->startDist;

			model->SetWorldPosition(pos + m_dragContext->objectPositionOffset);
			model->UpdateBound();
		}

	}
}