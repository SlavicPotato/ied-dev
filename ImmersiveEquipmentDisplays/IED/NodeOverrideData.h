#pragma once

#include "IED/ConfigAdditionalCMENode.h"
#include "IED/ConfigConvertNodes.h"
#include "IED/ConfigConvertNodes2.h"
#include "IED/ConfigExtraNode.h"
#include "IED/ConfigNodeMonitor.h"
#include "IED/ConfigTransform.h"
#include "IED/I3DIModelID.h"
#include "IED/WeaponPlacementID.h"

#include "AnimationWeaponSlot.h"
#include "GearNodeID.h"

namespace IED
{
	class SkeletonID;

	enum class NodeOverrideDataEntryFlags : std::uint32_t
	{
		kNone = 0
	};

	DEFINE_ENUM_CLASS_BITWISE(NodeOverrideDataEntryFlags);

	enum class OverrideNodeEntryFlags : std::uint32_t
	{
		kNone = 0,

		kAdditionalNode = 1u << 0
	};

	DEFINE_ENUM_CLASS_BITWISE(OverrideNodeEntryFlags);

	class NodeOverrideData :
		ILog
	{
	public:
		SKMP_ALIGNED_REDEFINE_NEW_PREF(stl::L1_CACHE_LINE_SIZE);

		struct weaponNodeListEntry_t
		{
			std::string desc;
		};

		using nodeList_t = stl::cache_aligned::vectormap<stl::fixed_string, weaponNodeListEntry_t>;

		struct weap_ctor_init_t
		{
			const char*                                                          node;
			const char*                                                          def;
			const char*                                                          desc;
			AnimationWeaponSlot                                                  animSlot;
			I3DIModelID                                                          modelID;
			GearNodeID                                                           nodeID;
			bool                                                                 vanilla;
			std::initializer_list<std::pair<const char*, weaponNodeListEntry_t>> movlist;
		};

		struct weaponNodeEntry_t
		{
			weaponNodeEntry_t(
				const weap_ctor_init_t& a_init) :
				bsname(a_init.node),
				bsdefParent(a_init.def),
				desc(a_init.desc),
				animSlot(a_init.animSlot),
				modelID(a_init.modelID),
				nodeID(a_init.nodeID),
				vanilla(a_init.vanilla),
				movs(a_init.movlist)
			{
			}

			weaponNodeEntry_t(const weaponNodeEntry_t&)            = delete;
			weaponNodeEntry_t& operator=(const weaponNodeEntry_t&) = delete;

			BSFixedString       bsname;
			BSFixedString       bsdefParent;
			const char*         desc;
			AnimationWeaponSlot animSlot;
			I3DIModelID         modelID;
			GearNodeID          nodeID;
			bool                vanilla;
			nodeList_t          movs;
		};

		struct ovr_node_ctor_init_t
		{
			const char*       p1;
			const char*       p2;
			WeaponPlacementID p3{ WeaponPlacementID::Default };
		};

		struct overrideNodeEntry_t
		{
			overrideNodeEntry_t() = delete;

			overrideNodeEntry_t(
				const ovr_node_ctor_init_t& a_init) :
				desc(a_init.p1),
				name(a_init.p2),
				bsname(a_init.p2),
				placementID(a_init.p3)
			{
			}

			overrideNodeEntry_t(
				const stl::fixed_string& a_name,
				const stl::fixed_string& a_desc,
				WeaponPlacementID        a_pid,
				OverrideNodeEntryFlags   a_flags = OverrideNodeEntryFlags::kNone) :
				desc(a_desc),
				name(a_name),
				bsname(a_name.c_str()),
				placementID(a_pid),
				flags(a_flags)
			{
			}

			overrideNodeEntry_t(const overrideNodeEntry_t&)            = delete;
			overrideNodeEntry_t& operator=(const overrideNodeEntry_t&) = delete;

			stl::fixed_string                 desc;
			stl::fixed_string                 name;
			BSFixedString                     bsname;
			WeaponPlacementID                 placementID;
			stl::flag<OverrideNodeEntryFlags> flags{ OverrideNodeEntryFlags::kNone };
		};

		struct extraNodeEntrySkelTransformSyncNode_t
		{
			BSFixedString                                             name;
			stl::flag<Data::ExtraNodeEntrySkelTransformSyncNodeFlags> flags;
		};

		struct extraNodeEntrySkelTransform_t
		{
#if defined(IED_PERF_BUILD)
			Bullet::btTransformEx
#else
			NiTransform
#endif
																			  xfrm;
			stl::cache_aligned::vector<extraNodeEntrySkelTransformSyncNode_t> syncNodes;
			stl::fixed_string                                                 readFromObj;
			BSFixedString                                                     bsReadFromObj;
			bool                                                              invert{ false };
		};

		struct extraNodeEntrySkel_t
		{
			extraNodeEntrySkel_t(
				const Data::configSkeletonMatch_t& a_sm,
				const stl::fixed_string&           a_ovr_parent) :
				match(a_sm),
				ovr_parent(a_ovr_parent.c_str())
			{
			}

			extraNodeEntrySkel_t(
				const Data::configSkeletonMatch_t&   a_sm,
				const extraNodeEntrySkelTransform_t& a_sxfrm1,
				const extraNodeEntrySkelTransform_t& a_sxfrm2) :
				match(a_sm),
				sxfrms{
					a_sxfrm1,
					a_sxfrm2
				}
			{
			}

			Data::configSkeletonMatch_t                                match;
			stl::cache_aligned::vector<std::pair<BSFixedString, bool>> objMatch;
			std::array<extraNodeEntrySkelTransform_t, 2>               sxfrms;
			BSFixedString                                              ovr_parent;
		};

		class skeletonEntryList_t :
			public stl::cache_aligned::vector<extraNodeEntrySkel_t>
		{
			using super = stl::cache_aligned::vector<extraNodeEntrySkel_t>;

		public:
			using super::vector;
			using super::operator=;

			//super::const_iterator find(const SkeletonID& a_value) const;
			super::const_iterator find(const SkeletonID& a_value, NiNode* a_root) const;
		};

		struct extraNodeEntry_t
		{
			using name_pair = std::pair<stl::fixed_string, BSFixedString>;

			extraNodeEntry_t(
				const stl::fixed_string& a_name,
				const stl::fixed_string& a_mov,
				const stl::fixed_string& a_cme,
				const stl::fixed_string& a_parent,
				WeaponPlacementID        a_pid,
				const stl::fixed_string& a_desc) :
				name_cme(a_cme),
				bsname_cme(a_cme.c_str()),
				names{
					name_pair{
						//
						a_mov,
						a_mov.c_str()
						//
					},
					name_pair{
						//
						a_name,
						a_name.c_str()
						//
					}
				},
				name_parent(a_parent.c_str()),
				placementID(a_pid),
				desc(a_desc)
			{
			}

			stl::fixed_string        name_cme;
			BSFixedString            bsname_cme;
			std::array<name_pair, 2> names;
			BSFixedString            name_parent;
			stl::fixed_string        desc;
			skeletonEntryList_t      skel;
			WeaponPlacementID        placementID;
		};

		struct exn_copy_ctor_init_t
		{
			const char* src;
			const char* dst;
		};

		struct extraNodeCopyEntry_t
		{
			extraNodeCopyEntry_t(
				const exn_copy_ctor_init_t& a_init) :
				src(a_init.src),
				bssrc(a_init.src),
				dst(a_init.dst)
			{
			}

			extraNodeCopyEntry_t(const extraNodeCopyEntry_t&)            = delete;
			extraNodeCopyEntry_t& operator=(const extraNodeCopyEntry_t&) = delete;

			stl::fixed_string src;
			BSFixedString     bssrc;
			BSFixedString     dst;
		};

		struct xfrm_ovr_ctor_init_t
		{
			const char* name;
			NiPoint3    rot;
		};

		struct xfrmOverrideNodeEntry_t
		{
			xfrmOverrideNodeEntry_t(
				const xfrm_ovr_ctor_init_t& a_init) :
				name(a_init.name)
			{
				rot.SetEulerAnglesIntrinsic(
					a_init.rot.x,
					a_init.rot.y,
					a_init.rot.z);
			}

			xfrmOverrideNodeEntry_t(const xfrmOverrideNodeEntry_t&)            = delete;
			xfrmOverrideNodeEntry_t& operator=(const xfrmOverrideNodeEntry_t&) = delete;

			BSFixedString name;
			NiMatrix33    rot;
		};

		struct rw_placement_ctor_init
		{
			const char* node;
			const char* nodeLeft;
		};

		struct randPlacementEntry_t
		{
			constexpr randPlacementEntry_t(
				const rw_placement_ctor_init& a_init) :
				node(a_init.node),
				nodeLeft(a_init.nodeLeft)
			{
			}

			stl::fixed_string node;
			stl::fixed_string nodeLeft;
		};

		struct rw_ctor_init_t
		{
			const char*                                   node;
			const char*                                   leftNode;
			std::initializer_list<rw_placement_ctor_init> movs;
		};

		struct randWeapEntry_t
		{
			randWeapEntry_t(
				const rw_ctor_init_t& a_init) :
				node(a_init.node),
				leftNode(a_init.leftNode),
				movs(a_init.movs)
			{
				assert(movs.size() > 0);

				try
				{
					rng = std::make_unique<ThreadSafeRandomNumberGenerator<std::ptrdiff_t>>(
						0,
						static_cast<std::ptrdiff_t>(movs.size() - 1));
				}
				catch (...)
				{
				}
			}

			randWeapEntry_t(const randWeapEntry_t&)            = delete;
			randWeapEntry_t& operator=(const randWeapEntry_t&) = delete;

			const randPlacementEntry_t* get_rand_entry() const;

			stl::fixed_string                                                             node;
			stl::fixed_string                                                             leftNode;
			stl::container_init_wrapper<stl::cache_aligned::vector<randPlacementEntry_t>> movs;
			std::unique_ptr<ThreadSafeRandomNumberGenerator<std::ptrdiff_t>>              rng;
		};

		using init_list_cm   = std::pair<const char*, ovr_node_ctor_init_t>;
		using init_list_weap = std::pair<const char*, weap_ctor_init_t>;

		using cm_data_type             = stl::cache_aligned::vectormap<stl::fixed_string, const overrideNodeEntry_t>;
		using mon_data_type            = stl::container_init_wrapper<stl::cache_aligned::vector<BSFixedString>>;
		using gear_node_data_type      = stl::cache_aligned::vectormap<stl::fixed_string, weaponNodeEntry_t>;
		using exn_data_type            = stl::cache_aligned::vector<extraNodeEntry_t>;
		using exn_copy_data_type       = stl::container_init_wrapper<stl::cache_aligned::vector<extraNodeCopyEntry_t>>;
		using xfrm_override_data_type  = stl::container_init_wrapper<stl::cache_aligned::vector<xfrmOverrideNodeEntry_t>>;
		using rand_placement_data_type = stl::container_init_wrapper<stl::cache_aligned::vector<randWeapEntry_t>>;
		using node_mon_data_type       = stl::unordered_map<std::uint32_t, Data::configNodeMonitorEntryBS_t>;
		using convert_nodes_data_type  = Data::configSkeletonMatch_t;

		NodeOverrideData();

		static void Create();

		static SKMP_143_CONSTEXPR const auto& GetCMENodeData() noexcept
		{
			return m_Instance->m_cme;
		}

		static SKMP_143_CONSTEXPR const auto& GetMOVNodeData() noexcept
		{
			return m_Instance->m_mov;
		}

		static SKMP_143_CONSTEXPR const auto& GetMonitorNodeData() noexcept
		{
			return m_Instance->m_monitor;
		}

		static SKMP_143_CONSTEXPR const auto& GetGearNodeData() noexcept
		{
			return m_Instance->m_gearNodes;
		}

		static SKMP_143_CONSTEXPR const auto& GetExtraMovNodes() noexcept
		{
			return m_Instance->m_extramov;
		}

		static SKMP_143_CONSTEXPR const auto& GetExtraCopyNodes() noexcept
		{
			return m_Instance->m_extraCopy;
		}

		static SKMP_143_CONSTEXPR const auto& GetTransformOverrideData() noexcept
		{
			return m_Instance->m_transformOverride;
		}

		static SKMP_143_CONSTEXPR const auto& GetRandPlacementData() noexcept
		{
			return m_Instance->m_randPlacement;
		}

		static SKMP_143_CONSTEXPR const auto& GetHumanoidSkeletonSignatures() noexcept
		{
			return m_Instance->m_humanoidSkeletonSignatures;
		}

		static SKMP_143_CONSTEXPR const auto& GetHumanoidSkeletonSignaturesXp32() noexcept
		{
			return m_Instance->m_humanoidSkeletonSignaturesXp32;
		}

		static SKMP_143_CONSTEXPR const auto& GetNodeMonitorEntries() noexcept
		{
			return m_Instance->m_nodeMonEntries;
		}

		static SKMP_143_CONSTEXPR const auto& GetConvertNodes() noexcept
		{
			return m_Instance->m_convertNodes;
		}
		
		static SKMP_143_CONSTEXPR const auto& GetConvertNodes2() noexcept
		{
			return m_Instance->m_convertNodes2;
		}

		static void LoadAndAddExtraNodes(const char* a_path);
		static void LoadAndAddNodeMonitor(const char* a_path);
		static void LoadAndAddConvertNodes(const char* a_path);
		static void LoadAndAddConvertNodes2(const char* a_path);
		static void LoadAndAddAdditionalCMENodes(const char* a_path);

		FN_NAMEPROC("NodeOverrideData");

	private:
		template <class T>
		std::unique_ptr<T> LoadDataFile(
			const fs::path& a_path);

		template <class T>
		bool LoadEntryList(
			const char*   a_path,
			stl::list<T>& a_out);

		void AddExtraNodeData(
			const stl::list<Data::configExtraNodeList_t>& a_data);

		void AddNodeMonitorData(
			stl::list<Data::configNodeMonitorEntryList_t>&& a_data);

		void AddConvertNodesData(
			const stl::list<Data::configConvertNodesList_t>& a_data);

		void AddConvertNodes2Data(
			const stl::list<Data::configConvertNodesList2_t>& a_data);

		void AddAdditionalCMENodeData(
			const stl::list<Data::configAdditionalCMENodeList_t>& a_data);

		cm_data_type                                            m_cme;
		cm_data_type                                            m_mov;
		mon_data_type                                           m_monitor;
		gear_node_data_type                                     m_gearNodes;
		exn_data_type                                           m_extramov;
		exn_copy_data_type                                      m_extraCopy;
		xfrm_override_data_type                                 m_transformOverride;
		rand_placement_data_type                                m_randPlacement;
		node_mon_data_type                                      m_nodeMonEntries;
		convert_nodes_data_type                                 m_convertNodes;
		stl::cache_aligned::vector<Data::configConvertNodes2_t> m_convertNodes2;

		stl::cache_aligned::flat_set<std::uint64_t> m_humanoidSkeletonSignatures;
		stl::cache_aligned::flat_set<std::uint64_t> m_humanoidSkeletonSignaturesXp32;

		static std::unique_ptr<NodeOverrideData> m_Instance;
	};
}