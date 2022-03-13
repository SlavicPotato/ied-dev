#pragma once

namespace IED
{
	namespace UI
	{
		namespace UIData
		{
			using controlDescData_t = stl::vectormap<std::uint32_t, const char*>;

			extern const controlDescData_t g_comboControlMap;
			extern const controlDescData_t g_controlMap;

			template <std::size_t _Size>
			const char* get_control_key_desc(
				const controlDescData_t& a_data,
				std::uint32_t            a_key,
				char (&a_buffer)[_Size])
			{
				auto it = a_data.find(a_key);
				if (it != a_data.end())
				{
					return it->second;
				}
				else
				{
					stl::snprintf(a_buffer, "0x%X", a_key);
					return a_buffer;
				}
			}

			class UICollapsibleStates
			{
			public:
				UICollapsibleStates() = default;

				[[nodiscard]] bool& get(ImGuiID a_key, bool a_default = true);
				void                insert(ImGuiID a_key, bool a_value);

				[[nodiscard]] bool& operator[](ImGuiID a_key);

				bool erase(ImGuiID a_key) noexcept;

				void clear() noexcept;

				[[nodiscard]] inline constexpr const auto& data() const noexcept
				{
					return m_data;
				}

			private:
				std::unordered_map<ImGuiID, bool> m_data;
			};

		}
	}
}