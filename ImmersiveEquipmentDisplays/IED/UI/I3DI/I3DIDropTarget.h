#pragma once

#include "IED/D3D/D3DObject.h"

namespace IED
{
	class D3DObject;

	namespace UI
	{
		class I3DIDraggable;
		class I3DIDragDropController;

		class I3DIDropTarget
		{
			friend class I3DIDragDropController;

		public:
			I3DIDropTarget(
				D3DObject& a_owner);

			virtual ~I3DIDropTarget() noexcept = default;

			[[nodiscard]] inline constexpr bool IsHovered() const noexcept
			{
				return m_isHovered;
			}

		private:
			virtual bool AcceptsDraggable(I3DIDraggable& a_item)   = 0;
			virtual bool ProcessDropRequest(I3DIDraggable& a_item) = 0;

			bool m_isHovered{ false };

			// parent
			D3DObject& m_owner;
		};
	}
}