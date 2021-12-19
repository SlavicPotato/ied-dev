#pragma once

#include "IED/ConfigOverrideCommon.h"
#include "IED/ConfigOverrideTransform.h"

namespace IED
{
	namespace Data
	{
		enum class NodeOverrideValuesFlags : std::uint32_t
		{
			kNone = 0,

			kInvisible = 1u << 0  // unused
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

			stl::flag<NodeOverrideValuesFlags> flags{ NodeOverrideValuesFlags::kNone };
			configTransform_t transform;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& transform;
			}
		};

		enum class NodeOverrideConditionFlags : std::uint32_t
		{
			kNone = 0,

			kTypeMask_V2 = 0x7u,
			kTypeMask_V3 = 0x1Fu,

			kAnd = 1u << 5,
			kNot = 1u << 6,

			kMatchEquipped = 1u << 7,
			kMatchSlots = 1u << 8,
			kMatchCategoryOperOR = 1u << 9,

			kMatchAll = kMatchEquipped | kMatchSlots,

			kNodeIgnoreScabbards = 1u << 10,
			kLayingDown = 1u << 11,
			kMatchSkin = 1u << 12,

			kNegateMatch1 = 1u << 13,
			kNegateMatch2 = 1u << 14
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideConditionFlags);

		enum class NodeOverrideConditionType : std::uint32_t
		{
			Node,
			Form,
			Keyword,
			BipedSlot,
			EquipmentSlot,
			Race,
			Furniture
		};

		struct NodeOverrideConditionFlagsBitfield
		{
			NodeOverrideConditionType type: 5 { NodeOverrideConditionType::Node };
			std::uint32_t unused: 27 { 0 };
		};

		struct configNodeOverrideCondition_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			inline configNodeOverrideCondition_t()
			{
				fbf.type = NodeOverrideConditionType::Node;
			};

			configNodeOverrideCondition_t(
				Game::FormID a_form,
				NodeOverrideConditionType a_type)
			{
				if (a_type == NodeOverrideConditionType::Form ||
				    a_type == NodeOverrideConditionType::Race)
				{
					form = a_form;
				}
				else if (a_type == NodeOverrideConditionType::Keyword)
				{
					keyword = a_form;
				}
				else
				{
					HALT("FIXME");
				}

				fbf.type = a_type;
			}

			configNodeOverrideCondition_t(
				NodeOverrideConditionType a_type)
			{
				if (a_type != NodeOverrideConditionType::Furniture)
				{
					HALT("FIXME");
				}

				fbf.type = a_type;
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
				Biped::BIPED_OBJECT a_biped) :
				bipedSlot(a_biped)
			{
				fbf.type = NodeOverrideConditionType::BipedSlot;
			}

			inline configNodeOverrideCondition_t(
				ObjectSlot a_slot) :
				equipmentSlot(a_slot)
			{
				fbf.type = NodeOverrideConditionType::EquipmentSlot;
			}

			union
			{
				stl::flag<NodeOverrideConditionFlags> flags{ NodeOverrideConditionFlags::kMatchEquipped };
				NodeOverrideConditionFlagsBitfield fbf;
			};

