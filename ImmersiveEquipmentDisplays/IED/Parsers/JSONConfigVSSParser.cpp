#include "pch.h"

#include "JSONConfigFixedStringListParser.h"
#include "JSONConfigVSSParser.h"
#include "JSONConfigVariableSource.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configVariableSourceSelector_t>::Parse(
			const Json::Value&                    a_in,
			Data::configVariableSourceSelector_t& a_out) const
		{
			Parser<Data::configVariableSource_t> vsparser(m_state);

			if (!vsparser.Parse(a_in["vsrc"], a_out.varSource))
			{
				return false;
			}

			Parser<Data::configFixedStringList_t> fslparser(m_state);

			if (!fslparser.Parse(a_in["fvars"], a_out.formVars))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::configVariableSourceSelector_t>::Create(
			const Data::configVariableSourceSelector_t& a_data,
			Json::Value&                                a_out) const
		{
			Parser<Data::configVariableSource_t> vsparser(m_state);

			vsparser.Create(a_data.varSource, a_out["vsrc"]);

			Parser<Data::configFixedStringList_t> fslparser(m_state);

			fslparser.Create(a_data.formVars, a_out["fvars"]);
		}

	}
}