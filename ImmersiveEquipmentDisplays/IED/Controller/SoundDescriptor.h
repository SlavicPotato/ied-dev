#pragma once

namespace IED
{

	struct SoundDescriptor
	{
		[[nodiscard]] constexpr explicit operator bool() const noexcept
		{
			return static_cast<bool>(form);
		}

		BGSSoundDescriptorForm* form;
		//bool                    followLight;
	};

}