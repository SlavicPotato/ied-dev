#pragma once

namespace IED
{
	namespace Data
	{
		struct configColorRGB_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			constexpr operator float*() noexcept
			{
				return std::addressof(r);
			}

			constexpr operator const float*() const noexcept
			{
				return std::addressof(r);
			}

			inline operator NiColor() const noexcept
			{
				return {
					r,
					g,
					b
				};
			}

			constexpr void clamp()
			{
				r = std::clamp(r, 0.0f, 1.0f);
				g = std::clamp(g, 0.0f, 1.0f);
				b = std::clamp(b, 0.0f, 1.0f);
			}

			float r{ 0.0f };
			float g{ 0.0f };
			float b{ 0.0f };

		protected:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& r;
				a_ar& g;
				a_ar& b;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& r;
				a_ar& g;
				a_ar& b;

				clamp();
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		static_assert(offsetof(configColorRGB_t, r) == 0x0);
		static_assert(offsetof(configColorRGB_t, g) == 0x4);
		static_assert(offsetof(configColorRGB_t, b) == 0x8);

		struct configColorRGBA_t :
			public configColorRGB_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			float a{ 1.0f };

			constexpr operator NiColorA() const noexcept
			{
				return {
					r,
					g,
					b,
					a
				};
			}

			constexpr void clamp()
			{
				configColorRGB_t::clamp();
				a = std::clamp(a, 0.0f, 1.0f);
			}

		protected:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& static_cast<const configColorRGB_t&>(*this);
				a_ar& a;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configColorRGB_t&>(*this);
				a_ar& a;

				a = std::clamp(a, 0.0f, 1.0f);
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		static_assert(offsetof(configColorRGBA_t, r) == 0x0);
		static_assert(offsetof(configColorRGBA_t, g) == 0x4);
		static_assert(offsetof(configColorRGBA_t, b) == 0x8);
		static_assert(offsetof(configColorRGBA_t, a) == 0xC);
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configColorRGB_t,
	::IED::Data::configColorRGB_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configColorRGBA_t,
	::IED::Data::configColorRGBA_t::Serialization::DataVersion1);
