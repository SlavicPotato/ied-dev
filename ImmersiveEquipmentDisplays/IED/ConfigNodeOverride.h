#pragma once

#include "ConfigCommon.h"
#include "ConfigData.h"
#include "ConfigTransform.h"

#include "TimeOfDay.h"
#include "WeatherClassificationFlags.h"

namespace IED
{
	namespace Data
	{
		enum class NodeOverrideValuesFlags : std::uint32_t
		{
			kNone = 0,

			kInvisible        = 1u << 0,  // unused
			kAbsolutePosition = 1u << 1
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideValuesFlags);

		struct configNodeOverrideValues_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			inline static constexpr auto DEFAULT_FLAGS = NodeOverrideValuesFlags::kAbsolutePosition;

			stl::flag<NodeOverrideValuesFlags> flags{ DEFAULT_FLAGS };
			configTransform_t                  transform;

			void clear()
			{
				flags = DEFAULT_FLAGS;
				transform.clear();
			}

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& transform;
			}
		};

		enum class NodeOverrideConditionFlags : std::uint32_t
		{
			kNone = 0,

			kTypeMask_V2 = 0x7u,
			kTypeMask_V3 = 0x1Fu,

			kAnd = 1u << 5,
			kNot = 1u << 6,

			kMatchEquipped       = 1u << 7,
			kMatchSlots          = 1u << 8,
			kMatchCategoryOperOR = 1u << 9,

			kMatchAll = kMatchEquipped | kMatchSlots,

			// ignore scb (Node)
			kExtraFlag0 = 1u << 10,

			// laying down (Furniture), is child (Location), match parent (Worldspace), playable (Race), is bolt (Biped)
			kExtraFlag1 = 1u << 11,

			// match skin (Biped), is child (Race)
			kExtraFlag2 = 1u << 12,

			kNegateMatch1 = 1u << 13,
			kNegateMatch2 = 1u << 14,
			kNegateMatch3 = 1u << 15,
			kNegateMatch4 = 1u << 16,

			//kMatchTemplate = 1u << 30
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideConditionFlags);

		enum class NodeOverrideConditionType : std::uint32_t
		{
			Node       = 0,
			Form       = 1,
			Keyword    = 2,
			BipedSlot  = 3,
			Type       = 4,
			Race       = 5,
			Furniture  = 6,
			Group      = 7,
			Actor      = 8,
			NPC        = 9,
			Extra      = 10,
			Location   = 11,
			Worldspace = 12,
			Package    = 13,
			Weather    = 14,
			Global     = 15
		};

		struct NodeOverrideConditionFlagsBitfield
		{
			NodeOverrideConditionType type  : 5 { NodeOverrideConditionType::Node };
			std::uint32_t             unused: 27 { 0 };
		};

		static_assert(sizeof(NodeOverrideConditionFlagsBitfield) == sizeof(std::uint32_t));

		struct configNodeOverrideCondition_t;

		enum class NodeOverrideConditionGroupFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideConditionGroupFlags);

		using configNodeOverrideConditionList_t = stl::boost_vector<configNodeOverrideCondition_t>;

		struct configNodeOverrideConditionGroup_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<NodeOverrideConditionGroupFlags> flags{ NodeOverrideConditionGroupFlags::kNone };
			configNodeOverrideConditionList_t          conditions;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& conditions;
			}
		};

		struct configNodeOverrideCondition_t
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
				NodeOverrideConditionFlags::kMatchEquipped |
				NodeOverrideConditionFlags::kMatchCategoryOperOR;

			inline configNodeOverrideCondition_t()
			{
				fbf.type = NodeOverrideConditionType::Node;
			};

			inline configNodeOverrideCondition_t(
				NodeOverrideConditionType a_type,
				Game::FormID              a_form)
			{
				switch (a_type)
				{
				case NodeOverrideConditionType::Global:
				case NodeOverrideConditionType::Race:
				case NodeOverrideConditionType::Actor:
				case NodeOverrideConditionType::NPC:
					form = a_form;
					break;
				case NodeOverrideConditionType::Form:
					form  = a_form;
					flags = DEFAULT_MATCH_CATEGORY_FLAGS;
					break;
				case NodeOverrideConditionType::Keyword:
					keyword = a_form;
					flags   = DEFAULT_MATCH_CATEGORY_FLAGS;
					break;
				default:
					HALT("FIXME");
					break;
				}

				fbf.type = a_type;
			}

			inline configNodeOverrideCondition_t(
				NodeOverrideConditionType a_type)
			{
				switch (a_type)
				{
				case NodeOverrideConditionType::Race:
				case NodeOverrideConditionType::Furniture:
				case NodeOverrideConditionType::Group:
				case NodeOverrideConditionType::Location:
				case NodeOverrideConditionType::Worldspace:
				case NodeOverrideConditionType::Package:
				case NodeOverrideConditionType::Weather:
					if (a_type == NodeOverrideConditionType::Location ||
					    a_type == NodeOverrideConditionType::Worldspace)
					{
						flags = NodeOverrideConditionFlags::kExtraFlag0;
					}

					fbf.type = a_type;
					break;
				default:
					HALT("FIXME");
					break;
				}
			}

			inline configNodeOverrideCondition_t(
				const stl::fixed_string& a_node) :
				node(a_node)
			{
				fbf.type = NodeOverrideConditionType::Node;
			}

			inline configNodeOverrideCondition_t(
				stl::fixed_string&& a_node) :
				node(std::move(a_node))
			{
				fbf.type = NodeOverrideConditionType::Node;
			}

			inline configNodeOverrideCondition_t(
				BIPED_OBJECT a_biped) :
				bipedSlot(a_biped)
			{
				fbf.type = NodeOverrideConditionType::BipedSlot;
			}

			inline configNodeOverrideCondition_t(
				ExtraConditionType a_type) :
				extraCondType(a_type)
			{
				fbf.type = NodeOverrideConditionType::Extra;
			}

			inline configNodeOverrideCondition_t(
				ObjectSlotExtra a_slot) :
				typeSlot(a_slot),
				flags(DEFAULT_MATCH_CATEGORY_FLAGS)
			{
				fbf.type = NodeOverrideConditionType::Type;
			}

			union
			{
				stl::flag<NodeOverrideConditionFlags> flags{ NodeOverrideConditionFlags::kNone };
				NodeOverrideConditionFlagsBitfield    fbf;
			};

			stl::fixed_string  node;
			configCachedForm_t form;
			configCachedForm_t keyword;

			ObjectSlotExtra typeSlot{ Data::ObjectSlotExtra::kNone };

			union
			{
				std::uint32_t              ui32a{ static_cast<std::uint32_t>(-1) };
				ExtraConditionType         extraCondType;
				BIPED_OBJECT               bipedSlot;
				PACKAGE_PROCEDURE_TYPE     procedureType;
				WeatherClassificationFlags weatherClass;
				ComparisonOperator         compOperator;

				static_assert(std::is_same_v<std::underlying_type_t<PACKAGE_PROCEDURE_TYPE>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<ExtraConditionType>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<BIPED_OBJECT>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<WeatherClassificationFlags>, std::uint32_t>);
				static_assert(std::is_same_v<std::underlying_type_t<ComparisonOperator>, std::uint32_t>);
			};

			float f32a{ 0.0f };

			union
			{
				std::uint32_t ui32b{ 0 };
				TimeOfDay     timeOfDay;

				static_assert(std::is_same_v<std::underlying_type_t<TimeOfDay>, std::uint32_t>);
			};

			configNodeOverrideConditionGroup_t group;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& node;
				a_ar& form;
				a_ar& keyword;
				a_ar& ui32a;
				a_ar& typeSlot;

				if (a_version >= DataVersion2)
				{
					a_ar& group;

					if (a_version >= DataVersion3)
					{
						a_ar& f32a;
						a_ar& ui32b;
					}
				}
			}
		};

		enum class NodeOverrideOffsetFlags : std::uint32_t
		{
			kNone = 0,

			kContinue                = 1u << 0,
			kMatchAll                = 1u << 1,
			kWeaponAdjust            = 1u << 2,
			kWeightAdjust            = 1u << 3,
			kAccumulatePos           = 1u << 4,
			kLockToAccum             = 1u << 5,
			kResetAccum              = 1u << 6,
			kRequiresConditionList   = 1u << 7,
			kIsGroup                 = 1u << 8,
			kWeaponAdjustMatchedOnly = 1u << 9,
			kAbsoluteRotation        = 1u << 10,

			// weapon adjust flags

			kAdjustX          = 1u << 16,
			kAdjustY          = 1u << 17,
			kAdjustZ          = 1u << 18,
			kAdjustIgnoreDead = 1u << 19,

			kAdjustFlags = kWeaponAdjust | kWeightAdjust
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideOffsetFlags);

		/*enum class NodeOverrideOffsetAdjustType : std::uint32_t
		{
			Weapon,
			Weight
		};

		struct NodeOverrideOffsetFlagsBitfield
		{
			std::uint32_t cont: 1 { 0 };
			std::uint32_t matchAll: 1 { 0 };
			std::uint32_t isAdjust: 1 { 0 };
			NodeOverrideOffsetAdjustType type: 3 { NodeOverrideOffsetAdjustType::Weapon };
			std::uint32_t unused: 26 { 0 };
		};*/

		struct configNodeOverrideOffset_t :
			public configNodeOverrideValues_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<NodeOverrideOffsetFlags>            offsetFlags{ NodeOverrideOffsetFlags::kNone };
			configNodeOverrideConditionList_t             conditions;
			std::string                                   description;
			NiPoint3                                      adjustScale{ 1.0f, 1.0f, 1.0f };
			stl::boost_vector<configNodeOverrideOffset_t> group;

			constexpr void clamp()
			{
				adjustScale.x = std::clamp(stl::zero_nan(adjustScale.x), -100.0f, 100.0f);
				adjustScale.y = std::clamp(stl::zero_nan(adjustScale.y), -100.0f, 100.0f);
				adjustScale.z = std::clamp(stl::zero_nan(adjustScale.z), -100.0f, 100.0f);
			}

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& static_cast<const configNodeOverrideValues_t&>(*this);
				a_ar& offsetFlags.value;
				a_ar& conditions;
				a_ar& description;
				a_ar& adjustScale;
				a_ar& group;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configNodeOverrideValues_t&>(*this);
				a_ar& offsetFlags.value;
				a_ar& conditions;
				a_ar& description;
				a_ar& adjustScale;
				a_ar& group;

				clamp();
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		using configNodeOverrideOffsetList_t = stl::boost_vector<configNodeOverrideOffset_t>;

		enum class NodeOverrideFlags : std::uint32_t
		{
			kNone = 0,

			kInvisible                       = 1u << 0,
			kVisibilityRequiresConditionList = 1u << 1
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideFlags);

		struct configNodeOverrideTransform_t :
			public configNodeOverrideValues_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<NodeOverrideFlags>      overrideFlags{ NodeOverrideFlags::kNone };
			configNodeOverrideOffsetList_t    offsets;
			configNodeOverrideConditionList_t visibilityConditionList;

			void clear()
			{
				configNodeOverrideValues_t::clear();
				overrideFlags = NodeOverrideFlags::kNone;
				offsets.clear();
				visibilityConditionList.clear();
			}

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configNodeOverrideValues_t&>(*this);
				a_ar& offsets;
				a_ar& overrideFlags.value;
				a_ar& visibilityConditionList;
			}
		};

		enum class NodeOverridePlacementValuesFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverridePlacementValuesFlags);

		struct configNodeOverridePlacementValues_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<NodeOverridePlacementValuesFlags> flags{ NodeOverridePlacementValuesFlags::kNone };
			stl::fixed_string                           targetNode;

			constexpr void clear()
			{
				flags = NodeOverridePlacementValuesFlags::kNone;
				targetNode.clear();
			}

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& targetNode;
			}
		};

		enum class NodeOverridePlacementOverrideFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverridePlacementOverrideFlags);

		struct configNodeOverridePlacementOverride_t :
			public configNodeOverridePlacementValues_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			constexpr void clear()
			{
				configNodeOverridePlacementValues_t::clear();
				overrideFlags = NodeOverridePlacementOverrideFlags::kNone;
				description.clear();
			}

			stl::flag<NodeOverridePlacementOverrideFlags> overrideFlags{ NodeOverridePlacementOverrideFlags::kNone };
			configNodeOverrideConditionList_t             conditions;
			std::string                                   description;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configNodeOverridePlacementValues_t&>(*this);
				a_ar& overrideFlags.value;
				a_ar& conditions;
				a_ar& description;
			}
		};

		using configNodeOverridePlacementOverrideList_t = stl::boost_vector<configNodeOverridePlacementOverride_t>;

		enum class NodeOverridePlacementFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverridePlacementFlags);

		struct configNodeOverridePlacement_t :
			public configNodeOverridePlacementValues_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			void clear()
			{
				configNodeOverridePlacementValues_t::clear();
				pflags = NodeOverridePlacementFlags::kNone;
				overrides.clear();
			}

			stl::flag<NodeOverridePlacementFlags>     pflags{ NodeOverridePlacementFlags::kNone };
			configNodeOverridePlacementOverrideList_t overrides;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configNodeOverridePlacementValues_t&>(*this);
				a_ar& pflags.value;
				a_ar& overrides;
			}
		};

		enum class NodeOverrideHolderFlags : std::uint32_t
		{
			kNone = 0,

			RandomGenerated = 1u << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideHolderFlags);

		using configNodeOverrideEntryTransform_t = configSexRoot_t<configNodeOverrideTransform_t>;
		using configNodeOverrideEntryPlacement_t = configSexRoot_t<configNodeOverridePlacement_t>;

		struct configNodeOverrideHolderCopy_t;

		struct configNodeOverrideHolder_t
		{
			friend class boost::serialization::access;

		public:
			using transform_data_type = stl::boost_unordered_map<stl::fixed_string, configNodeOverrideEntryTransform_t>;
			using placement_data_type = stl::boost_unordered_map<stl::fixed_string, configNodeOverrideEntryPlacement_t>;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configNodeOverrideHolder_t() = default;

			explicit configNodeOverrideHolder_t(
				const configNodeOverrideHolderCopy_t& a_rhs);

			explicit configNodeOverrideHolder_t(
				configNodeOverrideHolderCopy_t&& a_rhs);

			configNodeOverrideHolder_t& operator=(
				const configNodeOverrideHolderCopy_t& a_rhs);

			configNodeOverrideHolder_t& operator=(
				configNodeOverrideHolderCopy_t&& a_rhs);

			stl::flag<NodeOverrideHolderFlags> flags{ NodeOverrideHolderFlags::kNone };
			transform_data_type                data;
			placement_data_type                placementData;

			void clear() noexcept
			{
				flags = NodeOverrideHolderFlags::kNone;
				data.clear();
				placementData.clear();
			}

			inline bool empty() const noexcept
			{
				return data.empty() &&
				       placementData.empty();
			}

			template <
				class Td,
				class data_type = stl::strip_type<Td>>
			[[nodiscard]] inline constexpr auto& get_data() noexcept
				requires stl::is_any_same_v<
					data_type,
					transform_data_type,
					configNodeOverrideEntryTransform_t>
			{
				return data;
			}

			template <
				class Td,
				class data_type = stl::strip_type<Td>>
			[[nodiscard]] inline constexpr auto& get_data() noexcept
				requires stl::is_any_same_v<
					data_type,
					placement_data_type,
					configNodeOverrideEntryPlacement_t>
			{
				return placementData;
			}

		private :
			template <class Archive>
			void
			serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& data;
				a_ar& placementData;
			}

			void __init(const configNodeOverrideHolderCopy_t& a_rhs);
			void __init(configNodeOverrideHolderCopy_t&& a_rhs);
		};

		struct configNodeOverrideHolderCopy_t
		{
			template <class Td>
			struct data_value_pair
			{
				ConfigClass first{ ConfigClass::Global };
				Td          second;
			};

		public:
			using transform_data_type = stl::boost_unordered_map<stl::fixed_string, data_value_pair<configNodeOverrideEntryTransform_t>>;
			using placement_data_type = stl::boost_unordered_map<stl::fixed_string, data_value_pair<configNodeOverrideEntryPlacement_t>>;

			configNodeOverrideHolderCopy_t() = default;

			configNodeOverrideHolderCopy_t(
				const configNodeOverrideHolder_t& a_rhs,
				ConfigClass                       a_initclass);

			configNodeOverrideHolderCopy_t(
				configNodeOverrideHolder_t&& a_rhs,
				ConfigClass                  a_initclass);

			stl::flag<NodeOverrideHolderFlags> flags{ NodeOverrideHolderFlags::kNone };
			transform_data_type                data;
			placement_data_type                placementData;

			void clear() noexcept
			{
				flags = NodeOverrideHolderFlags::kNone;
				data.clear();
				placementData.clear();
			}

			inline bool empty() const noexcept
			{
				return data.empty() && placementData.empty();
			}

			template <
				class Td,
				class data_type = stl::strip_type<Td>>
			[[nodiscard]] inline constexpr auto& get_data() noexcept
				requires stl::is_any_same_v<
					data_type,
					transform_data_type,
					configNodeOverrideEntryTransform_t>
			{
				return data;
			}

			template <
				class Td,
				class data_type = stl::strip_type<Td>>
			[[nodiscard]] inline constexpr auto& get_data() noexcept
				requires stl::is_any_same_v<
					data_type,
					placement_data_type,
					configNodeOverrideEntryPlacement_t>
			{
				return placementData;
			}

			configNodeOverrideHolder_t copy_cc(
				ConfigClass a_class) const;

			void copy_cc(
				ConfigClass                 a_class,
				configNodeOverrideHolder_t& a_dst) const;
		};

		struct configNodeOverrideHolderClipboardData_t
		{
			ConfigClass                conf_class;
			ConfigSex                  sex;
			configNodeOverrideHolder_t data;
		};

		using configMapNodeOverrides_t = configFormMap_t<configNodeOverrideHolder_t>;

		class configStoreNodeOverride_t :
			public configStoreBase_t<configNodeOverrideHolder_t>
		{
		public:
			using holderCache_t = configHolderCache_t<configMapNodeOverrides_t>;

			static void CopyEntries(
				const configNodeOverrideHolder_t& a_src,
				configNodeOverrideHolder_t&       a_dst)
			{
				for (auto& e : a_src.data)
				{
					a_dst.data.emplace(e.first, e.second);
				}

				for (auto& e : a_src.placementData)
				{
					a_dst.placementData.emplace(e.first, e.second);
				}
			}

			static void CopyEntries(
				const configNodeOverrideHolder_t& a_src,
				configNodeOverrideHolderCopy_t&   a_dst,
				ConfigClass                       a_class)
			{
				for (auto& e : a_src.data)
				{
					a_dst.data.try_emplace(e.first, a_class, e.second);
				}

				for (auto& e : a_src.placementData)
				{
					a_dst.placementData.try_emplace(e.first, a_class, e.second);
				}
			}

			configNodeOverrideHolderCopy_t GetActorCopy(
				Game::FormID a_actor,
				Game::FormID a_npc,
				Game::FormID a_race) const;

			configNodeOverrideHolderCopy_t GetNPCCopy(
				Game::FormID a_npc,
				Game::FormID a_race) const;

			configNodeOverrideHolderCopy_t GetRaceCopy(
				Game::FormID     a_race,
				GlobalConfigType a_globtype) const;

			configNodeOverrideHolderCopy_t GetGlobalCopy(
				GlobalConfigType a_globtype) const;

			const configNodeOverrideEntryTransform_t* GetActorTransform(
				Game::FormID             a_actor,
				Game::FormID             a_npc,
				Game::FormID             a_race,
				const stl::fixed_string& a_node,
				holderCache_t&           a_hc) const;

			const configNodeOverrideEntryPlacement_t* GetActorPlacement(
				Game::FormID             a_actor,
				Game::FormID             a_npc,
				Game::FormID             a_race,
				const stl::fixed_string& a_node,
				holderCache_t&           a_hc) const;
		};
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configNodeOverrideTransform_t,
	::IED::Data::configNodeOverrideTransform_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configNodeOverrideOffset_t,
	::IED::Data::configNodeOverrideOffset_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configNodeOverrideConditionGroup_t,
	::IED::Data::configNodeOverrideConditionGroup_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configNodeOverrideCondition_t,
	::IED::Data::configNodeOverrideCondition_t::Serialization::DataVersion3);

BOOST_CLASS_VERSION(
	::IED::Data::configNodeOverrideValues_t,
	::IED::Data::configNodeOverrideValues_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configNodeOverridePlacement_t,
	::IED::Data::configNodeOverridePlacement_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configNodeOverridePlacementValues_t,
	::IED::Data::configNodeOverridePlacementValues_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configNodeOverridePlacementOverride_t,
	::IED::Data::configNodeOverridePlacementOverride_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configNodeOverrideHolder_t,
	::IED::Data::configNodeOverrideHolder_t::Serialization::DataVersion1);
