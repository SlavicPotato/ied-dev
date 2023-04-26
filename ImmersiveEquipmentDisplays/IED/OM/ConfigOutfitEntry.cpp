#include "pch.h"

#include "ConfigOutfitEntry.h"

#include "IED/ConfigBase.h"
#include "IED/ProcessParams.h"

namespace IED
{
	namespace Data
	{
		namespace OM
		{
			const configOutfit_t& configOutfitEntry_t::get_config(
				IED::ProcessParams& a_params) const noexcept
			{
				if (const auto v = get_override_impl(a_params, overrides))
				{
					return *v;
				}
				else
				{
					return *this;
				}
			}

			const outfitOverride_t* configOutfitEntry_t::get_override(
				IED::ProcessParams& a_params) const noexcept
			{
				return get_override_impl(a_params, overrides);
			}

			const outfitOverride_t* configOutfitEntry_t::get_override_impl(
				IED::ProcessParams&       a_params,
				const outfitOverrideList_t& a_list) noexcept
			{
				for (auto& e : a_list)
				{
					if (IED::Data::configBase_t::do_match(e.conditions, a_params, true))
					{
						if (e.overrideFlags.test(IED::Data::ConfigOverrideFlags::kIsGroup))
						{
							if (auto result = get_override_impl(
									a_params,
									e.group))
							{
								return result;
							}

							if (!e.overrideFlags.test(IED::Data::ConfigOverrideFlags::kContinue))
							{
								break;
							}
						}
						else
						{
							return std::addressof(e);
						}
					}
				}

				return nullptr;
			}

		}
	}
}