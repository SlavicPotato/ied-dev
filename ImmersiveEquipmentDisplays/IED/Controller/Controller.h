#pragma once

#include "IED/ActorState.h"
#include "IED/ConfigBlockList.h"
#include "IED/ConfigCommon.h"
#include "IED/ConfigINI.h"
#include "IED/ConfigSerializationFlags.h"
#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/Inventory.h"
#include "IED/KeyBindDataHolder.h"
#include "IED/KeyBindEventHandler.h"
#include "IED/SettingHolder.h"
#include "IED/StringHolder.h"

#include "ActorProcessorTask.h"
#include "BipedDataCache.h"
#include "ControllerCommon.h"
#include "IAnimationManager.h"
#include "IEquipment.h"
#include "IForm.h"
#include "IJSONSerialization.h"
#include "IObjectManager.h"
#include "IUINotification.h"
//#include "KeyToggleStates.h"

#include "Drivers/Input/Handlers.h"

#include <ext/SDSPlayerShieldOnBackSwitchEvent.h>

#include "IED/OM/OutfitController.h"

namespace IED
{
	class Controller :
		public IObjectManager,
		public IEquipment,
		public IAnimationManager,
		public ActorProcessorTask,
		public ISerializationBase,
		public IJSONSerialization,
		public IUINotification,
		public OM::OutfitController,
		public BSTEventSink<TESObjectLoadedEvent>,
		public BSTEventSink<TESInitScriptEvent>,
		public BSTEventSink<TESEquipEvent>,
		public BSTEventSink<TESContainerChangedEvent>,
		public BSTEventSink<TESFurnitureEvent>,
		public BSTEventSink<TESDeathEvent>,
		public BSTEventSink<TESSwitchRaceCompleteEvent>,
		public BSTEventSink<MenuOpenCloseEvent>,
		public BSTEventSink<TESActorLocationChangeEvent>,
		public ::Events::EventSink<SKSESerializationEvent>,
		public ::Events::EventSink<SKSESerializationLoadEvent>,
		public ::Events::EventSink<SKSESerializationFormDeleteEvent>,
		public ::Events::EventSink<SKSEMessagingEvent>,
		public ::Events::EventSink<SDSPlayerShieldOnBackSwitchEvent>,
		public KB::KeyBindEventHandler,
		virtual private ILog
	{
		friend class boost::serialization::access;

		enum class SerializationVersion : std::uint32_t
		{
			kDataVersion1  = 1,
			kDataVersion2  = 2,
			kDataVersion3  = 3,
			kDataVersion4  = 4,
			kDataVersion5  = 5,
			kDataVersion6  = 6,
			kDataVersion7  = 7,
			kDataVersion8  = 8,
			kDataVersion9  = 9,
			kDataVersion10 = 10,
			kDataVersion11 = 11,
			kDataVersion12 = 12,
			kDataVersion13 = 13,

			kCurrentVersion = kDataVersion13
		};

		static constexpr std::uint32_t SKSE_SERIALIZATION_TYPE_ID = 'DDEI';

		struct ActorLookupResult
		{
			NiPointer<Actor>      actor;
			Game::ObjectRefHandle handle;
		};

		struct NRP
		{
			TESNPC*  npc;
			TESRace* race;
		};

		struct ConfigData
		{
			std::unique_ptr<Data::configStore_t> active;
			std::unique_ptr<Data::configStore_t> stash;
			std::unique_ptr<Data::configStore_t> initial;
			std::unique_ptr<Data::SettingHolder> settings;
		};

		struct InputHandlers
		{
			Handlers::ComboKeyPressHandler playerBlock;
			Handlers::ComboKeyPressHandler uiOpen;
		};

		using actorLookupResultMap_t =
			stl::unordered_map<
				Game::ObjectRefHandle,
				NiPointer<Actor>>;

		friend class ActorProcessorTask;

	public:
		enum : unsigned int
		{
			DataVersion1 = 1,
			DataVersion2 = 2,
			DataVersion3 = 3,
		};

		enum class EventSinkInstallationFlags : std::uint8_t
		{
			kNone = 0,

			kT0 = 1ui8 << 0,
			kT1 = 1ui8 << 1,
			kT2 = 1ui8 << 2
		};

		enum class InventoryChangeConsumerFlags : std::uint8_t
		{
			kNone = 0,

			kInventoryInfo = 1u << 0,

			kAll = static_cast<std::underlying_type_t<InventoryChangeConsumerFlags>>(-1)
		};

		struct cachedActorInfo_t
		{
			NiPointer<Actor>      actor;
			Game::ObjectRefHandle handle;
			TESNPC*               npc;
			TESNPC*               npcOrTemplate;
			TESRace*              race;
			BSFadeNode*           root;
			NiNode*               npcRoot;
			Data::ConfigSex       sex;
			ActorObjectHolder&    objects;
		};

		struct cachedActorInfo2_t
		{
			TESNPC*     npc;
			TESRace*    race;
			BSFadeNode* root;
			NiNode*     npcRoot;
		};

		Controller(const stl::smart_ptr<const ConfigINI>& a_config);

		Controller(const Controller&)            = delete;
		Controller& operator=(const Controller&) = delete;

		void InitializeInputHandlers();
		void SinkEventsT0();
		bool DispatchIntroBanner();

	private:
		void InitializeSound();
		void UpdateSoundForms();
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
			auto result = stl::make_smart<IUITimedRenderTask>(
				*this,
				a_lifetime);

			result->InitializeContext<Tc>(
				*this,
				std::forward<Args>(a_args)...);

			return result;
		}

