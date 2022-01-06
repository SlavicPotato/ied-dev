#pragma once

#include "../ConfigOverride.h"
#include "../Data.h"
#include "../ProcessParams.h"
#include "ControllerCommon.h"
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
			TESObjectREFR* a_actor,
			Game::ObjectRefHandle a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveActorImpl(
			TESObjectREFR* a_actor,
			stl::flag<ControllerUpdateFlags> a_flags);

	protected:
		void CleanupActorObjectsImpl(
			TESObjectREFR* a_actor,
			Game::ObjectRefHandle a_handle,
			ActorObjectHolder& a_data,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveObject(
			TESObjectREFR* a_actor,
			Game::ObjectRefHandle a_handle,
			objectEntryBase_t& a_objectEntry,
			const ActorObjectHolder& a_data,
			stl::flag<ControllerUpdateFlags> a_flags);

		void RemoveActorGear(
			TESObjectREFR* a_actor,
			Game::ObjectRefHandle a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void RemoveActorGear(
			TESObjectREFR* a_actor,
			Game::ObjectRefHandle a_handle,
			ActorObjectHolder& a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		/*void CleanupActorObjectsImpl(
			ActorObjectHolder& a_objects,
			Game::ObjectRefHandle a_rhandle);*/

		bool RemoveInvisibleObjects(
			ActorObjectHolder& a_objects,
			Game::ObjectRefHandle a_handle);

		void ClearObjectsImpl();

		bool ConstructArmorNode(
			TESForm* a_form,
			const std::vector<TESObjectARMA*>& a_in,
			bool a_isFemale,
			std::vector<ObjectDatabase::ObjectDatabaseEntry>& a_dbEntries,
			NiPointer<NiNode>& a_out);

		bool LoadAndAttach(
			processParams_t& a_params,
			const Data::configBaseValues_t& a_entry,
			const Data::NodeDescriptor& a_node,
			objectEntryBase_t& a_objectEntry,
			TESForm* a_form,
			bool a_leftWeapon,
			bool a_loadArma,
			bool a_visible);

		void PlayObjectSound(
			const processParams_t& a_params,
			const Data::configBaseValues_t& a_config,
			const objectEntryBase_t& a_objectEntry,
			bool a_equip);

		bool m_playSound{ false };
		bool m_playSoundNPC{ false };
	};

}  // namespace IED