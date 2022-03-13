#pragma once

#include "ConfigBaseValues.h"
#include "ConfigCommon.h"
#include "ConfigData.h"

#include "WeatherClassificationFlags.h"

namespace IED
{
	namespace Data
	{
		enum class EquipmentOverrideFlags : std::uint32_t
		{
			kNone = 0,
		};

		DEFINE_ENUM_CLASS_BITWISE(EquipmentOverrideFlags);

		enum class EquipmentOverrideConditionFlags : std::uint32_t
		{
			kNone = 0,

			kTypeMask_V1 = 0x7u,
			kTypeMask_V2 = 0x1Fu,

			kAnd = 1u << 5,
			kNot = 1u << 6,

			kMatchEquipped       = 1u << 7,
			kMatchSlots          = 1u << 8,
			kMatchCategoryOperOR = 1u << 9,

			kMatchAll = kMatchEquipped | kMatchSlots,

			// laying down (Furniture), loc child (Location), match parent (Worldspace), playable (Race), is bolt (Biped)
			kExtraFlag1 = 1u << 11,

			// match skin (Biped), is child (Race)
			kExtraFlag2 = 1u << 12,

			kNegateMatch1 = 1u << 13,
			kNegateMatch2 = 1u << 14,
			kNegateMatch3 = 1u << 15,
			kNegateMatch4 = 1u << 16,

			//kMatchTemplate = 1u << 30
		};

		DEFINE_ENUM_CLASS_BITWISE(EquipmentOverrideConditionFlags);

		enum class EquipmentOverrideConditionType : std::uint32_t
		{
			Form,
			Type,
			Keyword,
			Race,
			Furniture,
			BipedSlot,
			Group,
			Quest,
			Actor,
			NPC,
			Extra,
			Location,
			Worldspace,
			Package,
			Weather
		};

		struct EquipmentOverrideConditionFlagsBitfield
		{
			EquipmentOverrideConditionType type  : 5 { EquipmentOverrideConditionType::Form };
			std::uint32_t                  unused: 27 { 0 };
		};

		static_assert(sizeof(EquipmentOverrideConditionFlagsBitfield) == sizeof(EquipmentOverrideConditionFlags));

		struct equipmentOverrideCondition_t;

		enum class EquipmentOverrideConditionGroupFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(EquipmentOverrideConditionGroupFlags);

		using equipmentOverrideConditionList_t = std::vector<equipmentOverrideCondition_t>;

