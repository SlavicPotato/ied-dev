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

			[[nodiscard]] inline constexpr auto& GetData() const noexcept
			{
				return m_data;
			}

			[[nodiscard]] inline constexpr auto& GetDragObject() const noexcept
			{
				return m_dragObject;
			}

			void RegisterObject(const std::shared_ptr<I3DIObject>& a_object);
			void UnregisterObject(const std::shared_ptr<I3DIObject>& a_object);

			//template <class T, class... Args>
			//auto& CreateObject(Args&&... a_args)  //
			//	requires(std::is_base_of_v<I3DIObject, T>)
			//{
			//	return m_data.emplace_back(std::make_shared<T>(std::forward<Args>(a_args)...)).object;
			//}

			void Run(I3DICommonData& a_data);
			void DrawObjects(I3DICommonData& a_data);

		protected:
			std::shared_ptr<I3DIObject> GetHovered(
				I3DICommonData& a_data);

			std::shared_ptr<I3DIObject> m_hovered;
			std::shared_ptr<I3DIObject> m_selected;

			std::shared_ptr<I3DIObject> m_dragObject;
			ImVec2                      m_lastClickPos{ -FLT_MAX, -FLT_MAX };
			DirectX::XMVECTOR           m_dragStartDist{ DirectX::g_XMZero.v };
			stl::vector<Entry>          m_data;

			using draw_queue_container_type = stl::vector<std::pair<float, I3DIModelObject*>>;

			draw_queue_container_type m_drawQueueOpaque;
			draw_queue_container_type m_drawQueueAlpha;

		private:
			bool ShouldProcessObject(I3DICommonData& a_data, I3DIObject* a_object);

			void SelectObject(
				I3DICommonData&                    a_data,
				const std::shared_ptr<I3DIObject>& a_object);

			void TryBeginDrag(
				I3DICommonData&                    a_data,
				const std::shared_ptr<I3DIObject>& a_object);

			std::pair<I3DIDragDropResult, I3DIDropTarget*> HandleDragEnd(
				I3DICommonData&                    a_data,
				I3DIDraggable*                     a_object,
				const std::shared_ptr<I3DIObject>& a_dropObject);

			void UpdateDragObjectPosition(
				I3DICommonData& a_data,
				I3DIObject*     a_object);
		};

	}
}