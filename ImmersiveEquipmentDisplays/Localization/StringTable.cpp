#include "pch.h"

#include "StringTable.h"

#include "IED/Parsers/JSONStringTableDataParser.h"

namespace IED
{
	namespace Localization
	{
		bool StringTable::Load(const fs::path& a_path)
		{
			using namespace Serialization;

			try
			{
				Json::Value root;

				ReadData(a_path, root);

				ParserState                     state;
				Parser<StringTable::table_data> parser(state);

				auto tmp = std::make_unique_for_overwrite<StringTable::table_data>();

				if (!parser.Parse(root, *tmp))
				{
					throw parser_exception("parser error");
				}

				m_data = std::move(*tmp);

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