#include "pch.h"

#include "NodeOverrideParams.h"

namespace IED
{
	float nodeOverrideParams_t::get_weapon_adjust() noexcept
	{
		if (!weaponAdjust)
		{
			weaponAdjust = 0.0f;

			if (auto data = get_biped())
			{
				for (auto& e : data->objects)
				{
					if (e.addon && e.addon != e.item)
					{
						if (auto arma = e.addon->As<TESObjectARMA>())
						{
							*weaponAdjust = std::max(
								stl::zero_nan(arma->data.weaponAdjust),
								*weaponAdjust);
						}
					}
				}
			}
		}

		return *weaponAdjust;
	}

	void nodeOverrideParams_t::make_item_data() noexcept
	{
		auto& idata = itemData;
		idata.clear();

		if (const auto* const data = get_biped())
		{
			using enum_type = std::underlying_type_t<BIPED_OBJECT>;

			for (enum_type i = stl::underlying(BIPED_OBJECT::kHead); i < stl::underlying(BIPED_OBJECT::kTotal); i++)
			{
				const auto item = data->objects[i].item;
				if (!item)
				{
					continue;
				}

				const auto addon = data->objects[i].addon;
				if (addon == item)
				{
					continue;
				}

				const auto r = idata.try_emplace(
					item->formID,
					item,
					static_cast<BIPED_OBJECT>(i));

				if (addon)
				{
					if (const auto armor = item->As<TESObjectARMO>())
					{
						if (const auto arma = addon->As<TESObjectARMA>())
						{
							r.first->second.weaponAdjust = std::max(
								arma->data.weaponAdjust,
								r.first->second.weaponAdjust);
						}
					}
				}
			}
		}
	}
}