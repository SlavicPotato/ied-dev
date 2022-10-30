#include "pch.h"

#include "I3DICommonData.h"

#include "I3DIActorContext.h"

namespace IED
{
	namespace UI
	{
		void I3DICommonData::UpdateMouseRay()
		{
			auto& io       = ImGui::GetIO();

			VectorMath::RayCastScreenPt(
				scene.GetViewport(),
				scene.GetViewMatrix(),
				scene.GetProjectionMatrix(),
				io.MousePos.x,
				io.MousePos.y,
				cursorRay.origin,
				cursorRay.dir);
		}

		bool I3DICommonData::IsCurrentActorObject(
			const I3DIActorObject* a_other) const noexcept
		{
			return actorContext && actorContext->GetActorObject().get() == a_other;
		}
	}
}