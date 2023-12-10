#pragma once

namespace IED
{
	class ObjectCloningTask;

	class AnimationUpdateController
	{
		using updateAnimationGraph_t = bool (*)(
			RE::IAnimationGraphManagerHolder* a_holder,
			const BSAnimationUpdateData&      a_data) noexcept;

		using loadWeaponGraph_t = bool (*)(
			RE::IAnimationGraphManagerHolder& a_weapHolder,
			const char*                       a_hkxPath) noexcept;

		using bindAnimationObject_t = bool (*)(
			RE::WeaponAnimationGraphManagerHolder& a_holder,
			NiAVObject*                            a_object) noexcept;

		using Entry = RE::WeaponAnimationGraphManagerHolderPtr;

		using lock_type        = stl::shared_mutex;
		using read_lock_guard  = stl::read_lock_guard<lock_type>;
		using write_lock_guard = stl::write_lock_guard<lock_type>;

	public:
		/*void BeginAnimationUpdate(
			Controller* a_controller) noexcept;

		void EndAnimationUpdate(
			Controller* a_controller) noexcept;*/

		[[nodiscard]] static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		[[nodiscard]] constexpr bool IsInitialized() const noexcept
		{
			return m_initialized;
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

		static bool CreateWeaponBehaviorGraph(
			const NiPointer<NiNode>&                  a_object,
			RE::WeaponAnimationGraphManagerHolderPtr& a_out,
			std::function<bool(const char*)>          a_allowFunc = [](const char*) { return true; });

		static bool LoadWeaponBehaviorGraph(
			const NiPointer<NiNode>&                                            a_object,
			std::pair<BSFixedString, RE::WeaponAnimationGraphManagerHolderPtr>& a_out);

		static bool CreateWeaponBehaviorGraph(
			const NiPointer<NiNode>&                  a_object,
			ObjectCloningTask&                        a_in,
			RE::WeaponAnimationGraphManagerHolderPtr& a_out,
			std::function<bool(const char*)>          a_allowFunc = [](const char*) { return true; });

		static void CleanupWeaponBehaviorGraph(
			const RE::WeaponAnimationGraphManagerHolderPtr& a_graph) noexcept;

	private:
		inline static const auto UpdateAnimationGraph         = IAL::Address<updateAnimationGraph_t>(32155, 32899);
		inline static const auto LoadAnimationBehahaviorGraph = IAL::Address<loadWeaponGraph_t>(32148, 32892);
		inline static const auto BindAnimationObject          = IAL::Address<bindAnimationObject_t>(32250, 32985);

		bool m_initialized{ false };

		mutable lock_type                                    m_lock;
		stl::unordered_map<Game::FormID, stl::vector<Entry>> m_data;

		static AnimationUpdateController m_Instance;
	};
}