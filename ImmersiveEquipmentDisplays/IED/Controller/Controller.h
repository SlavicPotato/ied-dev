#pragma once

#include "../ActorState.h"
#include "../Config.h"
#include "../ConfigOverride.h"
#include "../ConfigOverrideBlockList.h"
#include "../ConfigOverrideDefault.h"
#include "../Data.h"
#include "../Inventory.h"
#include "../StringHolder.h"
#include "ControllerCommon.h"
#include "IEquipment.h"
#include "IForm.h"
#include "IJSONSerialization.h"
#include "IMaintenance.h"
#include "IObjectManager.h"
#include "IUI.h"
#include "NodeProcessorTask.h"

#include "../SettingHolder.h"

#include "Localization/ILocalization.h"

#include "Drivers/Input/Handlers.h"

#include <ext/Serialization.h>

namespace IED
{
	class Controller :
		virtual private ILog,
		private ISerializationBase,
		public IForm,
		private IEquipment,
		public IObjectManager,
		public NodeProcessorTask,
		public IUI,
		public IMaintenance,
		public IJSONSerialization,
		public Localization::ILocalization,
		public BSTEventSink<TESObjectLoadedEvent>,
		public BSTEventSink<TESInitScriptEvent>,
		public BSTEventSink<TESEquipEvent>,
		public BSTEventSink<TESContainerChangedEvent>,
		public BSTEventSink<TESFurnitureEvent>,
		public BSTEventSink<TESDeathEvent>,
		public BSTEventSink<TESSwitchRaceCompleteEvent>,
		public BSTEventSink<MenuOpenCloseEvent>,
		public BSTEventSink<SKSENiNodeUpdateEvent>,
		public BSTEventSink<TESQuestStartStopEvent>
	{
		enum class SerializationVersion : std::uint32_t
		{
			kDataVersion1 = 1,
			kDataVersion2 = 2,
			kDataVersion3 = 3,

			kCurrentVersion = kDataVersion3
		};

		static inline constexpr std::uint32_t SKSE_SERIALIZATION_TYPE_ID = 'DDEI';

		struct actorLookupResult_t
		{
			NiPointer<Actor> actor;
			Game::ObjectRefHandle handle;
		};

		struct actorInfo_t
		{
			NiPointer<Actor> actor;
			Game::ObjectRefHandle handle;
			TESNPC* npc;
			TESRace* race;
			NiNode* root;
			NiNode* npcRoot;
			Data::ConfigSex sex;
			const ActorObjectHolder* objects;
		};

		struct npcRacePair_t
		{
			TESNPC* npc;
			TESRace* race;
		};

		using actorLookupResultMap_t =
			std::unordered_map<Game::ObjectRefHandle, NiPointer<Actor>>;

		friend class NodeProcessorTask;

	public:
		enum class EventSinkInstallationFlags : std::uint8_t
		{
			kNone = 0,

			kT0 = 1ui8 << 0,
			kT1 = 1ui8 << 1,
			kT2 = 1ui8 << 2
		};

		Controller(const std::shared_ptr<const ConfigINI>& a_config);

		Controller(const Controller&) = delete;
		Controller(Controller&&) = delete;
		Controller& operator=(const Controller&) = delete;
		Controller& operator=(Controller&&) = delete;

		void SinkInputEvents();
		void SinkEventsT0();
		bool SinkEventsT1();
		bool SinkEventsT2();
		void InitializeData();
		void InitializeStrings();

	private:
		void InitializeSound();
		void UpdateSoundForms();
		void InitializeInputHandlers();
		void InitializeUI();
		void InitializeConfig();
		void InitializeLocalization();

	public:
		[[nodiscard]] inline const auto* GetBSStringHolder() const noexcept
		{
			return m_bsstrings.get();
		}

		bool RemoveActor(
			TESObjectREFR* a_actor,
			Game::ObjectRefHandle a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveActor(
			TESObjectREFR* a_actor,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveActor(
			Game::FormID a_actor,
			stl::flag<ControllerUpdateFlags> a_flags);

		/*void RemoveActorByHandle(
			Game::ObjectRefHandle a_mhandle,
			Game::ObjectRefHandle a_rhandle);*/

		void QueueNiNodeUpdate(Game::FormID a_actor);

		void QueueEvaluate(TESObjectREFR* a_actor, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueEvaluate(Game::ActorHandle a_handle, stl::flag<ControllerUpdateFlags> a_flags);
		//void QueueTestTransforms(TESObjectREFR* a_actor);
		//void QueueEvaluate2(TESObjectREFR* a_actor, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueEvaluate(Game::FormID a_actor, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueEvaluateNPC(Game::FormID a_npc, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueEvaluateRace(Game::FormID a_race, stl::flag<ControllerUpdateFlags> a_flags);

		// use when acquiring global lock may be detrimental to performance
		void QueueRequestEvaluate(Game::FormID a_actor, bool a_defer, bool a_xfrmUpdate, bool a_xfrmUpdateNoDefer = false) const;

		void RequestEvaluate(Game::FormID a_actor, bool a_defer, bool a_xfrmUpdate, bool a_xfrmUpdateNoDefer) const;
		void QueueEvaluateAll(stl::flag<ControllerUpdateFlags> a_flags);

		// use when acquiring global lock may be detrimental to performance
		void QueueRequestEvaluateTransformsActor(Game::FormID a_actor, bool a_noDefer) const;

		void RequestEvaluateTransformsActor(Game::FormID a_actor, bool a_noDefer) const;
		void RequestEvaluateTransformsNPC(Game::FormID a_npc, bool a_noDefer) const;
		void RequestEvaluateTransformsRace(Game::FormID a_race, bool a_noDefer) const;
		void RequestEvaluateTransformsAll(bool a_noDefer) const;

		void QueueActorRemove(TESObjectREFR* a_actor, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueReset(TESObjectREFR* a_actor, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueReset(Game::FormID a_actor, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueResetNPC(Game::FormID a_npc, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueResetRace(Game::FormID a_race, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueReset(Game::FormID a_actor, stl::flag<ControllerUpdateFlags> a_flags, Data::ObjectSlot a_slot);
		void QueueResetNPC(Game::FormID a_npc, stl::flag<ControllerUpdateFlags> a_flags, Data::ObjectSlot a_slot);
		void QueueResetRace(Game::FormID a_race, stl::flag<ControllerUpdateFlags> a_flags, Data::ObjectSlot a_slot);
		void QueueResetAll(stl::flag<ControllerUpdateFlags> a_flags);
		void QueueResetAll(stl::flag<ControllerUpdateFlags> a_flags, Data::ObjectSlot a_slot);
		void QueueClearObjects();

		void QueueUpdateTransformSlot(Game::FormID a_actor, Data::ObjectSlot a_slot);
		void QueueUpdateTransformSlotNPC(Game::FormID a_npc, Data::ObjectSlot a_slot);
		void QueueUpdateTransformSlotRace(Game::FormID a_race, Data::ObjectSlot a_slot);
		void QueueUpdateTransformSlotAll(Data::ObjectSlot a_slot);
		void QueueAttachSlotNode(Game::FormID a_actor, Data::ObjectSlot a_slot, bool a_evalIfNone);
		void QueueAttachSlotNodeNPC(Game::FormID a_npc, Data::ObjectSlot a_slot, bool a_evalIfNone);
		void QueueAttachSlotNodeRace(Game::FormID a_race, Data::ObjectSlot a_slot, bool a_evalIfNone);
		void QueueAttachSlotNodeAll(Data::ObjectSlot a_slot, bool a_evalIfNone);

		/*void QueueUpdateTransformCustom(Game::FormID a_actor, Data::ObjectSlot a_slot);
		void QueueUpdateTransformCustomAll(Data::ObjectSlot a_slot);
		void QueueAttachCustomNode(Game::FormID a_actor, Data::ObjectSlot a_slot, bool a_evalIfNone);
		void QueueAttachCustomNodeAll(Data::ObjectSlot a_slot, bool a_evalIfNone);*/

		void QueueResetCustom(
			Game::FormID a_actor,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueResetCustomNPC(
			Game::FormID a_npc,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueResetCustomRace(
			Game::FormID a_race,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueResetCustom(
			Game::FormID a_actor,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueResetCustomNPC(
			Game::FormID a_npc,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueResetCustomRace(
			Game::FormID a_race,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueResetCustom(
			Game::FormID a_actor,
			Data::ConfigClass a_class);

		void QueueResetCustomNPC(
			Game::FormID a_npc,
			Data::ConfigClass a_class);

		void QueueResetCustomRace(
			Game::FormID a_race,
			Data::ConfigClass a_class);

		void QueueResetCustomAll(
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueResetCustomAll(
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueResetCustomAll(
			Data::ConfigClass a_class);

		void QueueUpdateTransformCustom(
			Game::FormID a_actor,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateTransformCustomNPC(
			Game::FormID a_npc,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateTransformCustomRace(
			Game::FormID a_race,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateTransformCustom(
			Game::FormID a_actor,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateTransformCustomNPC(
			Game::FormID a_npc,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateTransformCustomRace(
			Game::FormID a_race,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateTransformCustom(
			Game::FormID a_actor,
			Data::ConfigClass a_class);

		void QueueUpdateTransformCustomNPC(
			Game::FormID a_npc,
			Data::ConfigClass a_class);

		void QueueUpdateTransformCustomRace(
			Game::FormID a_race,
			Data::ConfigClass a_class);

		void QueueUpdateTransformCustomAll(
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateTransformCustomAll(
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateTransformCustomAll(
			Data::ConfigClass a_class);

		void QueueUpdateAttachCustom(
			Game::FormID a_actor,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateAttachCustomNPC(
			Game::FormID a_npc,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateAttachCustomRace(
			Game::FormID a_race,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateAttachCustom(
			Game::FormID a_actor,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateAttachNPC(
			Game::FormID a_npc,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateAttachRace(
			Game::FormID a_race,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateAttachCustom(
			Game::FormID a_actor,
			Data::ConfigClass a_class);

		void QueueUpdateAttachCustomNPC(
			Game::FormID a_npc,
			Data::ConfigClass a_class);

		void QueueUpdateAttachCustomRace(
			Game::FormID a_race,
			Data::ConfigClass a_class);

		void QueueUpdateAttachCustomAll(
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateAttachCustomAll(
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateAttachCustomAll(
			Data::ConfigClass a_class);

		void QueueEvaluateNearbyActors(bool a_removeFirst = false);

		void QueueLookupFormInfo(Game::FormID a_formId, form_lookup_result_func_t a_func);
		void QueueLookupFormInfoCrosshairRef(form_lookup_result_func_t a_func);
		void QueueGetFormDatabase(form_db_get_func_t a_func);

		void QueueGetCrosshairRef(std::function<void(Game::FormID)> a_func);

		void QueueUpdateSoundForms();

		void Evaluate(
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void AddActorBlock(Game::FormID a_actor, const stl::fixed_string& a_key);
		void RemoveActorBlock(Game::FormID a_actor, const stl::fixed_string& a_key);
		void TogglePlayerBlock();
		bool IsActorBlocked(Game::FormID a_actor) const;
		bool IsActorBlockedImpl(Game::FormID a_actor) const;

		// serialization handlers

		void SaveGameHandler(SKSESerializationInterface* a_intfc);
		void LoadGameHandler(SKSESerializationInterface* a_intfc);
		void RevertHandler(SKSESerializationInterface* a_intfc);

		[[nodiscard]] inline constexpr auto& GetConfigStore() noexcept
		{
			return m_config;
		}

		[[nodiscard]] inline constexpr const auto& GetConfigStore() const noexcept
		{
			return m_config;
		}

		[[nodiscard]] inline constexpr auto& GetInputHandlers() noexcept
		{
			return m_inputHandlers;
		}

		[[nodiscard]] inline constexpr const auto& GetLastException() const noexcept
		{
			return m_lastException;
		}

		bool SaveCurrentConfigAsDefault(stl::flag<Data::ConfigStoreSerializationFlags> a_flags);

		void SaveSettings();

		[[nodiscard]] inline constexpr auto& GetLock() const noexcept
		{
			return m_lock;
		}

		[[nodiscard]] inline constexpr const auto& GetObjects() const noexcept
		{
			return m_objects;
		}

		void QueueUpdateActorInfo(Game::FormID a_actor);
		void QueueUpdateActorInfo(Game::FormID a_actor, std::function<void(bool)> a_callback);
		void QueueUpdateNPCInfo(Game::FormID a_npc, std::function<void(bool)> a_callback);

		[[nodiscard]] inline constexpr auto GetNodeOverrideEnabled() const noexcept
		{
			return m_nodeOverrideEnabled;
		}

		[[nodiscard]] inline constexpr auto GetNodeOverridePlayerEnabled() const noexcept
		{
			return m_nodeOverridePlayerEnabled;
		}

		void StoreActiveHandles();
		void EvaluateStoredHandles(ControllerUpdateFlags a_flags);

		void QueueObjectDatabaseClear();
		void SetObjectDatabaseLevel(ObjectDatabaseLevel a_level);

		[[nodiscard]] inline constexpr auto IsDefaultConfigForced() const noexcept
		{
			return m_forceDefaultConfig;
		}

		[[nodiscard]] inline constexpr auto UIGetIniKeysForced() const noexcept
		{
			return m_iniKeysForced;
		}

		void QueueSetLanguage(const stl::fixed_string& a_lang);

	private:
		FN_NAMEPROC("Controller");

		struct updateActionFunc_t
		{
			using func_t = std::function<bool(
				actorInfo_t& a_info,
				const Data::configCustomEntry_t& a_confEntry,
				objectEntryCustom_t& a_entry)>;

			func_t func;
			bool evalDirty{ false };
			mutable bool clean{ false };
		};

		void EvaluateImpl(
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateImpl(
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			ActorObjectHolder& a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateImpl(
			NiNode* a_root,
			NiNode* a_npcroot,
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateImpl(
			NiNode* a_root,
			NiNode* a_npcroot,
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			ActorObjectHolder& a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateImpl(
			ActorObjectHolder& a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateTransformsImpl(
			Game::FormID a_actor);

		void EvaluateTransformsImpl(
			const ActorObjectHolder& a_objects);

		bool ProcessTransformsImpl(
			NiNode* a_npcRoot,
			Actor* a_actor,
			TESNPC* a_npc,
			TESRace* a_race,
			Data::ConfigSex a_sex,
			const ActorObjectHolder& a_objects,
			const Data::collectorData_t::container_type* a_equippedForms);

		void ActorResetImpl(
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void ActorResetImpl(
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			stl::flag<ControllerUpdateFlags> a_flags,
			Data::ObjectSlot a_slot);

		void ResetCustomImpl(
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void ResetCustomImpl(
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_vkey);

		void ResetCustomImpl(
			Actor* a_actor,
			Game::ObjectRefHandle a_handle,
			Data::ConfigClass a_class);

		void UpdateTransformSlotImpl(
			Game::FormID a_actor,
			Data::ObjectSlot a_slot);

		void UpdateTransformSlotImpl(
			ActorObjectHolder& a_record,
			Data::ObjectSlot a_slot);

		static void UpdateRootPaused(NiNode* a_root);

		updateActionFunc_t MakeTransformUpdateFunc();
		updateActionFunc_t MakeAttachUpdateFunc();

		static const Data::configBaseValues_t& GetConfigForActor(
			Actor* a_actor,
			TESRace* a_race,
			const Data::configCustom_t& a_config,
			const ActorObjectHolder::slot_container_type& a_slots);

		static const Data::configBaseValues_t& GetConfigForActor(
			Actor* a_actor,
			TESRace* a_race,
			const Data::configSlot_t& a_config,
			const objectEntrySlot_t& a_entry);

		void UpdateCustomImpl(
			Game::FormID a_actor,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey,
			updateActionFunc_t a_func);

		void UpdateCustomNPCImpl(
			Game::FormID a_npc,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey,
			updateActionFunc_t a_func);

		void UpdateCustomRaceImpl(
			Game::FormID a_race,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey,
			updateActionFunc_t a_func);

		void UpdateCustomImpl(
			Game::FormID a_actor,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			updateActionFunc_t a_func);

		void UpdateCustomNPCImpl(
			Game::FormID a_npc,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			updateActionFunc_t a_func);

		void UpdateCustomRaceImpl(
			Game::FormID a_race,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			updateActionFunc_t a_func);

		void UpdateCustomImpl(
			Game::FormID a_actor,
			Data::ConfigClass a_class,
			updateActionFunc_t a_func);

		void UpdateCustomNPCImpl(
			Game::FormID a_npc,
			Data::ConfigClass a_class,
			updateActionFunc_t a_func);

		void UpdateCustomRaceImpl(
			Game::FormID a_race,
			Data::ConfigClass a_class,
			updateActionFunc_t a_func);

		void UpdateCustomImpl(
			ActorObjectHolder& a_record,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey,
			const updateActionFunc_t& a_func);

		void UpdateCustomImpl(
			ActorObjectHolder& a_record,
			Data::ConfigClass a_class,
			const stl::fixed_string& a_pkey,
			const updateActionFunc_t& a_func);

		void UpdateCustomImpl(
			ActorObjectHolder& a_record,
			Data::ConfigClass a_class,
			const updateActionFunc_t& a_func);

		void UpdateCustomImpl(
			actorInfo_t& a_info,
			const Data::configCustomPluginMap_t& a_confPluginMap,
			ActorObjectHolder::customPluginMap_t& a_pluginMap,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey,
			const updateActionFunc_t& a_func);

		void UpdateCustomAllImpl(
			actorInfo_t& a_info,
			const Data::configCustomPluginMap_t& a_confPluginMap,
			ActorObjectHolder::customPluginMap_t& a_pluginMap,
			const stl::fixed_string& a_pkey,
			const updateActionFunc_t& a_func);

		void UpdateCustomAllImpl(
			actorInfo_t& a_info,
			const Data::configCustomPluginMap_t& a_confPluginMap,
			ActorObjectHolder::customPluginMap_t& a_pluginMap,
			const updateActionFunc_t& a_func);

		void UpdateCustomImpl(
			actorInfo_t& a_info,
			const Data::configCustomEntryMap_t& a_confEntryMap,
			ActorObjectHolder::customEntryMap_t& a_entryMap,
			const stl::fixed_string& a_vkey,
			const updateActionFunc_t& a_func);

		void UpdateTransformCustomImpl(
			actorInfo_t& a_info,
			const Data::configCustom_t& a_configEntry,
			const Data::configTransform_t& a_xfrmConfigEntry,
			objectEntryCustom_t& a_entry);

		void AttachSlotNodeImpl(
			Game::FormID a_actor,
			Data::ObjectSlot a_slot,
			bool a_evalIfNone);

		bool AttachSlotNodeImpl(
			ActorObjectHolder& a_record,
			Data::ObjectSlot a_slot,
			bool a_evalIfNone);

		bool AttachNodeImpl(
			const actorInfo_t& a_info,
			NiNode* a_root,
			const Data::NodeDescriptor& a_node,
			bool a_atmReference,
			objectEntryBase_t& a_cacheEntry);

		bool AttachNodeImpl(
			NiNode* a_root,
			const Data::NodeDescriptor& a_node,
			bool a_atmReference,
			objectEntryBase_t& a_cacheEntry);

		bool ProcessItemUpdate(
			processParams_t& a_params,
			const Data::configBaseValues_t& a_config,
			const Data::configModelGroup_t* a_groupConfig,
			const Data::NodeDescriptor& a_node,
			objectEntryBase_t& a_entry,
			bool a_visible);

		void ProcessSlots(processParams_t& a_params);

		bool CustomEntryValidateInventoryForm(
			processParams_t& a_params,
			const Data::collectorData_t::itemData_t& a_itemData,
			const Data::configCustom_t& a_config,
			bool& a_hasMinCount);

		Data::collectorData_t::container_type::iterator CustomEntrySelectInventoryForm(
			processParams_t& a_params,
			const Data::configCustom_t& a_config,
			bool& a_hasMinCount);

		bool IsBlockedByChance(
			processParams_t& a_params,
			const Data::configCustom_t& a_config,
			objectEntryCustom_t& a_objectEntry);

		bool ProcessCustomEntry(
			processParams_t& a_params,
			const Data::configCustom_t& a_config,
			objectEntryCustom_t& a_cacheEntry);

		void ProcessCustomEntryMap(
			processParams_t& a_params,
			const Data::configCustomHolder_t& a_confData,
			ActorObjectHolder::customEntryMap_t& a_entryMap);

		void ProcessCustomMap(
			processParams_t& a_params,
			const Data::configCustomPluginMap_t& a_confPluginMap,
			Data::ConfigClass a_class);

		void ProcessCustom(processParams_t& a_params);

		void SaveLastEquippedItems(
			Actor* a_actor,
			const equippedItemInfo_t& a_info,
			ActorObjectHolder& a_cache);

		bool GetVisibilitySwitch(
			Actor* a_actor,
			stl::flag<Data::FlagsBase> a_flags,
			processParams_t& a_params);

		bool LookupTrackedActor(
			Game::FormID a_actor,
			actorLookupResult_t& a_out);

		bool LookupTrackedActor(
			const ActorObjectHolder& a_record,
			actorLookupResult_t& a_out);

		bool LookupCachedActorInfo(
			const ActorObjectHolder& a_record,
			actorInfo_t& a_out);

		void CollectKnownActors(
			actorLookupResultMap_t& a_out);

		bool SetLanguageImpl(const stl::fixed_string& a_lang);

		// events
		virtual EventResult ReceiveEvent(
			const TESObjectLoadedEvent* a_evn,
			BSTEventSource<TESObjectLoadedEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESInitScriptEvent* a_evn,
			BSTEventSource<TESInitScriptEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESEquipEvent* evn,
			BSTEventSource<TESEquipEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESContainerChangedEvent* a_evn,
			BSTEventSource<TESContainerChangedEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESFurnitureEvent* a_evn,
			BSTEventSource<TESFurnitureEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESDeathEvent* a_evn,
			BSTEventSource<TESDeathEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESSwitchRaceCompleteEvent* a_evn,
			BSTEventSource<TESSwitchRaceCompleteEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const MenuOpenCloseEvent* evn,
			BSTEventSource<MenuOpenCloseEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESQuestStartStopEvent* evn,
			BSTEventSource<TESQuestStartStopEvent>* a_dispatcher) override;

		void FillGlobalSlotConfig(Data::configStoreSlot_t& a_data) const;
		//void FillInitialConfig(Data::configStore_t& a_data) const;

		static bool GetNPCRacePair(Actor* a_actor, npcRacePair_t& a_out) noexcept;

		// serialization

		virtual std::size_t Store(boost::archive::binary_oarchive& a_out) override;

		virtual std::size_t Load(
			SKSESerializationInterface* a_intfc,
			std::uint32_t a_version,
			boost::archive::binary_iarchive& a_in) override;

		// ui overrides

		virtual constexpr WCriticalSection& UIGetLock() noexcept override
		{
			return m_lock;
		}

		virtual void OnUIOpen() override;

		// actor info overrides

		virtual constexpr const Data::configStore_t& AIGetConfigStore() noexcept override
		{
			return m_config.active;
		}

		// json serialization

		virtual constexpr WCriticalSection& JSGetLock() noexcept override
		{
			return m_lock;
		}

		virtual constexpr Data::configStore_t& JSGetConfigStore() noexcept override
		{
			return m_config.active;
		}

		virtual void JSOnDataImport() override;

		// members

		std::unique_ptr<BSStringHolder> m_bsstrings;
		std::shared_ptr<const ConfigINI> m_iniconf;

		Data::actorBlockList_t m_actorBlockList;

		bool m_nodeOverrideEnabled{ false };
		bool m_nodeOverridePlayerEnabled{ false };
		bool m_forceDefaultConfig{ false };
		bool m_npcProcessingDisabled{ false };
		bool m_iniKeysForced{ false };

		struct
		{
			Handlers::ComboKeyPressHandler playerBlock;
			Handlers::ComboKeyPressHandler uiOpen;
		} m_inputHandlers;

		struct
		{
			Data::configStore_t active;
			Data::configStore_t stash;
			Data::configStore_t initial;
			Data::SettingHolder settings;
		} m_config;

		Data::actorStateHolder_t m_storedActorStates;

		RandomNumberGenerator<float> m_rng1;

		std::vector<Game::ObjectRefHandle> m_activeHandles;

		stl::flag<EventSinkInstallationFlags> m_esif{ EventSinkInstallationFlags::kNone };

		except::descriptor m_lastException;

		mutable WCriticalSection m_lock;
	};

	DEFINE_ENUM_CLASS_BITWISE(Controller::EventSinkInstallationFlags);

}
