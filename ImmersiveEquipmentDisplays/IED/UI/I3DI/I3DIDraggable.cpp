#include "pch.h"

#include "I3DIDraggable.h"

#include "IED/D3D/D3DObject.h"

namespace IED
{
	namespace UI
	{
		I3DIDraggable::I3DIDraggable(
			std::uint32_t a_type) :
			m_type(a_type)
		{
		}
	}
}