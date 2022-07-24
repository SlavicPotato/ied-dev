#pragma once

#include <exception>
#include <source_location>
#include <string>

namespace IED
{
	// directxtk helper header isn't exposed, this basically does the same

	class d3d_exception : public std::exception
	{
	public:
		d3d_exception(
			HRESULT              a_hr,
			std::source_location a_src = std::source_location::current()) noexcept :
			m_result(a_hr),
			m_file(a_src.file_name()),
			m_line(a_src.line()),
			m_function(a_src.function_name())
		{}

		const char* what() const override
		{
			static char s_str[255] = {};

			_snprintf_s(
				s_str,
				_TRUNCATE,
				"Failure with HRESULT of %.8X\n\n"
				"%s:%u [%s]\n",
				static_cast<unsigned int>(m_result),
				m_file,
				m_line,
				m_function);

			return s_str;
		}

		inline constexpr HRESULT get_result() const noexcept
		{
			return m_result;
		}

	private:
		const char*   m_file;
		const char*   m_function;
		std::uint32_t m_line;
		HRESULT       m_result;
	};

	inline constexpr void ThrowIfFailed(
		HRESULT              a_hresult,
		std::source_location a_src = std::source_location::current()) noexcept(false)
	{
		if (FAILED(a_hresult))
		{
			throw d3d_exception(a_hresult, a_src);
		}
	}
}