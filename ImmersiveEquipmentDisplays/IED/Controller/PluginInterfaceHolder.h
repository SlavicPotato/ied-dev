#pragma once

namespace IED
{
	template <class T>
	class PluginInterfaceHolder
	{
	public:
		inline constexpr void SetPluginInterface(T* a_interface)
		{
			m_interface = a_interface;
		}

	protected:
		T* m_interface{ nullptr };
	};
}