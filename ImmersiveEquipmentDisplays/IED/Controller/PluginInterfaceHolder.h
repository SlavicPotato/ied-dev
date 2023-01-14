#pragma once

namespace IED
{
	template <class T>
	class PluginInterfaceHolder
	{
		static_assert(std::is_base_of_v<PluginInterfaceBase, T>);

	public:
		constexpr void SetPluginInterface(
			T*                             a_interface,
			PluginInterfaceQueryErrorState a_error) noexcept
		{
			m_interface  = a_interface;
			m_queryError = a_error;
		}

		constexpr void GetPluginInterface(
			T*&                             a_interfaceOut,
			PluginInterfaceQueryErrorState& a_errorOut) const noexcept
		{
			a_interfaceOut = m_interface;
			a_errorOut     = m_queryError;
		}

		constexpr void GetPluginInterface(T*& a_interfaceOut) const noexcept
		{
			a_interfaceOut = m_interface;
		}

	private:
		T*                             m_interface{ nullptr };
		PluginInterfaceQueryErrorState m_queryError{ PluginInterfaceQueryErrorState::kNone };
	};
}