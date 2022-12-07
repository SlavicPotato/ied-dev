#pragma once

namespace IED
{
	namespace Data
	{
		// always generate
		struct configLUIDTagAG_t :
			luid_tag
		{
			inline configLUIDTagAG_t() noexcept :
				luid_tag(ILUID()())
			{
			}

			configLUIDTagAG_t(const configLUIDTagAG_t&) noexcept :
				luid_tag(ILUID()())
			{
			}

			configLUIDTagAG_t(configLUIDTagAG_t&&) noexcept :
				luid_tag(ILUID()())
			{
			}

			configLUIDTagAG_t& operator=(const configLUIDTagAG_t&) noexcept
			{
				static_cast<luid_tag&>(*this) = ILUID()();
				return *this;
			}

			configLUIDTagAG_t operator=(configLUIDTagAG_t&&) noexcept
			{
				static_cast<luid_tag&>(*this) = ILUID()();
				return *this;
			}

			inline void update_tag() noexcept
			{
				static_cast<luid_tag&>(*this) = ILUID()();
			}
		};

		// generate in copy ctor, default elsewhere
		struct configLUIDTagAC_t :
			luid_tag
		{
			inline configLUIDTagAC_t() noexcept :
				luid_tag(ILUID()())
			{
			}

			configLUIDTagAC_t(const configLUIDTagAC_t&) noexcept :
				luid_tag(ILUID()())
			{
			}

			configLUIDTagAC_t(configLUIDTagAC_t&&) noexcept = default;

			configLUIDTagAC_t& operator=(const configLUIDTagAC_t&) = default;
			configLUIDTagAC_t& operator=(configLUIDTagAC_t&&) = default;

			inline void update_tag() noexcept
			{
				static_cast<luid_tag&>(*this) = ILUID()();
			}
		};

		// generate in copy ctor, noop in copy assign, default move ctor/assign
		struct configLUIDTagMCG_t :
			luid_tag
		{
			inline configLUIDTagMCG_t() noexcept :
				luid_tag(ILUID()())
			{
			}

			configLUIDTagMCG_t(const configLUIDTagMCG_t&) noexcept :
				luid_tag(ILUID()())
			{
			}

			configLUIDTagMCG_t(configLUIDTagMCG_t&&) noexcept = default;

			configLUIDTagMCG_t& operator=(const configLUIDTagMCG_t&)
			{
				return *this;
			}

			configLUIDTagMCG_t& operator=(configLUIDTagMCG_t&&) = default;

			configLUIDTagMCG_t(const luid_tag& a_rhs) noexcept :
				luid_tag(a_rhs)
			{
			}
		};
	}
}