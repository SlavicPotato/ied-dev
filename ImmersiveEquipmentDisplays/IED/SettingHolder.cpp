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
			using namespace Serialization;

			try
			{
				Json::Value root;

				ReadData(m_path, root);

				ParserState      state;
				Parser<Settings> parser(state);

				auto tmp = std::make_unique_for_overwrite<Settings>();

				if (!parser.Parse(root, *tmp))
				{
					throw std::exception("parser error");
				}

				data = std::move(*tmp);

				m_loadHasErrors = state.has_errors();
				m_dirty         = false;

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
			using namespace Serialization;

			try
			{
				Json::Value root;

				ParserState      state;
				Parser<Settings> parser(state);

				parser.Create(data, root);

				WriteData(m_path, root);

				m_dirty = false;

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
			return m_dirty ? Save() : true;
		}

	}
}