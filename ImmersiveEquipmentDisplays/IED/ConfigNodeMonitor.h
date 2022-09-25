#pragma once

namespace IED
{
	namespace Data
	{
		enum class NodeMonitorFlags : std::uint32_t
		{
			kNone = 0,

			kRecursive          = 1u << 0,
			kTargetAllSkeletons = 1u << 1,
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeMonitorFlags);

		enum class NodeMonitorTestType : std::uint32_t
		{
			kObject                = 0,
			kNode                  = 1,
			kGeometry              = 2,
			kNodeWithGeometryChild = 3
		};

		struct configNodeMonitorEntry_t
		{
			stl::flag<NodeMonitorFlags> flags{ NodeMonitorFlags::kNone };
			std::uint32_t               uid{ 0 };
			NodeMonitorTestType         testType{ NodeMonitorTestType::kObject };
			stl::vector<std::int32_t>   targetSkeletons;
			stl::fixed_string           description;
			stl::fixed_string           parent;
			stl::fixed_string           subject;
		};

		using configNodeMonitorEntryList_t = std::list<configNodeMonitorEntry_t>;

		struct configNodeMonitorEntryBS_t
		{
			configNodeMonitorEntryBS_t(
				const configNodeMonitorEntry_t& a_in) :
				data(a_in),
				parent(a_in.parent.c_str()),
				subject(a_in.subject.c_str())
			{
			}

			configNodeMonitorEntry_t data;
			BSFixedString            parent;
			BSFixedString            subject;
		};
	}
}