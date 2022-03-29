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

	class NodeOverrideData
	{
	public:
		struct weaponNodeListEntry_t
		{
			const char* desc;
		};

		using nodeList_t = stl::vectormap<stl::fixed_string, weaponNodeListEntry_t>;

		struct weap_ctor_init_t
		{
			const char*                                                node;
			const char*                                                def;
			const char*                                                desc;
			AnimationWeaponSlot                                        animSlot;
			std::initializer_list<std::pair<const char*, const char*>> movlist;
		};

		struct weaponNodeEntry_t
		{
			weaponNodeEntry_t(
				const weap_ctor_init_t& a_init) :
				bsname(a_init.node),
				bsdefParent(a_init.def),
				desc(a_init.desc),
				animSlot(a_init.animSlot),
				movs(a_init.movlist)
			{
			}

			BSFixedString       bsname;
			BSFixedString       bsdefParent;
			const char*         desc;
			AnimationWeaponSlot animSlot;
			nodeList_t          movs;
		};

		using weapnode_data_type = stl::vectormap<stl::fixed_string, weaponNodeEntry_t>;

		using mon_data_type = std::vector<BSFixedString>;

		struct overrideNodeEntryInit_t
		{
			const char*       p1;
			const char*       p2;
			WeaponPlacementID p3{ WeaponPlacementID::Default };
		};

		struct overrideNodeEntry_t
		{
			overrideNodeEntry_t() = delete;

			overrideNodeEntry_t(
				const overrideNodeEntryInit_t& a_pair) :
				desc(a_pair.p1),
				name(a_pair.p2),
				bsname(a_pair.p2),
				placementID(a_pair.p3)
			{
			}

			const char*       desc;
			stl::fixed_string name;
			BSFixedString     bsname;
			WeaponPlacementID placementID;
		};

		struct extraNodeEntry_t
		{
			extraNodeEntry_t(
				const char*        a_mov,
				const char*        a_cme,
				const char*        a_parent,
				const NiTransform& a_transform_m,
				const NiTransform& a_transform_f,
				WeaponPlacementID  a_placementID) :
				name_cme(a_cme),
				name_mov(a_mov),
				bsname_cme(a_cme),
				bsname_mov(a_mov),
				name_parent(a_parent),
				transform_m(a_transform_m),
				transform_f(a_transform_f),
				placementID(a_placementID)
			{
			}

			stl::fixed_string name_cme;
			stl::fixed_string name_mov;
			BSFixedString     bsname_cme;
			BSFixedString     bsname_mov;
			BSFixedString     name_parent;
			NiTransform       transform_m;
			NiTransform       transform_f;
			WeaponPlacementID placementID;
		};

		struct extraNodeCopyEntry_t
		{
			extraNodeCopyEntry_t(
				const char* a_src,
				const char* a_dst) :
				src(a_src),
				bssrc(a_src),
				dst(a_dst)
			{}

			stl::fixed_string src;
			BSFixedString     bssrc;
			BSFixedString     dst;
		};

		struct xfrmOverrideNodeEntry_t
		{
			xfrmOverrideNodeEntry_t(
				const char* a_name,
				NiPoint3    a_rot) :
				name(a_name)
			{
				rot.SetEulerAngles(a_rot.x, a_rot.y, a_rot.z);
			}

			BSFixedString name;
			NiMatrix33    rot;
		};

		using init_list_cm   = std::pair<const char*, overrideNodeEntryInit_t>;
		using init_list_weap = std::pair<const char*, weap_ctor_init_t>;

		using cm_data_type            = stl::vectormap<stl::fixed_string, const overrideNodeEntry_t>;
		using exn_data_type           = std::vector<extraNodeEntry_t>;
		using exn_copy_data_type      = std::vector<extraNodeCopyEntry_t>;
		using xfrm_override_data_type = std::vector<xfrmOverrideNodeEntry_t>;

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

	private:
		cm_data_type            m_cme;
		cm_data_type            m_mov;
		mon_data_type           m_monitor;
		weapnode_data_type      m_weap;
		exn_data_type           m_extra;
		exn_copy_data_type      m_extraCopy;
		xfrm_override_data_type m_transformOverride;

		BSFixedString m_npcNodeName{ "NPC" };
		BSFixedString m_XPMSE{ "XPMSE" };
		BSFixedString m_skeletonID{ "SkeletonID" };

		static std::unique_ptr<NodeOverrideData> m_Instance;
	};
}