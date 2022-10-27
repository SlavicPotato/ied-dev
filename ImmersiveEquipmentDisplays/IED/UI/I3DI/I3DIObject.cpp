#include "pch.h"

#include "I3DIObject.h"

#include "I3DICommonData.h"

#include "IED/D3D/D3DCommon.h"
#include "IED/D3D/D3DModelData.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		bool I3DIObject::ObjectIntersects(
			I3DICommonData& a_data,
			const I3DIRay&  a_ray,
			float&          a_dist)
		{
			return false;
		}

		bool I3DIObject::OnSelectInt(I3DICommonData& a_data)
		{
			bool result = OnSelect(a_data);

			if (result)
			{
				m_objectFlags.set(I3DIObjectFlags::kSelected);
			}

			return result;
		}

		void I3DIObject::OnUnselectInt(I3DICommonData& a_data)
		{
			m_objectFlags.clear(I3DIObjectFlags::kSelected);

			OnUnselect(a_data);
		}

	}
}