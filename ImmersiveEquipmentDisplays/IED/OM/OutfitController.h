#pragma once

#include "ConfigStoreOutfit.h"

namespace IED
{
	struct ProcessParams;

	namespace OM
	{
		class OutfitController
		{
			friend class boost::serialization::access;

		public:
			OutfitController();

			void EvaluateOutfit(
				const Data::OM::configStoreOutfit_t& a_config,
				IED::ProcessParams&                a_params) noexcept;

		private:
			static void TrySetOutfit(
				IED::ProcessParams&           a_params,
				const Data::OM::configOutfit_t& a_config) noexcept;

			static BGSOutfit* GetOutfit(
				IED::ProcessParams&           a_params,
				const Data::OM::configOutfit_t& a_config) noexcept;
		};
	}
}