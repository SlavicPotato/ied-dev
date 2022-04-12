#pragma once

#include "ConfigLUIDTag.h"

namespace IED
{
	namespace Data
	{
		struct configTransform_t :
			configLUIDTag_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configTransform_t()
			{
				*scale = 1.0f;
			}

			configTransform_t(
				float           a_scale,
				const NiPoint3& a_pos,
				const NiPoint3& a_rot) :
				scale(a_scale),
				position(a_pos),
				rotation(a_rot)
			{
			}

			stl::optional<float>    scale;
			stl::optional<NiPoint3> position;
			stl::optional<NiPoint3> rotation;

			void clamp() noexcept
			{
				if (!scale && !position && !rotation)
				{
					return;
				}

				if (scale)
				{
					*scale = std::clamp(stl::zero_nan(*scale), 0.01f, 100.0f);
				}

				if (position)
				{
					position->x = std::clamp(stl::zero_nan(position->x), -5000.0f, 5000.0f);
					position->y = std::clamp(stl::zero_nan(position->y), -5000.0f, 5000.0f);
					position->z = std::clamp(stl::zero_nan(position->z), -5000.0f, 5000.0f);
				}

				if (rotation)
				{
					constexpr auto pi2 = std::numbers::pi_v<float> * 2.0f;

					rotation->x = std::clamp(stl::zero_nan(rotation->x), -pi2, pi2);
					rotation->y = std::clamp(stl::zero_nan(rotation->y), -pi2, pi2);
					rotation->z = std::clamp(stl::zero_nan(rotation->z), -pi2, pi2);
				}

				update_tag();
			}

			NiTransform to_nitransform() const
			{
				NiTransform result;

				if (scale)
				{
					result.scale = *scale;
				}

				if (position)
				{
					result.pos = *position;
				}

				if (rotation)
				{
					result.rot.SetEulerAngles(
						rotation->x,
						rotation->y,
						rotation->z);
				}

				return result;
			}

			void clear()
			{
				scale.clear();
				*scale = 1.0f;

				position.reset();
				rotation.reset();

				update_tag();
			}

			[[nodiscard]] inline constexpr bool empty() const noexcept
			{
				return !scale && !position && !rotation;
			}

		protected:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& scale;
				a_ar& position;
				a_ar& rotation;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& scale;
				a_ar& position;
				a_ar& rotation;

				clamp();
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configTransform_t,
	::IED::Data::configTransform_t::Serialization::DataVersion1);
