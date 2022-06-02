#pragma once

namespace IED
{
	namespace Data
	{
		enum class NodeDescriptorFlags : std::uint32_t
		{
			kNone = 0,

			kManaged     = 1ui32 << 0,
			kDoNotTouch  = 1ui32 << 1,
			kPapyrus     = 1ui32 << 2,
			kXP32        = 1ui32 << 3,
			kDefault     = 1ui32 << 4,
			kWeapon      = 1ui32 << 5,
			kDefaultCopy = 1ui32 << 6,
			kUserNode    = 1ui32 << 15,
		};

		DEFINE_ENUM_CLASS_BITWISE(NodeDescriptorFlags);

		struct NodeDescriptor
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::fixed_string              name;
			stl::flag<NodeDescriptorFlags> flags{ NodeDescriptorFlags::kNone };

			[[nodiscard]] inline constexpr bool managed() const noexcept
			{
				return flags.test(NodeDescriptorFlags::kManaged);
			}

			inline constexpr explicit operator bool() const noexcept
			{
				return !name.empty();
			}

			void lookup_flags();

		protected:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& name;
				a_ar& flags.value;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& name;
				a_ar& flags.value;

				lookup_flags();
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::NodeDescriptor,
	::IED::Data::NodeDescriptor::Serialization::DataVersion1);