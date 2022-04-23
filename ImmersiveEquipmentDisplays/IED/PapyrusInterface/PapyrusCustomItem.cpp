#include "pch.h"

#include "IED/Controller/Controller.h"
#include "IED/FormCommon.h"
#include "IED/Main.h"
#include "IED/NodeMap.h"
#include "IED/StringHolder.h"

#include "PapyrusCommon.h"
#include "PapyrusCustomItem.h"
#include "PapyrusCustomItemCommon.h"
#include "PapyrusCustomItemImpl.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Custom
		{
			using namespace Data;

			template <class T>
			static bool CreateItem(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				TESForm*      a_form,
				bool          a_inventoryForm,
				BSFixedString a_node)
			{
				if (!a_target || !a_form)
				{
					return false;
				}

				if (!a_form->formID ||
				    a_form->formID.IsTemporary())
				{
					return false;
				}

				if (!IFormCommon::IsValidCustomForm(a_form))
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return CreateItemImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_form,
					a_inventoryForm,
					a_node);
			}

			template <class T>
			static bool DeleteItem(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return DeleteItemImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name);
			}

			template <class T>
			static bool DeleteAll(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key)
			{
				if (!a_target)
				{
					return false;
				}

				auto key = GetKey(a_key);
				if (key.empty())
				{
					return false;
				}

				return DeleteAllImpl(
					a_target->formID,
					GetConfigClass<T>(),
					key);
			}

			static bool DeleteAll(
				StaticFunctionTag*,
				BSFixedString a_key)
			{
				auto key = GetKey(a_key);
				if (key.empty())
				{
					return false;
				}

				return DeleteAllImpl(key);
			}

			template <class T>
			static bool SetItemAttachmentMode(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				int           a_attachmentMode,
				bool          a_syncReference)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemAttachmentModeImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_attachmentMode,
					a_syncReference);
			}

			template <class T>
			static bool SetItemEnabled(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				bool          a_switch)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemEnabledImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_switch);
			}

			template <class T>
			static bool SetItemNode(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				BSFixedString a_node)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemNodeImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_node);
			}

			template <class T>
			static bool SetItemPosition(
				StaticFunctionTag*,
				T*             a_target,
				BSFixedString  a_key,
				BSFixedString  a_name,
				bool           a_female,
				VMArray<float> a_pos)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				NiPoint3 tmp;
				if (!GetNiPoint3(a_pos, tmp))
				{
					return false;
				}

				for (std::uint32_t i = 0; i < 3; i++)
				{
					tmp[i] = std::clamp(stl::zero_nan(tmp[i]), -5000.0f, 5000.0f);
				}

				return SetItemPositionImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					tmp);
			}

			template <class T>
			static bool SetItemRotation(
				StaticFunctionTag*,
				T*             a_target,
				BSFixedString  a_key,
				BSFixedString  a_name,
				bool           a_female,
				VMArray<float> a_pos)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				NiPoint3 tmp;
				if (!GetNiPoint3(a_pos, tmp))
				{
					return false;
				}

				constexpr auto pi = std::numbers::pi_v<float>;

				for (std::uint32_t i = 0; i < 3; i++)
				{
					tmp[i] = std::clamp(stl::zero_nan(tmp[i]), -360.0f, 360.0f) * (pi / 180.0f);
				}

				return SetItemRotationImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					tmp);
			}

			template <class T>
			static bool SetItemScale(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				float         a_scale)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemScaleImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					std::clamp(stl::zero_nan(a_scale), 0.01f, 100.0f));
			}

			template <class T>
			static bool SetItemInventory(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				bool          a_switch)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemInventoryImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_switch);
			}

			template <class T>
			static bool SetItemEquipmentMode(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				bool          a_switch,
				bool          a_ignoreRaceEquipTypes,
				bool          a_disableIfEquipped)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemEquipmentModeImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_switch,
					a_ignoreRaceEquipTypes,
					a_disableIfEquipped);
			}

			template <class T>
			static bool SetItemLeftWeapon(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				bool          a_switch)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemLeftWeaponImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_switch);
			}

			template <class T>
			static bool SetItemUseWorldModel(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				bool          a_switch)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemUseWorldModelImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_switch);
			}

			template <class T>
			static bool SetIgnoreRaceEquipTypes(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				bool          a_switch)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetIgnoreRaceEquipTypesImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_switch);
			}

			template <class T>
			static bool SetItemLoadChance(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				bool          a_enable,
				float         a_chance)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemLoadChanceImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_enable,
					a_chance);
			}

			template <class T>
			static bool SetItemAnimationEnabled(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				bool          a_enable)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemAnimationEnabledImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_enable);
			}

			template <class T>
			static bool SetItemAnimationSequence(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				BSFixedString a_sequence)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				if (!ValidateString(a_sequence))
				{
					return false;
				}

				stl::fixed_string seq(a_sequence.c_str());

				return SetItemAnimationSequenceImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					seq);
			}
			template <class T>
			static bool SetItemAnimationEventEnabled(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				bool          a_enable)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemAnimationEventEnabledImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_enable);
			}

			template <class T>
			static bool SetItemAnimationEvent(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				BSFixedString a_event)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				if (!ValidateString(a_event))
				{
					return false;
				}

				stl::fixed_string ev(a_event.c_str());

				return SetItemAnimationEventImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					ev);
			}

			template <class T>
			static bool DoClearTransform(
				T*                   a_target,
				const BSFixedString& a_key,
				const BSFixedString& a_name,
				bool                 a_female,
				TransformClearFlags  a_flags)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return ClearItemTransformImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_flags);
			}

			template <class T>
			static bool ClearItemPosition(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female)
			{
				return DoClearTransform(
					a_target,
					a_key,
					a_name,
					a_female,
					TransformClearFlags::Position);
			}

			template <class T>
			static bool ClearItemRotation(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female)
			{
				return DoClearTransform(
					a_target,
					a_key,
					a_name,
					a_female,
					TransformClearFlags::Rotation);
			}

			template <class T>
			static bool ClearItemScale(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female)
			{
				return DoClearTransform(
					a_target,
					a_key,
					a_name,
					a_female,
					TransformClearFlags::Scale);
			}

			template <class T>
			static bool SetItemForm(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				TESForm*      a_form)
			{
				if (!a_target || !a_form)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemFormImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_form);
			}

			template <class T>
			static bool AddItemExtraForm(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				TESForm*      a_form,
				std::int32_t  a_index)
			{
				if (!a_target || !a_form)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return AddItemExtraFormImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_form,
					a_index);
			}

			template <class T>
			static bool RemoveItemExtraForm(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				TESForm*      a_form)
			{
				if (!a_target || !a_form)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return RemoveItemExtraFormImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_form);
			}

			template <class T>
			static bool RemovetemExtraFormByIndex(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				std::int32_t  a_index)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return RemoveItemExtraFormImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_index);
			}

			template <class T>
			static std::int32_t GetNumExtraForms(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return GetNumExtraFormsImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female));
			}

			template <class T>
			static bool SetItemModelSwapForm(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				TESForm*      a_form)
			{
				if (!a_target || !a_form)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemModelSwapFormImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_form);
			}

			template <class T>
			static bool ClearItemModelSwapForm(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return ClearItemModelSwapFormImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female));
			}

			template <class T>
			static bool SetItemCountRange(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female,
				std::int32_t  a_min,
				std::int32_t  a_max)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return SetItemCountRangeImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female),
					a_min,
					a_max);
			}

			template <class T>
			static bool ItemExists(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return ItemExistsImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name);
			}

			template <class T>
			static bool ItemEnabled(
				StaticFunctionTag*,
				T*            a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool          a_female)
			{
				if (!a_target)
				{
					return false;
				}

				auto keys = GetKeys(a_key, a_name);
				if (!keys)
				{
					return false;
				}

				return ItemEnabledImpl(
					a_target->formID,
					GetConfigClass<T>(),
					keys.key,
					keys.name,
					GetSex(a_female));
			}

			bool Register(VMClassRegistry* a_registry)
			{
				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, TESForm*, bool, BSFixedString>(
						"CreateItemActor",
						SCRIPT_NAME,
						CreateItem,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*, bool, BSFixedString>(
						"CreateItemNPC",
						SCRIPT_NAME,
						CreateItem,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*, bool, BSFixedString>(
						"CreateItemRace",
						SCRIPT_NAME,
						CreateItem,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString>(
						"DeleteItemActor",
						SCRIPT_NAME,
						DeleteItem,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString>(
						"DeleteItemNPC",
						SCRIPT_NAME,
						DeleteItem,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString>(
						"DeleteItemRace",
						SCRIPT_NAME,
						DeleteItem,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction2<StaticFunctionTag, bool, Actor*, BSFixedString>(
						"DeleteAllActor",
						SCRIPT_NAME,
						DeleteAll,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction2<StaticFunctionTag, bool, TESNPC*, BSFixedString>(
						"DeleteAllNPC",
						SCRIPT_NAME,
						DeleteAll,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction2<StaticFunctionTag, bool, TESRace*, BSFixedString>(
						"DeleteAllRace",
						SCRIPT_NAME,
						DeleteAll,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction1<StaticFunctionTag, bool, BSFixedString>(
						"DeleteAll",
						SCRIPT_NAME,
						DeleteAll,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, int, bool>(
						"SetItemAttachmentModeActor",
						SCRIPT_NAME,
						SetItemAttachmentMode,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, int, bool>(
						"SetItemAttachmentModeNPC",
						SCRIPT_NAME,
						SetItemAttachmentMode,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, int, bool>(
						"SetItemAttachmentModeRace",
						SCRIPT_NAME,
						SetItemAttachmentMode,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemEnabledActor",
						SCRIPT_NAME,
						SetItemEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemEnabledNPC",
						SCRIPT_NAME,
						SetItemEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemEnabledRace",
						SCRIPT_NAME,
						SetItemEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemNodeActor",
						SCRIPT_NAME,
						SetItemNode,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemNodeNPC",
						SCRIPT_NAME,
						SetItemNode,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemNodeRace",
						SCRIPT_NAME,
						SetItemNode,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemInventoryActor",
						SCRIPT_NAME,
						SetItemInventory,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemInventoryNPC",
						SCRIPT_NAME,
						SetItemInventory,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemInventoryRace",
						SCRIPT_NAME,
						SetItemInventory,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool, bool, bool>(
						"SetItemEquipmentModeActor",
						SCRIPT_NAME,
						SetItemEquipmentMode,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool, bool, bool>(
						"SetItemEquipmentModeNPC",
						SCRIPT_NAME,
						SetItemEquipmentMode,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool, bool, bool>(
						"SetItemEquipmentModeRace",
						SCRIPT_NAME,
						SetItemEquipmentMode,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemLeftWeaponActor",
						SCRIPT_NAME,
						SetItemLeftWeapon,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemLeftWeaponNPC",
						SCRIPT_NAME,
						SetItemLeftWeapon,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemLeftWeaponRace",
						SCRIPT_NAME,
						SetItemLeftWeapon,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemUseWorldModelActor",
						SCRIPT_NAME,
						SetItemUseWorldModel,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemUseWorldModelNPC",
						SCRIPT_NAME,
						SetItemUseWorldModel,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemUseWorldModelRace",
						SCRIPT_NAME,
						SetItemUseWorldModel,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetIgnoreRaceEquipTypesActor",
						SCRIPT_NAME,
						SetIgnoreRaceEquipTypes,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetIgnoreRaceEquipTypesNPC",
						SCRIPT_NAME,
						SetIgnoreRaceEquipTypes,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetIgnoreRaceEquipTypesRace",
						SCRIPT_NAME,
						SetIgnoreRaceEquipTypes,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool, float>(
						"SetItemLoadChanceActor",
						SCRIPT_NAME,
						SetItemLoadChance,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool, float>(
						"SetItemLoadChanceNPC",
						SCRIPT_NAME,
						SetItemLoadChance,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool, float>(
						"SetItemLoadChanceRace",
						SCRIPT_NAME,
						SetItemLoadChance,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemAnimationEnabledActor",
						SCRIPT_NAME,
						SetItemAnimationEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemAnimationEnabledNPC",
						SCRIPT_NAME,
						SetItemAnimationEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemAnimationEnabledRace",
						SCRIPT_NAME,
						SetItemAnimationEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemAnimationSequenceActor",
						SCRIPT_NAME,
						SetItemAnimationSequence,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemAnimationSequenceNPC",
						SCRIPT_NAME,
						SetItemAnimationSequence,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemAnimationSequenceRace",
						SCRIPT_NAME,
						SetItemAnimationSequence,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemAnimationEventEnabledActor",
						SCRIPT_NAME,
						SetItemAnimationEventEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemAnimationEventEnabledNPC",
						SCRIPT_NAME,
						SetItemAnimationEventEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemAnimationEventEnabledRace",
						SCRIPT_NAME,
						SetItemAnimationEventEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemAnimationEventActor",
						SCRIPT_NAME,
						SetItemAnimationEvent,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemAnimationEventNPC",
						SCRIPT_NAME,
						SetItemAnimationEvent,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemAnimationEventRace",
						SCRIPT_NAME,
						SetItemAnimationEvent,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemPositionActor",
						SCRIPT_NAME,
						SetItemPosition,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemPositionNPC",
						SCRIPT_NAME,
						SetItemPosition,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemPositionRace",
						SCRIPT_NAME,
						SetItemPosition,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemRotationActor",
						SCRIPT_NAME,
						SetItemRotation,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemRotationNPC",
						SCRIPT_NAME,
						SetItemRotation,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemRotationRace",
						SCRIPT_NAME,
						SetItemRotation,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, float>(
						"SetItemScaleActor",
						SCRIPT_NAME,
						SetItemScale,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, float>(
						"SetItemScaleNPC",
						SCRIPT_NAME,
						SetItemScale,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, float>(
						"SetItemScaleRace",
						SCRIPT_NAME,
						SetItemScale,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ClearItemPositionActor",
						SCRIPT_NAME,
						ClearItemPosition,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ClearItemPositionNPC",
						SCRIPT_NAME,
						ClearItemPosition,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ClearItemPositionRace",
						SCRIPT_NAME,
						ClearItemPosition,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ClearItemRotationActor",
						SCRIPT_NAME,
						ClearItemRotation,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ClearItemRotationNPC",
						SCRIPT_NAME,
						ClearItemRotation,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ClearItemRotationRace",
						SCRIPT_NAME,
						ClearItemRotation,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ClearItemScaleActor",
						SCRIPT_NAME,
						ClearItemScale,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ClearItemScaleNPC",
						SCRIPT_NAME,
						ClearItemScale,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ClearItemScaleRace",
						SCRIPT_NAME,
						ClearItemScale,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemFormActor",
						SCRIPT_NAME,
						SetItemForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemFormNPC",
						SCRIPT_NAME,
						SetItemForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemFormRace",
						SCRIPT_NAME,
						SetItemForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, TESForm*, std::int32_t>(
						"AddItemExtraFormActor",
						SCRIPT_NAME,
						AddItemExtraForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*, std::int32_t>(
						"AddItemExtraFormNPC",
						SCRIPT_NAME,
						AddItemExtraForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*, std::int32_t>(
						"AddItemExtraFormRace",
						SCRIPT_NAME,
						AddItemExtraForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, TESForm*>(
						"RemoveItemExtraFormActor",
						SCRIPT_NAME,
						RemoveItemExtraForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*>(
						"RemoveItemExtraFormNPC",
						SCRIPT_NAME,
						RemoveItemExtraForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*>(
						"RemoveItemExtraFormRace",
						SCRIPT_NAME,
						RemoveItemExtraForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, std::int32_t>(
						"RemoveItemExtraFormByIndexActor",
						SCRIPT_NAME,
						RemovetemExtraFormByIndex,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, std::int32_t>(
						"RemoveItemExtraFormByIndexNPC",
						SCRIPT_NAME,
						RemovetemExtraFormByIndex,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, std::int32_t>(
						"RemoveItemExtraFormByIndexRace",
						SCRIPT_NAME,
						RemovetemExtraFormByIndex,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, std::int32_t, Actor*, BSFixedString, BSFixedString, bool>(
						"GetNumExtraFormsActor",
						SCRIPT_NAME,
						GetNumExtraForms,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, std::int32_t, TESNPC*, BSFixedString, BSFixedString, bool>(
						"GetNumExtraFormsNPC",
						SCRIPT_NAME,
						GetNumExtraForms,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, std::int32_t, TESRace*, BSFixedString, BSFixedString, bool>(
						"GetNumExtraFormsRace",
						SCRIPT_NAME,
						GetNumExtraForms,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemModelSwapFormActor",
						SCRIPT_NAME,
						SetItemModelSwapForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemModelSwapFormNPC",
						SCRIPT_NAME,
						SetItemModelSwapForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemModelSwapFormRace",
						SCRIPT_NAME,
						SetItemModelSwapForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ClearItemModelSwapFormActor",
						SCRIPT_NAME,
						ClearItemModelSwapForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ClearItemModelSwapFormNPC",
						SCRIPT_NAME,
						ClearItemModelSwapForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ClearItemModelSwapFormRace",
						SCRIPT_NAME,
						ClearItemModelSwapForm,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, std::int32_t, std::int32_t>(
						"SetItemCountRangeActor",
						SCRIPT_NAME,
						SetItemCountRange,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, std::int32_t, std::int32_t>(
						"SetItemCountRangeNPC",
						SCRIPT_NAME,
						SetItemCountRange,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, std::int32_t, std::int32_t>(
						"SetItemCountRangeRace",
						SCRIPT_NAME,
						SetItemCountRange,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString>(
						"ItemExistsActor",
						SCRIPT_NAME,
						ItemExists,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString>(
						"ItemExistsNPC",
						SCRIPT_NAME,
						ItemExists,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString>(
						"ItemExistsRace",
						SCRIPT_NAME,
						ItemExists,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ItemEnabledActor",
						SCRIPT_NAME,
						ItemEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ItemEnabledNPC",
						SCRIPT_NAME,
						ItemEnabled,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ItemEnabledRace",
						SCRIPT_NAME,
						ItemEnabled,
						a_registry));

				return true;
			}
		}
	}
}