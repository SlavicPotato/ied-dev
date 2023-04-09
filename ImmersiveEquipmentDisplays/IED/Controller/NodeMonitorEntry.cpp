#include "pch.h"

#include "NodeMonitorEntry.h"

namespace IED
{
	using namespace ::Util::Node;

	static constexpr bool HasGeometry(
		NiAVObject*                             a_object,
		const Data::configNodeMonitorEntryBS_t& a_entry) noexcept
	{
		const auto result = Traverse(
			a_object,
			[&](NiAVObject* a_object) noexcept [[msvc::forceinline]] {
				if (!a_entry.check_visibility(a_object))
				{
					return VisitorControl::kSkip;
				}

				return static_cast<bool>(a_object->AsGeometry()) ?
			               VisitorControl::kStop :
			               VisitorControl::kContinue;
			});

		return result == VisitorControl::kStop;
	}

	static constexpr bool HasChildObject(
		NiNode*                                 a_node,
		const Data::configNodeMonitorEntryBS_t& a_entry) noexcept
	{
		for (const auto& object : a_node->m_children)
		{
			if (!object)
			{
				continue;
			}

			if (a_entry.has_subject(object->m_name) &&
			    a_entry.check_visibility(object))
			{
				return true;
			}
		}

		return false;
	}

	static constexpr bool HasChildNode(
		NiNode*                                 a_node,
		const Data::configNodeMonitorEntryBS_t& a_entry) noexcept
	{
		for (const auto& object : a_node->m_children)
		{
			if (!object)
			{
				continue;
			}

			if (a_entry.has_subject(object->m_name) &&
			    a_entry.check_visibility(object) &&
			    static_cast<bool>(object->AsNode()))
			{
				return true;
			}
		}

		return false;
	}

	static constexpr bool HasChildGeometry(
		NiNode*                                 a_node,
		const Data::configNodeMonitorEntryBS_t& a_entry) noexcept
	{
		for (const auto& object : a_node->m_children)
		{
			if (!object)
			{
				continue;
			}

			if (a_entry.has_subject(object->m_name) &&
			    HasGeometry(object, a_entry))
			{
				return true;
			}
		}

		return false;
	}

	static constexpr bool HasChildNodeWithGeometryChild(
		NiNode*                                 a_node,
		const Data::configNodeMonitorEntryBS_t& a_entry) noexcept
	{
		for (const auto& object : a_node->m_children)
		{
			if (!object)
			{
				continue;
			}

			if (a_entry.has_subject(object->m_name) &&
			    a_entry.check_visibility(object))
			{
				if (const auto node = object->AsNode())
				{
					for (const auto& e : node->m_children)
					{
						if (!e)
						{
							continue;
						}

						if (HasGeometry(e, a_entry))
						{
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	template <class Tp>
	static constexpr bool TraverseChildren(
		NiNode*   a_node,
		const Tp& a_func)  //
		noexcept(std::is_nothrow_invocable_r_v<VisitorControl, Tp, NiAVObject*>)
	{
		for (const auto& object : a_node->m_children)
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
				n = TraverseChildren(m_parent, [&](NiAVObject* a_object) noexcept {
					if (!m_config.check_visibility(a_object))
					{
						return VisitorControl::kSkip;
					}

					return m_config.has_subject(a_object->m_name) ?
					           VisitorControl::kStop :
					           VisitorControl::kContinue;
				});
			}
			else
			{
				n = HasChildObject(m_parent, m_config);
			}

			break;

		case Data::NodeMonitorTestType::kNode:

			if (m_config.data.flags.test(Data::NodeMonitorFlags::kRecursive))
			{
				n = TraverseChildren(m_parent, [&](NiAVObject* a_object) noexcept {
					if (!m_config.check_visibility(a_object))
					{
						return VisitorControl::kSkip;
					}

					return (m_config.has_subject(a_object->m_name) &&
					        static_cast<bool>(a_object->AsNode())) ?
					           VisitorControl::kStop :
					           VisitorControl::kContinue;
				});
			}
			else
			{
				n = HasChildNode(m_parent, m_config);
			}

			break;

		case Data::NodeMonitorTestType::kGeometry:

			if (m_config.data.flags.test(Data::NodeMonitorFlags::kRecursive))
			{
				n = TraverseChildren(m_parent, [&](NiAVObject* a_object) noexcept {
					if (!m_config.check_visibility(a_object))
					{
						return VisitorControl::kSkip;
					}

					return (m_config.has_subject(a_object->m_name) &&
					        HasGeometry(a_object, m_config)) ?
					           VisitorControl::kStop :
					           VisitorControl::kContinue;
				});
			}
			else
			{
				n = HasChildGeometry(m_parent, m_config);
			}

			break;

		case Data::NodeMonitorTestType::kNodeWithGeometryChild:

			if (m_config.data.flags.test(Data::NodeMonitorFlags::kRecursive))
			{
				n = TraverseChildren(m_parent, [&](NiAVObject* a_object) noexcept {
					if (!m_config.check_visibility(a_object))
					{
						return VisitorControl::kSkip;
					}

					if (!m_config.has_subject(a_object->m_name))
					{
						return VisitorControl::kContinue;
					}

					if (auto node = a_object->AsNode())
					{
						for (const auto& e : node->m_children)
						{
							if (!e)
							{
								continue;
							}

							if (HasGeometry(e, m_config))
							{
								return VisitorControl::kStop;
							}
						}
					}

					return VisitorControl::kContinue;
				});
			}
			else
			{
				n = HasChildNodeWithGeometryChild(m_parent, m_config);
			}

			break;

		default:

			n = false;

			break;
		}

		const bool result = n != m_present;

		if (result)
		{
			m_present = n;
		}

		return result;
	}
}