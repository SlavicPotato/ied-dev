#pragma once

namespace IED
{
	class Controller;
	class AnimationGraphManagerHolderList;

	class AnimationUpdateManager
	{
		using entry_type = std::pair<
			Game::FormID::held_type,
			std::shared_ptr<AnimationGraphManagerHolderList>>;

	public:
		void PrepareAnimationUpdateList(
			Controller* a_controller);

		void ClearAnimationUpdateList(
			Controller* a_controller);

		void UpdateQueuedAnimationList(
			Actor*                       a_actor,
			const BSAnimationUpdateData& a_data,
			Controller*                  a_controller);

		static void UpdateActorAnimationList(
			Actor*                       a_actor,
			const BSAnimationUpdateData& a_data,
			Controller*                  a_controller);

	private:

		std::atomic<bool>     m_running{ false };
	};
}