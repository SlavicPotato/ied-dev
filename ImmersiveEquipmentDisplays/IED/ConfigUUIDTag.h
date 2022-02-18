#pragma once

namespace IED
{
	namespace Data
	{
		struct configUUIDTag_t :
			uuid_tag
		{
		public:
			inline configUUIDTag_t() noexcept :
				uuid_tag(IUUID()())
			{
			}

			inline configUUIDTag_t(const configUUIDTag_t& a_rhs) noexcept :
				uuid_tag(IUUID()())
			{
			}

			inline configUUIDTag_t(configUUIDTag_t&& a_rhs) noexcept :
				uuid_tag(IUUID()())
			{
			}

			configUUIDTag_t& operator=(const configUUIDTag_t& a_rhs) noexcept
			{
				update_tag();
				return *this;
			}

			configUUIDTag_t& operator=(configUUIDTag_t&& a_rhs) noexcept
			{
				update_tag();
				return *this;
			}

			inline void update_tag() noexcept
			{
				static_cast<uuid_tag&>(*this) = IUUID()();
			}
		};
	}
}