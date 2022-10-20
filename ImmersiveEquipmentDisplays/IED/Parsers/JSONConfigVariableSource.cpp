#include "pch.h"

#include "JSONConfigVariableSource.h"

#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configVariableSource_t>::Parse(
			const Json::Value&            a_in,
			Data::configVariableSource_t& a_out) const
		{
			Parser<Game::FormID> fp(m_state);

			fp.Parse(a_in["form"], a_out.form);

			a_out.flags = a_in.get("flags", stl::underlying(Data::VariableSourceFlags::kNone)).asUInt();

			a_out.source = static_cast<Data::VariableSource>(
				a_in.get("src", stl::underlying(Data::VariableSource::kActor)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configVariableSource_t>::Create(
			const Data::configVariableSource_t& a_data,
			Json::Value&                        a_out) const
		{
			Parser<Game::FormID> fp(m_state);

			fp.Create(a_data.form, a_out["form"]);

			a_out["flags"] = a_data.flags.underlying();
			a_out["src"]   = stl::underlying(a_data.source);
		}

	}
}