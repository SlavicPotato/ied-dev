#include "pch.h"

#include "MOVNodeEntry.h"

namespace IED
{
	bool MOVNodeEntry::has_visible_geometry() const
	{
		return ::Util::Node::HasVisibleGeometry(node);
	}

	bool MOVNodeEntry::parent_has_visible_geometry() const
	{
		return ::Util::Node::HasVisibleGeometry(node->m_parent);
	}
}