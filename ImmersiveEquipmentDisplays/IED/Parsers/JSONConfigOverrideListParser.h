#pragma once

#include "IED/ConfigEquipment.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <class T>
		class ParserTemplateVA<Data::configOverrideList_t, T> :
			public ParserBase
		{
			static constexpr std::uint32_t CURRENT_VERSION = 1;

		public:
			using ParserBase::ParserBase;

			bool Parse(
				const Json::Value&             a_in,
				Data::configOverrideList_t<T>& a_out) const
			{
				JSON_PARSE_VERSION()

				ParserTemplateVA<Data::configOverride_t, T> parser(m_state);

				auto& data = a_in["data"];

				for (auto& e : data)
				{
					if (!parser.Parse(e, a_out.emplace_back(), version))
					{
						Error("%s: failed parsing equipment override entry", __FUNCTION__);
						return false;
					}
				}

				return true;
			}

			void Create(
				const Data::configOverrideList_t<T>& a_data,
				Json::Value&                         a_out) const
			{
				auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

				ParserTemplateVA<Data::configOverride_t, T> parser(m_state);

				for (auto& e : a_data)
				{
					parser.Create(e, data.append(Json::Value()));
				}

				a_out["version"] = CURRENT_VERSION;
			}
		};

	}
}