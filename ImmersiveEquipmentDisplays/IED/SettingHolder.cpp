#include "pch.h"

#include "SettingHolder.h"

#include "Serialization/Serialization.h"

#include "IED/Parsers/JSONSettingsHolder.h"

namespace IED
{
	namespace Data
	{
		bool SettingHolder::Load()
		{
			try
			{
				Json::Value root;

				Serialization::ReadData(m_path, root);

				Settings tmp;

				Serialization::ParserState state;
				Serialization::Parser<Settings> parser(state);

				if (!parser.Parse(root, tmp))
				{
					throw std::exception("parser error");
				}

				data = std::move(tmp);

				return true;
			}
			catch (const std::exception& e)
			{
				m_lastException = e;
				return false;
			}
		}
		bool SettingHolder::Save()
		{
			try
			{
				Json::Value root;

				Serialization::ParserState state;
				Serialization::Parser<Settings> parser(state);

				parser.Create(data, root);

				Serialization::WriteData(m_path, root);

				return true;
			}
			catch (const std::exception& e)
			{
				m_lastException = e;
				return false;
			}
		}

		bool SettingHolder::SaveIfDirty()
		{
			return m_dirty ? Save() : false;
		}

	}  // namespace Data
}  // namespace IED