#pragma once

#include "ActorAnimationState.h"
#include "BipedSlotData.h"
#include "EffectShaderData.h"
#include "INode.h"
#include "NodeOverrideData.h"
#include "ObjectDatabase.h"
#include "ObjectManagerCommon.h"
#include "SkeletonID.h"

#include "IED/ActorState.h"
#include "IED/ConfigModelGroup.h"
#include "IED/ConfigStore.h"
#include "IED/SkeletonCache.h"

#include <ext/WeaponAnimationGraphManagerHolder.h>

namespace IED
{

	enum class ObjectEntryFlags : std::uint32_t
	{
		kNone = 0,

		kRefSyncDisableFailedOrphan = 1u << 1,
		kScbLeft                    = 1u << 2,
		kSyncReferenceTransform     = 1u << 6,
		kPlaySound                  = 1u << 8,
		kIsGroup                    = 1u << 9,
	};

	DEFINE_ENUM_CLASS_BITWISE(ObjectEntryFlags);

	/*class AnimationEventFilter
	{
	public:
		SKMP_FORCEINLINE bool test(const BSFixedString& a_event)
		{
			if (m_allow.contains(a_event))
			{
				return true;
			}

			return !m_denyAll &&
			       !m_deny.contains(a_event);
		}

		bool                              m_denyAll{ false };
		stl::unordered_set<BSFixedString> m_allow;
		stl::unordered_set<BSFixedString> m_deny;
	};*/

	class AnimationGraphManagerHolderList
	{
	public:
		void Add(RE::WeaponAnimationGraphManagerHolderPtr& a_ptr)
		{
			stl::scoped_lock lock(m_lock);

			m_data.emplace_back(a_ptr);
		}

		void Remove(RE::WeaponAnimationGraphManagerHolderPtr& a_ptr)
		{
			stl::scoped_lock lock(m_lock);

			auto it = std::find(m_data.begin(), m_data.end(), a_ptr);
			if (it != m_data.end())
			{
				m_data.erase(it);
			}
		}

		void Notify(const BSFixedString& a_event) const
		{
			stl::scoped_lock lock(m_lock);

			for (auto& e : m_data)
			{
				e->NotifyAnimationGraph(a_event);
			}
		}

		void Update(const BSAnimationUpdateData& a_data) const;

		void UpdateNoLock(const BSAnimationUpdateData& a_data) const;

		void Clear() noexcept
		{
			stl::scoped_lock lock(m_lock);

			m_data.clear();
		}

		inline constexpr auto& GetList() const noexcept
		{
			return m_data;
		}

		inline bool Empty() const noexcept
		{
			return m_data.empty();
		}

	private:
		mutable stl::critical_section                       m_lock;
		stl::list<RE::WeaponAnimationGraphManagerHolderPtr> m_data;
	};

	struct objectEntryBase_t
	{
		objectEntryBase_t()  = default;
		~objectEntryBase_t() = default;

		objectEntryBase_t(const objectEntryBase_t&) = delete;
		objectEntryBase_t(objectEntryBase_t&&)      = delete;
		objectEntryBase_t& operator=(const objectEntryBase_t&) = delete;
		objectEntryBase_t& operator=(objectEntryBase_t&&) = delete;

		void reset(Game::ObjectRefHandle a_handle, NiPointer<NiNode>& a_root);

		inline void SetNodeVisible(bool a_switch)
		{
			if (state)
			{
				state->nodes.rootNode->SetVisible(a_switch);
			}
		}

		SKMP_FORCEINLINE auto IsActive() const
		{
			return state && state->nodes.rootNode->IsVisible();
		}

		SKMP_FORCEINLINE auto GetFormIfActive() const
		{
			return IsActive() ? state->form : nullptr;
		}

		SKMP_FORCEINLINE auto IsNodeVisible() const
		{
			return state && state->nodes.rootNode->IsVisible();
		}

		struct AnimationState
		{
			RE::WeaponAnimationGraphManagerHolderPtr weapAnimGraphManagerHolder;
			stl::fixed_string                        currentAnimationEvent;

			void UpdateAndSendAnimationEvent(const stl::fixed_string& a_event);
		};

