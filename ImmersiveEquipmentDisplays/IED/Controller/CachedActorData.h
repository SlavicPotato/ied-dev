#pragma once

namespace IED
{
	inline static constexpr Actor::Flags1 ACTOR_CHECK_FLAGS_1 =
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
		Actor::Flags2::kIsInKillMove;

	inline static constexpr Actor::Flags1 ACTOR_CHECK_FLAGS_LF_1 =
		Actor::Flags1::kInWater;

	inline static constexpr Actor::Flags2 ACTOR_CHECK_FLAGS_LF_2 =
		Actor::Flags2::kUnderwater;

	class CachedFactionData
	{
		using container_type =
			stl::map_sa<
				Game::FormID,
				std::int8_t>;

	public:
		CachedFactionData(Actor* a_actor);

		bool UpdateFactions(Actor* a_actor);

		[[nodiscard]] inline constexpr auto GetNumFactions() const noexcept
		{
			return active_container->size();
		}

		inline constexpr const auto& GetFactionContainer() const noexcept
		{
			return *active_container;
		}

	private:
		inline constexpr auto& GetWorkingContainer() noexcept
		{
			return active_container == std::addressof(b1) ? b2 : b1;
		}

		inline constexpr void SwapContainers() noexcept
		{
			active_container = std::addressof(GetWorkingContainer());
		}

		inline constexpr bool BuffersEqual() const
		{
			auto& r1 = b1.raw();
			auto& r2 = b2.raw();

			return r1.size() == r2.size() &&
			       std::equal(
					   r1.begin(),
					   r1.end(),
					   r2.begin());
		}

		container_type b1;
		container_type b2;

		container_type* active_container;
	};

	struct CachedActorData :
		CachedFactionData
	{
		CachedActorData(Actor* a_actor);

		bool UpdateState(Actor* a_actor);
		bool UpdateStateLF(Actor* a_actor);

		TESWorldSpace*           worldspace{ nullptr };
		TESPackage*              currentPackage{ nullptr };
		TESIdleForm*             currentIdle{ nullptr };
		stl::flag<Actor::Flags1> flags1{ Actor::Flags1::kNone };
		stl::flag<Actor::Flags2> flags2{ Actor::Flags2::kNone };
		stl::flag<Actor::Flags1> flagslf1{ Actor::Flags1::kNone };
		stl::flag<Actor::Flags2> flagslf2{ Actor::Flags2::kNone };
		bool                     cellAttached{ false };
		bool                     inCombat{ false };
		bool                     swimming{ false };
		bool                     sitting{ false };
		bool                     sleeping{ false };
		bool                     beingRidden{ false };
		bool                     weaponDrawn{ false };
		bool                     inInterior{ false };
		bool                     arrested{ false };
		bool                     unconscious{ false };
	};
}