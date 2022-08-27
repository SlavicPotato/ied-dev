#pragma once

#include "IED/ActorState.h"
#include "IED/ConfigBlockList.h"
#include "IED/ConfigCommon.h"
#include "IED/ConfigINI.h"
#include "IED/ConfigSerializationFlags.h"
#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/Inventory.h"
#include "IED/SettingHolder.h"
#include "IED/StringHolder.h"

#include "ActorProcessorTask.h"
#include "BipedDataCache.h"
#include "ControllerCommon.h"
#include "EffectController.h"
#include "IAnimationManager.h"
#include "IEquipment.h"
#include "IForm.h"
#include "IJSONSerialization.h"
#include "IMaintenance.h"
#include "IObjectManager.h"
#include "IUI.h"

#include "Localization/ILocalization.h"

#include "Drivers/Input/Handlers.h"

#include <ext/SDSPlayerShieldOnBackSwitchEvent.h>

namespace IED
{
	class Controller :
		virtual private ILog,
		private ISerializationBase,
		public IForm,
		private IEquipment,
		public IObjectManager,
		public EffectController,
		public ActorProcessorTask,
		public IUI,
		public IMaintenance,
		public IJSONSerialization,
		public IAnimationManager,
		public Localization::ILocalization,
		public ::Events::EventSink<SKSESerializationEvent>,
		public ::Events::EventSink<SKSESerializationLoadEvent>,
		public ::Events::EventSink<SKSEMessagingEvent>,
		public ::Events::EventSink<SDSPlayerShieldOnBackSwitchEvent>,
		public BSTEventSink<TESObjectLoadedEvent>,
		public BSTEventSink<TESInitScriptEvent>,
		public BSTEventSink<TESEquipEvent>,
		public BSTEventSink<TESContainerChangedEvent>,
		public BSTEventSink<TESFurnitureEvent>,
		public BSTEventSink<TESDeathEvent>,
		//public BSTEventSink<TESCombatEvent>, // useless
		public BSTEventSink<TESSwitchRaceCompleteEvent>,
		public BSTEventSink<MenuOpenCloseEvent>,
		//public BSTEventSink<TESSceneEvent>,
		//public BSTEventSink<SKSENiNodeUpdateEvent>,
		//public BSTEventSink<TESQuestStartStopEvent>,
		//public BSTEventSink<TESPackageEvent>,
		public BSTEventSink<TESActorLocationChangeEvent>
	{
		friend class boost::serialization::access;

		enum class SerializationVersion : std::uint32_t
		{
			kDataVersion1 = 1,
			kDataVersion2 = 2,
			kDataVersion3 = 3,
			kDataVersion4 = 4,
			kDataVersion5 = 5,
			kDataVersion6 = 6,
			kDataVersion7 = 7,

			kCurrentVersion = kDataVersion7
		};

		static inline constexpr std::uint32_t SKSE_SERIALIZATION_TYPE_ID = 'DDEI';

		struct actorLookupResult_t
		{
			NiPointer<Actor>      actor;
			Game::ObjectRefHandle handle;
		};

		struct actorInfo_t
		{
			NiPointer<Actor>      actor;
			Game::ObjectRefHandle handle;
			TESNPC*               npc;
			TESNPC*               npcOrTemplate;
			TESRace*              race;
			NiNode*               root;
			NiNode*               npcRoot;
			Data::ConfigSex       sex;
			ActorObjectHolder&    objects;
		};

		struct npcRacePair_t
		{
			TESNPC*  npc;
			TESRace* race;
		};

		using actorLookupResultMap_t =
			stl::unordered_map<Game::ObjectRefHandle, NiPointer<Actor>>;

		friend class ActorProcessorTask;

	public:
		enum : unsigned int
		{
			DataVersion1 = 1,
		};

		enum class EventSinkInstallationFlags : std::uint8_t
		{
			kNone = 0,

			kT0 = 1ui8 << 0,
			kT1 = 1ui8 << 1,
			kT2 = 1ui8 << 2
		};

		Controller(const std::shared_ptr<const ConfigINI>& a_config);

		Controller(const Controller&) = delete;
		Controller(Controller&&)      = delete;
		Controller& operator=(const Controller&) = delete;
		Controller& operator=(Controller&&) = delete;

		void SinkEventsT0();
		bool DispatchIntroBanner();

	private:
		void InitializeSound();
		void UpdateSoundForms();
		void InitializeInputHandlers();
		void InitializeUI();
		void InitializeConfig();
		void InitializeLocalization();
		void LoadAnimationData();
		void InitializeData();
		void GetSDSInterface();
		void InitializeBSFixedStringTable();

		void SinkInputEvents();
		void SinkSerializationEvents();
		bool SinkEventsT1();
		bool SinkEventsT2();

		void OnDataLoaded();

		void StoreActiveHandles();
		void EvaluateStoredHandles(ControllerUpdateFlags a_flags);
		void ClearStoredHandles();

		// serialization handlers

		void SaveGameHandler(SKSESerializationInterface* a_intfc);
		//void LoadGameHandler(SKSESerializationInterface* a_intfc);
		void RevertHandler(SKSESerializationInterface* a_intfc);

	public:
		template <class Tc, class... Args>
		inline auto make_timed_ui_task(
			long long a_lifetime,
			Args&&... a_args)
		{
			auto result = std::make_shared<IUITimedRenderTask>(
				*this,
				a_lifetime);

			result->InitializeContext<Tc>(*this, std::forward<Args>(a_args)...);

			return result;
		}

		bool RemoveActor(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveActor(
			TESObjectREFR*                   a_actor,
			stl::flag<ControllerUpdateFlags> a_flags);

		bool RemoveActor(
			Game::FormID                     a_actor,
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
		void QueueRequestEvaluate(TESObjectREFR* a_actor, bool a_defer, bool a_xfrmUpdate, bool a_xfrmUpdateNoDefer = false) const;

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
		void QueueResetGearAll(stl::flag<ControllerUpdateFlags> a_flags);
		void QueueClearObjects();

		void QueueUpdateTransformSlot(Game::FormID a_actor, Data::ObjectSlot a_slot);
		void QueueUpdateTransformSlotNPC(Game::FormID a_npc, Data::ObjectSlot a_slot);
		void QueueUpdateTransformSlotRace(Game::FormID a_race, Data::ObjectSlot a_slot);
		void QueueUpdateTransformSlotAll(Data::ObjectSlot a_slot);

		void QueueResetAAAll();

		void QueueResetCustom(
			Game::FormID             a_actor,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueResetCustomNPC(
			Game::FormID             a_npc,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueResetCustomRace(
			Game::FormID             a_race,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueResetCustom(
			Game::FormID             a_actor,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey);

		void QueueResetCustomNPC(
			Game::FormID             a_npc,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey);

		void QueueResetCustomRace(
			Game::FormID             a_race,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey);

		void QueueResetCustom(
			Game::FormID      a_actor,
			Data::ConfigClass a_class);

		void QueueResetCustomNPC(
			Game::FormID      a_npc,
			Data::ConfigClass a_class);

		void QueueResetCustomRace(
			Game::FormID      a_race,
			Data::ConfigClass a_class);

		void QueueResetCustomAll(
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueResetCustomAll(
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey);

		void QueueResetCustomAll(
			Data::ConfigClass a_class);

		void QueueUpdateTransformCustom(
			Game::FormID             a_actor,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateTransformCustomNPC(
			Game::FormID             a_npc,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateTransformCustomRace(
			Game::FormID             a_race,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateTransformCustom(
			Game::FormID             a_actor,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateTransformCustomNPC(
			Game::FormID             a_npc,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateTransformCustomRace(
			Game::FormID             a_race,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateTransformCustom(
			Game::FormID      a_actor,
			Data::ConfigClass a_class);

		void QueueUpdateTransformCustomNPC(
			Game::FormID      a_npc,
			Data::ConfigClass a_class);

		void QueueUpdateTransformCustomRace(
			Game::FormID      a_race,
			Data::ConfigClass a_class);

		void QueueUpdateTransformCustomAll(
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void QueueUpdateTransformCustomAll(
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey);

		void QueueUpdateTransformCustomAll(
			Data::ConfigClass a_class);

		void QueueEvaluateNearbyActors(bool a_removeFirst = false);

		void QueueLookupFormInfo(Game::FormID a_formId, form_lookup_result_func_t a_func);
		void QueueLookupFormInfoCrosshairRef(form_lookup_result_func_t a_func);
		void QueueGetFormDatabase(form_db_get_func_t a_func);

		void QueueGetCrosshairRef(std::function<void(Game::FormID)> a_func);

		void QueueUpdateSoundForms();

		void QueueClearRand();

		void QueueSendAnimationEventToActor(
			Game::FormID a_actor,
			std::string  a_event);

		void Evaluate(
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void AddActorBlock(Game::FormID a_actor, const stl::fixed_string& a_key);
		void RemoveActorBlock(Game::FormID a_actor, const stl::fixed_string& a_key);
		void TogglePlayerBlock();
		bool IsActorBlocked(Game::FormID a_actor) const;
		bool IsActorBlockedImpl(Game::FormID a_actor) const;

		bool SkeletonCheck(Game::FormID a_actor);

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

		bool SaveCurrentConfigAsDefault(
			stl::flag<ExportFlags>                         a_exportFlags,
			stl::flag<Data::ConfigStoreSerializationFlags> a_flags);

		void SaveSettings();

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

		[[nodiscard]] inline auto GetEquippedItemCacheSize() const noexcept
		{
			return m_bipedCache.size();
		}

		[[nodiscard]] inline constexpr auto GetEquippedItemCacheSizeMax() const noexcept
		{
			return m_bipedCache.max_size();
		}

		void QueueSetLanguage(const stl::fixed_string& a_lang);

		void ProcessEffectShaders();

	private:
		FN_NAMEPROC("Controller");

		struct updateActionFunc_t
		{
			using func_t = std::function<bool(
				actorInfo_t&                     a_info,
				const Data::configCustomEntry_t& a_confEntry,
				objectEntryCustom_t&             a_entry)>;

			func_t       func;
			bool         evalDirty{ false };
			mutable bool clean{ false };
		};

		void EvaluateImpl(
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateImpl(
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateImpl(
			NiNode*                          a_root,
			NiNode*                          a_npcroot,
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateImpl(
			NiNode*                          a_root,
			NiNode*                          a_npcroot,
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		void UpdateBipedSlotCache(
			processParams_t&   a_params,
			ActorObjectHolder& a_objects);

		void DoObjectEvaluation(
			NiNode*                          a_root,
			NiNode*                          a_npcroot,
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateImpl(
			ActorObjectHolder&               a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		void EvaluateTransformsImpl(
			Game::FormID a_actor);

		void EvaluateTransformsImpl(
			ActorObjectHolder& a_objects);

		bool ProcessTransformsImpl(
			NiNode*                                      a_npcRoot,
			Actor*                                       a_actor,
			TESNPC*                                      a_npc,
			TESRace*                                     a_race,
			Data::ConfigSex                              a_sex,
			ActorObjectHolder&                           a_objects,
			const Data::collectorData_t::container_type* a_equippedForms);

		void ActorResetImpl(
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags);

		void ActorResetImpl(
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags,
			Data::ObjectSlot                 a_slot);

		void ResetCustomImpl(
			Actor*                   a_actor,
			Game::ObjectRefHandle    a_handle,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey);

		void ResetCustomImpl(
			Actor*                   a_actor,
			Game::ObjectRefHandle    a_handle,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_vkey);

		void ResetCustomImpl(
			Actor*                a_actor,
			Game::ObjectRefHandle a_handle,
			Data::ConfigClass     a_class);

		void ResetGearImpl(
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_objects,
			stl::flag<ControllerUpdateFlags> a_flags);

		void UpdateTransformSlotImpl(
			Game::FormID     a_actor,
			Data::ObjectSlot a_slot);

		void UpdateTransformSlotImpl(
			ActorObjectHolder& a_record,
			Data::ObjectSlot   a_slot);

		static void UpdateIfPaused(NiNode* a_root);

		updateActionFunc_t MakeTransformUpdateFunc();

		const Data::configBaseValues_t& GetConfigForActor(
			const actorInfo_t&                            a_info,
			const Data::configCustom_t&                   a_config,
			const ActorObjectHolder::slot_container_type& a_slots,
			objectEntryCustom_t&                          a_entry);

		const Data::configBaseValues_t& GetConfigForActor(
			const actorInfo_t&        a_info,
			const Data::configSlot_t& a_config,
			const objectEntrySlot_t&  a_entry);

		void UpdateCustomImpl(
			Game::FormID             a_actor,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey,
			updateActionFunc_t       a_func);

		void UpdateCustomNPCImpl(
			Game::FormID             a_npc,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey,
			updateActionFunc_t       a_func);

		void UpdateCustomRaceImpl(
			Game::FormID             a_race,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			const stl::fixed_string& a_vkey,
			updateActionFunc_t       a_func);

		void UpdateCustomImpl(
			Game::FormID             a_actor,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			updateActionFunc_t       a_func);

		void UpdateCustomNPCImpl(
			Game::FormID             a_npc,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			updateActionFunc_t       a_func);

		void UpdateCustomRaceImpl(
			Game::FormID             a_race,
			Data::ConfigClass        a_class,
			const stl::fixed_string& a_pkey,
			updateActionFunc_t       a_func);

		void UpdateCustomImpl(
			Game::FormID       a_actor,
			Data::ConfigClass  a_class,
			updateActionFunc_t a_func);

		void UpdateCustomNPCImpl(
			Game::FormID       a_npc,
			Data::ConfigClass  a_class,
			updateActionFunc_t a_func);

		void UpdateCustomRaceImpl(
			Game::FormID       a_race,
			Data::ConfigClass  a_class,
			updateActionFunc_t a_func);

		void UpdateCustomImpl(
			ActorObjectHolder&        a_record,
			Data::ConfigClass         a_class,
			const stl::fixed_string&  a_pkey,
			const stl::fixed_string&  a_vkey,
			const updateActionFunc_t& a_func);

		void UpdateCustomImpl(
			ActorObjectHolder&        a_record,
			Data::ConfigClass         a_class,
			const stl::fixed_string&  a_pkey,
			const updateActionFunc_t& a_func);

		void UpdateCustomImpl(
			ActorObjectHolder&        a_record,
			Data::ConfigClass         a_class,
			const updateActionFunc_t& a_func);

		void UpdateCustomImpl(
			actorInfo_t&                          a_info,
			const Data::configCustomPluginMap_t&  a_confPluginMap,
			ActorObjectHolder::customPluginMap_t& a_pluginMap,
			const stl::fixed_string&              a_pkey,
			const stl::fixed_string&              a_vkey,
			const updateActionFunc_t&             a_func);

		void UpdateCustomAllImpl(
			actorInfo_t&                          a_info,
			const Data::configCustomPluginMap_t&  a_confPluginMap,
			ActorObjectHolder::customPluginMap_t& a_pluginMap,
			const stl::fixed_string&              a_pkey,
			const updateActionFunc_t&             a_func);

		void UpdateCustomAllImpl(
			actorInfo_t&                          a_info,
			const Data::configCustomPluginMap_t&  a_confPluginMap,
			ActorObjectHolder::customPluginMap_t& a_pluginMap,
			const updateActionFunc_t&             a_func);

		void UpdateCustomImpl(
			actorInfo_t&                         a_info,
			const Data::configCustomEntryMap_t&  a_confEntryMap,
			ActorObjectHolder::customEntryMap_t& a_entryMap,
			const stl::fixed_string&             a_vkey,
			const updateActionFunc_t&            a_func);

		void UpdateTransformCustomImpl(
			actorInfo_t&                   a_info,
			const Data::configCustom_t&    a_configEntry,
			const Data::configTransform_t& a_xfrmConfigEntry,
			objectEntryCustom_t&           a_entry);

		bool AttachNodeImpl(
			NiNode*                     a_root,
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference,
			objectEntryBase_t&          a_cacheEntry);

		bool ProcessItemUpdate(
			processParams_t&                 a_params,
			const Data::configBaseValues_t&  a_config,
			const Data::equipmentOverride_t* a_override,
			objectEntryBase_t&               a_entry,
			bool                             a_visible);

		template <class Ta, class Tb>
		constexpr void UpdateObjectEffectShaders(
			processParams_t& a_params,
			const Ta&        a_config,
			Tb&              a_objectEntry,
			bool             a_updateValues = false) requires(  //
			(std::is_same_v<Ta, Data::configCustom_t>&&
		         std::is_same_v<Tb, objectEntryCustom_t>) ||
			(std::is_same_v<Ta, Data::configSlot_t> &&
		     std::is_same_v<Tb, objectEntrySlot_t>));

		void ProcessSlots(processParams_t& a_params);

		bool CustomEntryValidateInventoryForm(
			processParams_t&                         a_params,
			const Data::collectorData_t::itemData_t& a_itemData,
			const Data::configCustom_t&              a_config,
			bool&                                    a_hasMinCount);

		Data::collectorData_t::container_type::iterator CustomEntrySelectInventoryForm(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			objectEntryCustom_t&        a_objectEntry,
			bool&                       a_hasMinCount);

		bool IsBlockedByChance(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			objectEntryCustom_t&        a_objectEntry);

		bool ProcessCustomEntry(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			objectEntryCustom_t&        a_cacheEntry);

		void ProcessCustomEntryMap(
			processParams_t&                     a_params,
			const Data::configCustomHolder_t&    a_confData,
			ActorObjectHolder::customEntryMap_t& a_entryMap);

		void ProcessCustomMap(
			processParams_t&                     a_params,
			const Data::configCustomPluginMap_t& a_confPluginMap,
			Data::ConfigClass                    a_class);

		void ProcessCustom(processParams_t& a_params);

		void SaveLastEquippedItems(
			processParams_t&          a_params,
			const equippedItemInfo_t& a_info,
			ActorObjectHolder&        a_objectHolder);

		bool GetVisibilitySwitch(
			Actor*                     a_actor,
			stl::flag<Data::BaseFlags> a_flags,
			processParams_t&           a_params);

		bool LookupTrackedActor(
			Game::FormID         a_actor,
			actorLookupResult_t& a_out);

		bool LookupTrackedActor(
			const ActorObjectHolder& a_record,
			actorLookupResult_t&     a_out);

		std::optional<actorInfo_t> LookupCachedActorInfo(
			ActorObjectHolder& a_record);

		void CollectKnownActors(
			actorLookupResultMap_t& a_out);

		bool SetLanguageImpl(const stl::fixed_string& a_lang);

		void GenerateRandomPlacementEntries(const ActorObjectHolder& a_holder);

		virtual void OnActorAcquire(ActorObjectHolder& a_holder) override;

		// internal events

		virtual void Receive(const SKSESerializationEvent& a_evn) override;
		virtual void Receive(const SKSESerializationLoadEvent& a_evn) override;
		virtual void Receive(const SKSEMessagingEvent& a_evn) override;
		virtual void Receive(const SDSPlayerShieldOnBackSwitchEvent& a_evn) override;

		// bs events
		virtual EventResult ReceiveEvent(
			const TESObjectLoadedEvent*           a_evn,
			BSTEventSource<TESObjectLoadedEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESInitScriptEvent*           a_evn,
			BSTEventSource<TESInitScriptEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESEquipEvent*           evn,
			BSTEventSource<TESEquipEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESContainerChangedEvent*           a_evn,
			BSTEventSource<TESContainerChangedEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESFurnitureEvent*           a_evn,
			BSTEventSource<TESFurnitureEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const TESDeathEvent*           a_evn,
			BSTEventSource<TESDeathEvent>* a_dispatcher) override;

		/*virtual EventResult ReceiveEvent(
			const TESCombatEvent*           a_evn,
			BSTEventSource<TESCombatEvent>* a_dispatcher) override;*/

		virtual EventResult ReceiveEvent(
			const TESSwitchRaceCompleteEvent*           a_evn,
			BSTEventSource<TESSwitchRaceCompleteEvent>* a_dispatcher) override;

		virtual EventResult ReceiveEvent(
			const MenuOpenCloseEvent*           evn,
			BSTEventSource<MenuOpenCloseEvent>* a_dispatcher) override;

		/*virtual EventResult ReceiveEvent(
			const TESSceneEvent*           evn,
			BSTEventSource<TESSceneEvent>* a_dispatcher) override;*/

		/*virtual EventResult ReceiveEvent(
			const TESQuestStartStopEvent* a_evn,
			BSTEventSource<TESQuestStartStopEvent>* a_dispatcher) override;*/

		/*virtual EventResult ReceiveEvent(
			const TESPackageEvent*           a_evn,
			BSTEventSource<TESPackageEvent>* a_dispatcher) override;*/

		virtual EventResult ReceiveEvent(
			const TESActorLocationChangeEvent*           a_evn,
			BSTEventSource<TESActorLocationChangeEvent>* a_dispatcher) override;

		void FillGlobalSlotConfig(Data::configStoreSlot_t& a_data) const;
		//void FillInitialConfig(Data::configStore_t& a_data) const;

		static std::optional<npcRacePair_t> GetNPCRacePair(Actor* a_actor) noexcept;

		// serialization

		virtual std::size_t Store(boost::archive::binary_oarchive& a_out) override;

		virtual std::size_t Load(
			SKSESerializationInterface*      a_intfc,
			std::uint32_t                    a_version,
			boost::archive::binary_iarchive& a_in) override;

		// ui overrides

		virtual constexpr stl::critical_section& UIGetLock() noexcept override
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

		virtual constexpr stl::critical_section& JSGetLock() noexcept override
		{
			return m_lock;
		}

		virtual constexpr Data::configStore_t& JSGetConfigStore() noexcept override
		{
			return m_config.active;
		}

		virtual void JSOnDataImport() override;

		// members

		std::shared_ptr<const ConfigINI> m_iniconf;
		Data::actorBlockList_t           m_actorBlockList;

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

		RandomNumberGeneratorBase             m_rngBase;
		RandomNumberGenerator3<float>         m_rng1;
		stl::vector<Game::ObjectRefHandle>    m_activeHandles;
		stl::flag<EventSinkInstallationFlags> m_esif{ EventSinkInstallationFlags::kNone };
		except::descriptor                    m_lastException;

		BipedDataCache m_bipedCache;

		struct
		{
			stl::vector<const BipedSlotEntry*> le;
			Data::configFormList_t             fl;
			SlotResults                        sr;
		} m_temp;

		bool m_nodeOverrideEnabled{ false };
		bool m_nodeOverridePlayerEnabled{ false };
		bool m_forceDefaultConfig{ false };
		bool m_npcProcessingDisabled{ false };
		bool m_iniKeysForced{ false };
		bool m_applyTransformOverrides{ false };
		bool m_enableCorpseScatter{ false };
		bool m_forceOrigWeapXFRM{ false };

		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& static_cast<IPersistentCounter&>(*this);
			a_ar& m_bipedCache;
		}
	};

	DEFINE_ENUM_CLASS_BITWISE(Controller::EventSinkInstallationFlags);

}

BOOST_CLASS_VERSION(
	::IED::Controller,
	::IED::Controller::DataVersion1);
