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

			float r;
			float g;
			float b;

		protected:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& r;
				ar& g;
				ar& b;
			}
		};

		struct configColorRGBA_t :
			public configColorRGB_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			float a;

		protected:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configColorRGB_t&>(*this);
				ar& a;
			}
		};
	}
}

BOOST_CLASS_VERSION(
	IED::Data::configColorRGB_t,
	IED::Data::configColorRGB_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configColorRGBA_t,
	IED::Data::configColorRGBA_t::Serialization::DataVersion1);
