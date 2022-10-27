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

		class I3DIDraggable
		{
			friend class I3DIObjectController;

		public:
			inline static constexpr std::uint32_t DRAGGABLE_TYPE = static_cast<std::uint32_t>(-1);

			I3DIDraggable(
				std::uint32_t a_type);

			virtual ~I3DIDraggable() noexcept = default;

			[[nodiscard]] inline constexpr bool IsDragging() const noexcept
			{
				return m_dragging;
			}

			[[nodiscard]] inline constexpr std::uint32_t GetDraggableType() const noexcept
			{
				return m_type;
			}

			virtual I3DIObject& GetDraggableObject() = 0;

		private:
			virtual bool OnDragBegin() = 0;

			virtual void OnDragEnd(I3DIDragDropResult a_result, I3DIDropTarget* a_target) = 0;

			inline constexpr void SetDragging(bool a_switch) noexcept
			{
				m_dragging = a_switch;
			}

			std::uint32_t m_type{ DRAGGABLE_TYPE };
			bool          m_dragging{ false };

		};
	}
}