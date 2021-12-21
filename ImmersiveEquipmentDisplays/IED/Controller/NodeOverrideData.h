#pragma once

namespace IED
{
	class OverrideNodeInfo
	{
	public:
		struct weaponNodeListEntry_t
		{
			std::string desc;
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
			std::string desc;
			nodeList_t movs;
		};

		using weapnode_data_type = stl::vectormap<stl::fixed_string, weaponNodeEntry_t>;

		using mon_data_type = std::vector<BSFixedString>;

		struct overrideNodeEntry_t
		{
			const char* desc;
			BSFixedString bsname;
		};

		using cm_data_type = stl::vectormap<stl::fixed_string, const overrideNodeEntry_t>;

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

	private:
		OverrideNodeInfo() = default;

		cm_data_type m_cme;
		cm_data_type m_mov;
		mon_data_type m_monitor;
		weapnode_data_type m_weap;

		static OverrideNodeInfo m_Instance;
	};
}