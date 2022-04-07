#include "pch.h"

#include "PEXReader.h"

#include "Serialization/Serialization.h"

namespace IED
{
	void PEXReader::Open(const fs::path& a_path)
	{
		m_stream.open(a_path, std::ios_base::binary | std::ios_base::in);

		if (!m_stream || !m_stream.is_open())
		{
			throw std::system_error(
				errno,
				std::system_category(),
				Serialization::SafeGetPath(a_path));
		}
	}

	void PEXReader::ReadData()
	{
		static_assert(sizeof(std::time_t) == 8);

		if (read<std::uint32_t>() != MAGIC)
		{
			throw std::exception("bad magic");
		}

		read<std::uint8_t>();   // major
		read<std::uint8_t>();   // minor
		read<std::uint16_t>();  // game id
		read<std::time_t>();    // time
		read_string();          // source file
		read_string();          // user
		read_string();          // comp name

		auto stringTableLen = read<std::uint16_t>();

		for (std::uint32_t i = 0; i < stringTableLen; i++)
		{
			m_stringTable.emplace_back(read_string());
		}
	}

	std::string PEXReader::read_string()
	{
		auto len  = read<std::uint16_t>();
		auto data = std::make_unique<char[]>(len);

		m_stream.read(data.get(), len);

		if (m_stream.gcount() != len)
		{
			throw std::exception("string read error");
		}

		return std::string(data.get(), data.get() + len);
	}

}