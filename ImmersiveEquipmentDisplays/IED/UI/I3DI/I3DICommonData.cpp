#include "pch.h"

#include "I3DICommonData.h"

namespace IED
{
	namespace UI
	{
		void I3DICommonData::UpdateRay()
		{
			auto& io       = ImGui::GetIO();
			auto& viewPort = scene.GetViewport();

			VectorMath::RayCastScreenPt(
				viewPort,
				scene.GetViewMatrix(),
				scene.GetProjectionMatrix(),
				io.MousePos.x,
				io.MousePos.y,
				ray.origin,
				ray.dir);
		}
	}
}