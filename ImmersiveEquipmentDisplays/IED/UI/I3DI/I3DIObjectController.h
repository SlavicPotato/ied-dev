#pragma once

#include "I3DIDragDropResult.h"

#include "Common/VectorMath.h"

namespace IED
{
	namespace UI
	{
		class I3DIObject;
		class I3DIModelObject;
		class I3DIDropTarget;
		class I3DIDraggable;
		struct I3DICommonData;

		class I3DIObjectController
		{
		public:
			struct Entry
			{
				inline Entry(const std::shared_ptr<I3DIObject>& a_object) :
					object(a_object)
				{
				}

				std::optional<float>        dist;
				std::shared_ptr<I3DIObject> object;
			};

			I3DIObjectController();

			[[nodiscard]] constexpr auto& GetData() const noexcept
			{
				return m_data;
			}

			[[nodiscard]] constexpr auto& GetDragContext() const noexcept
			{
				return m_dragContext;
			}

			void RegisterObject(const std::shared_ptr<I3DIObject>& a_object);
			void UnregisterObject(const std::shared_ptr<I3DIObject>& a_object);

			//template <class T, class... Args>
			//auto& CreateObject(Args&&... a_args)  //
			//	requires(std::is_base_of_v<I3DIObject, T>)
			//{
			//	return m_data.emplace_back(std::make_shared<T>(std::forward<Args>(a_args)...)).object;
			//}

			using run_func_t = std::function<void()>;

			void Run(I3DICommonData& a_data);
			void DrawObjects(I3DICommonData& a_data);

			[[nodiscard]] constexpr auto GetLastRunTime() const noexcept
			{
				return m_lastRunTime;
			}

		private:
			struct mouse_state_t
			{
				std::shared_ptr<I3DIObject> lastClickedObject;
				ImVec2                      lastClickPos{ -FLT_MAX, -FLT_MAX };
			};

			void ObjectControlInputHandler(
				I3DICommonData&                    a_data,
				const std::shared_ptr<I3DIObject>& a_hoveredObject,
				ImGuiMouseButton                   a_button);

			std::shared_ptr<I3DIObject> GetHovered(
				I3DICommonData& a_data);

			bool ShouldProcessObject(I3DICommonData& a_data, I3DIObject* a_object, bool a_allowParent);

			void SelectObject(
				I3DICommonData&                    a_data,
				const std::shared_ptr<I3DIObject>& a_object);
			
			void UnSelectObject(
				I3DICommonData&                    a_data,
				const std::shared_ptr<I3DIObject>& a_object);

			bool TryBeginDrag(
				I3DICommonData&                    a_data,
				ImGuiMouseButton                   a_button);

			std::pair<I3DIDragDropResult, I3DIDropTarget*> HandleDragEnd(
				I3DICommonData&                    a_data,
				const std::shared_ptr<I3DIObject>& a_dropObject);

			void UpdateDragObjectPosition(
				I3DICommonData& a_data,
				I3DIObject*     a_object);

			std::shared_ptr<I3DIObject> m_hovered;
			std::shared_ptr<I3DIObject> m_selected;

			struct drag_context_t
			{
				inline drag_context_t(
					ImGuiMouseButton                   a_button,
					const std::shared_ptr<I3DIObject>& a_object) :
					button(a_button),
					object(a_object)
				{
				}

				[[nodiscard]] inline bool operator==(
					const std::shared_ptr<I3DIObject>& a_rhs) const noexcept
				{
					return object == a_rhs;
				}

				ImGuiMouseButton            button;
				std::shared_ptr<I3DIObject> object;
				DirectX::XMVECTOR           objectPositionOffset{ DirectX::g_XMZero.v };
				float                       startDist{ 0.0f };
			};

			std::optional<drag_context_t> m_dragContext;

			stl::vector<Entry> m_data;

			using draw_queue_container_type = stl::vector<std::pair<float, I3DIModelObject*>>;

			draw_queue_container_type m_drawQueueOpaque;
			draw_queue_container_type m_drawQueueAlpha;

			std::array<mouse_state_t, 3> m_mouseState;

			PerfTimerInt m_runPT;
			long long    m_lastRunTime{ 0 };
		};

	}
}