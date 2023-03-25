#pragma once

#include "ActiveActorAnimation.h"
#include "ActorAnimationState.h"
#include "BipedSlotData.h"
#include "CachedActorData.h"
#include "NodeMonitorEntry.h"

#include "CMENodeEntry.h"
#include "MOVNodeEntry.h"
#include "WeaponNodeEntry.h"

#include "ObjectEntryCustom.h"
#include "ObjectEntrySlot.h"

#include "IED/ActorState.h"
#include "IED/CollectorData.h"
#include "IED/ConditionalVariableStorage.h"
#include "IED/ConfigData.h"
#include "IED/GearNodeID.h"
#include "IED/Inventory.h"
#include "IED/NodeOverrideData.h"
#include "IED/NodeOverrideParams.h"
#include "IED/ProcessParams.h"
#include "IED/SkeletonCache.h"
#include "IED/SkeletonID.h"

#include "ActorTempData.h"

//#include <ext/WeaponAnimationGraphManagerHolder.h>

struct BSAnimationUpdateData;

namespace IED
{
	class IObjectManager;
	class IEquipment;
	class Controller;
	class ActorProcessorTask;
	class INodeOverride;

	enum class ActorObjectHolderFlags : std::uint32_t
	{
		kNone = 0,

		kWantTransformUpdate      = 1u << 0,
		kImmediateTransformUpdate = 1u << 1,
		kSkipNextTransformUpdate  = 1u << 2,

		kWantEval      = 1u << 3,
		kImmediateEval = 1u << 4,

		kEvalCountdownMask =
			1u << 5 |
			1u << 6 |
			1u << 7 |
			1u << 8 |
			1u << 9 |
			1u << 10 |
			1u << 11 |
			1u << 12,

		kEvalThisFrame = 1u << 13,

		kWantVarUpdate = 1u << 14,

		kRequestTransformUpdateDefer     = kWantTransformUpdate | kSkipNextTransformUpdate,
		kRequestTransformUpdateImmediate = kWantTransformUpdate | kImmediateTransformUpdate,
		kRequestTransformUpdateMask      = kWantTransformUpdate | kImmediateTransformUpdate | kSkipNextTransformUpdate,

		kRequestEval          = kWantEval,
		kRequestEvalImmediate = kWantEval | kImmediateEval,
		kRequestEvalMask      = kWantEval | kImmediateEval | kEvalCountdownMask,

		kDestroyed         = 1u << 15,
		kIsPlayer          = 1u << 16,
		kIsFemale          = 1u << 17,
		kHumanoidSkeleton  = 1u << 18,
		kForceNodeCondTrue = 1u << 19,
	};

	DEFINE_ENUM_CLASS_BITWISE(ActorObjectHolderFlags);

	struct ActorObjectHolderFlagsBitfield
	{
		std::uint32_t wantTransformUpdate     : 1;
		std::uint32_t immediateTransformUpdate: 1;
		std::uint32_t skipNextTransformUpdate : 1;
		std::uint32_t wantEval                : 1;
		std::uint32_t immediateEval           : 1;
		std::uint32_t evalCountdown           : 8;
		std::uint32_t wantVarUpdate           : 1;
		std::uint32_t unused                  : 18;
	};

	static_assert(sizeof(ActorObjectHolderFlagsBitfield) == sizeof(ActorObjectHolderFlags));

