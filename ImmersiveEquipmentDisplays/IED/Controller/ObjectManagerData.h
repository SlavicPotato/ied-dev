#pragma once

#include "IED/ConfigOverride.h"
#include "INode.h"
#include "NodeOverrideData.h"
#include "ObjectDatabase.h"
//#include "ObjectEntryStatus.h"
#include "ObjectManagerCommon.h"

#include "IED/ActorState.h"

namespace IED
{
	enum class ObjectEntryFlags : std::uint32_t
	{
		kNone = 0,

		kRefSyncDisableFailedOrphan = 1u << 1,
		kDropOnDeath = 1u << 4,
		kScbLeft = 1u << 2,

		kSyncReferenceTransform = 1u << 6,
		kPlaySound = 1u << 8
	};

	DEFINE_ENUM_CLASS_BITWISE(ObjectEntryFlags);

	struct objectEntryBase_t
	{
		objectEntryBase_t() = default;

		objectEntryBase_t(const objectEntryBase_t&) = delete;
		objectEntryBase_t(objectEntryBase_t&&) = delete;
		objectEntryBase_t& operator=(const objectEntryBase_t&) = delete;
		objectEntryBase_t& operator=(objectEntryBase_t&&) = delete;

		inline void Reset()
		{
			if (state)
			{
				for (auto& e : state->dbEntries)
				{
					e->accessed = IPerfCounter::Query();
				}

				state.reset();
			}
		}

		inline void SetNodeVisible(bool a_switch) noexcept
		{
			if (state)
			{
				state->nodes.obj->SetVisible(a_switch);
			}
		}

		SKMP_FORCEINLINE auto IsActive() const noexcept
		{
			return state && state->nodes.obj->IsVisible();
		}

		SKMP_FORCEINLINE auto GetFormIfActive() const noexcept
		{
			return IsActive() ? state->form : nullptr;
		}

		SKMP_FORCEINLINE auto IsNodeVisible() const noexcept
		{
			return state && state->nodes.obj->IsVisible();
		}

		struct State
		{
			void UpdateData(const Data::configBaseValues_t& a_in) noexcept
			{
				// gross but efficient

				static_assert(
					std::is_same_v<std::underlying_type_t<ObjectEntryFlags>, std::underlying_type_t<Data::FlagsBase>> &&
					stl::underlying((ObjectEntryFlags::kPlaySound)) == stl::underlying((Data::FlagsBase::kPlaySound)) &&
					stl::underlying((ObjectEntryFlags::kDropOnDeath)) == stl::underlying((Data::FlagsBase::kDropOnDeath)) &&
					stl::underlying((ObjectEntryFlags::kSyncReferenceTransform)) == stl::underlying((Data::FlagsBase::kSyncReferenceTransform)));

				flags =
					(flags & ~(ObjectEntryFlags::kPlaySound | ObjectEntryFlags::kDropOnDeath | ObjectEntryFlags::kSyncReferenceTransform | ObjectEntryFlags::kRefSyncDisableFailedOrphan)) |
					static_cast<ObjectEntryFlags>((a_in.flags & (Data::FlagsBase::kPlaySound | Data::FlagsBase::kDropOnDeath | Data::FlagsBase::kSyncReferenceTransform)));

				transform.UpdateData(a_in);
			}

			TESForm* form{ nullptr };
			Game::FormID formid;
			stl::flag<ObjectEntryFlags> flags{ ObjectEntryFlags::kNone };
			Data::NodeDescriptor nodeDesc;
			nodesRef_t nodes;
			Data::cacheTransform_t transform;
			std::vector<ObjectDatabase::ObjectDatabaseEntry> dbEntries;
			bool atmReference{ true };
		};

		std::unique_ptr<State> state;
	};

	struct objectEntrySlot_t :
		public objectEntryBase_t
	{
		Data::actorStateSlotEntry_t slotState;
		Data::ObjectSlot slotid{ Data::ObjectSlot::kMax };
		std::uint8_t hideCountdown{ 0 };

		inline constexpr void ResetDeferedHide() noexcept
		{
			hideCountdown = 0;
		}
	};

	enum class CustomObjectEntryFlags : std::uint32_t
	{
		kNone = 0,

		kProcessedChance = 1u << 0,
		kBlockedByChance = 1u << 1,

		kChanceMask = kProcessedChance | kBlockedByChance
	};

	DEFINE_ENUM_CLASS_BITWISE(CustomObjectEntryFlags);

	struct objectEntryCustom_t :
		public objectEntryBase_t
	{
		inline constexpr void clear_chance_flags() noexcept
		{
			cflags.clear(CustomObjectEntryFlags::kChanceMask);
		}

		Game::FormID modelForm;
		stl::flag<CustomObjectEntryFlags> cflags{ CustomObjectEntryFlags::kNone };
	};

	class IObjectManager;
	class Controller;
	class NodeProcessorTask;
	class INodeOverride;

	enum class ActorObjectHolderFlags : std::uint32_t
	{
		kNone = 0,

		kWantTransformUpdate = 1u << 0,
		kImmediateTransformUpdate = 1u << 1,
		kSkipNextTransformUpdate = 1u << 2,

