#pragma once

namespace IED
{
	class Controller;
	class AnimationGraphManagerHolderList;

	class AnimationUpdateManager
	{
	public:
		void BeginAnimationUpdate(
			Controller* a_controller);

		void EndAnimationUpdate(
			Controller* a_controller);

		void ProcessAnimationUpdateList(
			Actor*                       a_actor,
			const BSAnimationUpdateData& a_data,
			const Controller&            a_controller);

		static void UpdateActorAnimationList(
			Actor*                       a_actor,
			const BSAnimationUpdateData& a_data,
			const Controller&            a_controller);

	private:
#if defined(DEBUG)
		std::atomic<bool> m_running{ false };
#endif
	};
}