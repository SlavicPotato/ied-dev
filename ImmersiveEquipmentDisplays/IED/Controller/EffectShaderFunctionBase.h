#pragma once

#include "IED/ConfigEffectShaderFunction.h"

namespace IED
{
	class EffectShaderFunctionBase :
		public Data::configLUIDTagMCG_t
	{
	public:
		EffectShaderFunctionBase(
			const Data::configEffectShaderFunction_t& a_data) noexcept :
			Data::configLUIDTagMCG_t(static_cast<const luid_tag&>(a_data.get_unique_id())),
			type(a_data.type)
		{
		}

		virtual ~EffectShaderFunctionBase() noexcept = default;

		[[nodiscard]] bool UpdateConfig(const Data::configEffectShaderFunction_t& a_data)
		{
			if (a_data.get_unique_id() != *this ||
				a_data.type != type)
			{
				return false;
			}
			else
			{
				UpdateConfigImpl(a_data);
				return true;
			}
		}

		virtual void Run(BSEffectShaderData* a_data, float a_step)                      = 0;
		virtual void UpdateConfigImpl(const Data::configEffectShaderFunction_t& a_data) = 0;
		virtual void UpdateConfigInitImpl(const Data::configEffectShaderFunction_t& a_data){};

		SKMP_REDEFINE_NEW_PREF()  //

	private:
		Data::EffectShaderFunctionType type;
	};

	template <class T>
	inline constexpr auto make_effect_shader_function(
		const Data::configEffectShaderFunction_t& a_data)  //
		noexcept(std::is_nothrow_constructible_v<T>)       //
		requires(std::is_base_of_v<EffectShaderFunctionBase, T>)
	{
		auto result = std::make_unique<T>(a_data);
		result->UpdateConfigInitImpl(a_data);
		result->UpdateConfigImpl(a_data);
		return result;
	}
}