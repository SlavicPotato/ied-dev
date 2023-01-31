#pragma once

namespace IED
{
	struct ObjectLight
	{
		[[nodiscard]] inline constexpr explicit operator bool() const noexcept
		{
			return static_cast<bool>(niObject.get());
		}

		[[nodiscard]] inline constexpr auto operator->() const noexcept
		{
			return niObject.get();
		}

		void Cleanup(NiNode* a_root) noexcept;

		NiPointer<NiPointLight>        niObject;
		NiPointer<RE::BSLight>         bsObject;
	};
}