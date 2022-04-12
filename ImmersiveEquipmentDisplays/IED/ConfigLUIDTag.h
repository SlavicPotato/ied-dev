#pragma once

#include <ext/ILUID.h>

namespace IED
{
	namespace Data
	{
		struct configLUIDTag_t :
			luid_tag
		{
			inline configLUIDTag_t() noexcept :
				luid_tag(ILUID()())
			{
			}

			inline void update_tag() noexcept
			{
				static_cast<luid_tag&>(*this) = ILUID()();
			}
		};

		struct configUniqueObjectTag_t :
			luid_tag
		{
			inline configUniqueObjectTag_t() noexcept :
				luid_tag(ILUID()())
			{
			}

			configUniqueObjectTag_t(const configUniqueObjectTag_t&) noexcept :
				luid_tag(ILUID()())
			{
			}

			configUniqueObjectTag_t(configUniqueObjectTag_t&&) noexcept :
				luid_tag(ILUID()())
			{
			}

			configUniqueObjectTag_t& operator=(const configUniqueObjectTag_t&)
			{
				return *this;
			}

			configUniqueObjectTag_t operator=(configUniqueObjectTag_t&&)
			{
				return *this;
			}
		};
	}
}