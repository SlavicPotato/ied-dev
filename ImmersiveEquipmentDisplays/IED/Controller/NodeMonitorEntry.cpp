#include "pch.h"

#include "NodeMonitorEntry.h"

namespace IED
{
	using namespace ::Util::Node;

	template <class Tp>
	constexpr bool FindRecursive(
		NiNode* a_node,
		Tp      a_func)
	{
		for (auto& object : a_node->m_children)
		{
			if (Traverse(object, a_func) == VisitorControl::kStop)
			{
				return true;
			}
		}

		return false;
	}

	bool NodeMonitorEntry::Update()
	{
		auto n =
			m_config.data.flags.test(Data::NodeMonitorFlags::kRecursive) ?
				FindRecursive(m_parent, [&](NiAVObject* a_object) {
					return a_object && a_object->m_name == m_config.subject ?
			                   VisitorControl::kStop :
                               VisitorControl::kContinue;
				}) :
                static_cast<bool>(FindChildNode(m_parent, m_config.subject));

		bool result = n != m_present;

		m_present = n;

		return result;
	}
}