		struct State :
			AnimationState
		{
			State()  = default;
			~State() = default;

			State(const State&) = delete;
			State(State&&)      = delete;
			State& operator=(const State&) = delete;
			State& operator=(State&&) = delete;

			struct GroupObject :
				AnimationState
			{
				GroupObject(
					NiNode*            a_rootNode,
					NiPointer<NiNode>& a_object) :
					rootNode(a_rootNode),
					object(a_object)
				{
				}

				NiPointer<NiNode>      rootNode;
				NiPointer<NiNode>      object;
				Data::cacheTransform_t transform;

				void PlayAnimation(Actor* a_actor, const stl::fixed_string& a_sequence);
			};

			void UpdateData(
				const Data::configBaseValues_t& a_in)
			{
				UpdateFlags(a_in);
				transform.Update(a_in);

				resetTriggerFlags = a_in.flags & Data::BaseFlags::kResetTriggerFlags;
			}

			void UpdateFlags(
				const Data::configBaseValues_t& a_in) noexcept
			{
				// gross but efficient

				static_assert(
					std::is_same_v<std::underlying_type_t<ObjectEntryFlags>, std::underlying_type_t<Data::BaseFlags>> &&
					stl::underlying(ObjectEntryFlags::kPlaySound) == stl::underlying(Data::BaseFlags::kPlaySound) &&
					stl::underlying(ObjectEntryFlags::kSyncReferenceTransform) == stl::underlying(Data::BaseFlags::kSyncReferenceTransform));

				flags =
					(flags & ~(ObjectEntryFlags::kPlaySound | ObjectEntryFlags::kSyncReferenceTransform | ObjectEntryFlags::kRefSyncDisableFailedOrphan)) |
					static_cast<ObjectEntryFlags>((a_in.flags & (Data::BaseFlags::kPlaySound | Data::BaseFlags::kSyncReferenceTransform)));
			}

			/*void UpdateGroupTransforms(const Data::configModelGroup_t& a_group)
			{
				for (auto& e : a_group.entries)
				{
					if (auto it = groupObjects.find(e.first);
					    it != groupObjects.end())
					{
						it->second.transform.Update(e.second.transform);
					}
				}
			}*/

			void Cleanup(Game::ObjectRefHandle a_handle);

			void UpdateAndPlayAnimation(
				Actor*                   a_actor,
				const stl::fixed_string& a_sequence);

			TESForm*                                           form{ nullptr };
			Game::FormID                                       formid;
			stl::flag<ObjectEntryFlags>                        flags{ ObjectEntryFlags::kNone };
			Data::NodeDescriptor                               nodeDesc;
			nodesRef_t                                         nodes;
			Data::cacheTransform_t                             transform;
			stl::list<ObjectDatabase::ObjectDatabaseEntry>     dbEntries;
			stl::unordered_map<stl::fixed_string, GroupObject> groupObjects;
			EffectShaderData                                   effectShaders;
			stl::fixed_string                                  currentSequence;
			long long                                          created{ 0 };
			stl::flag<Data::BaseFlags>                         resetTriggerFlags{ Data::BaseFlags::kNone };
			bool                                               atmReference{ true };
		};

		std::unique_ptr<State> state;
	};

	struct objectEntrySlot_t :
		objectEntryBase_t
	{
		Data::actorStateSlotEntry_t slotState;
		Data::ObjectSlot            slotid{ Data::ObjectSlot::kMax };
		Data::ObjectSlotExtra       slotidex{ Data::ObjectSlotExtra::kNone };
		std::uint8_t                hideCountdown{ 0 };

		inline constexpr void ResetDeferredHide() noexcept
		{
			hideCountdown = 0;
		}
	};

	enum class CustomObjectEntryFlags : std::uint32_t
	{
		kNone = 0,

		kProcessedChance = 1u << 0,
		kBlockedByChance = 1u << 1,
		kUseGroup        = 1u << 2,

		kChanceMask = kProcessedChance | kBlockedByChance
	};

	DEFINE_ENUM_CLASS_BITWISE(CustomObjectEntryFlags);

	struct objectEntryCustom_t :
		objectEntryBase_t
	{
		inline constexpr void clear_chance_flags() noexcept
		{
			cflags.clear(CustomObjectEntryFlags::kChanceMask);
		}

		Game::FormID                      modelForm;
		stl::flag<CustomObjectEntryFlags> cflags{ CustomObjectEntryFlags::kNone };
	};

	class IObjectManager;
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

	struct weapNodeEntry_t
	{
		friend class INodeOverride;

