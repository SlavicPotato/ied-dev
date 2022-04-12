#pragma once

namespace IED
{
	namespace Data
	{
		struct cacheTransform_t
		{
			inline static constexpr float zero_nan(float a_value) noexcept
			{
				return std::isnan(a_value) ? 0.0f : a_value;
			}

			void Update(const Data::configTransform_t& a_in)
			{
				scale    = a_in.scale;
				position = a_in.position;

				if (a_in.rotation)
				{
					rotation->SetEulerAngles(
						a_in.rotation->x,
						a_in.rotation->y,
						a_in.rotation->z);

					rotation.mark(true);
				}
				else
				{
					rotation.clear();
				}

				tag = a_in;
			}

			void clear() noexcept
			{
				scale.clear();
				position.clear();
				rotation.clear();
				tag.reset();
			}

			[[nodiscard]] inline constexpr bool operator==(
				const Data::configTransform_t& a_rhs) const
			{
				return tag == a_rhs;
			}

			stl::optional<NiPoint3>   position;
			stl::optional<NiMatrix33> rotation;
			stl::optional<float>      scale;
			std::optional<luid_tag>   tag;
		};
	}
}