	class ActorObjectHolder  // :
							 //public BSTEventSink<BSAnimationGraphEvent>
	{
		friend class IObjectManager;
		friend class IEquipment;
		friend class Controller;
		friend class ActorProcessorTask;
		friend class ObjectManagerData;

		static constexpr std::size_t MAX_RPC_SIZE = 1024 * 1024;

		struct MonitorGearNodeEntry
		{
			const NiPointer<NiNode> node;
			NiNode*                 parent;
			std::uint16_t           size;
			bool                    visible;
		};

	public:
		static constexpr long long STATE_CHECK_INTERVAL_LOW  = 1000000;
		static constexpr long long STATE_CHECK_INTERVAL_MH   = 66666;
		static constexpr long long STATE_CHECK_INTERVAL_HIGH = 33333;

		using customEntryMap_t  = stl::unordered_map<stl::fixed_string, ObjectEntryCustom>;
		using customPluginMap_t = stl::unordered_map<stl::fixed_string, customEntryMap_t>;

		ActorObjectHolder() = delete;
		ActorObjectHolder(
			Actor*                  a_actor,
			TESNPC*                 a_npc,
			TESRace*                a_race,
			NiNode*                 a_root,
			NiNode*                 a_npcroot,
			IObjectManager&         a_owner,
			Game::ObjectRefHandle   a_handle,
			bool                    a_nodeOverrideEnabled,
			bool                    a_nodeOverrideEnabledPlayer,
			bool                    a_syncToFirstPersonSkeleton,
			const BipedSlotDataPtr& a_slotCache) noexcept;

		~ActorObjectHolder() noexcept;

		ActorObjectHolder(const ActorObjectHolder&)            = delete;
		ActorObjectHolder& operator=(const ActorObjectHolder&) = delete;

		[[nodiscard]] constexpr auto& GetSlot(
			Data::ObjectSlot a_slot) noexcept
		{
			assert(a_slot < Data::ObjectSlot::kMax);
			return m_entriesSlot[stl::underlying(a_slot)];
		}

		[[nodiscard]] constexpr const auto& GetSlot(
			Data::ObjectSlot a_slot) const noexcept
		{
			assert(a_slot < Data::ObjectSlot::kMax);
			return m_entriesSlot[stl::underlying(a_slot)];
		}

		[[nodiscard]] constexpr auto& GetSlots() const noexcept
		{
			return m_entriesSlot;
		}

		[[nodiscard]] constexpr auto& GetActor() const noexcept
		{
			return m_actor;
		}

		[[nodiscard]] constexpr auto& Get3D() const noexcept
		{
			return m_root;
		}

		[[nodiscard]] constexpr auto& Get3D1p() const noexcept
		{
			return m_root1p;
		}

		[[nodiscard]] constexpr auto& GetNPCRoot() const noexcept
		{
			return m_npcroot;
		}

		[[nodiscard]] constexpr auto& GetRoot() const noexcept
		{
			return m_root;
		}

		[[nodiscard]] bool        IsAnySlotOccupied() const noexcept;
		[[nodiscard]] std::size_t GetNumOccupiedSlots() const noexcept;
		[[nodiscard]] std::size_t GetNumOccupiedCustom() const noexcept;

		[[nodiscard]] inline auto GetAge() const noexcept
		{
			return IPerfCounter::delta_us(m_created, IPerfCounter::Query());
		}

		[[nodiscard]] constexpr auto& GetHandle() const noexcept
		{
			return m_handle;
		}

		constexpr void SetHandle(Game::ObjectRefHandle a_handle) noexcept
		{
			m_handle = a_handle;
		}

		[[nodiscard]] inline auto& GetCustom(Data::ConfigClass a_class) noexcept
		{
			return m_entriesCustom[stl::underlying(a_class)];
		}

		[[nodiscard]] inline auto& GetCustom(Data::ConfigClass a_class) const noexcept
		{
			return m_entriesCustom[stl::underlying(a_class)];
		}

		[[nodiscard]] constexpr auto& GetCustom() const noexcept
		{
			return m_entriesCustom;
		}

		[[nodiscard]] constexpr auto& GetCMENodes() const noexcept
		{
			return m_cmeNodes;
		}

		[[nodiscard]] constexpr auto& GetMOVNodes() const noexcept
		{
			return m_movNodes;
		}

		[[nodiscard]] constexpr auto& GetWeapNodes() const noexcept
		{
			return m_weapNodes;
		}

		[[nodiscard]] constexpr bool IsActive() const noexcept
		{
			return m_state.active;
		}

		/*[[nodiscard]] constexpr bool GetEnemiesNearby() const noexcept
		{
			return m_enemiesNearby;
		}*/

		inline void RequestTransformUpdateDefer() const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kRequestTransformUpdateDefer);
		}

		inline void RequestTransformUpdateDeferNoSkip() const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kWantTransformUpdate);
		}

		inline void RequestTransformUpdate() const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kRequestTransformUpdateImmediate);
		}

		constexpr void RequestEvalDefer(std::uint8_t a_delay = 2) const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kRequestEval);
			if (m_flagsbf.evalCountdown == 0)
			{
				m_flagsbf.evalCountdown = a_delay;
			}
		}

		constexpr void RequestEval() const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kRequestEvalImmediate);
		}

		[[nodiscard]] bool IsActorNPCOrTemplate(Game::FormID a_npc) const;
		[[nodiscard]] bool IsActorRace(Game::FormID a_race) const;

		template <class Tv>
		constexpr void state_var_update(
			Tv&       a_var,
			const Tv& a_current) noexcept
		{
			if (a_var != a_current)
			{
				a_var = a_current;
				RequestEval();
			}
		}

		template <class Tv>
		constexpr void state_var_update_defer(
			Tv&           a_var,
			const Tv&     a_current,
			std::uint32_t a_delay = 2) noexcept
		{
			if (a_var != a_current)
			{
				a_var = a_current;
				RequestEvalDefer(a_delay);
			}
		}

		template <class Tv>
		constexpr void state_var_update_defer(
			stl::flag<Tv>& a_var,
			const Tv&      a_current,
			std::uint32_t  a_delay = 2) noexcept
		{
			if (a_var != a_current)
			{
				a_var = a_current;
				RequestEvalDefer(a_delay);
			}
		}

		template <class Tv>
		constexpr bool state_var_update_b(
			Tv&       a_var,
			const Tv& a_current) noexcept
		{
			if (a_var != a_current)
			{
				a_var = a_current;
				return true;
			}
			else
			{
				return false;
			}
		}

		template <class Tv>
		constexpr bool state_var_update_b(
			stl::flag<Tv>& a_var,
			const Tv&      a_current) noexcept
		{
			if (a_var != a_current)
			{
				a_var = a_current;
				return true;
			}
			else
			{
				return false;
			}
		}

		template <class Tf>
		constexpr void visit(Tf a_func)  //
										 /*noexcept(
				std::is_nothrow_invocable_v<Tf, ObjectEntrySlot&>&&
					std::is_nothrow_invocable_v<Tf, ObjectEntryCustom&>)*/
		{
			for (auto& e : m_entriesSlot)
			{
				a_func(e);
			}

			for (auto& e : m_entriesCustom)
			{
				for (auto& f : e)
				{
					for (auto& g : f.second)
					{
						a_func(g.second);
					}
				}
			}
		}

		template <class Tf>
		constexpr void visit(Tf a_func) const  //
											   /* noexcept(
				std::is_nothrow_invocable_v<Tf, const ObjectEntrySlot&>&&
					std::is_nothrow_invocable_v<Tf, const ObjectEntryCustom&>)*/
		{
			for (auto& e : m_entriesSlot)
			{
				a_func(e);
			}

			for (auto& e : m_entriesCustom)
			{
				for (auto& f : e)
				{
					for (auto& g : f.second)
					{
						a_func(g.second);
					}
				}
			}
		}

		template <class Tf>
		constexpr void visit_custom(Tf a_func) const                             //
			noexcept(std::is_nothrow_invocable_v<Tf, const ObjectEntryCustom&>)  //
			requires(std::invocable<Tf, const ObjectEntryCustom&>)
		{
			for (auto& e : m_entriesCustom)
			{
				for (auto& f : e)
				{
					for (auto& g : f.second)
					{
						a_func(g.second);
					}
				}
			}
		}

		template <class Tf>
		constexpr void visit_custom(Tf a_func)                             //
			noexcept(std::is_nothrow_invocable_v<Tf, ObjectEntryCustom&>)  //
			requires(std::invocable<Tf, ObjectEntryCustom&>)
		{
			for (auto& e : m_entriesCustom)
			{
				for (auto& f : e)
				{
					for (auto& g : f.second)
					{
						a_func(g.second);
					}
				}
			}
		}

		template <class Tf>
		constexpr void visit_slot(Tf a_func) const                             //
			noexcept(std::is_nothrow_invocable_v<Tf, const ObjectEntrySlot&>)  //
			requires(std::invocable<Tf, const ObjectEntrySlot&>)
		{
			for (auto& e : m_entriesSlot)
			{
				a_func(e);
			}
		}

		template <class Tf>
		constexpr void visit_slot(Tf a_func)                             //
			noexcept(std::is_nothrow_invocable_v<Tf, ObjectEntrySlot&>)  //
			requires(std::invocable<Tf, ObjectEntrySlot&>)
		{
			for (auto& e : m_entriesSlot)
			{
				a_func(e);
			}
		}

		[[nodiscard]] constexpr auto& GetActorFormID() const noexcept
		{
			return m_actorid;
		}

		[[nodiscard]] constexpr auto& GetNPCFormID() const noexcept
		{
			return m_npcid;
		}

		[[nodiscard]] constexpr auto& GetNPCTemplateFormID() const noexcept
		{
			return m_npcTemplateId;
		}

		[[nodiscard]] constexpr auto& GetRaceFormID() const noexcept
		{
			return m_raceid;
		}

		[[nodiscard]] constexpr auto& GetSkeletonID() const noexcept
		{
			return m_skeletonID;
		}

		[[nodiscard]] constexpr auto& GetAnimState() const noexcept
		{
			return m_animState;
		}

		[[nodiscard]] constexpr bool IsPlayer() const noexcept
		{
			return m_flags.test(ActorObjectHolderFlags::kIsPlayer);
		}

		[[nodiscard]] constexpr bool IsFemale() const noexcept
		{
			return m_flags.test(ActorObjectHolderFlags::kIsFemale);
		}

		[[nodiscard]] constexpr auto GetSex() const noexcept
		{
			return static_cast<Data::ConfigSex>(IsFemale());
		}

		/*[[nodiscard]] constexpr bool IsAnimEventForwardingEnabled() const noexcept
		{
			return m_enableAnimEventForwarding;
		}*/

		//void ReSinkAnimationGraphs();

		/*void RegisterWeaponAnimationGraphManagerHolder(
			RE::WeaponAnimationGraphManagerHolderPtr& a_ptr,
			bool                                      a_forward);

		void UnregisterWeaponAnimationGraphManagerHolder(
			RE::WeaponAnimationGraphManagerHolderPtr& a_ptr);*/

		/*[[nodiscard]] constexpr auto& GetAnimationUpdateList() noexcept
		{
			return m_animationUpdateList;
		}

		[[nodiscard]] constexpr auto& GetAnimationUpdateList() const noexcept
		{
			return m_animationUpdateList;
		}*/

		[[nodiscard]] constexpr bool HasHumanoidSkeleton() const noexcept
		{
			return m_flags.test(ActorObjectHolderFlags::kHumanoidSkeleton);
		}

		[[nodiscard]] constexpr bool HasXP32Skeleton() const noexcept
		{
			return m_skeletonID.xp_version().has_value();
		}

		[[nodiscard]] constexpr bool GetNodeConditionForced() const noexcept
		{
			return m_flags.test(ActorObjectHolderFlags::kForceNodeCondTrue);
		}

		[[nodiscard]] constexpr void SetNodeConditionForced(bool a_switch) noexcept
		{
			if (m_flags.test(ActorObjectHolderFlags::kForceNodeCondTrue) != a_switch)
			{
				m_flags.set(ActorObjectHolderFlags::kForceNodeCondTrue, a_switch);
				RequestTransformUpdate();
			}
		}

		[[nodiscard]] inline auto& GetCachedData() const noexcept
		{
			return m_state;
		}

		constexpr void ClearCurrentProcessParams() noexcept
		{
			return m_currentParams.reset();
		}

		template <class... Args>
		[[nodiscard]] constexpr auto& GetOrCreateProcessParams(Args&&... a_args) noexcept
		{
			if (!m_currentParams)
			{
				m_currentParams.emplace(std::forward<Args>(a_args)...);
			}

			return *m_currentParams;
		}

		template <class... Args>
		constexpr auto& CreateProcessParams(Args&&... a_args) noexcept
		{
			m_currentParams.emplace(std::forward<Args>(a_args)...);
			return *m_currentParams;
		}

		[[nodiscard]] constexpr auto& GetCurrentProcessParams() noexcept
		{
			return m_currentParams;
		}

		/*[[nodiscard]] std::optional<conditionalVariableStorage_t> GetVariable(
			const stl::fixed_string& a_name) const
		{
			auto it = m_variables.find(a_name);
			if (it != m_variables.end())
			{
				return it->second;
			}
			else
			{
				return {};
			}
		}*/

		[[nodiscard]] constexpr auto& GetVariables() const noexcept
		{
			return m_variables;
		}

		[[nodiscard]] constexpr auto& GetVariables() noexcept
		{
			return m_variables;
		}

		[[nodiscard]] constexpr auto& GetBipedSlotData() const noexcept
		{
			return m_slotCache;
		}

		inline void ClearVariables(bool a_requestEval) noexcept
		{
			m_variables.clear();

			if (a_requestEval)
			{
				m_flags.set(
					ActorObjectHolderFlags::kWantVarUpdate |
					ActorObjectHolderFlags::kRequestEval);
			}
		}

		constexpr void RequestVariableUpdate() const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kWantVarUpdate);
		}

		constexpr void MarkDestroyed() noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kDestroyed);
		}

		float GetRandomPercent(const luid_tag& a_luid) noexcept;
		void  ClearRPC() noexcept;

		bool UpdateNodeMonitorEntries() noexcept;
		bool GetNodeMonitorResult(std::uint32_t a_uid) const noexcept;

		bool GetSheathNodes(Data::ObjectSlot a_slot, std::pair<NiNode*, NiNode*>& a_out) const noexcept;

		void QueueDisposeMOVSimComponents() noexcept;
		bool QueueDisposeAllObjectEntries(Game::ObjectRefHandle a_handle) noexcept;

		void SimReadTransforms(float a_step) const noexcept;
		void SimWriteTransforms() const noexcept;
		void SimUpdate(float a_step) const noexcept;

		std::size_t GetSimComponentListSize() const noexcept;
		std::size_t GetNumAnimObjects() const noexcept;

		template <class... Args>
		[[nodiscard]] auto& CreateAndAddSimComponent(Args&&... a_args) noexcept
		{
			return m_simNodeList.emplace_back(
				new PHYSimComponent(
					std::forward<Args>(a_args)...));
		}

		void RemoveSimComponent(const stl::smart_ptr<PHYSimComponent>& a_sc) noexcept;
		void RemoveAndDestroySimComponent(stl::smart_ptr<PHYSimComponent>& a_sc) noexcept;

		[[nodiscard]] constexpr auto& GetSimComponentList() const noexcept
		{
			return m_simNodeList;
		}

		[[nodiscard]] SKMP_143_CONSTEXPR auto& GetTempData() noexcept
		{
			return *m_temp;
		}

		[[nodiscard]] constexpr auto& GetMonitorNodes() noexcept
		{
			return m_monitorNodes;
		}

		[[nodiscard]] constexpr auto& GetLastQueuedOutfitEquipFrame() const noexcept
		{
			return m_lastQueuedOutfitEquipFrame;
		}

		[[nodiscard]] constexpr void SetLastQueuedOutfitEquipFrame(std::uint32_t a_value) noexcept
		{
			m_lastQueuedOutfitEquipFrame = a_value;
		}

		constexpr void AddQueuedModel(ObjectDatabaseEntry&& a_entry) noexcept
		{
			m_queuedModels.emplace(std::move(a_entry));
		}

		bool ProcessQueuedModels() noexcept;

		[[nodiscard]] constexpr auto GetNumQueuedModels() const noexcept
		{
			return m_queuedModels.size();
		}

	private:
		void CreateExtraCopyNode(
			const SkeletonCache::ActorEntry&              a_sc,
			NiNode*                                       a_npcroot,
			const NodeOverrideData::extraNodeCopyEntry_t& a_entry) const noexcept;

		void ApplyXP32NodeTransformOverrides() const noexcept;

		/*EventResult ReceiveEvent(
			const BSAnimationGraphEvent*           a_event,
			BSTEventSource<BSAnimationGraphEvent>* a_eventSource) override;*/

		//std::optional<ActiveActorAnimation> GetNewActiveAnimation(const BSAnimationGraphEvent* a_event) const;

		CachedActorData m_state;

		mutable bool m_wantLFUpdate{ false };
		mutable bool m_wantHFUpdate{ false };
		//bool m_wantLFVarUpdate{ false };

		Game::ObjectRefHandle m_handle;
		long long             m_created{ 0 };

		union
		{
			mutable stl::flag<ActorObjectHolderFlags> m_flags{ ActorObjectHolderFlags::kWantVarUpdate };
			mutable ActorObjectHolderFlagsBitfield    m_flagsbf;
		};

		ObjectSlotArray   m_entriesSlot;
		customPluginMap_t m_entriesCustom[Data::CONFIG_CLASS_MAX];

		stl::cache_aligned::vector<MonitorGearNodeEntry>            m_monitorNodes;
		stl::cache_aligned::vector<WeaponNodeEntry>                 m_weapNodes;
		stl::cache_aligned::vector<stl::smart_ptr<PHYSimComponent>> m_simNodeList;

		stl::unordered_map<stl::fixed_string, CMENodeEntry> m_cmeNodes;
		stl::unordered_map<stl::fixed_string, MOVNodeEntry> m_movNodes;

		stl::cache_aligned::vectormap<std::uint32_t, NodeMonitorEntry> m_nodeMonitorEntries;
		conditionalVariableMap_t                                       m_variables;

		std::optional<processParams_t> m_currentParams;

		NiPointer<Actor>  m_actor;
		NiPointer<NiNode> m_root;
		NiPointer<NiNode> m_root1p;
		NiPointer<NiNode> m_npcroot;

		Game::FormID m_actorid;
		Game::FormID m_npcid;
		Game::FormID m_npcTemplateId;
		Game::FormID m_raceid;

		long long m_nextLFStateCheck;
		long long m_nextMFStateCheck;
		long long m_nextHFStateCheck;

		SkeletonID m_skeletonID;

		mutable ActorAnimationState m_animState;

		BipedSlotDataPtr m_slotCache;

		std::unique_ptr<ActorTempData> m_temp;

		stl::unordered_map<luid_tag, float> m_rpc;

		std::uint32_t m_lastQueuedOutfitEquipFrame{ 0 };

		stl::flat_set<ObjectDatabaseEntry> m_queuedModels;

		// parent, it's never destroyed
		IObjectManager& m_owner;

		static std::atomic_ullong m_lfsc_delta_lf;
		static std::atomic_ullong m_lfsc_delta_mf;
		static std::atomic_ullong m_lfsc_delta_hf;
	};

}