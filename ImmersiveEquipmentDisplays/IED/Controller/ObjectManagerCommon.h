#pragma once

namespace IED
{
	namespace Data
	{
		struct cacheTransform_t
		{
			constexpr void Update(const Data::configTransform_t& a_in)
			{
				scale    = a_in.scale;
				position = a_in.position;

				if (a_in.rotation)
				{
					if (a_in.xfrmFlags.test(ConfigTransformFlags::kExtrinsicRotation))
					{
						rotation->SetEulerAnglesExtrinsic(
							a_in.rotation->x,
							a_in.rotation->y,
							a_in.rotation->z);
					}
					else
					{
						rotation->SetEulerAnglesIntrinsic(
							a_in.rotation->x,
							a_in.rotation->y,
							a_in.rotation->z);
					}

					rotation.mark(true);
				}
				else
				{
					rotation.clear();
				}

				tag = a_in;
			}

			constexpr void clear() noexcept
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