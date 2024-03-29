#include "pch.h"

#include "WeaponNodeEntry.h"

namespace IED
{
	bool GearNodeEntry::has_visible_geometry() const noexcept
	{
		return ::Util::Node::HasVisibleGeometry(node3p.node);
	}
}