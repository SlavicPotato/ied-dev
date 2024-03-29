#pragma once

#include "ConfigCommon.h"

#include "Controller/ActiveActorAnimation.h"

namespace IED
{
	class ObjectManagerData;
	class ActorObjectHolder;

	namespace Data
	{
		struct actorStateSlotEntry_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
				DataVersion3 = 3,
			};

			configFormZeroMissing_t lastEquipped;
			configFormZeroMissing_t lastSlotted;
			long long               lastSeenEquipped{ std::numeric_limits<long long>::min() };

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& lastEquipped;
				a_ar& lastSeenEquipped;
				a_ar& lastSlotted;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				if (a_version < DataVersion2)
				{
					configForm_t tmp;
					a_ar&        tmp;
					lastEquipped = tmp;
				}
				else
				{
					a_ar& lastEquipped;
				}

				a_ar& lastSeenEquipped;

				if (a_version >= DataVersion3)
				{
					a_ar& lastSlotted;
				}

				if (!lastEquipped)
				{
					lastSeenEquipped = std::numeric_limits<long long>::min();
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		struct actorStateEntry_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
			};

			actorStateEntry_t() = default;
			//actorStateEntry_t(const ActorObjectHolder& a_holder);

			actorStateSlotEntry_t slots[stl::underlying(ObjectSlot::kMax)];

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& slots;
			}
		};

		struct actorStateHolder_t
		{
			friend class boost::serialization::access;

		public:
			actorStateHolder_t() = default;
			//actorStateHolder_t(const ObjectManagerData& a_data);

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			inline void clear() noexcept
			{
				data.clear();
			}

			configFormMap_t<actorStateEntry_t> data;

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& data;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& data;
				data.erase(0);
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::actorStateSlotEntry_t,
	::IED::Data::actorStateSlotEntry_t::Serialization::DataVersion3);

BOOST_CLASS_VERSION(
	::IED::Data::actorStateEntry_t,
	::IED::Data::actorStateEntry_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::actorStateHolder_t,
	::IED::Data::actorStateHolder_t::Serialization::DataVersion1);
