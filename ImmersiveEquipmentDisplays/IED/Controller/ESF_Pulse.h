#pragma once

#include "EffectShaderFunctionBase.h"

#include "IED/ConfigEffectShaderFunction.h"

namespace IED
{
	namespace ESF
	{
		class Pulse :
			public EffectShaderFunctionBase
		{
		public:
			using EffectShaderFunctionBase::EffectShaderFunctionBase;

			virtual void Run(
				BSEffectShaderData* a_data,
				float               a_step) override;

			virtual void UpdateConfigImpl(
				const Data::configEffectShaderFunction_t& a_data) override;

			virtual void UpdateConfigInitImpl(
				const Data::configEffectShaderFunction_t& a_data) override;

		private:
			Data::EffectShaderWaveform   function;
			Data::EffectShaderPulseFlags flags;
			float                        ftp;
			float                        exponent;
			float                        pos{ 0.0f };
			float                        maxexpr{ 1.0f };
			float                        ummd;
			float                        umin;
			float                        vmmd;
			float                        vmin;
			float                        rmmd;
			float                        rmin;
			bool                         inverse;
			bool                         exponential;
		};
	}
}