		kRequestTransformUpdateDefer = kWantTransformUpdate | kSkipNextTransformUpdate,
		kRequestTransformUpdateImmediate = kWantTransformUpdate | kImmediateTransformUpdate,
		kRequestTransformUpdateMask = kWantTransformUpdate | kImmediateTransformUpdate | kSkipNextTransformUpdate,

		kWantEval = 1u << 3,
		kImmediateEval = 1u << 4,
		kEvalCountdown1 = 1u << 5,
		kEvalCountdown2 = 1u << 6,

		kRequestEval = kWantEval,
		kRequestEvalImmediate = kWantEval | kImmediateEval,
		kRequestEvalMask = kWantEval | kImmediateEval | kEvalCountdown1 | kEvalCountdown2
	};

	DEFINE_ENUM_CLASS_BITWISE(ActorObjectHolderFlags);

	struct ActorObjectHolderFlagsBitfield
	{
		std::uint32_t wantTransformUpdate: 1;
		std::uint32_t immediateTransformUpdate: 1;
		std::uint32_t skipNextTransformUpdate: 1;
		std::uint32_t wantEval: 1;
		std::uint32_t immediateEval: 1;
		std::uint32_t evalCountdown: 2;
		std::uint32_t unused: 26;
	};

	struct weapNodeEntry_t
	{
		friend class INodeOverride;

	public:
		weapNodeEntry_t(
			const stl::fixed_string& a_nodeName,
			const BSFixedString& a_bsnodeName,
			const stl::fixed_string& a_defaultParent) :
			nodeName(a_nodeName),
			bsNodeName(a_bsnodeName),
			defaultParent(a_defaultParent)
		{
		}

		const stl::fixed_string nodeName;
		const BSFixedString& bsNodeName;
		const stl::fixed_string defaultParent;

		inline constexpr const auto& get_current_target() const noexcept
		{
			return currentTarget;
		}

	private:
		mutable stl::fixed_string currentTarget;
	};

	struct cmeNodeEntry_t
	{
		NiPointer<NiNode> node;
		NiTransform originalTransform;
		bool originalVisibility;

		constexpr bool has_visible_geometry() const noexcept;
		constexpr bool has_visible_geometry(
			const BSFixedString& a_scb,
			const BSFixedString& a_scbLeft) const noexcept;

		static constexpr bool find_visible_geometry(
			NiAVObject* a_object) noexcept;

		static constexpr bool find_visible_geometry(
			NiAVObject* a_object,
			const BSFixedString& a_scb,
			const BSFixedString& a_scbLeft) noexcept;

		mutable const Data::configNodeOverrideEntryTransform_t* cachedConfCME{ nullptr };
	};

	class ActorObjectHolder
	{
		friend class IObjectManager;
		friend class Controller;
		friend class NodeProcessorTask;
		friend class ObjectManagerData;

		struct monitorNodeEntry_t
		{
			NiPointer<NiNode> node;
			NiPointer<NiNode> parent;
			std::uint16_t size;
			bool visible;
		};

	public:
		using slot_container_type = objectEntrySlot_t[stl::underlying(Data::ObjectSlot::kMax)];

		ActorObjectHolder() = delete;
		ActorObjectHolder(
			const stl::optional<Data::actorStateEntry_t>& a_playerState,
			Actor* a_actor,
			NiNode* a_root,
			NiNode* a_npcroot,
			IObjectManager& a_owner,
			Game::ObjectRefHandle a_handle,
			bool a_nodeOverrideEnabled,
			bool a_nodeOverrideEnabledPlayer,
			Data::actorStateHolder_t& a_actorState);

		~ActorObjectHolder();

		ActorObjectHolder(const ActorObjectHolder&) = delete;
		ActorObjectHolder(ActorObjectHolder&&) = default;
		ActorObjectHolder& operator=(const ActorObjectHolder&) = delete;
		ActorObjectHolder& operator=(ActorObjectHolder&&) = default;

		[[nodiscard]] inline auto& GetSlot(
			Data::ObjectSlot a_slot) noexcept
		{
			return m_entriesSlot[stl::underlying(a_slot)];
		}

		[[nodiscard]] inline const auto& GetSlot(
			Data::ObjectSlot a_slot) const noexcept
		{
			return m_entriesSlot[stl::underlying(a_slot)];
		}

		[[nodiscard]] inline constexpr const auto& GetSlots() const noexcept
		{
			return m_entriesSlot;
		}

		[[nodiscard]] bool AnySlotOccupied() const noexcept;

		[[nodiscard]] std::size_t GetNumOccupiedSlots() const noexcept;

		[[nodiscard]] std::size_t GetNumOccupiedCustom() const noexcept;

		[[nodiscard]] inline auto GetAge() const noexcept
		{
			return IPerfCounter::delta_us(m_created, IPerfCounter::Query());
		}

		[[nodiscard]] inline constexpr const auto& GetHandle() const noexcept
		{
			return m_handle;
		}

		[[nodiscard]] inline auto& GetCustom(Data::ConfigClass a_class) noexcept
		{
			return m_entriesCustom[stl::underlying(a_class)];
		}

