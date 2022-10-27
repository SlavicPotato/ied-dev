#include "pch.h"

#include "MOVNodeEntry.h"

namespace IED
{
	bool MOVNodeEntry::has_visible_geometry() const
	{
		return ::Util::Node::HasVisibleGeometry(node);
	}
}