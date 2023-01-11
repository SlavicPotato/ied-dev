#pragma once

namespace IED
{
	struct ObjectLight
	{
		[[nodiscard]] inline explicit operator bool() const noexcept
		{
			return static_cast<bool>(niObject.get());
		}

		[[nodiscard]] inline auto operator->() const noexcept
		{
			return niObject.get();
		}

		void Cleanup(NiNode *a_object) noexcept;

		NiPointer<NiPointLight> niObject;
		NiPointer<RE::BSLight>  bsObject;
		BSSoundHandle           sound;
	};
}