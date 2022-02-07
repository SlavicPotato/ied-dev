#pragma once

namespace IED
{
	class DeadKey
	{
	public:
		DeadKey& operator=(WCHAR a_code) noexcept;

		[[nodiscard]] inline constexpr explicit operator bool() const noexcept
		{
			return static_cast<bool>(m_comb);
		}

		[[nodiscard]] inline constexpr auto code() const noexcept
		{
			return m_code;
		}

		[[nodiscard]] inline constexpr auto comb() const noexcept
		{
			return m_comb;
		}

		inline constexpr void clear() noexcept
		{
			m_comb = 0;
		}

		static WCHAR translate(WCHAR a_code) noexcept;

	private:
		WCHAR m_code{ 0 };
		WCHAR m_comb{ 0 };
	};

}
