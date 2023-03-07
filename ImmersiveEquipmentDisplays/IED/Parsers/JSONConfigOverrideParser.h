#pragma once

#include "IED/ConfigEquipmentOverrideCondition.h"

#include "Serialization/Serialization.h"

#include "JSONConfigBaseValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <class T>
		class ParserTemplateVA<Data::configOverride_t, T> :
			public ParserBase
		{
		public:
			using ParserBase::ParserBase;

			bool Parse(
				const Json::Value&         a_in,
				Data::configOverride_t<T>& a_out,
				const std::uint32_t        a_version) const
			{
				Parser<T> bvParser(m_state);

				if (!bvParser.Parse(a_in["config"], a_out, a_version))
				{
					return false;
				}

				a_out.overrideFlags = a_in.get("flags", stl::underlying(Data::ConfigOverrideFlags::kNone)).asUInt();

				if (auto& matches = a_in["matches"])
				{
					Parser<Data::equipmentOverrideConditionList_t> mlParser(m_state);

					if (!mlParser.Parse(matches, a_out.conditions))
					{
						return false;
					}
				}

				if (auto& desc = a_in["desc"])
				{
					a_out.description = desc.asString();
				}

				if (auto& group = a_in["group"])
				{
					ParserTemplateVA<Data::configOverrideList_t, T> olparser(m_state);

					if (!olparser.Parse(group, a_out.group))
					{
						return false;
					}
				}

				return true;
			}

			void Create(
				const Data::configOverride_t<T>& a_data,
				Json::Value&                     a_out) const
			{
				Parser<T> bvParser(m_state);

				bvParser.Create(a_data, a_out["config"]);

				a_out["flags"] = stl::underlying(a_data.overrideFlags.value);

				if (!a_data.conditions.empty())
				{
					Parser<Data::equipmentOverrideConditionList_t> mlParser(m_state);

					mlParser.Create(a_data.conditions, a_out["matches"]);
				}

				if (!a_data.description.empty())
				{
					a_out["desc"] = a_data.description;
				}

				if (!a_data.group.empty())
				{
					ParserTemplateVA<Data::configOverrideList_t, T> olparser(m_state);

					olparser.Create(a_data.group, a_out["group"]);
				}
			}
		};

	}
}