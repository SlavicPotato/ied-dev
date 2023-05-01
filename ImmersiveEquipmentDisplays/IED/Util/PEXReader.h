#pragma once

#include <boost/predef/other/endian.h>

namespace IED
{
	class PEXReader
	{
		static constexpr std::uint32_t MAGIC = 0xFA57C0DE;

	public:
		void Open(const fs::path& a_path);

		void ReadData();

		[[nodiscard]] constexpr auto& GetStringTable() const noexcept
		{
			return m_stringTable;
		}

	private:
		template <class T>
		T swap_endian(T a_value)
		{
			union
			{
				T            u;
				std::uint8_t u8[sizeof(T)];
			} s, d;

			s.u = a_value;

			for (std::size_t i = 0; i < sizeof(T); i++)
			{
				d.u8[i] = s.u8[sizeof(T) - i - 1];
			}

			return d.u;
		}

		template <class T>
		T read() requires
			std::is_trivially_constructible_v<T> &&
			std::is_trivially_destructible_v<T>
		{
			T result;

			if (!m_stream.read(
					reinterpret_cast<char*>(std::addressof(result)),
					sizeof(T)))
			{
				throw std::runtime_error("read error");
			}

			return
#if defined(BOOST_ENDIAN_BIG_BYTE)
				swap_endian(result)
#else
				result
#endif
					;
		}

		std::string read_string();

		stl::vector<std::string> m_stringTable;

		std::ifstream m_stream;
	};
}