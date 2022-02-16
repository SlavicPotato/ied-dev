#pragma once

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIActorInfoInterface
		{
		public:
			UIActorInfoInterface(Controller& a_controller);

			[[nodiscard]] const ActorInfoHolder&             GetActorInfo() const noexcept;
			[[nodiscard]] const NPCInfoHolder&               GetNPCInfo() const noexcept;
			[[nodiscard]] std::uint64_t                      GetActorInfoUpdateID() const noexcept;
			[[nodiscard]] const stl::optional<Game::FormID>& GetCrosshairRef() const noexcept;

		private:
			Controller& m_controller;
		};
	}
}