			stl::fixed_string node;
			configCachedForm_t form;
			configCachedForm_t keyword;
			std::uint32_t bipedSlot{ stl::underlying(Biped::kNone) };
			ObjectSlot equipmentSlot{ ObjectSlot::kMax };

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& node;
				ar& form;
				ar& keyword;
				ar& bipedSlot;
				ar& equipmentSlot;
			}
		};

		using configNodeOverrideConditionList_t = std::vector<configNodeOverrideCondition_t>;

		enum class NodeOverrideOffsetFlags : std::uint32_t
		{
			kNone = 0,

			kContinue = 1u << 0,
			kMatchAll = 1u << 1,
			kWeaponAdjust = 1u << 2,
			kWeightAdjust = 1u << 3,
			kAccumulatePos = 1u << 4,
			kLockToAccum = 1u << 5,
			kResetAccum = 1u << 6,
			kRequiresConditionList = 1u << 7,
			kIsGroup = 1u << 8,
			kWeaponAdjustMatchedOnly = 1u << 9,
			kAbsoluteRotation = 1u << 10,

			// weapon adjust flags

			kAdjustX = 1u << 16,
			kAdjustY = 1u << 17,
			kAdjustZ = 1u << 18,
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

			stl::flag<NodeOverrideOffsetFlags> offsetFlags{ NodeOverrideOffsetFlags::kNone };
			configNodeOverrideConditionList_t matches;
			std::string description;
			NiPoint3 adjustScale{ 1.0f, 1.0f, 1.0f };
			std::vector<configNodeOverrideOffset_t> group;

			void clamp()
			{
				using namespace ::Math;

				adjustScale.x = std::clamp(zero_nan(adjustScale.x), -100.0f, 100.0f);
				adjustScale.y = std::clamp(zero_nan(adjustScale.y), -100.0f, 100.0f);
				adjustScale.z = std::clamp(zero_nan(adjustScale.z), -100.0f, 100.0f);
			}

		private:
			template <class Archive>
			void save(Archive& ar, const unsigned int version) const
			{
				ar& static_cast<const configNodeOverrideValues_t&>(*this);
				ar& offsetFlags.value;
				ar& matches;
				ar& description;
				ar& adjustScale;
				ar& group;
			}

			template <class Archive>
			void load(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configNodeOverrideValues_t&>(*this);
				ar& offsetFlags.value;
				ar& matches;
				ar& description;
				ar& adjustScale;
				ar& group;

				clamp();
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		using configNodeOverrideOffsetList_t = std::vector<configNodeOverrideOffset_t>;

		enum class NodeOverrideFlags : std::uint32_t
		{
			kNone = 0,

			kInvisible = 1u << 0,
			kVisibilityRequiresConditionList = 1u << 1
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideFlags);

		struct configNodeOverride_t :
			public configNodeOverrideValues_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<NodeOverrideFlags> overrideFlags{ NodeOverrideFlags::kNone };
			configNodeOverrideOffsetList_t offsets;
			configNodeOverrideConditionList_t visibilityConditionList;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configNodeOverrideValues_t&>(*this);
				ar& offsets;
				ar& overrideFlags.value;
				ar& visibilityConditionList;
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
			stl::fixed_string targetNode;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& targetNode;
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

			stl::flag<NodeOverridePlacementOverrideFlags> overrideFlags{ NodeOverridePlacementOverrideFlags::kNone };
			configNodeOverrideConditionList_t matches;
			std::string description;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configNodeOverridePlacementValues_t&>(*this);
				ar& overrideFlags.value;
				ar& matches;
				ar& description;
			}
		};

		using configNodeOverridePlacementOverrideList_t = std::vector<configNodeOverridePlacementOverride_t>;

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

			stl::flag<NodeOverridePlacementFlags> pflags{ NodeOverridePlacementFlags::kNone };
			configNodeOverridePlacementOverrideList_t overrides;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configNodeOverridePlacementValues_t&>(*this);
				ar& pflags.value;
				ar& overrides;
			}
		};

		enum class NodeOverrideHolderFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeOverrideHolderFlags);

		using configNodeOverrideEntry_t = configSexRoot_t<configNodeOverride_t>;
		using configNodeOverrideEntryParent_t = configSexRoot_t<configNodeOverridePlacement_t>;

		struct configNodeOverrideHolder_t
		{
			friend class boost::serialization::access;

		public:
			using transform_data_type = std::unordered_map<stl::fixed_string, configNodeOverrideEntry_t>;
			using placement_data_type = std::unordered_map<stl::fixed_string, configNodeOverrideEntryParent_t>;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<NodeOverrideHolderFlags> flags{ NodeOverrideHolderFlags::kNone };
			transform_data_type data;
			placement_data_type placementData;

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

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& data;
				ar& placementData;
			}
		};

		using configMapNodeOverrides_t = configFormMap_t<configNodeOverrideHolder_t>;

		class configStoreNodeOverride_t :
			public configStoreBase_t<configNodeOverrideHolder_t>
		{
			//using merged_type = std::unordered_map<stl::fixed_string, configNodeOverrideEntry_t>;

		public:
			struct holderCache_t
			{
			public:
				const configNodeOverrideHolder_t* get_actor(
					Game::FormID a_actor,
					const configMapNodeOverrides_t& a_data) const
				{
					if (!actor)
					{
						if (auto it = a_data.find(a_actor); it != a_data.end())
						{
							actor = std::addressof(it->second);
						}
					}

					return actor;
				}

				const configNodeOverrideHolder_t* get_npc(
					Game::FormID a_npc,
					const configMapNodeOverrides_t& a_data) const
				{
					if (!npc)
					{
						if (auto it = a_data.find(a_npc); it != a_data.end())
						{
							npc = std::addressof(it->second);
						}
					}

					return npc;
				}

				const configNodeOverrideHolder_t* get_race(
					Game::FormID a_race,
					const configMapNodeOverrides_t& a_data) const
				{
					if (!race)
					{
						if (auto it = a_data.find(a_race); it != a_data.end())
						{
							race = std::addressof(it->second);
						}
					}

					return race;
				}

			private:
				mutable const configNodeOverrideHolder_t* actor{ nullptr };
				mutable const configNodeOverrideHolder_t* npc{ nullptr };
				mutable const configNodeOverrideHolder_t* race{ nullptr };
			};

			template <class T>
			SKMP_FORCEINLINE static const typename T::mapped_type* GetEntry(
				const T& a_data,
				const stl::fixed_string& a_node)
			{
				if (a_data.empty())
				{
					return nullptr;
				}
				else
				{
					auto it = a_data.find(a_node);
					return it != a_data.end() ? std::addressof(it->second) : nullptr;
				}
			}

			static void CopyEntries(
				const configNodeOverrideHolder_t& a_src,
				configNodeOverrideHolder_t& a_dst)
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

			configNodeOverrideHolder_t GetActor(
				Game::FormID a_actor,
				Game::FormID a_npc,
				Game::FormID a_race) const
			{
				configNodeOverrideHolder_t result;

				if (auto& actorData = GetActorData(); !actorData.empty())
				{
					if (auto it = actorData.find(a_actor); it != actorData.end())
					{
						CopyEntries(it->second, result);
					}
				}

				if (auto& npcData = GetNPCData(); !npcData.empty())
				{
					if (auto it = npcData.find(a_npc); it != npcData.end())
					{
						CopyEntries(it->second, result);
					}
				}

				if (auto& raceData = GetRaceData(); !raceData.empty())
				{
					if (auto it = raceData.find(a_race); it != raceData.end())
					{
						CopyEntries(it->second, result);
					}
				}

				CopyEntries(GetGlobalData()[0], result);

				return result;
			}

			configNodeOverrideHolder_t GetNPC(
				Game::FormID a_npc,
				Game::FormID a_race) const
			{
				configNodeOverrideHolder_t result;

				if (auto& npcData = GetNPCData(); !npcData.empty())
				{
					if (auto it = npcData.find(a_npc); it != npcData.end())
					{
						CopyEntries(it->second, result);
					}
				}

				if (auto& raceData = GetRaceData(); !raceData.empty())
				{
					if (auto it = raceData.find(a_race); it != raceData.end())
					{
						CopyEntries(it->second, result);
					}
				}

				CopyEntries(GetGlobalData()[0], result);

				return result;
			}

			configNodeOverrideHolder_t GetRace(
				Game::FormID a_race) const
			{
				configNodeOverrideHolder_t result;

				if (auto& raceData = GetRaceData(); !raceData.empty())
				{
					if (auto it = raceData.find(a_race); it != raceData.end())
					{
						CopyEntries(it->second, result);
					}
				}

				CopyEntries(GetGlobalData()[0], result);

				return result;
			}

			bool HasCMEClass(
				Game::FormID a_handle,
				ConfigClass a_class,
				const stl::fixed_string& a_node,
				holderCache_t& a_hc) const
			{
				switch (a_class)
				{
				case ConfigClass::Actor:

					if (auto& actorData = GetActorData(); !actorData.empty())
					{
						if (auto data = a_hc.get_actor(a_handle, actorData))
						{
							return GetEntry(data->data, a_node) != nullptr;
						}
					}

					break;
				case ConfigClass::NPC:

					if (auto& npcData = GetNPCData(); !npcData.empty())
					{
						if (auto data = a_hc.get_npc(a_handle, npcData))
						{
							return GetEntry(data->data, a_node) != nullptr;
						}
					}

					break;
				case ConfigClass::Race:

					if (auto& raceData = GetRaceData(); !raceData.empty())
					{
						if (auto data = a_hc.get_race(a_handle, raceData))
						{
							return GetEntry(data->data, a_node) != nullptr;
						}
					}

					break;
				case ConfigClass::Global:

					return GetEntry(GetGlobalData()[0].data, a_node) != nullptr;
				}

				return false;
			}

			bool HasPlacementClass(
				Game::FormID a_handle,
				ConfigClass a_class,
				const stl::fixed_string& a_node,
				holderCache_t& a_hc) const
			{
				switch (a_class)
				{
				case ConfigClass::Actor:

					if (auto& actorData = GetActorData(); !actorData.empty())
					{
						if (auto data = a_hc.get_actor(a_handle, actorData))
						{
							return GetEntry(data->placementData, a_node) != nullptr;
						}
					}

					break;
				case ConfigClass::NPC:

					if (auto& npcData = GetNPCData(); !npcData.empty())
					{
						if (auto data = a_hc.get_npc(a_handle, npcData))
						{
							return GetEntry(data->placementData, a_node) != nullptr;
						}
					}

					break;
				case ConfigClass::Race:

					if (auto& raceData = GetRaceData(); !raceData.empty())
					{
						if (auto data = a_hc.get_race(a_handle, raceData))
						{
							return GetEntry(data->placementData, a_node) != nullptr;
						}
					}

					break;
				case ConfigClass::Global:

					return GetEntry(GetGlobalData()[0].placementData, a_node) != nullptr;
				}

				return false;
			}

			const auto* GetActorCME(
				Game::FormID a_actor,
				Game::FormID a_npc,
				Game::FormID a_race,
				const stl::fixed_string& a_node,
				holderCache_t& a_hc) const
			{
				if (auto& actorData = GetActorData(); !actorData.empty())
				{
					if (auto data = a_hc.get_actor(a_actor, actorData))
					{
						if (auto r = GetEntry(data->data, a_node))
						{
							return r;
						}
					}
				}

				if (auto& npcData = GetNPCData(); !npcData.empty())
				{
					if (auto data = a_hc.get_npc(a_npc, npcData))
					{
						if (auto r = GetEntry(data->data, a_node))
						{
							return r;
						}
					}
				}

				if (auto& raceData = GetRaceData(); !raceData.empty())
				{
					if (auto data = a_hc.get_race(a_race, raceData))
					{
						if (auto r = GetEntry(data->data, a_node))
						{
							return r;
						}
					}
				}

				return GetEntry(GetGlobalData()[0].data, a_node);
			}

			const auto* GetActorParent(
				Game::FormID a_actor,
				Game::FormID a_npc,
				Game::FormID a_race,
				const stl::fixed_string& a_node,
				holderCache_t& a_hc) const
			{
				if (auto& actorData = GetActorData(); !actorData.empty())
				{
					if (auto data = a_hc.get_actor(a_actor, actorData))
					{
						if (auto r = GetEntry(data->placementData, a_node))
						{
							return r;
						}
					}
				}

				if (auto& npcData = GetNPCData(); !npcData.empty())
				{
					if (auto data = a_hc.get_npc(a_npc, npcData))
					{
						if (auto r = GetEntry(data->placementData, a_node))
						{
							return r;
						}
					}
				}

				if (auto& raceData = GetRaceData(); !raceData.empty())
				{
					if (auto data = a_hc.get_race(a_race, raceData))
					{
						if (auto r = GetEntry(data->placementData, a_node))
						{
							return r;
						}
					}
				}

				return GetEntry(GetGlobalData()[0].placementData, a_node);
			}

			void clear()
			{
				for (auto& e : data)
				{
					e.clear();
				}

				for (auto& e : global)
				{
					e.clear();
				}
			}
		};
	}
}

BOOST_CLASS_VERSION(
	IED::Data::configNodeOverride_t,
	IED::Data::configNodeOverride_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configNodeOverrideOffset_t,
	IED::Data::configNodeOverrideOffset_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configNodeOverrideCondition_t,
	IED::Data::configNodeOverrideCondition_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configNodeOverrideValues_t,
	IED::Data::configNodeOverrideValues_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configNodeOverridePlacement_t,
	IED::Data::configNodeOverridePlacement_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configNodeOverridePlacementValues_t,
	IED::Data::configNodeOverridePlacementValues_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configNodeOverridePlacementOverride_t,
	IED::Data::configNodeOverridePlacementOverride_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configNodeOverrideHolder_t,
	IED::Data::configNodeOverrideHolder_t::Serialization::DataVersion1);
