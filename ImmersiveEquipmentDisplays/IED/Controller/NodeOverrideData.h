#pragma once

#include "IED/ConfigTransform.h"

#include "AnimationWeaponSlot.h"

namespace IED
{
	enum class NodeOverrideDataEntryFlags : std::uint32_t
	{
		kNone = 0
	};

	DEFINE_ENUM_CLASS_BITWISE(NodeOverrideDataEntryFlags);

	enum class WeaponPlacementID : std::uint32_t
	{
		None,
		Default,
		OnBack,
		OnBackHip,
		Ankle,
		AtHip,
		Frostfall,
	};

	namespace concepts
	{
		template <class T, class I>
		concept AcceptVectorInit = requires(std::initializer_list<I> a_init)
		{
			stl::vector<T>(a_init.begin(), a_init.end());
		};
	}

	class NodeOverrideData
	{
	public:
		template <class T>
		class vector_init_wrapper :
			public std::vector<T>
		{
			using super = std::vector<T>;

		public:
			//using super::vector;

			vector_init_wrapper() = delete;

			template <class Ti>
			constexpr vector_init_wrapper(std::initializer_list<Ti> a_rhs) requires
				concepts::AcceptVectorInit<T, Ti> :
				super::vector(a_rhs.begin(), a_rhs.end())
			{
			}
		};

		struct weaponNodeListEntry_t
		{
			const char* desc;
		};

		using nodeList_t = stl::vectormap<stl::fixed_string, weaponNodeListEntry_t>;

		struct weap_ctor_init_t
		{
			const char*                                                          node;
			const char*                                                          def;
			const char*                                                          desc;
			AnimationWeaponSlot                                                  animSlot;
			bool                                                                 vanilla;
			std::initializer_list<std::pair<const char*, weaponNodeListEntry_t>> movlist;
		};

		struct weaponNodeEntry_t
		{
			constexpr weaponNodeEntry_t(
				const weap_ctor_init_t& a_init) :
				bsname(a_init.node),
				bsdefParent(a_init.def),
				desc(a_init.desc),
				animSlot(a_init.animSlot),
				vanilla(a_init.vanilla),
				movs(a_init.movlist)
			{
			}

			weaponNodeEntry_t(const weaponNodeEntry_t&) = delete;
			weaponNodeEntry_t& operator=(const weaponNodeEntry_t&) = delete;
			weaponNodeEntry_t(weaponNodeEntry_t&&)                 = delete;
			weaponNodeEntry_t& operator=(weaponNodeEntry_t&&) = delete;

			BSFixedString       bsname;
			BSFixedString       bsdefParent;
			const char*         desc;
			AnimationWeaponSlot animSlot;
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

			constexpr overrideNodeEntry_t(
				const ovr_node_ctor_init_t& a_init) :
				desc(a_init.p1),
				name(a_init.p2),
				bsname(a_init.p2),
				placementID(a_init.p3)
			{
			}

			overrideNodeEntry_t(const overrideNodeEntry_t&) = delete;
			overrideNodeEntry_t& operator=(const overrideNodeEntry_t&) = delete;
			overrideNodeEntry_t(overrideNodeEntry_t&&)                 = delete;
			overrideNodeEntry_t& operator=(overrideNodeEntry_t&&) = delete;

			const char*       desc;
			stl::fixed_string name;
			BSFixedString     bsname;
			WeaponPlacementID placementID;
		};

		struct exn_ctor_init_t
		{
			const char*       mov;
			const char*       cme;
			const char*       parent;
			NiTransform       xfrm_m;
			NiTransform       xfrm_f;
			WeaponPlacementID placementID;
		};

		struct extraNodeEntry_t
		{
			constexpr extraNodeEntry_t(
				const exn_ctor_init_t& a_init) :
				name_cme(a_init.cme),
				name_mov(a_init.mov),
				bsname_cme(a_init.cme),
				bsname_mov(a_init.mov),
				name_parent(a_init.parent),
				transform_m(a_init.xfrm_m),
				transform_f(a_init.xfrm_f),
				placementID(a_init.placementID)
			{
			}

			extraNodeEntry_t(const extraNodeEntry_t&) = delete;
			extraNodeEntry_t& operator=(const extraNodeEntry_t&) = delete;
			extraNodeEntry_t(extraNodeEntry_t&&)                 = delete;
			extraNodeEntry_t& operator=(extraNodeEntry_t&&) = delete;

			stl::fixed_string name_cme;
			stl::fixed_string name_mov;
			BSFixedString     bsname_cme;
			BSFixedString     bsname_mov;
			BSFixedString     name_parent;
			NiTransform       transform_m;
			NiTransform       transform_f;
			WeaponPlacementID placementID;
		};

