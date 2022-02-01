#pragma once

#include "IED/ConfigOverrideTransform.h"

namespace IED
{
	enum class NodeOverrideDataEntryFlags : std::uint32_t
	{
		kNone = 0
	};

	DEFINE_ENUM_CLASS_BITWISE(NodeOverrideDataEntryFlags);

	class NodeOverrideData
	{
	public:
		struct weaponNodeListEntry_t
		{
			const char* desc;
		};

		using nodeList_t = stl::vectormap<stl::fixed_string, weaponNodeListEntry_t>;

		struct weaponNodeEntry_t
		{
			weaponNodeEntry_t(
				const char* a_node,
				const char* a_def,
				const char* a_desc,
				std::initializer_list<nodeList_t::value_type> a_movlist) :
				bsname(a_node),
				defParent(a_def),
				bsdefParent(a_def),
				desc(a_desc),
				movs(a_movlist)
			{
			}

			BSFixedString bsname;
			stl::fixed_string defParent;
			BSFixedString bsdefParent;
			const char* desc;
			nodeList_t movs;
		};

		using weapnode_data_type = stl::vectormap<stl::fixed_string, weaponNodeEntry_t>;

		using mon_data_type = std::vector<BSFixedString>;

		struct overrideNodeEntry_t
		{
			const char* desc;
			BSFixedString bsname;
			stl::flag<NodeOverrideDataEntryFlags> flags{ NodeOverrideDataEntryFlags::kNone };
		};

		struct extraNodeEntry_t
		{
			extraNodeEntry_t(
				const char* a_mov,
				const char* a_cme,
				const char *a_parent,
				const Data::configTransform_t& a_transform_m,
				const Data::configTransform_t& a_transform_f) :
				name_cme(a_cme),
				name_mov(a_mov),
				bsname_cme(a_cme),
				bsname_mov(a_mov),
				name_parent(a_parent),
				transform_m(a_transform_m.to_nitransform()),
				transform_f(a_transform_f.to_nitransform())
			{
			}

			stl::fixed_string name_cme;
			stl::fixed_string name_mov;
			BSFixedString bsname_cme;
			BSFixedString bsname_mov;
			BSFixedString name_parent;
			NiTransform transform_m;
			NiTransform transform_f;
		};

		using cm_data_type = stl::vectormap<stl::fixed_string, const overrideNodeEntry_t>;
		using exn_data_type = std::list<extraNodeEntry_t>;

		static void Create();

		inline static constexpr const auto& GetCMENodeData() noexcept
		{
			return m_Instance.m_cme;
		}

		inline static constexpr const auto& GetMOVNodeData() noexcept
		{
			return m_Instance.m_mov;
		}

		inline static constexpr const auto& GetMonitorNodeData() noexcept
		{
			return m_Instance.m_monitor;
		}

		inline static constexpr const auto& GetWeaponNodeData() noexcept
		{
			return m_Instance.m_weap;
		}
		
		inline static constexpr const auto& GetExtraNodes() noexcept
		{
			return m_Instance.m_extra;
		}

	private:
		NodeOverrideData() = default;

		cm_data_type m_cme;
		cm_data_type m_mov;
		mon_data_type m_monitor;
		weapnode_data_type m_weap;
		exn_data_type m_extra;

		bool m_initialized{ false };

		static NodeOverrideData m_Instance;
	};
}