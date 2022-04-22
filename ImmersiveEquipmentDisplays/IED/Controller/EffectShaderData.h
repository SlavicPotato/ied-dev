#pragma once

#include "EffectShaderFunctionBase.h"

#include "IED/ConfigEffectShader.h"

namespace IED
{
	struct EffectShaderData
	{
		enum class EntryFlags : std::uint32_t
		{
			kNone = 0,

			kForce = 1u << 0
		};

		struct Entry
		{
			Entry(const stl::fixed_string& a_key) :
				key(a_key)
			{
			}

			~Entry() = default;

			Entry(const Entry&) = delete;
			Entry(Entry&&)      = default;
			Entry& operator=(const Entry&) = delete;
			Entry& operator=(Entry&&) = default;

			struct node_t
			{
				NiPointer<BSShaderProperty> prop;
			};

			stl::fixed_string                                      key;
			stl::flag<EntryFlags>                                  flags{ EntryFlags::kNone };
			RE::BSTSmartPointer<BSEffectShaderData>                shaderData;
			stl::vector<node_t>                                    nodes;
			stl::vector<std::unique_ptr<EffectShaderFunctionBase>> functions;

			void update_effect_data(float a_step) const;
			void create_function_list(const Data::configEffectShaderFunctionList_t& a_data);
		};

		using data_type = stl::vector<Entry>;

		[[nodiscard]] inline constexpr bool operator==(
			const Data::configEffectShaderHolder_t& a_rhs) const
		{
			return tag == a_rhs;
		}

		[[nodiscard]] inline constexpr explicit operator bool() const noexcept
		{
			return tag.has_value();
		}

		void clear();

		bool UpdateIfChanged(
			NiNode*                                 a_object,
			const Data::configEffectShaderHolder_t& a_data);

		void Update(
			NiNode*                                 a_object,
			const Data::configEffectShaderHolder_t& a_data);

		bool UpdateConfigValues(
			const Data::configEffectShaderHolder_t& a_data);

		template <class Tf>
		constexpr void visit_nodes(Tf a_func)
		{
			for (auto& e : data)
			{
				for (auto& f : e.nodes)
				{
					a_func(e, f);
				}
			}
		}

		data_type               data;
		std::optional<luid_tag> tag;
	};

	DEFINE_ENUM_CLASS_BITWISE(EffectShaderData::EntryFlags);

}