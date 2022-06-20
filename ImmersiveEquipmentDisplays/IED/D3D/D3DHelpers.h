#pragma once

#include <exception>
#include <string>

namespace IED
{
	// directxtk helper header isn't exposed, this basically does the same

	class d3d_exception : public std::exception
	{
	public:
		d3d_exception(HRESULT a_hr) noexcept :
			m_result(a_hr) {}

		const char* what() const override
		{
			static char s_str[64] = {};

			_snprintf_s(
				s_str,
				_TRUNCATE,
				"Failure with HRESULT of %08X",
				static_cast<unsigned int>(m_result));

			return s_str;
		}

		inline constexpr HRESULT get_result() const noexcept
		{
			return m_result;
		}

	private:
		HRESULT m_result;
	};

	inline void ThrowIfFailed(HRESULT hr) noexcept(false)
	{
		if (FAILED(hr))
		{
			throw d3d_exception(hr);
		}
	}
}