#pragma once

#include "ConfigOverrideCommon.h"

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
				DataVersion1 = 1
			};

			configForm_t lastEquipped;
			long long    lastSeenEquipped{ std::numeric_limits<long long>::min() };

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& lastEquipped;
				ar& lastSeenEquipped;
			}
		};

		struct actorStateEntry_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			actorStateEntry_t() = default;
			actorStateEntry_t(const ActorObjectHolder& a_holder);

			actorStateSlotEntry_t slots[stl::underlying(ObjectSlot::kMax)];

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& slots;
			}
		};

		struct actorStateHolder_t
		{
			friend class boost::serialization::access;

		public:
			actorStateHolder_t() = default;
			actorStateHolder_t(const ObjectManagerData& a_data);

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
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& data;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	IED::Data::actorStateSlotEntry_t,
	IED::Data::actorStateSlotEntry_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::actorStateEntry_t,
	IED::Data::actorStateEntry_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::actorStateHolder_t,
	IED::Data::actorStateHolder_t::Serialization::DataVersion1);
