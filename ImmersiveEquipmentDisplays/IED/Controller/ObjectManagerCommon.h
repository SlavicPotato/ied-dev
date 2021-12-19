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

			void UpdateData(const Data::configTransform_t& a_in) noexcept
			{
				scale = a_in.scale;
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
			}

			void clear() noexcept
			{
				scale.clear();
				position.clear();
				rotation.clear();
			}

			inline bool operator==(const cacheTransform_t& a_rhs) const
			{
				if (scale.has() != a_rhs.scale.has())
				{
					return false;
				}

				if (scale && a_rhs.scale)
				{
					if (*scale != *a_rhs.scale)
					{
						return false;
					}
				}

				if (position.has() != a_rhs.position.has())
				{
					return false;
				}

				if (position && a_rhs.position)
				{
					if (position->x != a_rhs.position->x ||
					    position->y != a_rhs.position->y ||
					    position->z != a_rhs.position->z)
					{
						return false;
					}
				}

				if (rotation.has() != a_rhs.rotation.has())
				{
					return false;
				}

				if (rotation && a_rhs.rotation)
				{
					if (std::memcmp(
							rotation->arr,
							a_rhs.rotation->arr,
							sizeof(rotation->arr)))
					{
						return false;
					}
				}

				return true;
			}

			SetObjectWrapper<float> scale;
			SetObjectWrapper<NiPoint3> position;
			SetObjectWrapper<NiMatrix33> rotation;
		};
	}  // namespace Data
}  // namespace IED