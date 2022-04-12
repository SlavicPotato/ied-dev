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
			inline static constexpr auto PI_X_2 = std::numbers::pi_v<float> * 2.0f;

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
			PulseFunction                function;
			Data::EffectShaderPulseFlags flags;
			float                        speed;

			float mod{ 0.0f };
		};
	}
}