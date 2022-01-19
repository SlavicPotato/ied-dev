#pragma once

#include "IED/ConfigCommon.h"

namespace IED
{
	namespace Data
	{
		enum class EquipmentOverrideFlags : std::uint32_t
		{
			kNone = 0,

			// unused
			kSlotItem = 1u << 0,
			kConditionInventory = 1u << 1,
			kMatchSlots = 1u << 2,
			kBothMustMatch = 1u << 3

		};

		DEFINE_ENUM_CLASS_BITWISE(EquipmentOverrideFlags);

		enum class EquipmentOverrideConditionFlags : std::uint32_t
		{
			kNone = 0,

			kTypeMask_V1 = 0x7u,
			kTypeMask_V2 = 0x1Fu,

			kAnd = 1u << 5,
			kNot = 1u << 6,

			kMatchEquipped = 1u << 7,
			kMatchSlots = 1u << 8,
			kMatchCategoryOperOR = 1u << 9,

			kMatchAll = kMatchEquipped | kMatchSlots,

			kLayingDown = 1u << 11,
			kMatchSkin = 1u << 12,

			kNegateMatch1 = 1u << 13,
			kNegateMatch2 = 1u << 14
		};

		DEFINE_ENUM_CLASS_BITWISE(EquipmentOverrideConditionFlags);

		enum class EquipmentOverrideConditionType : std::uint32_t
		{
			Form,
			Type,
			Keyword,
			Race,
			Furniture,
			BipedSlot
		};

		struct EquipmentOverrideConditionFlagsBitfield
		{
			EquipmentOverrideConditionType type: 5 { EquipmentOverrideConditionType::Form };
			std::uint32_t unused: 27 { 0 };
		};

		static_assert(sizeof(EquipmentOverrideConditionFlagsBitfield) == sizeof(std::uint32_t));

		struct equipmentOverrideCondition_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2
			};

			equipmentOverrideCondition_t() = default;

			equipmentOverrideCondition_t(
				EquipmentOverrideConditionType a_type,
				Game::FormID a_form)
			{
				if (a_type == EquipmentOverrideConditionType::Race)
				{
					form = a_form;
				}
				else if (a_type == EquipmentOverrideConditionType::Form)
				{
					form = a_form;
					flags = EquipmentOverrideConditionFlags::kMatchEquipped;
				}
				else if (a_type == EquipmentOverrideConditionType::Keyword)
				{
					keyword = a_form;
					flags = EquipmentOverrideConditionFlags::kMatchEquipped;
				}
				else
				{
					HALT("FIXME");
				}

				fbf.type = a_type;
			}

			equipmentOverrideCondition_t(
				Data::ObjectSlotExtra a_slot) :
				slot(a_slot),
				flags(EquipmentOverrideConditionFlags::kMatchEquipped)
			{
				fbf.type = EquipmentOverrideConditionType::Type;
			}
			
			equipmentOverrideCondition_t(
				Biped::BIPED_OBJECT a_slot) :
				bipedSlot(a_slot)
			{
				fbf.type = EquipmentOverrideConditionType::BipedSlot;
			}

			equipmentOverrideCondition_t(
				EquipmentOverrideConditionType a_matchType)
			{
				if (a_matchType == EquipmentOverrideConditionType::Furniture)
				{
					fbf.type = a_matchType;
				}
				else
				{
					HALT("FIXME");
				}
			}

			union
			{
				stl::flag<EquipmentOverrideConditionFlags> flags{ EquipmentOverrideConditionFlags::kNone };
				EquipmentOverrideConditionFlagsBitfield fbf;
			};

			configForm_t form;
			Data::ObjectSlotExtra slot{ Data::ObjectSlotExtra::kNone };
			configCachedForm_t keyword;
			std::uint32_t bipedSlot{ stl::underlying(Biped::kNone) };

		private:
			template <class Archive>
			void save(Archive& ar, const unsigned int version) const
			{
				ar& flags.value;
				ar& form;
				ar& slot;
				ar& keyword;
				ar& bipedSlot;
			}

			template <class Archive>
			void load(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& form;
				ar& slot;
				ar& keyword;

				if (version >= DataVersion2)
				{
					ar& bipedSlot;
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		using equipmentOverrideConditionList_t = std::vector<equipmentOverrideCondition_t>;

		struct equipmentOverride_t :
			public configBaseValues_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			equipmentOverride_t() = default;

			equipmentOverride_t(
				const configBaseValues_t& a_config,
				const std::string& a_desc) :
				configBaseValues_t(a_config),
				description(a_desc)
			{
			}

			equipmentOverride_t(
				const configBaseValues_t& a_config,
				std::string&& a_desc) :
				configBaseValues_t(a_config),
				description(std::move(a_desc))
			{
			}

			stl::flag<EquipmentOverrideFlags> eoFlags{ EquipmentOverrideFlags::kNone };
			equipmentOverrideConditionList_t conditions;
			std::string description;

		protected:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configBaseValues_t&>(*this);
				ar& eoFlags.value;
				ar& conditions;
				ar& description;
			}
		};

		using equipmentOverrideList_t = std::vector<equipmentOverride_t>;

	}
}

BOOST_CLASS_VERSION(
	IED::Data::equipmentOverride_t,
	IED::Data::equipmentOverride_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::equipmentOverrideCondition_t,
	IED::Data::equipmentOverrideCondition_t::Serialization::DataVersion2);
