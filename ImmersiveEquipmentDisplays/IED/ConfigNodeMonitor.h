#pragma once

#include "ConfigSkeletonMatch.h"

namespace IED
{
	namespace Data
	{
		enum class NodeMonitorFlags : std::uint32_t
		{
			kNone = 0,

			kRecursive          = 1u << 0,
			kTargetAllSkeletons = 1u << 1,
			kDetectInvisible    = 1u << 2,
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeMonitorFlags);

		enum class NodeMonitorTestType : std::uint32_t
		{
			kObject                = 0,
			kNode                  = 1,
			kGeometry              = 2,
			kNodeWithGeometryChild = 3,
		};

		struct configNodeMonitorEntryCommonData_t
		{
			stl::flag<NodeMonitorFlags> flags{ NodeMonitorFlags::kNone };
			std::uint32_t               uid{ 0 };
			NodeMonitorTestType         testType{ NodeMonitorTestType::kObject };
			configSkeletonMatch_t       targetSkeletons;
			std::string                 description;
		};

		struct configNodeMonitorEntry_t
		{
			configNodeMonitorEntryCommonData_t data;
			std::string                        parent;
			stl::vector<std::string>           subjects;
		};

		using configNodeMonitorEntryList_t = stl::list<configNodeMonitorEntry_t>;

		struct configNodeMonitorEntryBS_t
		{
			configNodeMonitorEntryBS_t(
				configNodeMonitorEntry_t&& a_in) :
				parent(a_in.parent.c_str()),
				data(std::move(a_in.data))
			{
				for (auto& e : a_in.subjects)
				{
					subjects.emplace(e.c_str());
				}
			}

			[[nodiscard]] constexpr bool has_subject(const BSFixedString& a_string) const
			{
				return subjects.find(a_string) != subjects.end();
			}

			[[nodiscard]] constexpr bool check_visibility(NiAVObject* a_object) const noexcept
			{
				return data.flags.test(NodeMonitorFlags::kDetectInvisible) || a_object->IsVisible();
			}

			configNodeMonitorEntryCommonData_t          data;
			BSFixedString                               parent;
			stl::cache_aligned::flat_set<BSFixedString> subjects;
		};
	}
}