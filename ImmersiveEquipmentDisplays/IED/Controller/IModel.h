#pragma once

#include "ModelType.h"

namespace IED
{
	class IModel
	{
	public:
		struct ModelParams
		{
			ModelType            type;
			const char*          path{ nullptr };
			TESModelTextureSwap* texSwap{ nullptr };
			TESObjectARMA*       arma{ nullptr };
		};

		static bool GetModelParams(
			Actor*       a_actor,
			TESForm*     a_form,
			TESRace*     a_race,
			bool         a_isFemale,
			bool         a_1pWeap,
			bool         a_useWorld,
			ModelParams& a_out) noexcept;
	};

}