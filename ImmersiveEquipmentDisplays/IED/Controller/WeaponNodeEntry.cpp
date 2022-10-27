#include "pch.h"

#include "WeaponNodeEntry.h"

namespace IED
{
	bool WeaponNodeEntry::has_visible_geometry() const
	{
		return ::Util::Node::HasVisibleGeometry(node);
	}
}