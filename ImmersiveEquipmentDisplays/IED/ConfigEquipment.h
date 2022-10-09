#pragma once

#include "ConfigBaseValues.h"
#include "ConfigCommon.h"
#include "ConfigData.h"
#include "ConfigLUIDTag.h"

#include "TimeOfDay.h"
#include "WeatherClassificationFlags.h"

namespace IED
{
	namespace Data
	{
		enum class EquipmentOverrideFlags : std::uint32_t
		{
			kNone = 0,

			kIsGroup  = 1u << 0,
			kContinue = 1u << 1
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
			kMatchEquipmentSlots = 1u << 8,
			kMatchCategoryOperOR = 1u << 9,
			kMatchThisItem       = 1u << 10,

			kMatchMaskEquippedAndThis     = kMatchEquipped | kMatchThisItem,
			kMatchMaskEquippedAndSlots    = kMatchEquipped | kMatchEquipmentSlots,
			kMatchMaskAllEquipmentAndThis = kMatchEquipmentSlots | kMatchThisItem,
			kMatchMaskAny                 = kMatchEquipped | kMatchMaskAllEquipmentAndThis,

			// laying down (Furniture), loc child (Location), match parent (Worldspace), playable (Race), is bolt (Biped), count (Form), rank (Faction)
			kExtraFlag1 = 1u << 11,

			// match skin (Biped), is child (Race)
			kExtraFlag2 = 1u << 12,

			kNegateMatch1 = 1u << 13,
			kNegateMatch2 = 1u << 14,
			kNegateMatch3 = 1u << 15,
			kNegateMatch4 = 1u << 16,

			kExtraFlag3 = 1u << 17,
			kExtraFlag4 = 1u << 18,

			//kMatchTemplate = 1u << 30
		};

		DEFINE_ENUM_CLASS_BITWISE(EquipmentOverrideConditionFlags);

		enum class EquipmentOverrideConditionType : std::uint32_t
		{
			Form       = 0,
			Type       = 1,
			Keyword    = 2,
			Race       = 3,
			Furniture  = 4,
			BipedSlot  = 5,
			Group      = 6,
			Quest      = 7,
			Actor      = 8,
			NPC        = 9,
			Extra      = 10,
			Location   = 11,
			Worldspace = 12,
			Package    = 13,
			Weather    = 14,
			Global     = 15,
			Mounting   = 16,
			Mounted    = 17,
			Presence   = 18,
			Idle       = 19,
			Skeleton   = 20,
			Faction    = 21,
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

		using equipmentOverrideConditionList_t = stl::boost_vector<equipmentOverrideCondition_t>;

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

		struct equipmentOverrideCondition_t :
			configLUIDTagAC_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
				DataVersion3 = 3,
				DataVersion4 = 4,
				DataVersion5 = 5,
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
				case EquipmentOverrideConditionType::Global:
				case EquipmentOverrideConditionType::Race:
				case EquipmentOverrideConditionType::Actor:
				case EquipmentOverrideConditionType::NPC:
				case EquipmentOverrideConditionType::Faction:
					form = a_form;
					break;
				case EquipmentOverrideConditionType::Form:
					form  = a_form;
					count = 1;
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
					assert(false);
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
				case EquipmentOverrideConditionType::Mounting:
				case EquipmentOverrideConditionType::Mounted:
				case EquipmentOverrideConditionType::Presence:
				case EquipmentOverrideConditionType::Idle:
				case EquipmentOverrideConditionType::Skeleton:

					switch (a_matchType)
					{
					case EquipmentOverrideConditionType::Location:
					case EquipmentOverrideConditionType::Worldspace:
						flags = EquipmentOverrideConditionFlags::kExtraFlag1;
						break;
					case EquipmentOverrideConditionType::Presence:
						flags = EquipmentOverrideConditionFlags::kMatchCategoryOperOR |
						        EquipmentOverrideConditionFlags::kMatchThisItem;
						break;
					}

