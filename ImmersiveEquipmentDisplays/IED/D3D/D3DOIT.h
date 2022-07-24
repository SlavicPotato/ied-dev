#pragma once

namespace IED
{
	class D3DCommon;

	class D3DOIT
	{
		using render_func_t = std::function<void()>;

	public:
		D3DOIT(ID3D11Device* pDevice, const DXGI_SWAP_CHAIN_DESC& a_swapChainDesc)
		noexcept(false);

		void Render(D3DCommon& a_scene, render_func_t a_renderFunc);

	private:
		void CreateFragmentCount(D3DCommon& a_scene, render_func_t a_renderFunc);
		void CreatePrefixSum(ID3D11DeviceContext* pD3DContext);
		void FillDeepBuffer(D3DCommon& a_scene, render_func_t a_renderFunc);
		void SortAndRenderFragments(D3DCommon& a_scene);

	protected:
		struct CS_CB
		{
			UINT nFrameWidth;
			UINT nFrameHeight;
			UINT nPassSize;
			UINT nReserved;
		};

		struct PS_CB
		{
			UINT nFrameWidth;
			UINT nFrameHeight;
			UINT nReserved0;
			UINT nReserved1;
		};

		UINT        m_nFrameHeight;
		UINT        m_nFrameWidth;
		DXGI_FORMAT m_format;

		// Shaders
		Microsoft::WRL::ComPtr<ID3D11PixelShader>   m_pFragmentCountPS;           // Counts the number of fragments in each pixel
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_pCreatePrefixSum_Pass0_CS;  // Creates the prefix sum in two passes, converting the
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_pCreatePrefixSum_Pass1_CS;  //   two dimensional frame buffer to a 1D prefix sum
		Microsoft::WRL::ComPtr<ID3D11PixelShader>   m_pFillDeepBufferPS;          // Fills the deep frame buffer with depth and color values
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_pSortAndRenderCS;           // Sorts and renders the fragments to the final frame buffer

		// States
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;

		// Constant Buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pCS_CB;  // Compute shader constant buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pPS_CB;  // Pixel shader constant buffer

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pFragmentCountBuffer;  // Keeps a count of the number of fragments rendered to each pixel
		Microsoft::WRL::ComPtr<ID3D11Buffer>    m_pPrefixSum;            // Count of total fragments in the frame buffer preceding each pixel
		Microsoft::WRL::ComPtr<ID3D11Buffer>    m_pDeepBuffer;           // Buffer that holds the depth of each fragment
		Microsoft::WRL::ComPtr<ID3D11Buffer>    m_pDeepBufferColor;      // Buffer that holds the color of each fragment

		// Debug Buffers used to copy resources to main memory to view more easily
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pPrefixSumDebug;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pDeepBufferDebug;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pDeepBufferColorDebug;

		// Unordered Access views of the buffers
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pFragmentCountUAV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pPrefixSumUAV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pDeepBufferUAV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pDeepBufferColorUAV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pDeepBufferColorUAV_UINT;  // Used to veiw the color buffer as a single UINT instead of 4 bytes

		// Shader Resource Views
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pFragmentCountRV;
	};
}