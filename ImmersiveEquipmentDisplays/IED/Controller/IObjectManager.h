#pragma once

#include "ControllerCommon.h"
#include "IED/ConfigModelGroup.h"
#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/ProcessParams.h"
#include "IModel.h"
#include "INode.h"
#include "INodeOverride.h"
#include "IPersistentCounter.h"
#include "IRNG.h"
#include "ISound.h"
#include "ObjectDatabase.h"
#include "ObjectManagerData.h"

namespace IED
{
	inline static constexpr bool IsActorValid(TESObjectREFR* a_refr) noexcept
	{
		return a_refr &&
		       a_refr->formID != 0 &&
		       a_refr->loadedState &&
		       !a_refr->IsDeleted() &&
		       a_refr->IsActor();
	}

	class IObjectManager :
		public INode,
		public IModel,
		public ISound,
		public INodeOverride,
		public ObjectManagerData,
		public ObjectDatabase,
		public IPersistentCounter,
		public IRNG,
		virtual public ILog
	{
	public:
		FN_NAMEPROC("IObjectManager");

		inline constexpr void SetPlaySound(bool a_switch) noexcept
		{
			m_playSound = a_switch;
		}

		inline constexpr void SetPlaySoundNPC(bool a_switch) noexcept
		{
			m_playSoundNPC = a_switch;
		}

		[[nodiscard]] inline constexpr auto& GetLock() const noexcept
		{
			return m_lock;
		}

		bool RemoveActorImpl(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveActorImpl(
			TESObjectREFR*                   a_actor,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveActorImpl(
			Game::FormID                     a_actor,
			stl::flag<ControllerUpdateFlags> a_flags);

		/*void QueueReSinkAnimationGraphs(
			Game::FormID a_actor);*/

		void RequestEvaluate(
			Game::FormID a_actor,
			bool         a_defer,
			bool         a_xfrmUpdate,
			bool         a_xfrmUpdateNoDefer) const;

		// use when acquiring global lock may be detrimental to performance
		void QueueRequestEvaluate(
			Game::FormID a_actor,
			bool         a_defer,
			bool         a_xfrmUpdate,
			bool         a_xfrmUpdateNoDefer = false) const;

		void QueueRequestEvaluate(
			TESObjectREFR* a_actor,
			bool           a_defer,
			bool           a_xfrmUpdate,
			bool           a_xfrmUpdateNoDefer = false) const;

		void QueueClearVariablesOnAll(bool a_requestEval);

		void QueueClearVariables(
			Game::FormID a_handle,
			bool         a_requestEval);

		void QueueRequestVariableUpdateOnAll() const;
		void QueueRequestVariableUpdate(Game::FormID a_handle) const;

	protected:
		void CleanupActorObjectsImpl(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_data,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveObject(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			ObjectEntryBase&                 a_objectEntry,
			ActorObjectHolder&               a_data,
			stl::flag<ControllerUpdateFlags> a_flags);

		void RemoveActorGear(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void RemoveActorGear(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveInvisibleObjects(
			ActorObjectHolder&    a_objects,
			Game::ObjectRefHandle a_handle);

		void ClearObjectsImpl();

		bool ConstructArmorNode(
			TESForm*                                          a_form,
			const stl::vector<TESObjectARMA*>&                a_in,
			bool                                              a_isFemale,
			stl::vector<ObjectDatabase::ObjectDatabaseEntry>& a_dbEntries,
			NiPointer<NiNode>&                                a_out);

		void GetNodeName(
			TESForm*             a_form,
			const modelParams_t& a_params,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]);

		bool LoadAndAttach(
			processParams_t&                a_params,
			const Data::configBaseValues_t& a_activeConfig,
			const Data::configBase_t&       a_config,
			ObjectEntryBase&                a_objectEntry,
			TESForm*                        a_form,
			TESForm*                        a_modelForm,
			bool                            a_leftWeapon,
			bool                            a_visible,
			bool                            a_disableHavok,
			bool                            a_bhkAnims);

		bool LoadAndAttachGroup(
			processParams_t&                a_params,
			const Data::configBaseValues_t& a_configEntry,
			const Data::configModelGroup_t& a_group,
			ObjectEntryBase&                a_objectEntry,
			TESForm*                        a_form,
			bool                            a_leftWeapon,
			bool                            a_visible,
			bool                            a_disableHavok,
			bool                            a_bgedAnims);

		void FinalizeObjectState(
			std::unique_ptr<ObjectEntryBase::State>& a_state,
			TESForm*                                 a_form,
			NiNode*                                  a_rootNode,
			const NiPointer<NiNode>&                 a_objectNode,
			nodesRef_t&                              a_targetNodes,
			const Data::configBaseValues_t&          a_config);

		void PlayObjectSound(
			const processParams_t&          a_params,
			const Data::configBaseValues_t& a_config,
			const ObjectEntryBase&          a_objectEntry,
			bool                            a_equip);

		bool AttachNodeImpl(
			NiNode*                     a_root,
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference,
			ObjectEntryBase&            a_entry);

		bool m_playSound{ false };
		bool m_playSoundNPC{ false };

		mutable stl::critical_section m_lock;
	};

}