	public:
		weapNodeEntry_t(
			const stl::fixed_string&          a_nodeName,
			NiNode*                           a_node,
			NiNode*                           a_defaultNode,
			AnimationWeaponSlot               a_animID,
			const std::optional<NiTransform>& a_xfrm) :
			nodeName(a_nodeName),
			node(a_node),
			defaultNode(a_defaultNode),
			animSlot(a_animID),
			originalTransform(a_xfrm)
		{
		}

		const stl::fixed_string    nodeName;
		NiPointer<NiNode>          node;
		NiPointer<NiNode>          defaultNode;
		AnimationWeaponSlot        animSlot;
		std::optional<NiTransform> originalTransform;

	private:
		mutable NiPointer<NiNode> target;
	};

	class BSStringHolder;

	struct cmeNodeEntry_t
	{
		cmeNodeEntry_t(
			NiNode*            a_node,
			const NiTransform& a_originalTransform) :
			node(a_node),
			orig(a_originalTransform)
#if defined(IED_ENABLE_1D10T_SAFEGUARDS)
			,
			current(a_originalTransform)
#endif
		{
		}

		NiPointer<NiNode> node;
		NiTransform       orig;  // cached or zero, never read from loaded actor 3D

#if defined(IED_ENABLE_1D10T_SAFEGUARDS)
		mutable NiTransform current;
#endif

		static bool find_visible_geometry(
			NiAVObject*           a_object,
			const BSStringHolder* a_sh) noexcept;

		bool has_visible_geometry(
			const BSStringHolder* a_sh) const noexcept;

		bool has_visible_object(
			NiAVObject* a_findObject) const noexcept;

		mutable const Data::configNodeOverrideEntryTransform_t* cachedConfCME{ nullptr };
	};

	struct movNodeEntry_t
	{
		NiPointer<NiNode> node;
		WeaponPlacementID placementID;
	};

	class ActorObjectHolder
	//public BSTEventSink<BSAnimationGraphEvent>
	{
		friend class IObjectManager;
		friend class Controller;
		friend class ActorProcessorTask;
		friend class ObjectManagerData;

		struct monitorNodeEntry_t
		{
			NiPointer<NiNode> node;
			NiPointer<NiNode> parent;
			std::uint16_t     size;
			bool              visible;
		};

		struct actorLocationData_t
		{
			bool           inInterior{ false };
			TESWorldSpace* worldspace{ nullptr };
		};

	public:
		inline static constexpr Actor::Flags1 ACTOR_CHECK_FLAGS_1 =
			Actor::Flags1::kInWater |
			Actor::Flags1::kPlayerTeammate |
			Actor::Flags1::kGuard |
			Actor::Flags1::kParalyzed;

		inline static constexpr Actor::Flags2 ACTOR_CHECK_FLAGS_2 =
			Actor::Flags2::kIsAMount |
			Actor::Flags2::kGettingOnOffMount |
			Actor::Flags2::kInBleedoutAnimation |
			Actor::Flags2::kIsTrespassing |
			Actor::Flags2::kIsCommandedActor |
			Actor::Flags2::kBribedByPlayer |
			Actor::Flags2::kAngryWithPlayer |
			Actor::Flags2::kEssential |
			Actor::Flags2::kProtected |
			Actor::Flags2::kUnderwater;

		using slot_container_type = std::array<
			objectEntrySlot_t,
			stl::underlying(Data::ObjectSlot::kMax)>;

		using customEntryMap_t  = stl::unordered_map<stl::fixed_string, objectEntryCustom_t>;
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
			return m_cellAttached;
		}

		/*[[nodiscard]] inline constexpr bool GetEnemiesNearby() const noexcept
		{
			return m_enemiesNearby;
		}*/

