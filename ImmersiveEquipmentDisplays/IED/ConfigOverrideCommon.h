#pragma once

#include "ConfigCommon.h"

namespace IED
{
	namespace Data
	{
		Game::FormID resolve_form(Game::FormID a_form);

		struct configRange_t
		{
			std::uint32_t min{ 0 };
			std::uint32_t max{ 0 };

			inline constexpr operator std::uint32_t*() noexcept
			{
				return std::addressof(min);
			}
		};

		struct configForm_t :
			Game::FormID
		{
			friend class boost::serialization::access;

		public:
			using Game::FormID::FormID;
			using Game::FormID::operator=;

			configForm_t(Game::FormID a_rhs) :
				Game::FormID(a_rhs)
			{
			}

		private:
			template <class Archive>
			void save(Archive& ar, const unsigned int version) const
			{
				ar& static_cast<const Game::FormID&>(*this);
			}

			template <class Archive>
			void load(Archive& ar, const unsigned int version)
			{
				ar& static_cast<Game::FormID&>(*this);

				if (*this)
				{
					*this = resolve_form(*this);
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		static_assert(sizeof(configForm_t) == sizeof(Game::FormID));

		struct configCachedForm_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configCachedForm_t() = default;

			inline configCachedForm_t(Game::FormID a_id) noexcept :
				id(a_id),
				form(nullptr)
			{
			}

			inline configCachedForm_t& operator=(Game::FormID a_id) noexcept
			{
				id = a_id;
				form = nullptr;
				return *this;
			}

			inline TESForm* get_form() const noexcept
			{
				if (!id)
				{
					return nullptr;
				}

				if (!form || form->formID != id)
				{
					form = lookup_form(id);
				}

				return form;
			}

			template <class T, class form_type = stl::strip_type<T>>
			inline form_type* get_form_as() const noexcept
			{
				if (auto form = get_form())
				{
					return form->As<form_type>();
				}
				else
				{
					return nullptr;
				}
			}

			inline constexpr const auto& get_id() const noexcept
			{
				return id;
			}

			inline constexpr auto& get_id() noexcept
			{
				return id;
			}

			/*inline void reset() const
			{
				form = nullptr;
			}*/

		private:
			template <class Archive>
			void save(Archive& ar, const unsigned int version) const
			{
				ar& id;
			}

			template <class Archive>
			void load(Archive& ar, const unsigned int version)
			{
				ar& id;

				if (id)
				{
					id = resolve_form(id);
				}
			}

			static TESForm* lookup_form(Game::FormID a_form) noexcept;

			Game::FormID id;
			mutable TESForm* form{ nullptr };

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

	}
}

STD_SPECIALIZE_HASH(IED::Data::configForm_t);

namespace IED
{
	namespace Data
	{
		template <class T>
		class configSexRoot_t
		{
			friend class boost::serialization::access;

		public:
			using config_type = T;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configSexRoot_t() = default;

			[[nodiscard]] inline constexpr auto& operator()() noexcept
			{
				return m_configs;
			}

			[[nodiscard]] inline constexpr const auto& operator()() const noexcept
			{
				return m_configs;
			}

			[[nodiscard]] inline auto& operator()(ConfigSex a_sex) noexcept
			{
				return get(a_sex);
			}

			[[nodiscard]] inline const auto& operator()(ConfigSex a_sex) const noexcept
			{
				return get(a_sex);
			}

			[[nodiscard]] inline auto& get(ConfigSex a_sex) noexcept
			{
				return m_configs[stl::underlying(a_sex)];
			}

			[[nodiscard]] inline const auto& get(ConfigSex a_sex) const noexcept
			{
				return m_configs[stl::underlying(a_sex)];
			}

			void clear()
			{
				for (auto& e : m_configs)
				{
					e.clear();
				}
			}

			template <class Tf>
			void visit(Tf a_func)
			{
				for (auto& e : m_configs)
				{
					a_func(e);
				}
			}

		private:
			T m_configs[2];

			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				for (auto& e : m_configs)
				{
					ar& e;
				}
			}
		};

		template <class T>
		using configFormMap_t = std::unordered_map<configForm_t, T>;

		template <class T>
		class configStoreBase_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			using data_type = T;
			//using config_type = typename T::data_type::config_type;

			inline constexpr auto& GetActorData() noexcept
			{
				return data[stl::underlying(ConfigClass::Actor)];
			}

			inline constexpr const auto& GetActorData() const noexcept
			{
				return data[stl::underlying(ConfigClass::Actor)];
			}

			inline constexpr auto& GetRaceData() noexcept
			{
				return data[stl::underlying(ConfigClass::Race)];
			}

			inline constexpr const auto& GetRaceData() const noexcept
			{
				return data[stl::underlying(ConfigClass::Race)];
			}

			inline constexpr auto& GetNPCData() noexcept
			{
				return data[stl::underlying(ConfigClass::NPC)];
			}

			inline constexpr const auto& GetNPCData() const noexcept
			{
				return data[stl::underlying(ConfigClass::NPC)];
			}

			/*inline constexpr auto& GetData(ConfigClass a_class) noexcept
			{
				return data[stl::underlying(a_class)];
			}

			inline constexpr const auto& GetData(ConfigClass a_class) const noexcept
			{
				return data[stl::underlying(a_class)];
			}*/

			inline constexpr auto& GetGlobalData() noexcept
			{
				return global;
			}

			inline constexpr const auto& GetGlobalData() const noexcept
			{
				return global;
			}

			inline constexpr auto& GetGlobalData(GlobalConfigType a_type) noexcept
			{
				return global[stl::underlying(a_type)];
			}

			inline constexpr const auto& GetGlobalData(GlobalConfigType a_type) const noexcept
			{
				return global[stl::underlying(a_type)];
			}

			inline constexpr auto& GetFormMaps() noexcept
			{
				return data;
			}

			inline constexpr const auto& GetFormMaps() const noexcept
			{
				return data;
			}

		protected:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& data[stl::underlying(ConfigClass::Actor)];
				ar& data[stl::underlying(ConfigClass::NPC)];
				ar& data[stl::underlying(ConfigClass::Race)];
				ar& global;
			}

			configFormMap_t<data_type> data[3];
			data_type global[2];
		};

