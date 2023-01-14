#pragma once

#include "I3DIDragDropResult.h"

namespace IED
{
	class D3DObject;

	namespace UI
	{
		class I3DIDropTarget;
		class I3DIObjectController;
		class I3DIObject;
		struct I3DICommonData;

		enum class I3DIDraggableType
		{
			Dynamic,
			Static
		};

		class I3DIDraggable
		{
			friend class I3DIObjectController;

		public:
			I3DIDraggable(
				I3DIDraggableType a_type);

			virtual ~I3DIDraggable() noexcept = default;

			[[nodiscard]] constexpr bool IsDragging() const noexcept
			{
				return m_dragging;
			}

			[[nodiscard]] constexpr I3DIDraggableType GetDraggableType() const noexcept
			{
				return m_type;
			}

			virtual I3DIObject& GetDraggableObject() = 0;

		private:
			virtual bool OnDragBegin(I3DICommonData& a_data, ImGuiMouseButton a_button)   = 0;
			virtual void OnDragEnd(I3DIDragDropResult a_result, I3DIDropTarget* a_target) = 0;
			virtual void OnDragUpdate(I3DICommonData& a_data){};

			constexpr void SetDragging(bool a_switch) noexcept
			{
				m_dragging = a_switch;
			}

			I3DIDraggableType m_type{ I3DIDraggableType::Dynamic };
			bool              m_dragging{ false };
		};
	}
}