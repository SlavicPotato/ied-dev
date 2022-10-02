#pragma once

#include "ActiveActorAnimation.h"
#include "ActorAnimationState.h"
#include "BipedSlotData.h"
#include "CachedActorData.h"
#include "EffectShaderData.h"
#include "INode.h"
#include "NodeMonitorEntry.h"
#include "ObjectDatabase.h"
#include "ObjectManagerCommon.h"

#include "CMENodeEntry.h"
#include "MOVNodeEntry.h"
#include "WeaponNodeEntry.h"

#include "ObjectEntryCustom.h"
#include "ObjectEntrySlot.h"

#include "IED/ActorState.h"
#include "IED/ConfigModelGroup.h"
#include "IED/ConfigStore.h"
#include "IED/NodeOverrideData.h"
#include "IED/SkeletonCache.h"
#include "IED/SkeletonID.h"

#include <ext/WeaponAnimationGraphManagerHolder.h>

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

		kRequestTransformUpdateDefer     = kWantTransformUpdate | kSkipNextTransformUpdate,
		kRequestTransformUpdateImmediate = kWantTransformUpdate | kImmediateTransformUpdate,
		kRequestTransformUpdateMask      = kWantTransformUpdate | kImmediateTransformUpdate | kSkipNextTransformUpdate,

		kWantEval       = 1u << 3,
		kImmediateEval  = 1u << 4,
		kEvalCountdown1 = 1u << 5,
		kEvalCountdown2 = 1u << 6,

		kRequestEval          = kWantEval,
		kRequestEvalImmediate = kWantEval | kImmediateEval,
		kRequestEvalMask      = kWantEval | kImmediateEval | kEvalCountdown1 | kEvalCountdown2
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
		std::uint32_t unused                  : 19;
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

		inline static constexpr std::size_t MAX_RPC_SIZE = 1024 * 1024;

		struct monitorNodeEntry_t
		{
			NiPointer<NiNode> node;
			NiPointer<NiNode> parent;
			std::uint16_t     size;
			bool              visible;
		};

	public:
		using customEntryMap_t  = stl::unordered_map<stl::fixed_string, ObjectEntryCustom>;
		using customPluginMap_t = stl::unordered_map<stl::fixed_string, customEntryMap_t>;

		ActorObjectHolder() = delete;
		ActorObjectHolder(
			Actor*                a_actor,
			NiNode*               a_root,
			NiNode*               a_npcroot,
			IObjectManager&       a_owner,
			Game::ObjectRefHandle a_handle,
			bool                  a_nodeOverrideEnabled,
			bool                  a_nodeOverrideEnabledPlayer,
			//bool                    a_animEventForwarding,
			const BipedSlotDataPtr& a_lastEquipped);

		~ActorObjectHolder();

		ActorObjectHolder(const ActorObjectHolder&) = delete;
		ActorObjectHolder(ActorObjectHolder&&)      = delete;
		ActorObjectHolder& operator=(const ActorObjectHolder&) = delete;
		ActorObjectHolder& operator=(ActorObjectHolder&&) = delete;

		[[nodiscard]] inline constexpr auto& GetSlot(
			Data::ObjectSlot a_slot) noexcept
		{
			assert(a_slot < Data::ObjectSlot::kMax);
			return m_entriesSlot[stl::underlying(a_slot)];
		}

		[[nodiscard]] inline constexpr const auto& GetSlot(
			Data::ObjectSlot a_slot) const noexcept
		{
			assert(a_slot < Data::ObjectSlot::kMax);
			return m_entriesSlot[stl::underlying(a_slot)];
		}

		[[nodiscard]] inline constexpr auto& GetSlots() const noexcept
		{
			return m_entriesSlot;
		}

		[[nodiscard]] inline constexpr auto& GetActor() const noexcept
		{
			return m_actor;
		}

		[[nodiscard]] inline constexpr auto& GetNPCRoot() const noexcept
		{
			return m_npcroot;
		}

		[[nodiscard]] inline constexpr auto& GetRoot() const noexcept
		{
			return m_root;
		}

		[[nodiscard]] bool        AnySlotOccupied() const noexcept;
		[[nodiscard]] std::size_t GetNumOccupiedSlots() const noexcept;
		[[nodiscard]] std::size_t GetNumOccupiedCustom() const noexcept;

		[[nodiscard]] inline auto GetAge() const noexcept
		{
			return IPerfCounter::delta_us(m_created, IPerfCounter::Query());
		}

		[[nodiscard]] inline constexpr auto& GetHandle() const noexcept
		{
			return m_handle;
		}

		[[nodiscard]] inline auto& GetCustom(Data::ConfigClass a_class) noexcept
		{
			return m_entriesCustom[stl::underlying(a_class)];
		}

		[[nodiscard]] inline auto& GetCustom(Data::ConfigClass a_class) const noexcept
		{
			return m_entriesCustom[stl::underlying(a_class)];
		}

		[[nodiscard]] inline constexpr auto& GetCustom() const noexcept
		{
			return m_entriesCustom;
		}

		[[nodiscard]] inline constexpr auto& GetCMENodes() const noexcept
		{
			return m_cmeNodes;
		}

		[[nodiscard]] inline constexpr auto& GetMOVNodes() const noexcept
		{
			return m_movNodes;
		}

		[[nodiscard]] inline constexpr auto& GetWeapNodes() const noexcept
		{
			return m_weapNodes;
		}

		[[nodiscard]] inline constexpr bool IsCellAttached() const noexcept
		{
			return m_state.cellAttached;
		}

		/*[[nodiscard]] inline constexpr bool GetEnemiesNearby() const noexcept
		{
			return m_enemiesNearby;
		}*/

		inline constexpr void UpdateCellAttached() noexcept
		{
			m_state.cellAttached = m_actor->IsParentCellAttached();
		}

		inline void RequestTransformUpdateDefer() const noexcept
		{
			if (!m_cmeNodes.empty() ||
			    !m_movNodes.empty())
			{
				m_flags.set(ActorObjectHolderFlags::kRequestTransformUpdateDefer);
			}
		}

		inline void RequestTransformUpdateDeferNoSkip() const noexcept
		{
			if (!m_cmeNodes.empty() ||
			    !m_movNodes.empty())
			{
				m_flags.set(ActorObjectHolderFlags::kWantTransformUpdate);
			}
		}

		inline void RequestTransformUpdate() const noexcept
		{
			if (!m_cmeNodes.empty() ||
			    !m_movNodes.empty())
			{
				m_flags.set(ActorObjectHolderFlags::kRequestTransformUpdateImmediate);
			}
		}

		inline constexpr void RequestEvalDefer(std::uint32_t a_delay = 2) const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kRequestEval);
			if (m_flagsbf.evalCountdown == 0)
			{
				m_flagsbf.evalCountdown = a_delay;
			}
		}

		inline constexpr void RequestEval() const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kRequestEvalImmediate);
		}

		void ApplyActorState(const Data::actorStateEntry_t& a_data);

		[[nodiscard]] bool IsActorNPCOrTemplate(Game::FormID a_npc) const;
		[[nodiscard]] bool IsActorRace(Game::FormID a_race) const;

		template <class Tv>
		inline constexpr void state_var_update(
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
		inline constexpr void state_var_update_defer(
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
		inline constexpr void state_var_update_defer(
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
		inline constexpr bool state_var_update_b(
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
		inline constexpr bool state_var_update_b(
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
		constexpr void visit(Tf a_func)
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
		constexpr void visit(Tf a_func) const
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

		[[nodiscard]] inline constexpr auto& GetActorFormID() const noexcept
		{
			return m_formid;
		}

		[[nodiscard]] inline constexpr auto& GetSkeletonCache() const noexcept
		{
			return m_skeletonCache;
		}

		[[nodiscard]] inline constexpr auto& GetAnimState() const noexcept
		{
			return m_animState;
		}

		[[nodiscard]] inline constexpr bool IsFemale() const noexcept
		{
			return m_female;
		}

		/*[[nodiscard]] inline constexpr bool IsAnimEventForwardingEnabled() const noexcept
		{
			return m_enableAnimEventForwarding;
		}*/

		[[nodiscard]] NiTransform GetCachedOrZeroTransform(
			const stl::fixed_string& a_name) const;

		[[nodiscard]] std::optional<NiTransform> GetCachedTransform(
			const stl::fixed_string& a_name) const;

		//void ReSinkAnimationGraphs();

		/*void RegisterWeaponAnimationGraphManagerHolder(
			RE::WeaponAnimationGraphManagerHolderPtr& a_ptr,
			bool                                      a_forward);

		void UnregisterWeaponAnimationGraphManagerHolder(
			RE::WeaponAnimationGraphManagerHolderPtr& a_ptr);*/

		/*[[nodiscard]] inline constexpr auto& GetAnimationUpdateList() noexcept
		{
			return m_animationUpdateList;
		}

		[[nodiscard]] inline constexpr auto& GetAnimationUpdateList() const noexcept
		{
			return m_animationUpdateList;
		}*/

		[[nodiscard]] inline constexpr bool HasHumanoidSkeleton() const noexcept
		{
			return m_humanoidSkeleton;
		}

		[[nodiscard]] inline constexpr bool IsXP32Skeleton() const noexcept
		{
			return m_skeletonID.get_version().has_value();
		}

		[[nodiscard]] inline constexpr bool GetNodeConditionForced() const noexcept
		{
			return m_forceNodeCondTrue;
		}

		[[nodiscard]] inline constexpr void SetNodeConditionForced(bool a_switch) noexcept
		{
			m_forceNodeCondTrue = a_switch;
		}

		[[nodiscard]] inline auto& GetCachedData() const noexcept
		{
			return m_state;
		}

		void UpdateAllAnimationGraphs(const BSAnimationUpdateData& a_data) const;

		float GetRandomPercent(const luid_tag& a_luid);

		bool UpdateNodeMonitorEntries();
		bool GetNodeMonitorResult(std::uint32_t a_uid);

	private:
		void CreateExtraMovNodes(
			NiNode* a_npcroot);

		void CreateExtraCopyNode(
			Actor*                                        a_actor,
			NiNode*                                       a_npcroot,
			const NodeOverrideData::extraNodeCopyEntry_t& a_entry) const;

		void ApplyXP32NodeTransformOverrides(
			NiNode* a_root) const;

		/*EventResult ReceiveEvent(
			const BSAnimationGraphEvent*           a_event,
			BSTEventSource<BSAnimationGraphEvent>* a_eventSource) override;*/

		//std::optional<ActiveActorAnimation> GetNewActiveAnimation(const BSAnimationGraphEvent* a_event) const;

		Game::ObjectRefHandle m_handle;
		long long             m_created{ 0 };

		union
		{
			mutable stl::flag<ActorObjectHolderFlags> m_flags{ ActorObjectHolderFlags::kNone };
			mutable ActorObjectHolderFlagsBitfield    m_flagsbf;
		};

		ObjectSlotArray   m_entriesSlot{};
		customPluginMap_t m_entriesCustom[Data::CONFIG_CLASS_MAX]{};

		stl::vector<monitorNodeEntry_t> m_monitorNodes;
		stl::vector<WeaponNodeEntry>    m_weapNodes;

		stl::unordered_map<stl::fixed_string, CMENodeEntry> m_cmeNodes;
		stl::unordered_map<stl::fixed_string, MOVNodeEntry> m_movNodes;

		stl::unordered_map<luid_tag, float> m_rpc;

		NiPointer<Actor>  m_actor;
		NiPointer<NiNode> m_root;
		NiPointer<NiNode> m_npcroot;

		Game::FormID m_formid;

		bool m_female{ false };
		bool m_humanoidSkeleton{ false };

		bool m_forceNodeCondTrue{ false };

		bool      m_wantLFUpdate{ false };
		long long m_lastLFStateCheck;
		long long m_lastHFStateCheck;

		SkeletonCache::const_actor_entry_type m_skeletonCache;
		SkeletonID                            m_skeletonID;

		CachedActorData             m_state;
		mutable ActorAnimationState m_animState;

		//AnimationGraphManagerHolderList m_animationUpdateList;
		//AnimationGraphManagerHolderList m_animEventForwardRegistrations;
		//const bool                      m_enableAnimEventForwarding{ false };

		BipedSlotDataPtr m_lastEquipped;

		stl::unordered_map<std::uint32_t, NodeMonitorEntry> m_nodeMonitorEntries;

		// parent, it's never destroyed
		IObjectManager& m_owner;

		static std::atomic_ullong m_lfsc_delta_lf;
		static std::atomic_ullong m_lfsc_delta_hf;
	};

}