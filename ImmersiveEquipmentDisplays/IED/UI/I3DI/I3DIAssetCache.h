#pragma once

#include "IED/I3DIModelID.h"

#include "IED/D3D/D3DModelData.h"

namespace IED
{
	namespace UI
	{
		class I3DIAssetCache
		{
		public:
			I3DIAssetCache(
				ID3D11Device*        a_device,
				ID3D11DeviceContext* a_context) noexcept(false);

			constexpr auto& GetModel(I3DIModelID a_index) const noexcept
			{
				assert(a_index < I3DIModelID::kMax);
				return m_models[stl::underlying(a_index)];
			}

		private:
			std::array<
				std::shared_ptr<D3DModelData>,
				stl::underlying(I3DIModelID::kMax)>
				m_models;

			static std::array<
				const char*,
				stl::underlying(I3DIModelID::kMax)>
				m_mdbNames;
		};
	}
}