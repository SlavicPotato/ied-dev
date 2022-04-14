#pragma once

#include "EffectShaderFunctionBase.h"

namespace IED
{
	namespace ESF
	{
		class UVLinearMotion :
			public EffectShaderFunctionBase
		{
		public:
			using EffectShaderFunctionBase::EffectShaderFunctionBase;

			virtual void Run(
				BSEffectShaderData* a_data,
				float               a_step) override;

			virtual void UpdateConfigImpl(
				const Data::configEffectShaderFunction_t& a_data) override;

		private:
			float angleSin;
			float angleCos;

			float uOffset{ 0.0f };
			float vOffset{ 0.0f };
		};
	}
}