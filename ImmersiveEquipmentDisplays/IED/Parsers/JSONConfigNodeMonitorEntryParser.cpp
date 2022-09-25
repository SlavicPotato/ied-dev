#include "pch.h"

#include "JSONConfigNodeMonitorEntryParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeMonitorEntry_t>::Parse(
			const Json::Value&              a_in,
			Data::configNodeMonitorEntry_t& a_out) const
		{
			a_out.flags = a_in.get("flags", stl::underlying(Data::NodeMonitorFlags::kNone)).asUInt();

			if (auto& uid = a_in["uid"])
			{
				a_out.uid = uid.asUInt();
			}
			else
			{
				throw std::exception("no uid");
			}

			a_out.testType = static_cast<Data::NodeMonitorTestType>(
				a_in.get("test", stl::underlying(Data::NodeMonitorTestType::kObject)).asUInt());

			if (auto& skels = a_in["skeletons"])
			{
				for (auto& e : skels)
				{
					a_out.targetSkeletons.emplace_back(e.asInt());
				}
			}

			a_out.description = a_in["desc"].asString();

			a_out.parent = a_in["parent"].asString();

			if (a_out.parent.empty())
			{
				throw std::exception("bad parent");
			}

			a_out.subject = a_in["subject"].asString();

			if (a_out.subject.empty())
			{
				throw std::exception("bad subject");
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeMonitorEntry_t>::Create(
			const Data::configNodeMonitorEntry_t& a_data,
			Json::Value&                          a_out) const
		{
			throw std::runtime_error("not implemented");
		}

	}
}