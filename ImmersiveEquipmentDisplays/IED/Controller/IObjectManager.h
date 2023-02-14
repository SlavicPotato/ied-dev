#pragma once

#include "ControllerCommon.h"
#include "IED/ConfigModelGroup.h"
#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/ProcessParams.h"
#include "IModel.h"
#include "INode.h"
#include "IPersistentCounter.h"
#include "IRNG.h"
#include "ISound.h"
#include "ObjectDatabase.h"
#include "ObjectManagerData.h"

namespace IED
{
	class IObjectManager :
		public ObjectManagerData,
		public ObjectDatabase,
		public IPersistentCounter,
		public IRNG,
		public ISound,
		virtual public ILog
	{
	public:
		FN_NAMEPROC("IObjectManager");

		constexpr void SetPlaySound(bool a_switch) noexcept
		{
			m_playSound = a_switch;
		}

		constexpr void SetPlaySoundNPC(bool a_switch) noexcept
		{
			m_playSoundNPC = a_switch;
		}

		[[nodiscard]] constexpr auto& GetLock() const noexcept
		{
			return m_lock;
		}

		bool RemoveActorImpl(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		bool RemoveActorImpl(
			TESObjectREFR*                   a_actor,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		bool RemoveActorImpl(
			Game::FormID                     a_actor,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		/*void QueueReSinkAnimationGraphs(
			Game::FormID a_actor);*/

		void RequestEvaluate(
			Game::FormID a_actor,
			bool         a_defer,
			bool         a_xfrmUpdate,
			bool         a_xfrmUpdateNoDefer) const noexcept;

		void RequestEvaluateLF(
			Game::FormID a_actor) const noexcept;

		// use when acquiring global lock may be detrimental to performance
		void QueueRequestEvaluate(
			Game::FormID a_actor,
			bool         a_defer,
			bool         a_xfrmUpdate,
			bool         a_xfrmUpdateNoDefer = false) const noexcept;

		void QueueRequestEvaluateLF(
			Game::FormID a_actor) const noexcept;

		void QueueRequestEvaluateAll() const noexcept;
		void QueueRequestEvaluateLFAll() const noexcept;

		void QueueRequestEvaluate(
			TESObjectREFR* a_actor,
			bool           a_defer,
			bool           a_xfrmUpdate,
			bool           a_xfrmUpdateNoDefer = false) const noexcept;

		void QueueClearVariablesOnAll(bool a_requestEval) noexcept;

		void QueueClearVariables(
			Game::FormID a_handle,
			bool         a_requestEval) noexcept;

		void QueueRequestVariableUpdateOnAll() const noexcept;
		void QueueRequestVariableUpdate(Game::FormID a_handle) const noexcept;

	protected:
		bool RemoveObject(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			ObjectEntryBase&                 a_objectEntry,
			ActorObjectHolder&               a_data,
			stl::flag<ControllerUpdateFlags> a_flags,
			bool                             a_defer) noexcept;

		void RemoveActorGear(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		bool RemoveActorGear(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_holder,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		bool RemoveInvisibleObjects(
			ActorObjectHolder&    a_holder,
			Game::ObjectRefHandle a_handle) noexcept;

		void ClearObjectsImpl() noexcept;

		static void GetNodeName(
			TESForm*                     a_form,
			const IModel::modelParams_t& a_params,
			char (&a_out)[INode::NODE_NAME_BUFFER_SIZE]) noexcept;

		bool LoadAndAttach(
			processParams_t&                a_params,
			const Data::configBaseValues_t& a_activeConfig,
			const Data::configBase_t&       a_config,
			ObjectEntryBase&                a_objectEntry,
			TESForm*                        a_form,
			TESForm*                        a_modelForm,
			const bool                      a_leftWeapon,
			const bool                      a_visible,
			const bool                      a_disableHavok,
			const bool                      a_physics) noexcept;

		bool LoadAndAttachGroup(
			processParams_t&                a_params,
			const Data::configBaseValues_t& a_activeConfig,
			const Data::configBase_t&       a_baseConfig,
			const Data::configModelGroup_t& a_group,
			ObjectEntryBase&                a_objectEntry,
			TESForm*                        a_form,
			const bool                      a_leftWeapon,
			const bool                      a_visible,
			const bool                      a_disableHavok,
			const bool                      a_physics) noexcept;

		static void FinalizeObjectState(
			std::unique_ptr<ObjectEntryBase::State>& a_state,
			TESForm*                                 a_form,
			NiNode*                                  a_rootNode,
			const NiPointer<NiNode>&                 a_objectNode,
			targetNodes_t&                           a_targetNodes,
			const Data::configBaseValues_t&          a_config,
			Actor*                                   a_actor) noexcept;

		static void TryMakeArrowState(
			std::unique_ptr<ObjectEntryBase::State>& a_state,
			NiNode*                                  a_object) noexcept;

		static void TryCreatePointLight(
			Actor*                        a_actor,
			NiNode*                       a_object,
			TESObjectLIGH*                a_lightForm,
			const Data::ExtraLightData&   a_config,
			std::unique_ptr<ObjectLight>& a_out) noexcept;

		static TESObjectLIGH* GetLightFormForAttach(TESForm* a_modelForm) noexcept;

		static void TryInitializeAndPlayLoopSound(
			Actor*       a_actor,
			ObjectSound& a_sound) noexcept;

		static BGSSoundDescriptorForm* GetSoundDescriptor(
			const TESForm* a_modelForm) noexcept;

		void PlayEquipObjectSound(
			const processParams_t&          a_params,
			const Data::configBaseValues_t& a_config,
			const ObjectEntryBase&          a_objectEntry,
			bool                            a_equip) noexcept;

		static bool AttachNodeImpl(
			NiNode*                     a_root,
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference,
			const ObjectEntryBase&      a_entry) noexcept;

		bool m_playSound{ false };
		bool m_playSoundNPC{ false };

		mutable stl::recursive_mutex m_lock;
	};

}