		bool RemoveActor(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		/*bool RemoveActor(
			TESObjectREFR*                   a_actor,
			stl::flag<ControllerUpdateFlags> a_flags);*/

		bool RemoveActor(
			Game::FormID                     a_actor,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

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

		void QueueEvaluateAll(stl::flag<ControllerUpdateFlags> a_flags);

		// use when acquiring global lock may be detrimental to performance
		void QueueRequestEvaluateTransformsActor(Game::FormID a_actor, bool a_noDefer) const noexcept;

		void RequestEvaluateTransformsActor(Game::FormID a_actor, bool a_noDefer) const noexcept;
		void RequestEvaluateTransformsNPC(Game::FormID a_npc, bool a_noDefer) const;
		void RequestEvaluateTransformsRace(Game::FormID a_race, bool a_noDefer) const;
		void RequestEvaluateTransformsAll(bool a_noDefer) const;

		void QueueActorRemove(TESObjectREFR* a_actor, stl::flag<ControllerUpdateFlags> a_flags) noexcept;
		void QueueReset(TESObjectREFR* a_actor, stl::flag<ControllerUpdateFlags> a_flags) noexcept;
		void QueueReset(Game::FormID a_actor, stl::flag<ControllerUpdateFlags> a_flags) noexcept;
		void QueueResetNPC(Game::FormID a_npc, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueResetRace(Game::FormID a_race, stl::flag<ControllerUpdateFlags> a_flags);
		void QueueReset(Game::FormID a_actor, stl::flag<ControllerUpdateFlags> a_flags, Data::ObjectSlot a_slot);
		void QueueResetNPC(Game::FormID a_npc, stl::flag<ControllerUpdateFlags> a_flags, Data::ObjectSlot a_slot);
		void QueueResetRace(Game::FormID a_race, stl::flag<ControllerUpdateFlags> a_flags, Data::ObjectSlot a_slot);
		void QueueResetAll(stl::flag<ControllerUpdateFlags> a_flags);
		void QueueResetAll(stl::flag<ControllerUpdateFlags> a_flags, Data::ObjectSlot a_slot);
		void QueueResetGear(Game::FormID a_actor, stl::flag<ControllerUpdateFlags> a_flags) noexcept;
		void QueueResetGearAll(stl::flag<ControllerUpdateFlags> a_flags);
		void QueueClearObjects();

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

		void QueueLookupFormInfo(Game::FormID a_formId, IForm::form_lookup_result_func_t a_func);
		void QueueLookupFormInfoCrosshairRef(IForm::form_lookup_result_func_t a_func);

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

		[[nodiscard]] SKMP_143_CONSTEXPR auto& GetActiveConfig() noexcept
		{
			assert(m_configData.active);
			return *m_configData.active;
		}

		[[nodiscard]] SKMP_143_CONSTEXPR auto& GetActiveConfig() const noexcept
		{
			assert(m_configData.active);
			return *m_configData.active;
		}

		[[nodiscard]] constexpr auto& GetConfigStore() noexcept
		{
			return m_configData;
		}

		[[nodiscard]] constexpr auto& GetConfigStore() const noexcept
		{
			return m_configData;
		}

		[[nodiscard]] SKMP_143_CONSTEXPR auto& GetSettings() noexcept
		{
			return *m_configData.settings;
		}

		[[nodiscard]] SKMP_143_CONSTEXPR auto& GetSettings() const noexcept
		{
			return *m_configData.settings;
		}

		[[nodiscard]] constexpr auto& GetInputHandlers() noexcept
		{
			return m_inputHandlers;
		}

		bool SaveCurrentConfigAsDefault(
			stl::flag<ExportFlags>                         a_exportFlags,
			stl::flag<Data::ConfigStoreSerializationFlags> a_flags);

		void SaveSettings(bool a_defer, bool a_dirtyOnly, const bool a_debug = false);

		void QueueUpdateActorInfo(Game::FormID a_actor);
		void QueueUpdateActorInfo(Game::FormID a_actor, std::function<void(bool)> a_callback);
		void QueueUpdateNPCInfo(Game::FormID a_npc);
		void QueueUpdateNPCInfo(Game::FormID a_npc, std::function<void(bool)> a_callback);

		[[nodiscard]] constexpr auto GetNodeOverrideEnabled() const noexcept
		{
			return m_nodeOverrideEnabled;
		}

		[[nodiscard]] constexpr auto GetNodeOverridePlayerEnabled() const noexcept
		{
			return m_nodeOverridePlayerEnabled;
		}

		void QueueObjectDatabaseClear();
		void SetObjectDatabaseLevel(ObjectDatabaseLevel a_level);

		[[nodiscard]] SKMP_143_CONSTEXPR auto IsDefaultConfigForced() const noexcept
		{
			return m_forceDefaultConfig;
		}

		[[nodiscard]] constexpr auto UIGetIniKeysForced() const noexcept
		{
			return m_iniKeysForced;
		}

		[[nodiscard]] inline auto GetEquippedItemCacheSize() const noexcept
		{
			return m_bipedCache.size();
		}

		[[nodiscard]] constexpr auto GetEquippedItemCacheSizeMax() const noexcept
		{
			return m_bipedCache.max_size();
		}

		[[nodiscard]] constexpr auto& GetBipedDataCache() const noexcept
		{
			return m_bipedCache;
		}

		void QueueSetLanguage(const stl::fixed_string& a_lang);

		//void QueueClearVariableStorage(bool a_requestEval);

		void RunUpdateBipedSlotCache(processParams_t& a_params) noexcept;

		[[nodiscard]] constexpr auto GetEvalCounter() const noexcept
		{
			return m_evalCounter;
		}

		[[nodiscard]] constexpr auto CPUHasSSE41() const noexcept
		{
			return m_cpuHasSSE41;
		}

		/*[[nodiscard]] constexpr bool ConsumeInventoryChangeFlags(
			InventoryChangeConsumerFlags a_mask)
		{
			return m_invChangeConsumerFlags.consume(a_mask);
		}*/

		std::size_t GetNumSimComponents() const noexcept;
		std::size_t GetNumAnimObjects() const noexcept;

		void QueueSetEffectShaders(Actor* a_actor) noexcept;

		void        ClearBipedCache();
		std::size_t RemoveBipedCacheEntries(std::function<bool(Game::FormID)> a_filter);

		void ClearPlayerRPC() noexcept;

	private:
		FN_NAMEPROC("Controller");

		struct updateActionFunc_t
		{
			using func_t = std::function<bool(
				cachedActorInfo_t&,
				const Data::configCustomEntry_t&,
				ObjectEntryCustom&)>;

			template <class... Args>
			constexpr bool operator()(Args&&... a_args) const
			{
				return func(std::forward<Args>(a_args)...);
			}

			func_t       func;
			bool         evalDirty{ false };
			mutable bool clean{ false };
		};

		void EvaluateImpl(
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		void EvaluateImpl(
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_holder,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		void EvaluateImpl(
			BSFadeNode*                      a_root,
			NiNode*                          a_npcroot,
			Actor*                           a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		void EvaluateImpl(
			const CommonParams&              a_params,
			ActorObjectHolder&               a_holder,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		void EvaluateImpl(
			BSFadeNode*                      a_root,
			NiNode*                          a_npcroot,
			Actor*                           a_actor,
			TESNPC*                          a_npc,
			TESRace*                         a_race,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_holder,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		void UpdateBipedSlotCache(
			processParams_t&   a_params,
			ActorObjectHolder& a_holder) noexcept;

		void RunVariableMapUpdate(
			processParams_t& a_params,
			bool             a_markAllForEval = false) noexcept;

		void DoObjectEvaluation(
			processParams_t& a_params) noexcept;

		void EvaluateImpl(
			ActorObjectHolder&               a_holder,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		/*void EvaluateTransformsImpl(
			Game::FormID a_actor);*/

		void EvaluateTransformsImpl(
			ActorObjectHolder&               a_holder,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		bool ProcessTransformsImpl(
			BSFadeNode*                      a_root,
			NiNode*                          a_npcRoot,
			Actor*                           a_actor,
			TESNPC*                          a_npc,
			TESRace*                         a_race,
			ActorObjectHolder&               a_holder,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		static void ProcessTransformsImplPhys(
			nodeOverrideParams_t&                         a_params,
			const Data::configNodeOverrideEntryPhysics_t* a_config,
			const MOVNodeEntry&                           a_entry) noexcept;

		static void ProcessTransformsImplPhysNode(
			nodeOverrideParams_t&                  a_params,
			const Data::configNodePhysicsValues_t& a_conf,
			const MOVNodeEntry::Node&              a_node) noexcept;

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
			ActorObjectHolder&               a_holder,
			stl::flag<ControllerUpdateFlags> a_flags);

		void UpdateTransformSlotImpl(
			Game::FormID     a_actor,
			Data::ObjectSlot a_slot);

		void UpdateTransformSlotImpl(
			ActorObjectHolder& a_record,
			Data::ObjectSlot   a_slot);

		const Data::configBaseValues_t& GetConfigForActor(
			const cachedActorInfo_t&    a_info,
			const Data::configCustom_t& a_config,
			const ObjectEntryCustom&    a_entry);

		const Data::configBaseValues_t& GetConfigForActor(
			const cachedActorInfo_t&  a_info,
			const Data::configSlot_t& a_config,
			const ObjectEntrySlot&    a_entry);

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
			cachedActorInfo_t&                    a_info,
			const Data::configCustomPluginMap_t&  a_confPluginMap,
			ActorObjectHolder::customPluginMap_t& a_pluginMap,
			const stl::fixed_string&              a_pkey,
			const stl::fixed_string&              a_vkey,
			const updateActionFunc_t&             a_func);

		void UpdateCustomAllImpl(
			cachedActorInfo_t&                    a_info,
			const Data::configCustomPluginMap_t&  a_confPluginMap,
			ActorObjectHolder::customPluginMap_t& a_pluginMap,
			const stl::fixed_string&              a_pkey,
			const updateActionFunc_t&             a_func);

		void UpdateCustomAllImpl(
			cachedActorInfo_t&                    a_info,
			const Data::configCustomPluginMap_t&  a_confPluginMap,
			ActorObjectHolder::customPluginMap_t& a_pluginMap,
			const updateActionFunc_t&             a_func);

		void UpdateCustomImpl(
			cachedActorInfo_t&                   a_info,
			const Data::configCustomEntryMap_t&  a_confEntryMap,
			ActorObjectHolder::customEntryMap_t& a_entryMap,
			const stl::fixed_string&             a_vkey,
			const updateActionFunc_t&            a_func);

		bool DoItemUpdate(
			processParams_t&                a_params,
			const Data::configBaseValues_t& a_usedConfig,
			ObjectEntryBase&                a_entry,
			bool                            a_visible,
			TESForm*                        a_currentModelForm) noexcept;

		void ResetEffectShaderData(
			processParams_t& a_params,
			ObjectEntryBase& a_entry) noexcept;

		void ResetEffectShaderData(
			processParams_t& a_params,
			ObjectEntryBase& a_entry,
			NiAVObject*      a_object) noexcept;

		void UpdateObjectEffectShaders(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry) noexcept;

		void UpdateCustomGroup(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry) noexcept;

		void RemoveSlotObjectEntry(
			processParams_t& a_params,
			ObjectEntrySlot& a_entry) noexcept;

		void ProcessSlots(processParams_t& a_params) noexcept;

		bool IsBlockedByChance(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry) noexcept;

		ActorObjectHolder* SelectCustomFormVariableSourceHolder(
			Game::FormID     a_id,
			processParams_t& a_params) noexcept;

		ActorObjectHolder* SelectCustomFormVariableSource(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config) noexcept;

		const Data::configCachedForm_t* SelectCustomForm(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config) noexcept;

		bool ProcessCustomEntry(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_cacheEntry) noexcept;

		void ProcessCustomEntryMap(
			processParams_t&                     a_params,
			const Data::configCustomHolder_t&    a_confData,
			ActorObjectHolder::customEntryMap_t& a_entryMap) noexcept;

		void ProcessCustomMap(
			processParams_t&                     a_params,
			const Data::configCustomPluginMap_t& a_confPluginMap,
			Data::ConfigClass                    a_class) noexcept;

		void ProcessCustom(processParams_t& a_params) noexcept;

		void SaveLastEquippedItems(
			processParams_t&          a_params,
			const equippedItemInfo_t& a_info,
			ActorObjectHolder&        a_objectHolder) noexcept;

		bool GetVisibilitySwitch(
			Actor*                     a_actor,
			stl::flag<Data::BaseFlags> a_flags,
			processParams_t&           a_params) noexcept;

		bool LookupTrackedActor(
			Game::FormID       a_actor,
			ActorLookupResult& a_out) noexcept;

		bool LookupTrackedActor(
			const ActorObjectHolder& a_record,
			ActorLookupResult&       a_out) noexcept;

		std::optional<cachedActorInfo_t> LookupCachedActorInfo(
			Actor*             a_actor,
			ActorObjectHolder& a_holder) noexcept;

		std::optional<cachedActorInfo2_t> LookupCachedActorInfo2(
			Actor*             a_actor,
			ActorObjectHolder& a_holder) noexcept;

		std::optional<cachedActorInfo_t> LookupCachedActorInfo(
			ActorObjectHolder& a_holder) noexcept;

		void CollectKnownActors(
			actorLookupResultMap_t& a_out);

		bool SetLanguageImpl(const stl::fixed_string& a_lang);

		void GenerateRandomPlacementEntries(const ActorObjectHolder& a_holder);

		static void FillGlobalSlotConfig(Data::configStoreSlot_t& a_data);
		//void FillInitialConfig(Data::configStore_t& a_data) const;

		static std::optional<NRP> GetNPCRacePair(Actor* a_actor) noexcept;

		virtual void OnActorAcquire(ActorObjectHolder& a_holder) noexcept override;
		virtual bool WantGlobalVariableUpdateOnAddRemove() const noexcept override;

		// internal events

		virtual void Receive(const SKSESerializationEvent& a_evn) override;
		virtual void Receive(const SKSESerializationLoadEvent& a_evn) override;
		virtual void Receive(const SKSESerializationFormDeleteEvent& a_evn) override;
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

		// serialization

		virtual std::size_t Store(boost::archive::binary_oarchive& a_out) override;

		virtual std::size_t Load(
			SKSESerializationInterface*      a_intfc,
			std::uint32_t                    a_version,
			boost::archive::binary_iarchive& a_in) override;

		// ui overrides

		virtual constexpr stl::recursive_mutex& UIGetLock() noexcept override
		{
			return m_lock;
		}

		virtual void OnUIOpen() override;
		virtual void OnUIClose() override;

		// actor info overrides

		virtual SKMP_143_CONSTEXPR const Data::configStore_t& AIGetConfigStore() noexcept override
		{
			return *m_configData.active;
		}

		// json serialization

		virtual constexpr stl::recursive_mutex& JSGetLock() noexcept override
		{
			return m_lock;
		}

		virtual SKMP_143_CONSTEXPR Data::configStore_t& JSGetConfigStore() noexcept override
		{
			return *m_configData.active;
		}

		virtual void JSOnDataImport() override;

		virtual void OnKBStateChanged() override;

		// members

		stl::smart_ptr<const ConfigINI> m_iniconf;
		Data::actorBlockList_t          m_actorBlockList;
		InputHandlers                   m_inputHandlers;
		ConfigData                      m_configData;

		stl::vector<Game::ObjectRefHandle> m_activeHandles;

		BipedDataCache m_bipedCache;

		const bool m_nodeOverrideEnabled{ false };
		const bool m_nodeOverridePlayerEnabled{ false };
		const bool m_npcProcessingDisabled{ false };
		bool       m_forceDefaultConfig{ false };
		const bool m_forceFlushSaveData{ false };
		bool       m_iniKeysForced{ false };
		bool       m_cpuHasSSE41{ false };

		std::uint64_t m_evalCounter{ 0 };

		template <class Archive>
		void save(Archive& a_ar, const unsigned int a_version) const
		{
			a_ar& static_cast<const IPersistentCounter&>(*this);
			a_ar& m_bipedCache;
			a_ar& GetKeyBindDataHolder()->GetKeyToggleStates();
		}

		template <class Archive>
		void load(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& static_cast<IPersistentCounter&>(*this);
			a_ar& m_bipedCache;

			if (a_version >= DataVersion2)
			{
				ser_load_kts(a_ar, a_version);
			}
		}

		template <class Archive>
		void ser_load_kts(Archive& a_ar, const unsigned int a_version)
		{
			if (a_version >= DataVersion3)
			{
				KB::KeyToggleStateEntryHolder::state_data2 tmp;
				a_ar&                                      tmp;

				GetKeyBindDataHolder()->InitializeKeyToggleStates(tmp);
			}
			else
			{
				KB::KeyToggleStateEntryHolder::state_data tmp;
				a_ar&                                     tmp;

				GetKeyBindDataHolder()->InitializeKeyToggleStates(tmp);
			}
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER();
	};

	DEFINE_ENUM_CLASS_BITWISE(Controller::EventSinkInstallationFlags);

}

BOOST_CLASS_VERSION(
	::IED::Controller,
	::IED::Controller::DataVersion3);
