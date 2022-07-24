#include "pch.h"

#include "D3DOIT.h"

#include "D3DCommon.h"
#include "D3DHelpers.h"

namespace IED
{
	using namespace DirectX;

	D3DOIT::D3DOIT(
		ID3D11Device*               pDevice,
		const DXGI_SWAP_CHAIN_DESC& a_swapChainDesc) noexcept(false)
	{
		// Create Shaders
		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorblob;

		ThrowIfFailed(D3DCompileFromFile(
			L"Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\OIT_PS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"FragmentCountPS",
			"ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			blob.ReleaseAndGetAddressOf(),
			errorblob.ReleaseAndGetAddressOf()));

		ThrowIfFailed(pDevice->CreatePixelShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pFragmentCountPS.ReleaseAndGetAddressOf()));

		ThrowIfFailed(D3DCompileFromFile(
			L"Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\OIT_CS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"CreatePrefixSum_Pass0_CS",
			"cs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			blob.ReleaseAndGetAddressOf(),
			errorblob.ReleaseAndGetAddressOf()));

		ThrowIfFailed(pDevice->CreateComputeShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pCreatePrefixSum_Pass0_CS.ReleaseAndGetAddressOf()));

		ThrowIfFailed(D3DCompileFromFile(
			L"Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\OIT_CS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"CreatePrefixSum_Pass1_CS",
			"cs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			blob.ReleaseAndGetAddressOf(),
			errorblob.ReleaseAndGetAddressOf()));

		ThrowIfFailed(pDevice->CreateComputeShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pCreatePrefixSum_Pass1_CS.ReleaseAndGetAddressOf()));

		ThrowIfFailed(D3DCompileFromFile(
			L"Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\OIT_PS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"FillDeepBufferPS",
			"ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			blob.ReleaseAndGetAddressOf(),
			errorblob.ReleaseAndGetAddressOf()));

		ThrowIfFailed(pDevice->CreatePixelShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pFillDeepBufferPS.ReleaseAndGetAddressOf()));

		ThrowIfFailed(D3DCompileFromFile(
			L"Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\OIT_CS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"SortAndRenderCS",
			"cs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			blob.ReleaseAndGetAddressOf(),
			errorblob.ReleaseAndGetAddressOf()));

		ThrowIfFailed(pDevice->CreateComputeShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pSortAndRenderCS.ReleaseAndGetAddressOf()));

		// Create constant buffers
		D3D11_BUFFER_DESC Desc;
		Desc.Usage          = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags      = 0;
		Desc.ByteWidth      = sizeof(CS_CB);

		ThrowIfFailed(pDevice->CreateBuffer(
			&Desc,
			nullptr,
			m_pCS_CB.ReleaseAndGetAddressOf()));

		Desc.ByteWidth = sizeof(PS_CB);

		ThrowIfFailed(pDevice->CreateBuffer(
			&Desc,
			nullptr,
			m_pPS_CB.ReleaseAndGetAddressOf()));

		// Create depth/stencil state
		D3D11_DEPTH_STENCIL_DESC DSDesc = {};
		DSDesc.DepthEnable              = FALSE;
		DSDesc.StencilEnable            = FALSE;

		ThrowIfFailed(pDevice->CreateDepthStencilState(
			&DSDesc,
			m_pDepthStencilState.ReleaseAndGetAddressOf()));

		//////////////////////////

		m_nFrameWidth  = a_swapChainDesc.BufferDesc.Width;
		m_nFrameHeight = a_swapChainDesc.BufferDesc.Height;
		m_format       = a_swapChainDesc.BufferDesc.Format;

		// Create buffers
		D3D11_BUFFER_DESC descBuf = {};
		descBuf.BindFlags         = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

		// Create the deep frame buffer.
		// This simple allocation scheme for the deep frame buffer allocates space for 8 times the size of the
		// frame buffer, which means that it can hold an average of 8 fragments per pixel.  This will usually waste some
		// space, and in some cases of high overdraw the buffer could run into problems with overflow.  It
		// may be useful to make the buffer size more intelligent to avoid these problems.
		descBuf.ByteWidth           = a_swapChainDesc.BufferDesc.Width * a_swapChainDesc.BufferDesc.Height * 8 * sizeof(float);
		descBuf.StructureByteStride = sizeof(float);
		ThrowIfFailed(pDevice->CreateBuffer(&descBuf, nullptr, m_pDeepBuffer.ReleaseAndGetAddressOf()));

		// Create deep frame buffer for color
		descBuf.StructureByteStride = 4 * sizeof(BYTE);
		descBuf.ByteWidth           = a_swapChainDesc.BufferDesc.Width * a_swapChainDesc.BufferDesc.Height * 8 * 4 * sizeof(BYTE);
		ThrowIfFailed(pDevice->CreateBuffer(&descBuf, nullptr, m_pDeepBufferColor.ReleaseAndGetAddressOf()));

		// Create prefix sum buffer
		descBuf.StructureByteStride = sizeof(float);
		descBuf.ByteWidth           = a_swapChainDesc.BufferDesc.Width * a_swapChainDesc.BufferDesc.Height * sizeof(UINT);
		ThrowIfFailed(pDevice->CreateBuffer(&descBuf, nullptr, m_pPrefixSum.ReleaseAndGetAddressOf()));

		// Create fragment count buffer
		D3D11_TEXTURE2D_DESC desc2D = {};
		desc2D.ArraySize            = 1;
		desc2D.BindFlags            = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc2D.Usage                = D3D11_USAGE_DEFAULT;
		desc2D.Format               = DXGI_FORMAT_R32_UINT;
		desc2D.Width                = a_swapChainDesc.BufferDesc.Width;
		desc2D.Height               = a_swapChainDesc.BufferDesc.Height;
		desc2D.MipLevels            = 1;
		desc2D.SampleDesc.Count     = 1;
		desc2D.SampleDesc.Quality   = 0;

		ThrowIfFailed(pDevice->CreateTexture2D(&desc2D, nullptr, m_pFragmentCountBuffer.ReleaseAndGetAddressOf()));

		// Create Fragment Count Resource View
		D3D11_SHADER_RESOURCE_VIEW_DESC descRV;
		descRV.Format                    = desc2D.Format;
		descRV.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
		descRV.Texture2D.MipLevels       = 1;
		descRV.Texture2D.MostDetailedMip = 0;

		ThrowIfFailed(pDevice->CreateShaderResourceView(m_pFragmentCountBuffer.Get(), &descRV, m_pFragmentCountRV.ReleaseAndGetAddressOf()));

		// Create Unordered Access Views
		D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
		descUAV.Format              = DXGI_FORMAT_R32_FLOAT;
		descUAV.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
		descUAV.Buffer.FirstElement = 0;
		descUAV.Buffer.NumElements  = a_swapChainDesc.BufferDesc.Width * a_swapChainDesc.BufferDesc.Height * 8;
		descUAV.Buffer.Flags        = 0;

		ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_pDeepBuffer.Get(), &descUAV, m_pDeepBufferUAV.ReleaseAndGetAddressOf()));

		descUAV.Format = DXGI_FORMAT_R8G8B8A8_UINT;
		ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_pDeepBufferColor.Get(), &descUAV, m_pDeepBufferColorUAV.ReleaseAndGetAddressOf()));

		descUAV.Format = DXGI_FORMAT_R32_UINT;
		ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_pDeepBufferColor.Get(), &descUAV, m_pDeepBufferColorUAV_UINT.ReleaseAndGetAddressOf()));

		descUAV.Format             = DXGI_FORMAT_R32_UINT;
		descUAV.Buffer.NumElements = a_swapChainDesc.BufferDesc.Width * a_swapChainDesc.BufferDesc.Height;
		ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_pPrefixSum.Get(), &descUAV, m_pPrefixSumUAV.ReleaseAndGetAddressOf()));

		descUAV.Format             = desc2D.Format;
		descUAV.ViewDimension      = D3D11_UAV_DIMENSION_TEXTURE2D;
		descUAV.Texture2D.MipSlice = 0;
		ThrowIfFailed(pDevice->CreateUnorderedAccessView(m_pFragmentCountBuffer.Get(), &descUAV, m_pFragmentCountUAV.ReleaseAndGetAddressOf()));
	}

	void D3DOIT::Render(D3DCommon& a_scene, render_func_t a_renderFunc)
	{
		auto context = a_scene.GetContext().Get();

		// Cache off the old depth/stencil state as we'll be mucking around with it a bit.
		ID3D11DepthStencilState* pDepthStencilStateStored = nullptr;
		UINT                     stencilRef;
		context->OMGetDepthStencilState(&pDepthStencilStateStored, &stencilRef);

		// Create a count of the number of fragments at each pixel location
		CreateFragmentCount(a_scene, a_renderFunc);

		// Create a prefix sum of the fragment counts.  Each pixel location will hold
		// a count of the total number of fragments of every preceding pixel location.
		CreatePrefixSum(context);

		// Fill in the deep frame buffer with depth and color values.  Use the prefix
		// sum to determine where in the deep buffer to place the current fragment.
		FillDeepBuffer(a_scene, a_renderFunc);

		// Sort and render the fragments.  Use the prefix sum to determine where the
		// fragments for each pixel reside.
		SortAndRenderFragments(a_scene);

		// Restore the cached depth/stencil state
		context->OMSetDepthStencilState(pDepthStencilStateStored, stencilRef);
	}

	void D3DOIT::CreateFragmentCount(D3DCommon& a_scene, render_func_t a_renderFunc)
	{
		auto context = a_scene.GetContext().Get();

		//a_scene.ResetRDV(true, true);

		static const FLOAT clearValueFLOAT[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

		context->ClearRenderTargetView(a_scene.m_backup.RenderTargetViews[0], clearValueFLOAT);
		//context->ClearDepthStencilView(a_scene.m_depthStencilViewGameCopy.Get(), D3D11_CLEAR_DEPTH, 1.0, 0);
		a_scene.ResetRDV(false, true);

		// Clear the fragment count buffer
		static const UINT clearValueUINT[1] = { 0 };
		context->ClearUnorderedAccessViewUint(m_pFragmentCountUAV.Get(), clearValueUINT);

		// Draw the transparent geometry
		ID3D11UnorderedAccessView* pUAVs[3];
		pUAVs[0] = m_pFragmentCountUAV.Get();

		ID3D11RenderTargetView* views[]{ a_scene.m_renderTargetViewGameCopy.Get() };

		context->OMSetRenderTargetsAndUnorderedAccessViews(1, views, a_scene.m_depthStencilViewGameCopy.Get(), 1, 1, pUAVs, nullptr);
		context->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0);
		context->PSSetShader(m_pFragmentCountPS.Get(), nullptr, 0);

		a_renderFunc();

		// Set render target and depth/stencil views to nullptr,
		//   we'll need to read the RTV in a shader later
		ID3D11RenderTargetView* pViewNULL[1] = { nullptr };
		ID3D11DepthStencilView* pDSVNULL     = nullptr;
		context->OMSetRenderTargets(1, pViewNULL, pDSVNULL);
	}

	void D3DOIT::CreatePrefixSum(ID3D11DeviceContext* pD3DContext)
	{
		ID3D11UnorderedAccessView* ppUAViewNULL[4] = { nullptr, nullptr, nullptr, nullptr };

		// prepare the constant buffer
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		pD3DContext->Map(m_pCS_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);  // CHECK RV
		auto pCS_CB          = reinterpret_cast<CS_CB*>(MappedResource.pData);
		pCS_CB->nFrameWidth  = m_nFrameWidth;
		pCS_CB->nFrameHeight = m_nFrameHeight;
		pD3DContext->Unmap(m_pCS_CB.Get(), 0);

		ID3D11Buffer* b1[] = { m_pCS_CB.Get() };
		pD3DContext->CSSetConstantBuffers(0, 1, b1);

		// First pass : convert the 2D frame buffer to a 1D array.  We could simply
		//   copy the contents over, but while we're at it, we may as well do
		//   some work and save a pass later, so we do the first summation pass;
		//   add the values at the even indices to the values at the odd indices.
		pD3DContext->CSSetShader(m_pCreatePrefixSum_Pass0_CS.Get(), nullptr, 0);

		ID3D11UnorderedAccessView* pUAViews[1] = { m_pPrefixSumUAV.Get() };
		pD3DContext->CSSetUnorderedAccessViews(3, 1, pUAViews, (UINT*)(&pUAViews));

		ID3D11ShaderResourceView* srvs[] = { m_pFragmentCountRV.Get() };

		pD3DContext->CSSetShaderResources(0, 1, srvs);
		pD3DContext->Dispatch(m_nFrameWidth, m_nFrameHeight, 1);

		// Second and following passes : each pass distributes the sum of the first half of the group
		//   to the second half of the group.  There are n/groupsize groups in each pass.
		//   Each pass doubles the group size until it is the size of the buffer.
		//   The resulting buffer holds the prefix sum of all preceding values in each
		//   position
		ID3D11ShaderResourceView* ppRVNULL[3] = { nullptr, nullptr, nullptr };
		pD3DContext->CSSetShaderResources(0, 1, ppRVNULL);
		pD3DContext->CSSetUnorderedAccessViews(3, 1, ppUAViewNULL, (UINT*)(&ppUAViewNULL));

		// Perform the passes.  The first pass would have been i = 2, but it was performed earlier
		for (UINT i = 4; i < (m_nFrameWidth * m_nFrameHeight * 2); i *= 2)
		{
			pD3DContext->Map(m_pCS_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);  // CHECK RV
			pCS_CB               = reinterpret_cast<CS_CB*>(MappedResource.pData);
			pCS_CB->nPassSize    = i;
			pCS_CB->nFrameWidth  = m_nFrameWidth;
			pCS_CB->nFrameHeight = m_nFrameHeight;
			pD3DContext->Unmap(m_pCS_CB.Get(), 0);

			ID3D11Buffer* b2[] = { m_pCS_CB.Get() };
			pD3DContext->CSSetConstantBuffers(0, 1, b2);

			pD3DContext->CSSetShader(m_pCreatePrefixSum_Pass1_CS.Get(), nullptr, 0);

			pUAViews[0] = m_pPrefixSumUAV.Get();
			pD3DContext->CSSetUnorderedAccessViews(3, 1, pUAViews, (UINT*)(&pUAViews));

			pD3DContext->CSSetShaderResources(0, 1, srvs);

			// the "ceil((float) m_nFrameWidth*m_nFrameHeight/i)" calculation ensures that
			//    we dispatch enough threads to cover the entire range.
			pD3DContext->Dispatch((int)(ceil((float)m_nFrameWidth * m_nFrameHeight / i)), 1, 1);
		}

		// Clear out the resource and unordered access views
		pD3DContext->CSSetShaderResources(0, 1, ppRVNULL);
		pD3DContext->CSSetUnorderedAccessViews(3, 1, ppUAViewNULL, (UINT*)(&ppUAViewNULL));
	}

	void D3DOIT::FillDeepBuffer(D3DCommon& a_scene, render_func_t a_renderFunc)
	{
		auto context = a_scene.GetContext().Get();

		static const FLOAT clearValueFLOAT[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
		static const UINT  clearValueUINT[1]  = { 0 };

		// Clear buffers, render target, and depth/stencil
		context->ClearUnorderedAccessViewFloat(m_pDeepBufferUAV.Get(), clearValueFLOAT);
		context->ClearUnorderedAccessViewUint(m_pFragmentCountUAV.Get(), clearValueUINT);
		context->ClearUnorderedAccessViewUint(m_pDeepBufferColorUAV_UINT.Get(), clearValueUINT);

		//a_scene.ResetRDV(true, true);

		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		context->ClearRenderTargetView(a_scene.m_backup.RenderTargetViews[0], ClearColor);
		//context->ClearDepthStencilView(a_scene.m_depthStencilViewGameCopy.Get(), D3D11_CLEAR_DEPTH, 1.0, 0);
		a_scene.ResetRDV(false, true);

		// Render the Deep Frame buffer using the Prefix Sum buffer to place the fragments in the correct bin
		ID3D11UnorderedAccessView* pUAVs[4];
		pUAVs[0] = m_pFragmentCountUAV.Get();
		pUAVs[1] = m_pDeepBufferUAV.Get();
		pUAVs[2] = m_pDeepBufferColorUAV.Get();
		pUAVs[3] = m_pPrefixSumUAV.Get();

		ID3D11RenderTargetView* views[]{ a_scene.m_renderTargetViewGameCopy.Get() };
		context->OMSetRenderTargetsAndUnorderedAccessViews(1, views, a_scene.m_depthStencilViewGameCopy.Get(), 1, 4, pUAVs, nullptr);

		context->PSSetShader(m_pFillDeepBufferPS.Get(), nullptr, 0);

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		context->Map(m_pPS_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);  // CHECK RV
		auto pPS_CB          = reinterpret_cast<PS_CB*>(MappedResource.pData);
		pPS_CB->nFrameWidth  = m_nFrameWidth;
		pPS_CB->nFrameHeight = m_nFrameHeight;
		context->Unmap(m_pPS_CB.Get(), 0);

		ID3D11Buffer* b1[] = { m_pPS_CB.Get() };
		context->PSSetConstantBuffers(0, 1, b1);

		a_renderFunc();

		ID3D11RenderTargetView* pViews[1] = { nullptr };
		context->OMSetRenderTargets(1, pViews, a_scene.m_depthStencilViewGameCopy.Get());
	}

	void D3DOIT::SortAndRenderFragments(D3DCommon& a_scene)
	{
		auto context = a_scene.GetContext().Get();
		auto device  = a_scene.GetDevice().Get();

		ID3D11UnorderedAccessView* ppUAViewNULL[4] = { nullptr, nullptr, nullptr, nullptr };

		//a_scene.ResetRDV(true, false);

		static const FLOAT clearValueFLOAT[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

		context->ClearRenderTargetView(a_scene.m_backup.RenderTargetViews[0], clearValueFLOAT);

		Microsoft::WRL::ComPtr<ID3D11Resource> pBackBufferRes;
		a_scene.m_renderTargetViewGameCopy->GetResource(pBackBufferRes.ReleaseAndGetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUAView;
		D3D11_UNORDERED_ACCESS_VIEW_DESC                  descUAV{};
		descUAV.Format             = m_format;
		descUAV.ViewDimension      = D3D11_UAV_DIMENSION_TEXTURE2D;
		descUAV.Texture2D.MipSlice = 0;
		device->CreateUnorderedAccessView(pBackBufferRes.Get(), &descUAV, pUAView.ReleaseAndGetAddressOf());

		ID3D11UnorderedAccessView* pUAViews[4] = { m_pDeepBufferUAV.Get(), m_pDeepBufferColorUAV_UINT.Get(), pUAView.Get(), m_pPrefixSumUAV.Get() };

		context->CSSetUnorderedAccessViews(0, 4, pUAViews, (UINT*)(&pUAViews));
		context->CSSetShader(m_pSortAndRenderCS.Get(), nullptr, 0);

		ID3D11ShaderResourceView* srvs[] = { m_pFragmentCountRV.Get() };
		context->CSSetShaderResources(0, 1, srvs);

		context->Dispatch(m_nFrameWidth, m_nFrameHeight, 1);

		ID3D11ShaderResourceView* ppRVNULL[3] = { nullptr, nullptr, nullptr };
		context->CSSetShaderResources(0, 1, ppRVNULL);

		// Unbind resources for CS
		context->CSSetUnorderedAccessViews(0, 4, ppUAViewNULL, (UINT*)(&ppUAViewNULL));
	}

}