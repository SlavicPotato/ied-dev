#pragma once

#include "EffectShaderFunctionBase.h"
#include "ShaderTextureLoadTask.h"

#include "IED/ConfigData.h"
#include "IED/ConfigEffectShader.h"

namespace IED
{
	class ActorObjectHolder;

	struct EffectShaderData
	{
		EffectShaderData(
			const ActorObjectHolder&                a_owner,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		EffectShaderData(
			const ActorObjectHolder&                a_owner,
			BIPED_OBJECT                            a_bipedObject,
			NiNode*                                 a_sheathNode,
			NiNode*                                 a_sheathNode1p,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		~EffectShaderData();

		struct Entry
		{
			explicit Entry(
				const stl::fixed_string&                  a_key,
				RE::BSTSmartPointer<BSEffectShaderData>&& a_shaderData,
				const Data::configEffectShaderData_t&     a_config);

			~Entry() = default;

			Entry(const Entry&)            = delete;
			Entry(Entry&&) noexcept        = default;
			Entry& operator=(const Entry&) = delete;
			Entry& operator=(Entry&&)      = default;

			void UpdateEffectData(float a_step) const noexcept;

			stl::fixed_string                                      key;
			stl::flag<Data::EffectShaderDataFlags>                 flags{ Data::EffectShaderDataFlags::kNone };
			RE::BSTSmartPointer<BSEffectShaderData>                shaderData;
			stl::vector<std::unique_ptr<EffectShaderFunctionBase>> functions;
			stl::flat_set<BSFixedString>                           targetNodes;
		};

		struct PendingEntry : Entry
		{
			explicit PendingEntry(
				const stl::fixed_string&                  a_key,
				RE::BSTSmartPointer<BSEffectShaderData>&& a_shaderData,
				NiPointer<ShaderTextureLoadTask>&&        a_task,
				const Data::configEffectShaderData_t&     a_config) :
				Entry(
					a_key,
					std::move(a_shaderData),
					a_config),
				task(std::move(a_task))
			{
			}

			PendingEntry(const PendingEntry&)            = delete;
			PendingEntry(PendingEntry&&) noexcept        = default;
			PendingEntry& operator=(const PendingEntry&) = delete;
			PendingEntry& operator=(PendingEntry&&)      = default;

			~PendingEntry() = default;

			NiPointer<ShaderTextureLoadTask> task;
		};

		using data_type         = stl::vector<Entry>;
		using pending_data_type = stl::vector<PendingEntry>;

		[[nodiscard]] constexpr bool operator==(
			const Data::configEffectShaderHolder_t& a_rhs) const noexcept
		{
			return tag == a_rhs;
		}

		/*[[nodiscard]] constexpr explicit operator bool() const noexcept
		{
			return tag.has_value();
		}*/

		bool UpdateIfChanged(
			const ActorObjectHolder&                a_owner,
			NiAVObject*                             a_root,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		bool UpdateIfChanged(
			const ActorObjectHolder&                a_holder,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		void Update(
			const ActorObjectHolder&                a_owner,
			NiAVObject*                             a_root,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		void Update(
			const ActorObjectHolder&                a_holder,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		bool UpdateConfigValues(
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		void ClearEffectShaderDataFromTree(NiAVObject* a_object) noexcept;

		bool ProcessPendingLoad(const NiPointer<ShaderTextureLoadTask>& a_task) noexcept;

		[[nodiscard]] constexpr auto& GetSheathNode(bool a_firstPerson) const noexcept
		{
			return a_firstPerson ? sheathNode1p : sheathNode;
		}

		static inline void EnableBackgroundLoading(bool a_switch) noexcept
		{
			_backgroundLoad.store(a_switch, std::memory_order_relaxed);
		}

		data_type               data;
		BIPED_OBJECT            bipedObject{ BIPED_OBJECT::kNone };
		bool                    targettingEquipped{ false };
		NiPointer<NiNode>       sheathNode;
		NiPointer<NiNode>       sheathNode1p;
		std::optional<luid_tag> tag;
		pending_data_type       pending;

	private:
		void UpdateImpl(
			const ActorObjectHolder&                a_owner,
			const Data::configEffectShaderHolder_t& a_data) noexcept;

		void CancelPendingTasks() noexcept;

		static std::atomic<bool> _backgroundLoad;
	};

}