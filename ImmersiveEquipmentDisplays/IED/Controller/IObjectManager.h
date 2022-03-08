#pragma once

#include "ControllerCommon.h"
#include "IED/ConfigOverride.h"
#include "IED/ConfigOverrideModelGroup.h"
#include "IED/Data.h"
#include "IED/ProcessParams.h"
#include "IModel.h"
#include "INode.h"
#include "INodeOverride.h"
#include "ISound.h"
#include "ObjectDatabase.h"
#include "ObjectManagerData.h"

namespace IED
{
	class IObjectManager :
		public INode,
		public IModel,
		public ISound,
		public INodeOverride,
		public ObjectManagerData,
		public ObjectDatabase,
		virtual public ILog
	{
	public:
		FN_NAMEPROC("IObjectManager");

		inline void SetPlaySound(bool a_switch) noexcept
		{
			m_playSound = a_switch;
		}

		inline void SetPlaySoundNPC(bool a_switch) noexcept
		{
			m_playSoundNPC = a_switch;
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

	protected:
		void CleanupActorObjectsImpl(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_data,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveObject(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			objectEntryBase_t&               a_objectEntry,
			const ActorObjectHolder&         a_data,
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

		/*void CleanupActorObjectsImpl(
			ActorObjectHolder& a_objects,
			Game::ObjectRefHandle a_rhandle);*/

		bool RemoveInvisibleObjects(
			ActorObjectHolder&    a_objects,
			Game::ObjectRefHandle a_handle);

		void ClearObjectsImpl();

		bool ConstructArmorNode(
			TESForm*                                          a_form,
			const std::vector<TESObjectARMA*>&                a_in,
			bool                                              a_isFemale,
			std::vector<ObjectDatabase::ObjectDatabaseEntry>& a_dbEntries,
			NiPointer<NiNode>&                                a_out);

		/*bool ConstructModelGroup(
			const Data::configModelGroup_t& a_in,
			bool a_isFemale,
			std::vector<ObjectDatabase::ObjectDatabaseEntry>& a_dbEntries,
			NiPointer<NiNode>& a_out);*/

		void GetNodeName(
			TESForm*             a_form,
			const modelParams_t& a_params,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]);

		bool LoadAndAttach(
			processParams_t&                a_params,
			const Data::configBaseValues_t& a_entry,
			objectEntryBase_t&              a_objectEntry,
			TESForm*                        a_form,
			TESForm*                        a_modelForm,
			bool                            a_leftWeapon,
			bool                            a_visible,
			bool                            a_disableHavok);

		bool LoadAndAttachGroup(
			processParams_t&                a_params,
			const Data::configBaseValues_t& a_configEntry,
			const Data::configModelGroup_t& a_group,
			objectEntryBase_t&              a_objectEntry,
			TESForm*                        a_form,
			bool                            a_leftWeapon,
			bool                            a_visible,
			bool                            a_disableHavok);

		void FinalizeObjectState(
			std::unique_ptr<objectEntryBase_t::State>& a_state,
			TESForm*                                   a_form,
			NiNode*                                    a_node,
			NiNode*                                    a_objectNode,
			nodesRef_t&                                a_targetNodes,
			const Data::configBaseValues_t&            a_config);

		void PlayObjectSound(
			const processParams_t&          a_params,
			const Data::configBaseValues_t& a_config,
			const objectEntryBase_t&        a_objectEntry,
			bool                            a_equip);

		bool m_playSound{ false };
		bool m_playSoundNPC{ false };
	};

}