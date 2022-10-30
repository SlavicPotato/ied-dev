#include "pch.h"

#include "I3DIAssetCache.h"

#include "IED/GlobalProfileManager.h"

#include "IED/D3D/D3DAssets.h"
#include "IED/D3D/D3DModelData.h"

namespace IED
{
	namespace UI
	{
		std::array<
			const char*,
			stl::underlying(I3DIModelID::kMax)>
			I3DIAssetCache::m_mdbNames{
				"sphere",
				"movap",

				"1hsword",
				"waraxe",
				"mace",
				"dagger",
				"greatsword",
				"bow",
				"staff",
				"quiver",
				"shield"
			};

		I3DIAssetCache::I3DIAssetCache(
			ID3D11Device*        a_device,
			ID3D11DeviceContext* a_context) noexcept(false)
		{
			auto& data = GlobalProfileManager::GetSingleton<ModelProfile>().Data();

			using enum_type = std::underlying_type_t<I3DIModelID>;

			for (enum_type i = 0; i < stl::underlying(I3DIModelID::kMax); i++)
			{
				auto it = data.find(m_mdbNames[i]);
				if (it == data.end())
				{
					continue;
				}

				m_models[i] = std::make_shared<D3DModelData>(a_device, it->second.Data());
			}
		}
	}
}