		[[nodiscard]] inline const auto& GetCustom(Data::ConfigClass a_class) const noexcept
		{
			return m_entriesCustom[stl::underlying(a_class)];
		}

		[[nodiscard]] inline constexpr const auto& GetCMENodes() const noexcept
		{
			return m_cmeNodes;
		}

		[[nodiscard]] inline constexpr const auto& GetWeapNodes() const noexcept
		{
			return m_weapNodes;
		}

		inline void RequestTransformUpdateDefer() const noexcept
		{
			if (!m_cmeNodes.empty())
			{
				m_flags.set(ActorObjectHolderFlags::kRequestTransformUpdateDefer);
			}
		}

		inline void RequestTransformUpdateDeferNoSkip() const noexcept
		{
			if (!m_cmeNodes.empty())
			{
				m_flags.set(ActorObjectHolderFlags::kWantTransformUpdate);
			}
		}

		inline void RequestTransformUpdate() const noexcept
		{
			if (!m_cmeNodes.empty())
			{
				m_flags.set(ActorObjectHolderFlags::kRequestTransformUpdateImmediate);
			}
		}

		inline void RequestEvalDefer() const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kRequestEval);
			if (m_flagsbf.evalCountdown == 0)
			{
				m_flagsbf.evalCountdown = 2;
			}
		}

		inline void RequestEval() const noexcept
		{
			m_flags.set(ActorObjectHolderFlags::kRequestEvalImmediate);
		}

		void ApplyActorState(const Data::actorStateEntry_t& a_data);

		[[nodiscard]] bool IsActorNPC(Game::FormID a_npc) const;
		[[nodiscard]] bool IsActorRace(Game::FormID a_race) const;

		using customEntryMap_t = std::unordered_map<stl::fixed_string, objectEntryCustom_t>;
		using customPluginMap_t = std::unordered_map<stl::fixed_string, customEntryMap_t>;

		template <class Tv>
		void visit(Tv a_func)
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

	private:
		Game::ObjectRefHandle m_handle;
		long long m_created{ 0 };

		union
		{
			mutable stl::flag<ActorObjectHolderFlags> m_flags{ ActorObjectHolderFlags::kNone };
			mutable ActorObjectHolderFlagsBitfield m_flagsbf;
		};

		slot_container_type m_entriesSlot;
		customPluginMap_t m_entriesCustom[Data::CONFIG_CLASS_MAX];

		std::vector<monitorNodeEntry_t> m_monitorNodes;
		std::unordered_map<stl::fixed_string, cmeNodeEntry_t> m_cmeNodes;
		std::vector<weapNodeEntry_t> m_weapNodes;

		NiPointer<Actor> m_actor;
		NiPointer<NiNode> m_root;
		NiPointer<NiNode> m_npcroot;

		Game::FormID m_formid;

		IObjectManager& m_owner;
	};

	using ActorObjectMap = std::unordered_map<Game::FormID, ActorObjectHolder>;

	class ObjectManagerData
	{
	public:
		template <class... Args>
		[[nodiscard]] inline constexpr auto& GetObjectHolder(
			Actor* a_actor,
			Args&&... a_args)
		{
			return m_objects.try_emplace(
								a_actor->formID,
								m_playerState,
								a_actor,
								std::forward<Args>(a_args)...)
			    .first->second;
		}

		[[nodiscard]] inline constexpr const auto& GetData() const noexcept
		{
			return m_objects;
		}

		inline constexpr void ClearPlayerState() noexcept
		{
			m_playerState.clear();
		}

	protected:
		ActorObjectMap m_objects;
		stl::optional<Data::actorStateEntry_t> m_playerState;
	};

	constexpr bool cmeNodeEntry_t::find_visible_geometry(NiAVObject* a_object) noexcept
	{
		if (!a_object->IsVisible())
		{
			return false;
		}

		if (a_object->GetAsBSGeometry())
		{
			return true;
		}

		if (auto node = a_object->GetAsNiNode())
		{
			for (auto object : node->m_children)
			{
				if (object)
				{
					if (find_visible_geometry(object))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	constexpr bool cmeNodeEntry_t::find_visible_geometry(
		NiAVObject* a_object,
		const BSFixedString& a_scb,
		const BSFixedString& a_scbLeft) noexcept
	{
		if (!a_object->IsVisible())
		{
			return false;
		}

		if (a_object->GetAsBSGeometry() &&
		    a_object->m_name != a_scb &&
		    a_object->m_name != a_scbLeft)
		{
			return true;
		}

		if (auto node = a_object->GetAsNiNode())
		{
			for (auto object : node->m_children)
			{
				if (object)
				{
					if (find_visible_geometry(
							object,
							a_scb,
							a_scbLeft))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	constexpr bool cmeNodeEntry_t::has_visible_geometry() const noexcept
	{
		return find_visible_geometry(node);
	}

	constexpr bool cmeNodeEntry_t::has_visible_geometry(
		const BSFixedString& a_scb,
		const BSFixedString& a_scbLeft) const noexcept
	{
		return find_visible_geometry(node, a_scb, a_scbLeft);
	}

}  // namespace IED