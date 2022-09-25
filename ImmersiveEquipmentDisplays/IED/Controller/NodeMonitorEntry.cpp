#include "pch.h"

#include "NodeMonitorEntry.h"

namespace IED
{
	using namespace ::Util::Node;

	inline static bool find_visible_geometry(
		NiAVObject* a_object) noexcept
	{
		auto result = Traverse(a_object, [](NiAVObject* a_object) {
			if (a_object->IsHidden())
			{
				return VisitorControl::kSkip;
			}

			return static_cast<bool>(a_object->AsGeometry()) ?
			           VisitorControl::kStop :
                       VisitorControl::kContinue;
		});

		return result == VisitorControl::kStop;
	}

	inline static bool HasVisibleChildObject(
		NiNode*              a_node,
		const BSFixedString& a_name) noexcept
	{
		for (auto& object : a_node->m_children)
		{
			if (!object)
			{
				continue;
			}

			if (object->m_name == a_name &&
			    object->IsVisible())
			{
				return true;
			}
		}

		return false;
	}

	inline static bool HasVisibleChildNode(
		NiNode*              a_node,
		const BSFixedString& a_name) noexcept
	{
		for (auto& object : a_node->m_children)
		{
			if (!object)
			{
				continue;
			}

			if (object->m_name == a_name &&
			    object->IsVisible() &&
			    static_cast<bool>(object->AsNode()))
			{
				return true;
			}
		}

		return false;
	}

	inline static bool HasVisibleChildGeometry(
		NiNode*              a_node,
		const BSFixedString& a_name) noexcept
	{
		for (auto& object : a_node->m_children)
		{
			if (!object)
			{
				continue;
			}

			if (object->m_name == a_name &&
			    find_visible_geometry(object))
			{
				return true;
			}
		}

		return false;
	}

	template <class Tp>
	inline static constexpr bool TraverseChildren(
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

	bool NodeMonitorEntry::Update() noexcept
	{
		bool n;

		switch (m_config.data.testType)
		{
		case Data::NodeMonitorTestType::kObject:

			if (m_config.data.flags.test(Data::NodeMonitorFlags::kRecursive))
			{
				n = TraverseChildren(m_parent, [&](NiAVObject* a_object) {
					if (a_object->IsHidden())
					{
						return VisitorControl::kSkip;
					}

					return a_object->m_name == m_config.subject ?
					           VisitorControl::kStop :
                               VisitorControl::kContinue;
				});
			}
			else
			{
				n = HasVisibleChildObject(m_parent, m_config.subject);
			}

			break;

		case Data::NodeMonitorTestType::kNode:

			if (m_config.data.flags.test(Data::NodeMonitorFlags::kRecursive))
			{
				n = TraverseChildren(m_parent, [&](NiAVObject* a_object) {
					if (a_object->IsHidden())
					{
						return VisitorControl::kSkip;
					}

					return (a_object->m_name == m_config.subject &&
					        static_cast<bool>(a_object->AsNode())) ?
					           VisitorControl::kStop :
                               VisitorControl::kContinue;
				});
			}
			else
			{
				n = HasVisibleChildNode(m_parent, m_config.subject);
			}

			break;

		case Data::NodeMonitorTestType::kGeometry:

			if (m_config.data.flags.test(Data::NodeMonitorFlags::kRecursive))
			{
				n = TraverseChildren(m_parent, [&](NiAVObject* a_object) {
					if (a_object->IsHidden())
					{
						return VisitorControl::kSkip;
					}

					return (a_object->m_name == m_config.subject &&
					        find_visible_geometry(a_object)) ?
					           VisitorControl::kStop :
                               VisitorControl::kContinue;
				});
			}
			else
			{
				n = HasVisibleChildGeometry(m_parent, m_config.subject);
			}

			break;

		default:

			n = false;

			break;
		}

		const bool result = n != m_present;

		m_present = n;

		return result;
	}
}