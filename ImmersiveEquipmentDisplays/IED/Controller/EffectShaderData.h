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
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		EffectShaderData(
			BIPED_OBJECT                            a_bipedObject,
			NiNode*                                 a_sheathNode,
			NiNode*                                 a_sheathNode1p,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

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

			Entry(const Entry&)            = delete;
			Entry(Entry&&) noexcept        = default;
			Entry& operator=(const Entry&) = delete;
			Entry& operator=(Entry&&)      = default;

			/*struct node_t
			{
				NiPointer<BSShaderProperty> prop;
			};*/

			stl::fixed_string                                      key;
			stl::flag<EntryFlags>                                  flags{ EntryFlags::kNone };
			RE::BSTSmartPointer<BSEffectShaderData>                shaderData;
			stl::vector<std::unique_ptr<EffectShaderFunctionBase>> functions;
			stl::flat_set<BSFixedString>                           targetNodes;

			//stl::vector<node_t>                                    nodes;

			SKMP_FORCEINLINE void update_effect_data(float a_step) const noexcept
			{
				auto sdata = shaderData.get();
				assert(sdata);

				for (auto& e : functions)
				{
					e->Run(sdata, a_step);
				}
			}

			void create_function_list(const Data::configEffectShaderFunctionList_t& a_data) noexcept;
		};

		using data_type = stl::vector<Entry>;

		[[nodiscard]] inline constexpr bool operator==(
			const Data::configEffectShaderHolder_t& a_rhs) const noexcept
		{
			return tag == a_rhs;
		}

		/*[[nodiscard]] inline constexpr explicit operator bool() const noexcept
		{
			return tag.has_value();
		}*/

		bool UpdateIfChanged(
			NiAVObject*                             a_root,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		bool UpdateIfChanged(
			const ActorObjectHolder&                a_holder,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		void Update(
			NiAVObject*                             a_root,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		void Update(
			const ActorObjectHolder&                a_holder,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		bool UpdateConfigValues(
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		void ClearEffectShaderDataFromTree(NiAVObject* a_object) noexcept;

		/*template <class Tf>
		constexpr void visit_nodes(Tf a_func)                       //
			noexcept(std::is_nothrow_invocable_v<Tf, node_t&>)  //
			requires(std::invocable<Tf, node_t&>)
		{
			for (auto& e : data)
			{
				for (auto& f : e.nodes)
				{
					a_func(e, f);
				}
			}
		}*/

		[[nodiscard]] inline constexpr auto& GetSheathNode(bool a_firstPerson) const noexcept
		{
			return a_firstPerson ? sheathNode1p : sheathNode;
		}

		data_type               data;
		BIPED_OBJECT            bipedObject{ BIPED_OBJECT::kNone };
		NiPointer<NiNode>       sheathNode;
		NiPointer<NiNode>       sheathNode1p;
		bool                    targettingEquipped{ false };
		std::optional<luid_tag> tag;

	private:
		void Update(const Data::configEffectShaderHolder_t& a_data) noexcept;
	};

	DEFINE_ENUM_CLASS_BITWISE(EffectShaderData::EntryFlags);

}