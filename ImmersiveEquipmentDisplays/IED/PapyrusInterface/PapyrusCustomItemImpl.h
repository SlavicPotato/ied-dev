#pragma once

#include "IED/ConfigCommon.h"

#include "PapyrusCustomItemCommon.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Custom
		{
			bool CreateItemImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				TESForm*                 a_form,
				bool                     a_inventoryForm,
				const BSFixedString&     a_node);

			bool DeleteItemImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name);

			bool DeleteAllImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key);

			bool DeleteAllImpl(
				const stl::fixed_string& a_key);

			bool SetItemAttachmentModeImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				int                      a_attachmentMode,
				bool                     a_syncReference);

			bool SetItemEnabledImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch);

			bool SetItemNodeImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				const BSFixedString&     a_node);

			bool SetItemPositionImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				const NiPoint3&          a_position);

			bool SetItemRotationImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				const NiPoint3&          a_rotation);

			bool SetItemScaleImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				float                    a_scale);

			bool ClearItemTransformImpl(
				Game::FormID                   a_target,
				Data::ConfigClass              a_class,
				const stl::fixed_string&       a_key,
				const stl::fixed_string&       a_name,
				Data::ConfigSex                a_sex,
				stl::flag<TransformClearFlags> a_flags);

			bool SetItemInventoryImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch);

			bool SetItemFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				TESForm*                 a_form);

			bool AddItemExtraFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				TESForm*                 a_form,
				std::int32_t             a_index);

			bool RemoveItemExtraFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				TESForm*                 a_form);

			bool RemoveItemExtraFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				std::int32_t             a_index);

			std::int32_t GetNumExtraFormsImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex);

			bool SetItemModelSwapFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				TESForm*                 a_form);

			bool ClearItemModelSwapFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex);

			bool SetItemCountRangeImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				std::int32_t             a_min,
				std::int32_t             a_max);

			bool SetItemEquipmentModeImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch,
				bool                     a_ignoreRaceEquipTypes,
				bool                     a_disableIfEquipped);

			bool SetItemLeftWeaponImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch);
			
			bool SetItemRemoveScabbardImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch);

			bool SetItemUseWorldModelImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch);

			bool SetIgnoreRaceEquipTypesImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch);

			bool SetItemLoadChanceImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_enable,
				float                    a_chance);

			bool SetItemAnimationEnabledImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_enable);

			bool SetItemAnimationSequenceImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				const stl::fixed_string& a_sequence);

			bool SetItemWeaponAnimationDisabledImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_disable);
			
			bool SetItemAnimationEventEnabledImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_enable);

			bool SetItemAnimationEventImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				const stl::fixed_string& a_event);

			bool SetItemDisableHavokImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_disable);
			
			bool SetItemRemoveTracersImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_disable);
			
			bool SetItemAttachLightImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch);

			bool ItemExistsImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name);

			bool ItemEnabledImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex);
		}
	}
}