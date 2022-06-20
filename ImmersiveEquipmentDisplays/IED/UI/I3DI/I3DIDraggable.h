#pragma once

#include "I3DIDragDropResult.h"

namespace IED
{
	class D3DObject;

	namespace UI
	{
		class I3DIDropTarget;
		class I3DIDragDropController;

		class I3DIDraggable
		{
			friend class I3DIDragDropController;

		public:
			inline static constexpr std::uint32_t DRAGGABLE_TYPE = static_cast<std::uint32_t>(-1);

			I3DIDraggable(
				std::uint32_t a_type,
				D3DObject&    a_owner);

			virtual ~I3DIDraggable() noexcept = default;

			[[nodiscard]] inline constexpr bool IsDragging() const noexcept
			{
				return m_dragging;
			}

			[[nodiscard]] inline constexpr std::uint32_t GetDraggableType() const noexcept
			{
				return m_type;
			}

		private:
			virtual bool OnDragBegin() = 0;

			virtual void OnDragEnd(I3DIDragDropResult a_result, I3DIDropTarget* a_target) = 0;

			std::uint32_t m_type{ DRAGGABLE_TYPE };
			bool          m_dragging{ false };

			// parent
			D3DObject& m_owner;
		};
	}
}