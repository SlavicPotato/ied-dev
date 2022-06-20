#include "pch.h"

#include "I3DIDropTarget.h"

#include "IED/D3D/D3DObject.h"

namespace IED
{
	namespace UI
	{
		I3DIDropTarget::I3DIDropTarget(
			D3DObject& a_owner) :
			m_owner(a_owner)
		{
		}
	}
}