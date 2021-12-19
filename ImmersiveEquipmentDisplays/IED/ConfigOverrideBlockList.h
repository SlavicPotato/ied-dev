#pragma once

#include "ConfigOverrideCommon.h"

namespace IED
{
	namespace Data
	{
		struct actorBlockEntry_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::set<stl::fixed_string, stl::fixed_string_less_equal_p> keys;

		private:
			template <class Archive>
			void load(Archive& ar, const unsigned int version)
			{
				ar& keys;
			}

			template <class Archive>
			void save(Archive& ar, const unsigned int version) const
			{
				ar& keys;
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		struct actorBlockList_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			std::unordered_map<configForm_t, actorBlockEntry_t> data;
			bool playerToggle{ false };

			inline void clear()
			{
				data.clear();
				playerToggle = false;
			}

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& data;
				ar& playerToggle;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	IED::Data::actorBlockEntry_t,
	IED::Data::actorBlockEntry_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::actorBlockList_t,
	IED::Data::actorBlockList_t::Serialization::DataVersion1);
