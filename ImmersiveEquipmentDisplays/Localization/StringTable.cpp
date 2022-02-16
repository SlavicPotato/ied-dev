#include "pch.h"

#include "StringTable.h"

#include "IED/Parsers/JSONStringTableDataParser.h"

namespace IED
{
	namespace Localization
	{
		bool StringTable::Load(const fs::path& a_path)
		{
			try
			{
				Json::Value root;

				Serialization::ReadData(a_path, root);

				Serialization::ParserState                            state;
				Serialization::Parser<StringTable::data_storage_type> parser(state);

				StringTable::data_storage_type tmp;

				if (!parser.Parse(root, tmp))
				{
					throw std::exception("parser error");
				}

				m_data = std::move(tmp);

				return true;
			}
			catch (const std::exception& e)
			{
				m_lastException = e;
				return false;
			}
		}
	}
}