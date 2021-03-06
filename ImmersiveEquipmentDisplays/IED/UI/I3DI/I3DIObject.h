#pragma once

#include "I3DIObjectBase.h"

//#include "IED/D3D/D3DObject.h"

#include <ext/ILUID.h>

namespace IED
{
	class D3DCommon;

	namespace UI
	{
		class I3DIDraggable;
		class I3DIDropTarget;
		class I3DIObjectController;
		class I3DIModelObject;
		class I3DIActorObject;
		struct I3DICommonData;

		enum class I3DIObjectFlags : std::uint32_t
		{
			kNone = 0,

			kHovered  = 1u << 0,
			kSelected = 1u << 1,

			kHSMask = kHovered | kSelected,

			kHidden       = 1u << 2,
			kHasWorldData = 1u << 3
		};

		DEFINE_ENUM_CLASS_BITWISE(I3DIObjectFlags);

		class I3DIObject
		{
			friend class I3DIObjectController;

		public:
			I3DIObject() = default;

			virtual ~I3DIObject() noexcept = default;

			virtual I3DIDraggable* GetAsDraggable()
			{
				return nullptr;
			};

			virtual I3DIDropTarget* GetAsDropTarget()
			{
				return nullptr;
			};

			virtual I3DIModelObject* GetAsModelObject()
			{
				return nullptr;
			};

			virtual I3DIActorObject* GetAsActorObject()
			{
				return nullptr;
			};

			virtual void RenderObject(D3DCommon& a_data){};

			virtual void DrawObjectExtra(I3DICommonData& a_data){};

			virtual void OnMouseMoveOver(I3DICommonData& a_data){};
			virtual void OnMouseMoveOut(I3DICommonData& a_data){};
			virtual bool OnSelect(I3DICommonData& a_data) { return false; };
			virtual void OnUnselect(I3DICommonData& a_data){};
			virtual void OnClick(I3DICommonData& a_data){};

			virtual bool ObjectIntersects(
				I3DICommonData& a_data,
				float&          a_dist);

			[[nodiscard]] inline constexpr bool IsHovered() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kHovered);
			}

			[[nodiscard]] inline constexpr bool IsSelected() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kSelected);
			}

			[[nodiscard]] inline constexpr bool IsHidden() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kHidden);
			}

			[[nodiscard]] inline constexpr bool HasWorldData() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kHasWorldData);
			}

			[[nodiscard]] inline constexpr void SetHasWorldData(bool a_switch) noexcept
			{
				return m_objectFlags.set(I3DIObjectFlags::kHasWorldData, a_switch);
			}

		private:
			virtual void OnMouseMoveOverInt(I3DICommonData& a_data);
			virtual void OnMouseMoveOutInt(I3DICommonData& a_data);
			virtual bool OnSelectInt(I3DICommonData& a_data);
			virtual void OnUnselectInt(I3DICommonData& a_data);

			virtual bool IsSelectable() { return false; };

		protected:
			stl::flag<I3DIObjectFlags> m_objectFlags{ I3DIObjectFlags::kNone };
		};

	}
}