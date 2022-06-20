#include "pch.h"

#include "I3DIObjectController.h"

#include "I3DICommonData.h"
#include "I3DIInputHelpers.h"
#include "I3DIObject.h"

namespace IED
{
	namespace UI
	{
		void I3DIObjectController::Update(I3DICommonData& a_data)
		{
			auto hoveredObject = GetHovered(a_data.ray.origin, a_data.ray.dir);

			if (hoveredObject != m_hovered)
			{
				if (m_hovered)
				{
					m_hovered->OnMouseMoveOutInt();
				}

				if (hoveredObject)
				{
					hoveredObject->OnMouseMoveOverInt();
				}

				m_hovered = hoveredObject;
			}

			if (I3DI::IsMouseReleased())
			{
				if (hoveredObject != m_selected)
				{
					if (m_selected)
					{
						m_selected->OnUnselectInt();
					}

					if (hoveredObject)
					{
						hoveredObject->OnSelectInt();
					}

					m_selected = hoveredObject;
				}
			}
		}

		I3DIObject* XM_CALLCONV I3DIObjectController::GetHovered(
			DirectX::XMVECTOR a_rayOrigin,
			DirectX::XMVECTOR a_rayDir)
		{
			auto& io = ImGui::GetIO();

			if (io.WantCaptureMouse)
			{
				return nullptr;
			}

			for (auto& e : m_data)
			{
				float dist;

				if (e.object->Intersects(a_rayOrigin, a_rayDir, dist))
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