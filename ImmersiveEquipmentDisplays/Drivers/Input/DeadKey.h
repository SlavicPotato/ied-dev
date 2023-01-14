#pragma once

namespace IED
{
	class DeadKey
	{
	public:
		DeadKey& operator=(WCHAR a_code) noexcept;

		[[nodiscard]] constexpr explicit operator bool() const noexcept
		{
			return static_cast<bool>(m_comb);
		}

		[[nodiscard]] constexpr auto code() const noexcept
		{
			return m_code;
		}

		[[nodiscard]] constexpr auto comb() const noexcept
		{
			return m_comb;
		}

		constexpr void clear() noexcept
		{
			m_comb = 0;
		}

		static WCHAR translate(WCHAR a_code) noexcept;

	private:
		WCHAR m_code{ 0 };
		WCHAR m_comb{ 0 };
	};

}
