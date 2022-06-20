#pragma once

#include "IED/D3D/D3DObject.h"

namespace IED
{
	namespace UI
	{
		class I3DIDraggable;
		class I3DIDropTarget;
		class I3DIObjectController;

		enum class I3DIObjectFlags : std::uint32_t
		{
			kNone = 0,

			kHovered  = 1u << 0,
			kSelected = 1u << 1,

			kHSMask = kHovered | kSelected
		};

		DEFINE_ENUM_CLASS_BITWISE(I3DIObjectFlags);

		class I3DIObject :
			public D3DObject
		{
			friend class I3DIObjectController;

		public:
			using D3DObject::D3DObject;

			virtual I3DIDraggable* GetAsDraggable()
			{
				return nullptr;
			};

			virtual I3DIDropTarget* GetAsDropTarget()
			{
				return nullptr;
			};

			virtual void OnMouseMoveOver()
			{
			}

			virtual void OnMouseMoveOut()
			{
			}

			virtual void OnSelect()
			{
			}

			virtual void OnUnselect()
			{
			}

		private:
			virtual void OnMouseMoveOverInt()
			{
				m_flags.set(I3DIObjectFlags::kHovered);

				m_effect->SetAlpha(1.0f);
			}

			virtual void OnMouseMoveOutInt()
			{
				m_flags.clear(I3DIObjectFlags::kHovered);

				if (!m_flags.test_any(I3DIObjectFlags::kHSMask))
				{
					m_effect->SetAlpha(0.5f);
				}
				
			}

			virtual void OnSelectInt()
			{
				m_flags.set(I3DIObjectFlags::kSelected);

				m_effect->SetAlpha(1.0f);
			}

			virtual void OnUnselectInt()
			{
				m_flags.clear(I3DIObjectFlags::kSelected);

				if (!m_flags.test_any(I3DIObjectFlags::kHSMask))
				{
					m_effect->SetAlpha(0.5f);
				}
			}

			stl::flag<I3DIObjectFlags> m_flags{ I3DIObjectFlags::kNone };
		};

	}
}