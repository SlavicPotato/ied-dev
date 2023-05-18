#pragma once

#include "IED/ConfigTransform.h"

namespace IED
{
	namespace Data
	{
		struct cacheTransform_t
		{
			constexpr void Update(const Data::configTransform_t& a_in) noexcept
			{
				if (a_in.scale || a_in.position || a_in.rotationMatrix)
				{
					NiTransform tmp{ NiTransform::noinit_arg_t{} };

					if (a_in.scale)
					{
						tmp.scale = *a_in.scale;
					}
					else
					{
						tmp.scale = 1.0f;
					}

					if (a_in.position)
					{
						tmp.pos = *a_in.position;
					}
					else
					{
						tmp.pos = {};
					}

					if (a_in.rotationMatrix)
					{
						tmp.rot = *a_in.rotationMatrix;
					}
					else
					{
						tmp.rot.Identity();
					}

					data.emplace(std::move(tmp));
				}
				else
				{
					data.reset();
				}

				tag = a_in;
			}

			constexpr void clear() noexcept
			{
				data.reset();
				tag.reset();
			}

			[[nodiscard]] constexpr bool operator==(
				const Data::configTransform_t& a_rhs) const noexcept
			{
				return tag == a_rhs;
			}

			using transform_type =
#if defined(IED_PERF_BUILD)
				Bullet::btTransformEx
#else
				NiTransform
#endif
				;

			std::optional<transform_type> data;
			std::optional<luid_tag>       tag;
		};
	}
}