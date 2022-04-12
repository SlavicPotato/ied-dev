#pragma once

#include "IED/ConfigEffectShaderFunction.h"

namespace IED
{
	class EffectShaderFunctionBase :
		public Data::configLUIDTag_t
	{
	public:
		EffectShaderFunctionBase(
			const Data::configEffectShaderFunction_t& a_data) noexcept :
			Data::configLUIDTag_t(a_data.get_unique_id())
		{
		}

		virtual ~EffectShaderFunctionBase() noexcept = default;

		void UpdateConfig(const Data::configEffectShaderFunction_t& a_data)
		{
			UpdateConfigImpl(a_data);
			static_cast<Data::configLUIDTag_t&>(*this) = a_data.get_unique_id();
		}

		virtual void Run(BSEffectShaderData* a_data, float a_step)                      = 0;
		virtual void UpdateConfigImpl(const Data::configEffectShaderFunction_t& a_data) = 0;
		virtual void UpdateConfigInitImpl(const Data::configEffectShaderFunction_t& a_data){};

		SKMP_REDEFINE_NEW_PREF()  //
	};

	template <class T>
	inline constexpr auto make_effect_shader_function(
		const Data::configEffectShaderFunction_t& a_data)  //
		noexcept(std::is_nothrow_constructible_v<T>)       //
		requires(std::is_base_of_v<EffectShaderFunctionBase, T>)
	{
		auto result = std::make_unique<T>(a_data);
		result->UpdateConfigImpl(a_data);
		result->UpdateConfigInitImpl(a_data);
		return result;
	}
}