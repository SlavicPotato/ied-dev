#pragma once

#include "EffectShaderFunctionBase.h"

#include "IED/ConfigEffectShaderFunction.h"

namespace IED
{
	namespace ESF
	{
		enum class PulseFunction : std::uint8_t
		{
			Sine,
			Cosine
		};

		class Pulse :
			public EffectShaderFunctionBase
		{
			inline static constexpr auto PI  = std::numbers::pi_v<float>;
			inline static constexpr auto PI2 = PI * 2.0f;

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
			bool                         inverse;
			bool                         exponential;
			float                        pos{ 0.0f };
			float                        maxexpr{ 1.0f };
		};
	}
}