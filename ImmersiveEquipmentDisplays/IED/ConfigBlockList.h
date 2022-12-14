#pragma once

#include "ConfigData.h"

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

			stl::flat_set<
				stl::fixed_string,
				stl::fixed_string_less_equal_ptr,
				stl::boost_container_allocator<stl::fixed_string>>
				keys;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& keys;
			}
		};

		struct actorBlockList_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::boost_unordered_map<configForm_t, actorBlockEntry_t> data;
			bool                                                      playerToggle{ false };

			inline void clear()
			{
				data.clear();
				playerToggle = false;
			}

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& data;
				a_ar& playerToggle;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::actorBlockEntry_t,
	::IED::Data::actorBlockEntry_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::actorBlockList_t,
	::IED::Data::actorBlockList_t::Serialization::DataVersion1);