		struct exn_copy_ctor_init_t
		{
			const char* src;
			const char* dst;
		};

		struct extraNodeCopyEntry_t
		{
			constexpr extraNodeCopyEntry_t(
				const exn_copy_ctor_init_t& a_init) :
				src(a_init.src),
				bssrc(a_init.src),
				dst(a_init.dst)
			{
			}

			extraNodeCopyEntry_t(const extraNodeCopyEntry_t&) = delete;
			extraNodeCopyEntry_t& operator=(const extraNodeCopyEntry_t&) = delete;
			extraNodeCopyEntry_t(extraNodeCopyEntry_t&&)                 = delete;
			extraNodeCopyEntry_t& operator=(extraNodeCopyEntry_t&&) = delete;

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
				rot.SetEulerAngles(a_init.rot.x, a_init.rot.y, a_init.rot.z);
			}

			xfrmOverrideNodeEntry_t(const xfrmOverrideNodeEntry_t&) = delete;
			xfrmOverrideNodeEntry_t& operator=(const xfrmOverrideNodeEntry_t&) = delete;
			xfrmOverrideNodeEntry_t(xfrmOverrideNodeEntry_t&&)                 = delete;
			xfrmOverrideNodeEntry_t& operator=(xfrmOverrideNodeEntry_t&&) = delete;

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

			randWeapEntry_t(const randWeapEntry_t&) = delete;
			randWeapEntry_t& operator=(const randWeapEntry_t&) = delete;
			randWeapEntry_t(randWeapEntry_t&&)                 = delete;
			randWeapEntry_t& operator=(randWeapEntry_t&&) = delete;

			const randPlacementEntry_t* get_rand_entry() const;

			stl::fixed_string                                                node;
			stl::fixed_string                                                leftNode;
			vector_init_wrapper<randPlacementEntry_t>                        movs;
			std::unique_ptr<ThreadSafeRandomNumberGenerator<std::ptrdiff_t>> rng;
		};

		using init_list_cm   = std::pair<const char*, ovr_node_ctor_init_t>;
		using init_list_weap = std::pair<const char*, weap_ctor_init_t>;

		using cm_data_type             = stl::vectormap<stl::fixed_string, const overrideNodeEntry_t>;
		using mon_data_type            = vector_init_wrapper<BSFixedString>;
		using weapnode_data_type       = stl::vectormap<stl::fixed_string, weaponNodeEntry_t>;
		using exn_data_type            = vector_init_wrapper<extraNodeEntry_t>;
		using exn_copy_data_type       = vector_init_wrapper<extraNodeCopyEntry_t>;
		using xfrm_override_data_type  = vector_init_wrapper<xfrmOverrideNodeEntry_t>;
		using rand_placement_data_type = vector_init_wrapper<randWeapEntry_t>;

		NodeOverrideData();

		static void Create();

		inline static const auto& GetCMENodeData() noexcept
		{
			return m_Instance->m_cme;
		}

		inline static const auto& GetMOVNodeData() noexcept
		{
			return m_Instance->m_mov;
		}

		inline static const auto& GetMonitorNodeData() noexcept
		{
			return m_Instance->m_monitor;
		}

		inline static const auto& GetWeaponNodeData() noexcept
		{
			return m_Instance->m_weap;
		}

		inline static const auto& GetExtraNodes() noexcept
		{
			return m_Instance->m_extra;
		}

		inline static const auto& GetExtraCopyNodes() noexcept
		{
			return m_Instance->m_extraCopy;
		}

		inline static const auto& GetTransformOverrideData() noexcept
		{
			return m_Instance->m_transformOverride;
		}

		inline static const auto& GetNPCNodeName() noexcept
		{
			return m_Instance->m_npcNodeName;
		}

		inline static const auto& GetXPMSEExtraDataName() noexcept
		{
			return m_Instance->m_XPMSE;
		}

		inline static const auto& GetSkelIDExtraDataName() noexcept
		{
			return m_Instance->m_skeletonID;
		}

		inline static const auto& GetRandPlacementData() noexcept
		{
			return m_Instance->m_randPlacement;
		}

	private:
		cm_data_type             m_cme;
		cm_data_type             m_mov;
		mon_data_type            m_monitor;
		weapnode_data_type       m_weap;
		exn_data_type            m_extra;
		exn_copy_data_type       m_extraCopy;
		xfrm_override_data_type  m_transformOverride;
		rand_placement_data_type m_randPlacement;

		BSFixedString m_npcNodeName{ "NPC" };
		BSFixedString m_XPMSE{ "XPMSE" };
		BSFixedString m_skeletonID{ "SkeletonID" };

		static std::unique_ptr<NodeOverrideData> m_Instance;
	};
}