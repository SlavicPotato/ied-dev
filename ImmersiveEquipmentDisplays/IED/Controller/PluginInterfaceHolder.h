#pragma once

namespace IED
{
	template <class T>
	class PluginInterfaceHolder
	{
	public:
		inline constexpr void SetPluginInterface(T* a_interface) noexcept
		{
			m_interface = a_interface;
		}

		inline constexpr T* GetPluginInterface() const noexcept
		{
			return m_interface;
		}

	protected:
		T* m_interface{ nullptr };
	};
}