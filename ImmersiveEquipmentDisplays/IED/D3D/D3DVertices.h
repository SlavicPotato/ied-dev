#pragma once

namespace IED
{
	struct VertexPositionColorAV
	{
		VertexPositionColorAV() = default;

		VertexPositionColorAV(const VertexPositionColorAV&) = default;
		VertexPositionColorAV& operator=(const VertexPositionColorAV&) = default;

		VertexPositionColorAV(VertexPositionColorAV&&) = default;
		VertexPositionColorAV& operator=(VertexPositionColorAV&&) = default;

		constexpr VertexPositionColorAV(
			DirectX::XMVECTOR const& a_position,
			DirectX::XMVECTOR const& a_color) noexcept :
			position(a_position),
			color(a_color)
		{
		}

		DirectX::XMVECTOR position;
		DirectX::XMVECTOR color;

		static constexpr unsigned int  InputElementCount = 2;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};
}