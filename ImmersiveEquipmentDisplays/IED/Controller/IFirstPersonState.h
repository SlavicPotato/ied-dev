#pragma once

namespace IED
{
	class IFirstPersonState
	{
	public:
		bool IsInFirstPerson() const noexcept;

		[[nodiscard]] constexpr bool IFPV_Detected() const noexcept
		{
			return static_cast<bool>(m_ifpvState);
		}

	protected:
		void InitializeFPStateData();

	private:
		TESGlobal* m_ifpvState{ nullptr };
	};
}