#pragma once

namespace IED
{
	class AnimationUpdateController
	{
		typedef bool (*updateAnimationGraph_t)(
			RE::IAnimationGraphManagerHolder* a_holder,
			const BSAnimationUpdateData&      a_data) noexcept;

		using Entry = RE::WeaponAnimationGraphManagerHolderPtr;

		using lock_type   = std::shared_mutex;
		using shared_lock = std::shared_lock<lock_type>;
		using unique_lock = std::unique_lock<lock_type>;

	public:
		/*void BeginAnimationUpdate(
			Controller* a_controller) noexcept;

		void EndAnimationUpdate(
			Controller* a_controller) noexcept;*/

		inline static const auto UpdateAnimationGraph = IAL::Address<updateAnimationGraph_t>(32155, 32899);

		[[nodiscard]] inline static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		[[nodiscard]] inline constexpr bool GetEnabled() const noexcept
		{
			return m_enabled;
		}

		std::size_t GetNumObjects() const noexcept;

		void Initialize() noexcept;

		void OnUpdate(
			Actor*                       a_actor,
			const BSAnimationUpdateData& a_data) const noexcept;

		void AddObject(
			Game::FormID                                    a_actor,
			const RE::WeaponAnimationGraphManagerHolderPtr& a_ptr) noexcept;

		void RemoveObject(
			Game::FormID                                    a_actor,
			const RE::WeaponAnimationGraphManagerHolderPtr& a_ptr) noexcept;

		void RemoveActor(Game::FormID a_actor) noexcept;

	private:
		bool m_enabled{ false };

		mutable lock_type                                          m_lock;
		stl::unordered_map<Game::FormID, stl::forward_list<Entry>> m_data;

		static AnimationUpdateController m_Instance;
	};
}