		inline constexpr void UpdateCellAttached() noexcept
		{
			m_cellAttached = m_actor->IsParentCellAttached();
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

		void RegisterWeaponAnimationGraphManagerHolder(
			RE::WeaponAnimationGraphManagerHolderPtr& a_ptr,
			bool                                      a_forward);

		void UnregisterWeaponAnimationGraphManagerHolder(
			RE::WeaponAnimationGraphManagerHolderPtr& a_ptr);

		[[nodiscard]] inline constexpr auto& GetAnimationUpdateList() noexcept
		{
			return m_animationUpdateList;
		}

		[[nodiscard]] inline constexpr auto& GetAnimationUpdateList() const noexcept
		{
			return m_animationUpdateList;
		}

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

	private:
		void CreateExtraMovNodes(
			NiNode*                                   a_npcroot,
			const NodeOverrideData::extraNodeEntry_t& a_entry);

		void CreateExtraCopyNode(
			Actor*                                        a_actor,
			NiNode*                                       a_npcroot,
			const NodeOverrideData::extraNodeCopyEntry_t& a_entry) const;

		void ApplyNodeTransformOverrides(
			NiNode* a_root) const;

		/*EventResult ReceiveEvent(
			const BSAnimationGraphEvent*           a_event,
			BSTEventSource<BSAnimationGraphEvent>* a_eventSource) override;*/

		Game::ObjectRefHandle m_handle;
		long long             m_created{ 0 };

		union
		{
			mutable stl::flag<ActorObjectHolderFlags> m_flags{ ActorObjectHolderFlags::kNone };
			mutable ActorObjectHolderFlagsBitfield    m_flagsbf;
		};

		slot_container_type m_entriesSlot{};
		customPluginMap_t   m_entriesCustom[Data::CONFIG_CLASS_MAX]{};

		stl::vector<monitorNodeEntry_t> m_monitorNodes;
		stl::vector<weapNodeEntry_t>    m_weapNodes;

		stl::unordered_map<stl::fixed_string, cmeNodeEntry_t> m_cmeNodes;
		stl::unordered_map<stl::fixed_string, movNodeEntry_t> m_movNodes;

		NiPointer<Actor>  m_actor;
		NiPointer<NiNode> m_root;
		NiPointer<NiNode> m_npcroot;

		Game::FormID m_formid;
		bool         m_female{ false };
		bool         m_humanoidSkeleton{ false };

		bool                m_cellAttached{ false };
		bool                m_isPlayerTeammate{ false };
		bool                m_wantLFUpdate{ false };
		bool                m_inCombat{ false };
		bool                m_swimming{ false };
		bool                m_sitting{ false };
		bool                m_sleeping{ false };
		bool                m_beingRidden{ false };
		bool                m_forceNodeCondTrue{ false };
		Actor::Flags1       m_cflags1{ Actor::Flags1::kNone };
		Actor::Flags2       m_cflags2{ Actor::Flags2::kNone };
		long long           m_lastLFStateCheck;
		long long           m_lastHFStateCheck;
		actorLocationData_t m_locData;
		TESPackage*         m_currentPackage{ nullptr };

		SkeletonCache::const_actor_entry_type m_skeletonCache;
		SkeletonID                            m_skeletonID;

		mutable ActorAnimationState m_animState;

		AnimationGraphManagerHolderList m_animationUpdateList;
		//AnimationGraphManagerHolderList m_animEventForwardRegistrations;
		//const bool                      m_enableAnimEventForwarding{ false };

		BipedSlotDataPtr m_lastEquipped;

		IObjectManager& m_owner;

		static std::atomic_ullong m_lfsc_delta_lf;
		static std::atomic_ullong m_lfsc_delta_hf;
	};

	using ActorObjectMap = stl::unordered_map<Game::FormID, ActorObjectHolder>;

	class ObjectManagerData
	{
	public:
		template <class... Args>
		[[nodiscard]] inline constexpr auto& GetObjectHolder(
			Actor* a_actor,
			Args&&... a_args)
		{
			auto r = m_objects.try_emplace(
				a_actor->formID,
				a_actor,
				std::forward<Args>(a_args)...);

			if (r.second)
			{
				ApplyActorState(r.first->second);
				OnActorAcquire(r.first->second);
			}

			return r.first->second;
		}

		[[nodiscard]] inline constexpr auto& GetData() const noexcept
		{
			return m_objects;
		}
		
		[[nodiscard]] inline constexpr auto& GetData() noexcept
		{
			return m_objects;
		}

		inline void ClearPlayerState() noexcept
		{
			m_playerState.reset();
		}

	private:
		void ApplyActorState(ActorObjectHolder& a_holder);

		virtual void OnActorAcquire(ActorObjectHolder& a_holder) = 0;

	protected:
		ActorObjectMap                           m_objects;
		std::unique_ptr<Data::actorStateEntry_t> m_playerState;
		Data::actorStateHolder_t                 m_storedActorStates;
	};

}