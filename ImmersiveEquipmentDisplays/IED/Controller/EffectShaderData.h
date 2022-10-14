#pragma once

#include "EffectShaderFunctionBase.h"

#include "IED/ConfigData.h"
#include "IED/ConfigEffectShader.h"

namespace IED
{
	class ActorObjectHolder;

	struct EffectShaderData
	{
		EffectShaderData(
			const Data::configEffectShaderHolder_t& a_data);

		EffectShaderData(
			BIPED_OBJECT                            a_bipedObject,
			NiNode*                                 a_sheathNode,
			const Data::configEffectShaderHolder_t& a_data);

		enum class EntryFlags : std::uint32_t
		{
			kNone = 0,

			kForce      = 1u << 0,
			kTargetRoot = 1u << 1,
		};

		struct Entry
		{
			Entry(
				const stl::fixed_string&                  a_key,
				RE::BSTSmartPointer<BSEffectShaderData>&& a_shaderData) :
				key(a_key),
				shaderData(std::move(a_shaderData))
			{
			}

			~Entry() = default;

			Entry(const Entry&)     = delete;
			Entry(Entry&&) noexcept = default;
			Entry& operator=(const Entry&) = delete;
			Entry& operator=(Entry&&) = default;

			/*struct node_t
			{
				NiPointer<BSShaderProperty> prop;
			};*/

			stl::fixed_string                                      key;
			stl::flag<EntryFlags>                                  flags{ EntryFlags::kNone };
			RE::BSTSmartPointer<BSEffectShaderData>                shaderData;
			stl::vector<std::unique_ptr<EffectShaderFunctionBase>> functions;
			stl::set<BSFixedString>                                targetNodes;

			//stl::vector<node_t>                                    nodes;

			void update_effect_data(float a_step) const;
			void create_function_list(const Data::configEffectShaderFunctionList_t& a_data);
		};

		using data_type = stl::vector<Entry>;

		[[nodiscard]] inline constexpr bool operator==(
			const Data::configEffectShaderHolder_t& a_rhs) const
		{
			return tag == a_rhs;
		}

		/*[[nodiscard]] inline constexpr explicit operator bool() const noexcept
		{
			return tag.has_value();
		}*/

		bool UpdateIfChanged(
			NiAVObject*                             a_root,
			const Data::configEffectShaderHolder_t& a_data);

		bool UpdateIfChanged(
			const ActorObjectHolder&                a_holder,
			const Data::configEffectShaderHolder_t& a_data);

		void Update(
			NiAVObject*                             a_root,
			const Data::configEffectShaderHolder_t& a_data);

		void Update(
			const ActorObjectHolder&                a_holder,
			const Data::configEffectShaderHolder_t& a_data);

		bool UpdateConfigValues(
			const Data::configEffectShaderHolder_t& a_data);

		void ClearEffectShaderDataFromTree(NiAVObject* a_object);

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
		BIPED_OBJECT            bipedObject{ BIPED_OBJECT::kNone };
		NiPointer<NiNode>       sheathNode;
		bool                    targettingEquipped{ false };
		std::optional<luid_tag> tag;

	private:
		void Update(
			const Data::configEffectShaderHolder_t& a_data);
	};

	DEFINE_ENUM_CLASS_BITWISE(EffectShaderData::EntryFlags);

}