		struct equipmentOverrideConditionGroup_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<EquipmentOverrideConditionGroupFlags> flags{ EquipmentOverrideConditionGroupFlags::kNone };
			equipmentOverrideConditionList_t                conditions;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& conditions;
			}
		};

		struct equipmentOverrideCondition_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
				DataVersion3 = 3,
			};

			inline static constexpr auto DEFAULT_MATCH_CATEGORY_FLAGS =
				EquipmentOverrideConditionFlags::kMatchEquipped |
				EquipmentOverrideConditionFlags::kMatchCategoryOperOR;

			equipmentOverrideCondition_t() = default;

			inline equipmentOverrideCondition_t(
				EquipmentOverrideConditionType a_type,
				Game::FormID                   a_form)
			{
				switch (a_type)
				{
				case EquipmentOverrideConditionType::Race:
				case EquipmentOverrideConditionType::Actor:
				case EquipmentOverrideConditionType::NPC:
					form = a_form;
					break;
				case EquipmentOverrideConditionType::Form:
					form  = a_form;
					flags = DEFAULT_MATCH_CATEGORY_FLAGS;
					break;
				case EquipmentOverrideConditionType::Quest:
					keyword       = a_form;
					questCondType = QuestConditionType::kComplete;
					break;
				case EquipmentOverrideConditionType::Keyword:
					keyword = a_form;
					flags   = DEFAULT_MATCH_CATEGORY_FLAGS;
					break;
				default:
					HALT("FIXME");
					break;
				}

				fbf.type = a_type;
			}

			inline equipmentOverrideCondition_t(
				Data::ObjectSlotExtra a_slot) :
				slot(a_slot),
				flags(DEFAULT_MATCH_CATEGORY_FLAGS)
			{
				fbf.type = EquipmentOverrideConditionType::Type;
			}

			inline equipmentOverrideCondition_t(
				BIPED_OBJECT a_slot) :
				bipedSlot(a_slot)
			{
				fbf.type = EquipmentOverrideConditionType::BipedSlot;
			}

			inline equipmentOverrideCondition_t(
				ExtraConditionType a_type) :
				extraCondType(a_type)
			{
				fbf.type = EquipmentOverrideConditionType::Extra;
			}

			inline equipmentOverrideCondition_t(
				EquipmentOverrideConditionType a_matchType)
			{
				switch (a_matchType)
				{
				case EquipmentOverrideConditionType::Race:
				case EquipmentOverrideConditionType::Furniture:
				case EquipmentOverrideConditionType::Group:
				case EquipmentOverrideConditionType::Location:
				case EquipmentOverrideConditionType::Worldspace:
				case EquipmentOverrideConditionType::Package:
				case EquipmentOverrideConditionType::Weather:

					if (a_matchType == EquipmentOverrideConditionType::Location ||
					    a_matchType == EquipmentOverrideConditionType::Worldspace)
					{
						flags = EquipmentOverrideConditionFlags::kExtraFlag1;
					}

					fbf.type = a_matchType;

					break;

				default:
					HALT("FIXME");
					break;
				}
			}

			union
			{
				stl::flag<EquipmentOverrideConditionFlags> flags{ EquipmentOverrideConditionFlags::kNone };
				EquipmentOverrideConditionFlagsBitfield    fbf;
			};

			configCachedForm_t    form;
			configCachedForm_t    keyword;
			Data::ObjectSlotExtra slot{ Data::ObjectSlotExtra::kNone };

			union
			{
				std::uint32_t              ui32a{ static_cast<std::uint32_t>(-1) };
				QuestConditionType         questCondType;
				ExtraConditionType         extraCondType;
				BIPED_OBJECT               bipedSlot;
				PACKAGE_PROCEDURE_TYPE     procedureType;
				WeatherClassificationFlags weatherClass;

				static_assert(std::is_same_v<std::underlying_type_t<BIPED_OBJECT>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<PACKAGE_PROCEDURE_TYPE>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<WeatherClassificationFlags>, std::uint32_t>);
			};

			equipmentOverrideConditionGroup_t group;

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar&        flags.value;
				configForm_t tmp = form.get_id();
				a_ar&        tmp;
				a_ar&        slot;
				a_ar&        keyword;
				a_ar&        ui32a;
				a_ar&        group;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar&        flags.value;
				configForm_t tmp;
				a_ar&        tmp;
				form = tmp;
				a_ar& slot;
				a_ar& keyword;

				if (a_version >= DataVersion2)
				{
					a_ar& ui32a;

					if (a_version >= DataVersion3)
					{
						a_ar& group;
					}
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

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
				const std::string&        a_desc) :
				configBaseValues_t(a_config),
				description(a_desc)
			{
			}

			equipmentOverride_t(
				const configBaseValues_t& a_config,
				std::string&&             a_desc) :
				configBaseValues_t(a_config),
				description(std::move(a_desc))
			{
			}

			stl::flag<EquipmentOverrideFlags> eoFlags{ EquipmentOverrideFlags::kNone };
			equipmentOverrideConditionList_t  conditions;
			std::string                       description;

		protected:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configBaseValues_t&>(*this);
				a_ar& eoFlags.value;
				a_ar& conditions;
				a_ar& description;
			}
		};

		using equipmentOverrideList_t = std::vector<equipmentOverride_t>;

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::equipmentOverride_t,
	::IED::Data::equipmentOverride_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::equipmentOverrideConditionGroup_t,
	::IED::Data::equipmentOverrideConditionGroup_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::equipmentOverrideCondition_t,
	::IED::Data::equipmentOverrideCondition_t::Serialization::DataVersion3);
