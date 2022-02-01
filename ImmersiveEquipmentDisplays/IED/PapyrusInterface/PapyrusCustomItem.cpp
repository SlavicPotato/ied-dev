#include "pch.h"

#include "../Controller/Controller.h"
#include "../FormCommon.h"
#include "../Main.h"
#include "../NodeMap.h"
#include "../StringHolder.h"

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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				TESForm* a_form,
				bool a_inventoryForm,
				BSFixedString a_node)
			{
				if (!a_target || !a_form)
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
				T* a_target,
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
				T* a_target,
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				int a_attachmentMode,
				bool a_syncReference)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				bool a_switch)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
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
					tmp[i] = std::clamp(Math::zero_nan(tmp[i]), -5000.0f, 5000.0f);
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
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
					tmp[i] = std::clamp(Math::zero_nan(tmp[i]), -360.0f, 360.0f) * (pi / 180.0f);
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				float a_scale)
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
					std::clamp(Math::zero_nan(a_scale), 0.01f, 100.0f));
			}

			template <class T>
			static bool SetItemInventory(
				StaticFunctionTag*,
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				bool a_switch)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				bool a_switch,
				bool a_ignoreRaceEquipTypes,
				bool a_disableIfEquipped)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				bool a_switch)
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
			static bool SetItemLoadChance(
				StaticFunctionTag*,
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				bool a_enable,
				float a_chance)
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
			static bool DoClearTransform(
				T* a_target,
				const BSFixedString& a_key,
				const BSFixedString& a_name,
				bool a_female,
				TransformClearFlags a_flags)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				TESForm* a_form)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				TESForm* a_form,
				std::int32_t a_index)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				TESForm* a_form)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				std::int32_t a_index)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				TESForm* a_form)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female)
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female,
				std::int32_t a_min,
				std::int32_t a_max)
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
				T* a_target,
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
				T* a_target,
				BSFixedString a_key,
				BSFixedString a_name,
				bool a_female)
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
						"IED",
						CreateItem<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*, bool, BSFixedString>(
						"CreateItemNPC",
						"IED",
						CreateItem<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*, bool, BSFixedString>(
						"CreateItemRace",
						"IED",
						CreateItem<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString>(
						"DeleteItemActor",
						"IED",
						DeleteItem<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString>(
						"DeleteItemNPC",
						"IED",
						DeleteItem<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString>(
						"DeleteItemRace",
						"IED",
						DeleteItem<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction2<StaticFunctionTag, bool, Actor*, BSFixedString>(
						"DeleteAllActor",
						"IED",
						DeleteAll<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction2<StaticFunctionTag, bool, TESNPC*, BSFixedString>(
						"DeleteAllNPC",
						"IED",
						DeleteAll<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction2<StaticFunctionTag, bool, TESRace*, BSFixedString>(
						"DeleteAllRace",
						"IED",
						DeleteAll<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction1<StaticFunctionTag, bool, BSFixedString>(
						"DeleteAll",
						"IED",
						DeleteAll,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, int, bool>(
						"SetItemAttachmentModeActor",
						"IED",
						SetItemAttachmentMode<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, int, bool>(
						"SetItemAttachmentModeNPC",
						"IED",
						SetItemAttachmentMode<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, int, bool>(
						"SetItemAttachmentModeRace",
						"IED",
						SetItemAttachmentMode<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemEnabledActor",
						"IED",
						SetItemEnabled<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemEnabledNPC",
						"IED",
						SetItemEnabled<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemEnabledRace",
						"IED",
						SetItemEnabled<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemNodeActor",
						"IED",
						SetItemNode<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemNodeNPC",
						"IED",
						SetItemNode<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, BSFixedString>(
						"SetItemNodeRace",
						"IED",
						SetItemNode<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemInventoryActor",
						"IED",
						SetItemInventory<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemInventoryNPC",
						"IED",
						SetItemInventory<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemInventoryRace",
						"IED",
						SetItemInventory<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool, bool, bool>(
						"SetItemEquipmentModeActor",
						"IED",
						SetItemEquipmentMode<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool, bool, bool>(
						"SetItemEquipmentModeNPC",
						"IED",
						SetItemEquipmentMode<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction7<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool, bool, bool>(
						"SetItemEquipmentModeRace",
						"IED",
						SetItemEquipmentMode<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemLeftWeaponActor",
						"IED",
						SetItemLeftWeapon<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemLeftWeaponNPC",
						"IED",
						SetItemLeftWeapon<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool>(
						"SetItemLeftWeaponRace",
						"IED",
						SetItemLeftWeapon<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, bool, float>(
						"SetItemLoadChanceActor",
						"IED",
						SetItemLoadChance<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, bool, float>(
						"SetItemLoadChanceNPC",
						"IED",
						SetItemLoadChance<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, bool, float>(
						"SetItemLoadChanceRace",
						"IED",
						SetItemLoadChance<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemPositionActor",
						"IED",
						SetItemPosition<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemPositionNPC",
						"IED",
						SetItemPosition<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemPositionRace",
						"IED",
						SetItemPosition<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemRotationActor",
						"IED",
						SetItemRotation<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemRotationNPC",
						"IED",
						SetItemRotation<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, VMArray<float>>(
						"SetItemRotationRace",
						"IED",
						SetItemRotation<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, float>(
						"SetItemScaleActor",
						"IED",
						SetItemScale<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, float>(
						"SetItemScaleNPC",
						"IED",
						SetItemScale<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, float>(
						"SetItemScaleRace",
						"IED",
						SetItemScale<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ClearItemPositionActor",
						"IED",
						ClearItemPosition<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ClearItemPositionNPC",
						"IED",
						ClearItemPosition<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ClearItemPositionRace",
						"IED",
						ClearItemPosition<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ClearItemRotationActor",
						"IED",
						ClearItemRotation<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ClearItemRotationNPC",
						"IED",
						ClearItemRotation<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ClearItemRotationRace",
						"IED",
						ClearItemRotation<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ClearItemScaleActor",
						"IED",
						ClearItemScale<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ClearItemScaleNPC",
						"IED",
						ClearItemScale<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ClearItemScaleRace",
						"IED",
						ClearItemScale<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemFormActor",
						"IED",
						SetItemForm<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemFormNPC",
						"IED",
						SetItemForm<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemFormRace",
						"IED",
						SetItemForm<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, TESForm*, std::int32_t>(
						"AddItemExtraFormActor",
						"IED",
						AddItemExtraForm<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*, std::int32_t>(
						"AddItemExtraFormNPC",
						"IED",
						AddItemExtraForm<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*, std::int32_t>(
						"AddItemExtraFormRace",
						"IED",
						AddItemExtraForm<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, TESForm*>(
						"RemoveItemExtraFormActor",
						"IED",
						RemoveItemExtraForm<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*>(
						"RemoveItemExtraFormNPC",
						"IED",
						RemoveItemExtraForm<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*>(
						"RemoveItemExtraFormRace",
						"IED",
						RemoveItemExtraForm<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, std::int32_t>(
						"RemoveItemExtraFormByIndexActor",
						"IED",
						RemovetemExtraFormByIndex<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, std::int32_t>(
						"RemoveItemExtraFormByIndexNPC",
						"IED",
						RemovetemExtraFormByIndex<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, std::int32_t>(
						"RemoveItemExtraFormByIndexRace",
						"IED",
						RemovetemExtraFormByIndex<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, std::int32_t, Actor*, BSFixedString, BSFixedString, bool>(
						"GetNumExtraFormsActor",
						"IED",
						GetNumExtraForms<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, std::int32_t, TESNPC*, BSFixedString, BSFixedString, bool>(
						"GetNumExtraFormsNPC",
						"IED",
						GetNumExtraForms<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, std::int32_t, TESRace*, BSFixedString, BSFixedString, bool>(
						"GetNumExtraFormsRace",
						"IED",
						GetNumExtraForms<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemModelSwapFormActor",
						"IED",
						SetItemModelSwapForm<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemModelSwapFormNPC",
						"IED",
						SetItemModelSwapForm<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction5<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, TESForm*>(
						"SetItemModelSwapFormRace",
						"IED",
						SetItemModelSwapForm<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ClearItemModelSwapFormActor",
						"IED",
						ClearItemModelSwapForm<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ClearItemModelSwapFormNPC",
						"IED",
						ClearItemModelSwapForm<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ClearItemModelSwapFormRace",
						"IED",
						ClearItemModelSwapForm<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool, std::int32_t, std::int32_t>(
						"SetItemCountRangeActor",
						"IED",
						SetItemCountRange<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool, std::int32_t, std::int32_t>(
						"SetItemCountRangeNPC",
						"IED",
						SetItemCountRange<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction6<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool, std::int32_t, std::int32_t>(
						"SetItemCountRangeRace",
						"IED",
						SetItemCountRange<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString>(
						"ItemExistsActor",
						"IED",
						ItemExists<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString>(
						"ItemExistsNPC",
						"IED",
						ItemExists<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction3<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString>(
						"ItemExistsRace",
						"IED",
						ItemExists<TESRace>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, Actor*, BSFixedString, BSFixedString, bool>(
						"ItemEnabledActor",
						"IED",
						ItemEnabled<Actor>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESNPC*, BSFixedString, BSFixedString, bool>(
						"ItemEnabledNPC",
						"IED",
						ItemEnabled<TESNPC>,
						a_registry));

				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, bool, TESRace*, BSFixedString, BSFixedString, bool>(
						"ItemEnabledRace",
						"IED",
						ItemEnabled<TESRace>,
						a_registry));

				return true;
			}
		}
	}
}