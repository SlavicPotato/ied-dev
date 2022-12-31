#pragma once

#include "Controller/ObjectLight.h"

namespace IED
{
	class ReferenceLightController :
		public BSTEventSink<TESCellAttachDetachEvent>
	{
		typedef bool (*updateRefrLight_t)(
			TESObjectLIGH*    a1,
			const REFR_LIGHT& a2,
			TESObjectREFR*    a3,
			float             a4) noexcept;

		typedef bool (*setNiPointLightAttenuation_t)(
			NiAVObject*  a_object,
			std::int32_t a_radius) noexcept;

		typedef bool (*shadowSceneNodeCleanupLights_t)(
			RE::ShadowSceneNode* a_ssn,
			NiNode*              a_node,
			bool                 a_lightNodeInvisibilitySwitch,
			bool                 a_remove) noexcept;

		typedef bool (*shadowSceneNodeCleanupCellMoveReAddLight_t)(
			RE::ShadowSceneNode* a_ssn,
			NiLight*             a2,
			bool                 a3) noexcept;

		typedef bool (*refreshLightOnSceneMove_t)(
			RE::ShadowSceneNode* a_ssn,
			RE::BSLight*         a_light) noexcept;

		typedef bool (*shadowSceneNode_UnkQueueBSLight_t)(
			RE::ShadowSceneNode* a_ssn,
			NiLight*             a_light) noexcept;

		struct Entry
		{
			Entry(
				TESObjectLIGH* a_form,
				NiPointLight*  a_light,
				RE::BSLight*   a_bsLight) :
				form(a_form),
				data{ a_light, -1.0f },
				bsLight(a_bsLight)
			{
			}

			TESObjectLIGH*         form;
			REFR_LIGHT             data;
			NiPointer<RE::BSLight> bsLight;
		};

	public:
		[[nodiscard]] static inline constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		void Initialize();

		void OnUpdate(Actor* a_actor) const noexcept;
		void OnActorCrossCellBoundary(Actor* a_actor) const noexcept;
		void OnActorCellAttached(Actor* a_actor) const noexcept;
		void OnRefreshLightOnSceneMove(Actor* a_actor) const noexcept;
		void OnUnkQueueBSLight(Actor* a_actor) const noexcept;

		static void ReAddActorExtraLight(Actor* a_actor) noexcept;

		void AddLight(
			Game::FormID       a_actor,
			TESObjectLIGH*     a_form,
			const ObjectLight& a_light) noexcept;

		void RemoveLight(
			Game::FormID  a_actor,
			NiPointLight* a_light) noexcept;

		void RemoveActor(Game::FormID a_actor) noexcept;

		[[nodiscard]] static ObjectLight AttachLight(
			const TESObjectLIGH* a_lightForm,
			Actor*               a_actor,
			NiNode*              a_object) noexcept;

		static void CleanupLights(NiNode* a_node) noexcept;

		[[nodiscard]] inline constexpr bool GetEnabled() const noexcept
		{
			return m_initialized;
		}

		inline constexpr void SetCellAttachFixEnabled(bool a_switch) noexcept
		{
			m_fixVanillaLightOnCellAttach = a_switch;
		}
		
		inline constexpr void SetNPCLightUpdateFixEnabled(bool a_switch) noexcept
		{
			m_fixVanillaNPCLightUpdates = a_switch;
		}

	private:
		virtual EventResult ReceiveEvent(
			const TESCellAttachDetachEvent*           a_evn,
			BSTEventSource<TESCellAttachDetachEvent>* a_dispatcher) override;

		inline static const auto UpdateRefrLight            = IAL::Address<updateRefrLight_t>(17212, 0);
		inline static const auto NiPointLightSetAttenuation = IAL::Address<setNiPointLightAttenuation_t>(17224, 0);
		inline static const auto CleanupLightsImpl          = IAL::Address<shadowSceneNodeCleanupLights_t>(99732, 106376);
		inline static const auto RefreshLightOnSceneMove    = IAL::Address<refreshLightOnSceneMove_t>(99693, 0);
		inline static const auto SSN_UnkQueueBSLight        = IAL::Address<shadowSceneNode_UnkQueueBSLight_t>(99706, 0);

		mutable boost::shared_mutex                                m_lock;
		stl::unordered_map<Game::FormID, std::forward_list<Entry>> m_data;

		bool m_initialized{ false };
		bool m_fixVanillaLightOnCellAttach{ false };
		bool m_fixVanillaNPCLightUpdates{ false };

		static ReferenceLightController m_Instance;
	};
}