					fbf.type = a_matchType;

					break;

				default:
					assert(false);
					break;
				}
			}

			union
			{
				stl::flag<EquipmentOverrideConditionFlags> flags{ EquipmentOverrideConditionFlags::kNone };
				EquipmentOverrideConditionFlagsBitfield    fbf;
			};

			configCachedForm_t form;
			configCachedForm_t keyword;

			Data::ObjectSlotExtra slot{ Data::ObjectSlotExtra::kNone };

			union
			{
				std::uint32_t              ui32a{ static_cast<std::uint32_t>(-1) };
				QuestConditionType         questCondType;
				ExtraConditionType         extraCondType;
				BIPED_OBJECT               bipedSlot;
				PACKAGE_PROCEDURE_TYPE     procedureType;
				WeatherClassificationFlags weatherClass;
				ComparisonOperator         compOperator;

				static_assert(std::is_same_v<std::underlying_type_t<QuestConditionType>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<ExtraConditionType>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<BIPED_OBJECT>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<PACKAGE_PROCEDURE_TYPE>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<WeatherClassificationFlags>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<ComparisonOperator>, std::uint32_t>);
			};

			union
			{
				float f32a{ 0.0f };
				float percent;
			};

			union
			{
				std::int32_t i32a{ 0 };
				std::int32_t skeletonID;
				std::int32_t factionRank;
			};

			union
			{
				std::uint32_t ui32b{ 0 };
				std::uint32_t count;
				TimeOfDay     timeOfDay;
				std::uint32_t uid;

				static_assert(std::is_same_v<std::underlying_type_t<TimeOfDay>, std::uint32_t>);
			};

			union
			{
				std::uint64_t ui64a{ 0 };
				std::uint64_t skeletonSignature;
			};

			equipmentOverrideConditionGroup_t group;

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar&              flags.value;
				const configForm_t tmp = form.get_id();
				a_ar&              tmp;
				a_ar&              slot;
				a_ar&              keyword;
				a_ar&              ui32a;
				a_ar&              group;
				a_ar&              f32a;
				a_ar&              ui32b;
				a_ar&              i32a;
				a_ar&              ui64a;
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

						if (a_version >= DataVersion4)
						{
							a_ar& f32a;
							a_ar& ui32b;

							if (a_version >= DataVersion5)
							{
								a_ar& i32a;
								a_ar& ui64a;
							}
						}
					}
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		struct equipmentOverride_t;

		using equipmentOverrideList_t = stl::boost_vector<equipmentOverride_t>;

		struct equipmentOverride_t :
			public configBaseValues_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
			};

			equipmentOverride_t() = default;

			template <class Td, class Ts>
			equipmentOverride_t(
				Td&& a_config,
				Ts&& a_desc)  //
				requires(
					std::is_constructible_v<configBaseValues_t, Td&&>&&
						std::is_constructible_v<std::string, Ts&&>) :
				configBaseValues_t(std::forward<Td>(a_config)),
				description(std::forward<Ts>(a_desc))
			{
			}

			stl::flag<EquipmentOverrideFlags> eoFlags{ EquipmentOverrideFlags::kNone };
			equipmentOverrideConditionList_t  conditions;
			std::string                       description;
			equipmentOverrideList_t           group;

		protected:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configBaseValues_t&>(*this);
				a_ar& eoFlags.value;
				a_ar& conditions;
				a_ar& description;

				if (a_version >= DataVersion2)
				{
					a_ar& group;
				}
			}
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::equipmentOverride_t,
	::IED::Data::equipmentOverride_t::Serialization::DataVersion2);

BOOST_CLASS_VERSION(
	::IED::Data::equipmentOverrideConditionGroup_t,
	::IED::Data::equipmentOverrideConditionGroup_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::equipmentOverrideCondition_t,
	::IED::Data::equipmentOverrideCondition_t::Serialization::DataVersion5);
