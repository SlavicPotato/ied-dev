#include "pch.h"

#include "JSONConfigNodeMonitorEntryParser.h"

#include "JSONConfigSkeletonMatchParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeMonitorEntry_t>::Parse(
			const Json::Value&              a_in,
			Data::configNodeMonitorEntry_t& a_out) const
		{
			a_out.data.flags = a_in.get("flags", stl::underlying(Data::NodeMonitorFlags::kNone)).asUInt();

			if (auto& uid = a_in["uid"])
			{
				a_out.data.uid = uid.asUInt();
			}
			else
			{
				throw std::exception("no uid");
			}

			a_out.data.testType = static_cast<Data::NodeMonitorTestType>(
				a_in.get("test", stl::underlying(Data::NodeMonitorTestType::kObject)).asUInt());

			if (auto& skels = a_in["skeletons"])
			{
				Parser<Data::configSkeletonMatch_t> smparser(m_state);

				if (!smparser.Parse(skels, a_out.data.targetSkeletons))
				{
					throw std::exception("bad skeleton match");
				}
			}

			a_out.data.description = a_in["desc"].asString();

			a_out.parent = a_in["parent"].asString();

			if (a_out.parent.empty())
			{
				throw std::exception("bad parent");
			}

			if (auto& subject = a_in["subject"])
			{
				auto s = a_in["subject"].asString();
				if (s.empty())
				{
					throw std::exception("bad subject");
				}

				a_out.subjects.emplace_back(std::move(s));
			}

			if (auto& subject = a_in["additional_subjects"])
			{
				for (auto& e : subject)
				{
					auto s = e.asString();
					if (s.empty())
					{
						throw std::exception("bad subject");
					}

					a_out.subjects.emplace_back(std::move(s));
				}
			}

			if (a_out.subjects.empty())
			{
				throw std::exception("no subjects defined");
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