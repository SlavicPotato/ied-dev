#pragma once

namespace IED
{
	template <class T>
	class PluginInterfaceHolder
	{
		static_assert(std::is_base_of_v<PluginInterfaceBase, T>);

	public:
		inline constexpr void SetPluginInterface(T* a_interface) noexcept
		{
			m_interface = a_interface;
		}

		inline constexpr void GetPluginInterface(T*& a_out) const noexcept
		{
			a_out = m_interface;
		}

	private:
		T* m_interface{ nullptr };
	};
}