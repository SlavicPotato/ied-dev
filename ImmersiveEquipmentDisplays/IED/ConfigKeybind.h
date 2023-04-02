#pragma once

namespace IED
{
	namespace Data
	{
		enum class KeybindEntryFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(KeybindEntryFlags);

		struct configKeybindEntry_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<KeybindEntryFlags> flags{ KeybindEntryFlags::kNone };
			std::uint32_t                key{ 0 };
			std::uint32_t                comboKey{ 0 };
			std::uint32_t                numStates{ 1 };

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& key;
				a_ar& comboKey;
				a_ar& numStates;
			}
		};

		enum class KeybindEntryHolderFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(KeybindEntryHolderFlags);

		struct configKeybindEntryHolder_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			using container_type =
				stl::boost_unordered_map<
					stl::fixed_string,
					configKeybindEntry_t>;

			stl::flag<KeybindEntryHolderFlags> flags{ KeybindEntryHolderFlags::kNone };
			container_type                     data;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& data;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configKeybindEntry_t,
	::IED::Data::configKeybindEntry_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configKeybindEntryHolder_t,
	::IED::Data::configKeybindEntryHolder_t::Serialization::DataVersion1);
