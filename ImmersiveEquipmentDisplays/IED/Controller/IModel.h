#pragma once

#include "ModelType.h"

namespace IED
{
	class IModel
	{
	public:
		struct modelParams_t
		{
			ModelType type;
			const char* path{ nullptr };
			TESModelTextureSwap* swap{ nullptr };
			bool isShield{ false };
			TESObjectARMA* arma{ nullptr };
		};

		template <
			class T,
			class = std::enable_if_t<
				std::is_convertible_v<
					T,
					TESForm>>>
		static bool ExtractFormModelParams(
			TESForm* a_form,
			modelParams_t& a_out,
			ModelType a_type = ModelType::kMisc);

		static bool GetModelParams(
			Actor* a_actor,
			TESForm* a_form,
			TESRace* a_race,
			bool a_isFemale,
			bool a_1pWeap,
			bool a_useWorld,
			modelParams_t& a_out);
	};

}  // namespace IED