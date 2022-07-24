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
		}

		void I3DIObjectController::Run(I3DICommonData& a_data)
		{
			auto hoveredObject = GetHovered(a_data);

			if (hoveredObject != m_hovered)
			{
				if (m_hovered)
				{
					m_hovered->OnMouseMoveOutInt(a_data);
				}

				if (hoveredObject)
				{
					hoveredObject->OnMouseMoveOverInt(a_data);
				}

				m_hovered = hoveredObject;
			}

			if (I3DI::IsMouseReleased())
			{
				if (hoveredObject)
				{
					hoveredObject->OnClick(a_data);

					if (hoveredObject->IsSelectable() && hoveredObject != m_selected)
					{
						if (hoveredObject->OnSelectInt(a_data))
						{
							if (m_selected)
							{
								m_selected->OnUnselectInt(a_data);
							}

							m_selected = hoveredObject;
						}
					}
				}
			}

			for (auto& e : m_data)
			{
				e.object->DrawObjectExtra(a_data);
			}
		}

		void I3DIObjectController::RenderObjects(I3DICommonData& a_data)
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

			std::sort(
				m_drawQueueAlpha.begin(),
				m_drawQueueAlpha.end(),
				[&](auto& a_lhs, auto& a_rhs) {
					return a_lhs.first > a_rhs.first;
				});

			for (auto& e : m_drawQueueOpaque)
			{
				e.second->Draw(a_data.scene);
			}

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

				if (e.object->ObjectIntersects(a_data, dist))
				{
					e.dist = dist;
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
					return a_v.dist.has_value();
				});

			return it != m_data.end() ?
			           it->object :
                       nullptr;
		}
	}
}