		class configFormSet_t :
			public stl::set<configForm_t>
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<stl::set<configForm_t>&>(*this);
			}
		};

		enum class FormFilterBaseFlags : std::uint32_t
		{
			kNone = 0,

			kDenyAll = 1u << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(FormFilterBaseFlags);

		struct configFormFilterBase_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<FormFilterBaseFlags> flags{ FormFilterBaseFlags::kNone };
			configFormSet_t allow;
			configFormSet_t deny;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& allow;
				ar& deny;
			}
		};

		struct configFormFilterProfile_t
		{
			stl::fixed_string name;
		};

		enum class FormFilterFlags : std::uint32_t
		{
			kNone = 0,

			kUseProfile = 1u << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(FormFilterFlags);

		struct configFormFilter_t :
			public configFormFilterBase_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<FormFilterFlags> filterFlags{ FormFilterFlags::kNone };
			configFormFilterProfile_t profile;

			bool test(Game::FormID a_form) const;

			inline bool test(
				const configFormFilterBase_t& a_data,
				Game::FormID a_form) const
			{
				if (a_data.allow.contains(a_form))
				{
					return true;
				}

				if (a_data.flags.test(FormFilterBaseFlags::kDenyAll) ||
				    a_data.deny.contains(a_form))
				{
					return false;
				}
				else
				{
					return true;
				}
			}

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configFormFilterBase_t&>(*this);
				ar& filterFlags.value;
				ar& profile.name;
			}
		};

		using configFormList_t = std::vector<configForm_t>;
	}
}

BOOST_CLASS_VERSION(
	IED::Data::configCachedForm_t,
	IED::Data::configCachedForm_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configFormFilterBase_t,
	IED::Data::configFormFilterBase_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configFormFilter_t,
	IED::Data::configFormFilter_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configFormSet_t,
	IED::Data::configFormSet_t::Serialization::DataVersion1);

BOOST_CLASS_TEMPLATE_VERSION(
	class T,
	IED::Data::configSexRoot_t<T>,
	IED::Data::configSexRoot_t<T>::Serialization::DataVersion1);

BOOST_CLASS_TEMPLATE_VERSION(
	class T,
	IED::Data::configStoreBase_t<T>,
	IED::Data::configStoreBase_t<T>::Serialization::DataVersion1);
