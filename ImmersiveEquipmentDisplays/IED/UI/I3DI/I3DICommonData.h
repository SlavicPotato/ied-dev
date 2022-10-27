#pragma once

#include "IED/D3D/D3DCommon.h"
//#include "IED/D3D/D3DOIT.h"
#include "IED/D3D/D3DPrimitiveBatch.h"

#include "I3DIActorContext.h"
#include "I3DIAssetCache.h"
#include "I3DIObjectController.h"
#include "I3DIPopupWindow.h"
#include "I3DIRay.h"

namespace IED
{
	namespace UI
	{
		class I3DIActorObject;
		class I3DIActorContext;

		struct I3DICommonData
		{
			I3DICommonData(
				ID3D11Device*                      a_device,
				ID3D11DeviceContext*               a_context,
				const DXGI_SWAP_CHAIN_DESC&        a_desc,
				std::unique_ptr<I3DIActorContext>& a_actorContext) noexcept(false) :
				scene(a_device, a_context, a_desc),
				batchNoDepth(a_device, a_context),
				batchDepth(a_device, a_context),
				assets(a_device, a_context),
				actorContext(a_actorContext)
			//oit(a_device, a_desc)
			{
				batchNoDepth.EnableDepth(false);
				batchDepth.EnableDepth(true);
			}

			I3DIRay cursorRay;

			D3DCommon            scene;
			D3DPrimitiveBatch    batchNoDepth;
			D3DPrimitiveBatch    batchDepth;
			I3DIAssetCache       assets;
			I3DIPopupWindow      commonPopup;
			I3DIObjectController objectController;
			//D3DOIT               oit;

			std::optional<Game::FormID>                                        queuedActor;
			stl::unordered_map<Game::FormID, std::shared_ptr<I3DIActorObject>> actors;
			std::unique_ptr<I3DIActorContext>&                                 actorContext;

			void UpdateMouseRay();

			bool IsCurrentActorObject(const I3DIActorObject* a_other) const noexcept;
			
		};

	}
}