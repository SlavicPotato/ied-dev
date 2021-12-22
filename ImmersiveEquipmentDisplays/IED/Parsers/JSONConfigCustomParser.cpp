#include "pch.h"

#include "JSONConfigBaseParser.h"
#include "JSONConfigCachedFormParser.h"
#include "JSONConfigCustomParser.h"
#include "JSONFormListParser.h"
#include "JSONFormParser.h"
#include "JSONRangeParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configCustom_t>::Parse(
			const Json::Value& a_in,
			Data::configCustom_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configBase_t> pbase(m_state);
			Parser<Data::configRange_t> prange(m_state);
			Parser<Data::configCachedForm_t> pform(m_state);
			Parser<Data::configFormList_t> pformList(m_state);

			if (!pbase.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			pform.Parse(a_in["item"], a_out.form);
			pform.Parse(a_in["model"], a_out.modelForm);

			if (!prange.Parse(a_in["cr"], a_out.countRange))
			{
				return false;
			}

			if (!pformList.Parse(a_in["extra"], a_out.extraItems, a_version))
			{
				return false;
			}

			a_out.customFlags = static_cast<Data::CustomFlags>(
				a_in.get("cflags", stl::underlying(Data::configCustom_t::DEFAULT_CUSTOM_FLAGS)).asUInt());

			a_out.priority = a_in.get("prio", 0u).asUInt();
			a_out.chance = a_in.get("chance", 100.0f).asFloat();

			return true;
		}

		template <>
		void Parser<Data::configCustom_t>::Create(
			const Data::configCustom_t& a_in,
			Json::Value& a_out) const
		{
			Parser<Data::configBase_t> pbase(m_state);
			Parser<Data::configRange_t> prange(m_state);
			Parser<Data::configCachedForm_t> pform(m_state);
			Parser<Data::configFormList_t> pformList(m_state);

			pbase.Create(a_in, a_out);

			if (a_in.form.get_id())
			{
				pform.Create(a_in.form.get_id(), a_out["item"]);
			}

			if (a_in.modelForm.get_id())
			{
				pform.Create(a_in.modelForm.get_id(), a_out["model"]);
			}

			if (a_in.countRange.min || a_in.countRange.max)
			{
				prange.Create(a_in.countRange, a_out["cr"]);
			}

			if (!a_in.extraItems.empty())
			{
				pformList.Create(a_in.extraItems, a_out["extra"]);
			}

			a_out["cflags"] = stl::underlying(a_in.customFlags.value);
			a_out["prio"] = a_in.priority;
			a_out["chance"] = a_in.chance;
		}

		template <>
		void Parser<Data::configCustom_t>::GetDefault(Data::configCustom_t& a_out) const
		{
		}
	}
}