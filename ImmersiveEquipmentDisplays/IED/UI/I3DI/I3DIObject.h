#pragma once

namespace IED
{
	class D3DCommon;
	class D3DBoundingOrientedBox;

	namespace UI
	{
		class I3DIDraggable;
		class I3DIDropTarget;
		class I3DIObjectController;
		class I3DIModelObject;
		class I3DIBoundObject;
		class I3DIActorObject;
		class I3DIMOVNode;
		class I3DIWeaponNode;
		struct I3DICommonData;
		struct I3DIRay;

		enum class I3DIObjectFlags : std::uint32_t
		{
			kNone = 0,

			kHovered               = 1u << 0,
			kSelected              = 1u << 1,
			kHideOtherWhenSelected = 1u << 2,
			kDisabled              = 1u << 3,

			kHSMask = kHovered | kSelected,

			kGeometryHidden = 1u << 10,
			kHasWorldData   = 1u << 11,

		};

		DEFINE_ENUM_CLASS_BITWISE(I3DIObjectFlags);

		class I3DIObject
		{
			friend class I3DIObjectController;

		public:
			I3DIObject() = default;

			virtual ~I3DIObject() noexcept = default;

			virtual I3DIDraggable* AsDraggable()
			{
				return nullptr;
			};

			virtual I3DIDropTarget* AsDropTarget()
			{
				return nullptr;
			};

			virtual I3DIModelObject* AsModelObject()
			{
				return nullptr;
			};

			virtual I3DIBoundObject* AsBoundObject()
			{
				return nullptr;
			};

			virtual I3DIActorObject* AsActorObject()
			{
				return nullptr;
			};

			virtual I3DIMOVNode* AsMOVNode()
			{
				return nullptr;
			};

			virtual I3DIWeaponNode* AsWeaponNode()
			{
				return nullptr;
			};

			virtual void RenderObject(D3DCommon& a_data){};

			virtual void DrawObjectExtra(I3DICommonData& a_data){};

			virtual void OnMouseMoveOver(I3DICommonData& a_data){};
			virtual void OnMouseMoveOut(I3DICommonData& a_data){};
			virtual bool OnSelect(I3DICommonData& a_data) { return false; };
			virtual void OnUnselect(I3DICommonData& a_data){};
			virtual void OnMouseUp(I3DICommonData& a_data, ImGuiMouseButton a_button){};
			virtual void OnMouseDown(I3DICommonData& a_data, ImGuiMouseButton a_button){};
			//virtual void OnMouseHeld(I3DICommonData& a_data, ImGuiMouseButton a_button){};
			//virtual void OnMousePressLost(I3DICommonData& a_data, ImGuiMouseButton a_button, bool a_released){};

			virtual bool ObjectIntersects(
				I3DICommonData& a_data,
				const I3DIRay&  a_ray,
				float&          a_dist);

			virtual void OnObjectRegistered(I3DIObjectController& a_data){};
			virtual void OnObjectUnregistered(I3DIObjectController& a_data){};

			virtual bool ShouldProcess(I3DICommonData& a_data) { return true; };

			virtual void                 SetLastDistance(const std::optional<float>& a_distance){};
			virtual std::optional<float> GetLastDistance() const { return {}; };

			[[nodiscard]] constexpr bool IsHovered() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kHovered);
			}

			[[nodiscard]] constexpr bool IsSelected() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kSelected);
			}
			
			[[nodiscard]] constexpr bool IsDisabled() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kDisabled);
			}

			[[nodiscard]] constexpr bool HideOtherWhenSelected() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kHideOtherWhenSelected);
			}

			[[nodiscard]] constexpr bool IsGeometryHidden() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kGeometryHidden);
			}

			[[nodiscard]] constexpr bool HasWorldData() const noexcept
			{
				return m_objectFlags.test(I3DIObjectFlags::kHasWorldData);
			}

			[[nodiscard]] constexpr void SetHasWorldData(bool a_switch) noexcept
			{
				return m_objectFlags.set(I3DIObjectFlags::kHasWorldData, a_switch);
			}
			
			[[nodiscard]] constexpr void SetDisabled(bool a_switch) noexcept
			{
				return m_objectFlags.set(I3DIObjectFlags::kDisabled, a_switch);
			}

			constexpr void SetGeometryHidden(bool a_switch) noexcept
			{
				m_objectFlags.set(I3DIObjectFlags::kGeometryHidden, a_switch);
			}

		private:
			virtual bool OnSelectInt(I3DICommonData& a_data);
			virtual void OnUnselectInt(I3DICommonData& a_data);

			virtual bool IsSelectable() { return false; };

		protected:
			stl::flag<I3DIObjectFlags> m_objectFlags{ I3DIObjectFlags::kNone };
		};

	}
}