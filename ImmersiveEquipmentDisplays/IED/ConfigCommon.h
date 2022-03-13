#pragma once

#include "ConfigData.h"
#include "ConfigUUIDTag.h"

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

			[[nodiscard]] inline constexpr bool empty() const noexcept
			{
				return !min && !max;
			}
		};

		struct configForm_t :
			Game::FormID
		{
			friend class boost::serialization::access;

		public:
			using Game::FormID::FormID;
			using Game::FormID::operator=;

			inline constexpr configForm_t(
				const Game::FormID& a_rhs) noexcept :
				Game::FormID(a_rhs)
			{
			}

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& static_cast<const Game::FormID&>(*this);
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<Game::FormID&>(*this);

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

			inline constexpr configCachedForm_t(
				const Game::FormID& a_id) noexcept :
				id(a_id),
				form(nullptr)
			{
			}

			inline configCachedForm_t& operator=(
				const Game::FormID& a_id) noexcept
			{
				id   = a_id;
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
			inline form_type* get_form() const noexcept
			{
				if (auto f = get_form())
				{
					return f->As<form_type>();
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
			static TESForm* lookup_form(Game::FormID a_form) noexcept;

			Game::FormID     id;
			mutable TESForm* form{ nullptr };

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& id;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& id;

				if (id)
				{
					id = resolve_form(id);
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		enum class QuestConditionType : std::uint32_t
		{
			kNone = static_cast<std::underlying_type_t<QuestConditionType>>(-1),

			kComplete = 0,
		};

		enum class ExtraConditionType : std::uint32_t
		{
			kNone = static_cast<std::underlying_type_t<ExtraConditionType>>(-1),

			kCanDualWield      = 0,
			kIsDead            = 1,
			kInInterior        = 2,
			kIsPlayerTeammate  = 3,
			kIsGuard           = 4,
			kIsMount           = 5,
			kShoutEquipped     = 6,
			kInMerchantFaction = 7,
			kCombatStyle       = 8,
			kClass             = 9,
		};

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

			[[nodiscard]] inline constexpr auto& operator()() noexcept
			{
				return m_configs;
			}

			[[nodiscard]] inline constexpr auto& operator()() const noexcept
			{
				return m_configs;
			}

			[[nodiscard]] inline constexpr auto& get(ConfigSex a_sex) noexcept
			{
				return m_configs[stl::underlying(a_sex)];
			}

			[[nodiscard]] inline constexpr auto& get(ConfigSex a_sex) const noexcept
			{
				return m_configs[stl::underlying(a_sex)];
			}

			[[nodiscard]] inline constexpr auto& operator()(ConfigSex a_sex) noexcept
			{
				return get(a_sex);
			}

			[[nodiscard]] inline constexpr auto& operator()(ConfigSex a_sex) const noexcept
			{
				return get(a_sex);
			}

			void clear()
			{
				for (auto& e : m_configs)
				{
					e.clear();
				}
			}

			template <class Tf>
			constexpr void visit(Tf a_func)
			{
				for (auto& e : m_configs)
				{
					a_func(e);
				}
			}

			template <class Tf>
			constexpr void visit(Tf a_func) const
			{
				for (auto& e : m_configs)
				{
					a_func(e);
				}
			}

			template <class Tf>
			constexpr void visit2(Tf a_func)
			{
				a_func(ConfigSex::Male, get(ConfigSex::Male));
				a_func(ConfigSex::Female, get(ConfigSex::Female));
			}

			template <class Tf>
			constexpr void visit2(Tf a_func) const
			{
				a_func(ConfigSex::Male, get(ConfigSex::Male));
				a_func(ConfigSex::Female, get(ConfigSex::Female));
			}

		private:
			T m_configs[2];

			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				for (auto& e : m_configs)
				{
					a_ar& e;
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

			void clear()
			{
				for (auto& e : data)
				{
					e.clear();
				}

				for (auto& e : global)
				{
					e.clear();
				}
			}

		protected:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& data[stl::underlying(ConfigClass::Actor)];
				a_ar& data[stl::underlying(ConfigClass::NPC)];
				a_ar& data[stl::underlying(ConfigClass::Race)];
				a_ar& global;
			}

			configFormMap_t<data_type> data[3];
			data_type                  global[2];
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
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<stl::set<configForm_t>&>(*this);
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
			configFormSet_t                allow;
			configFormSet_t                deny;

			inline bool test(
				Game::FormID a_form) const
			{
				if (allow.contains(a_form))
				{
					return true;
				}

				return !flags.test(FormFilterBaseFlags::kDenyAll) &&
				       !deny.contains(a_form);
			}

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& allow;
				a_ar& deny;
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
			configFormFilterProfile_t  profile;

			bool test(Game::FormID a_form) const;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configFormFilterBase_t&>(*this);
				a_ar& filterFlags.value;
				a_ar& profile.name;
			}
		};

		using configFormList_t       = std::vector<configForm_t>;
		using configFixedStringSet_t = stl::set<stl::fixed_string>;

	}

	template <class T>
	struct configHolderCache_t
	{
	public:
		using mapped_type = typename T::mapped_type;

		const mapped_type* get_actor(
			Game::FormID a_actor,
			const T&     a_data) const
		{
			if (!actor)
			{
				auto it = a_data.find(a_actor);

				actor = it != a_data.end() ?
				            std::addressof(it->second) :
                            nullptr;
			}

			return *actor;
		}

		const mapped_type* get_npc(
			Game::FormID a_npc,
			const T&     a_data) const
		{
			if (!npc)
			{
				auto it = a_data.find(a_npc);

				npc = it != a_data.end() ?
				          std::addressof(it->second) :
                          nullptr;
			}

			return *npc;
		}

		const mapped_type* get_race(
			Game::FormID a_race,
			const T&     a_data) const
		{
			if (!race)
			{
				auto it = a_data.find(a_race);

				race = it != a_data.end() ?
				           std::addressof(it->second) :
                           nullptr;
			}

			return *race;
		}

		template <class Td>
		SKMP_FORCEINLINE static constexpr const typename Td::mapped_type* get_entry(
			const Td&                    a_data,
			const typename Td::key_type& a_key)
		{
			if (a_data.empty())
			{
				return nullptr;
			}
			else
			{
				auto it = a_data.find(a_key);
				return it != a_data.end() ?
				           std::addressof(it->second) :
                           nullptr;
			}
		}

	private:
		mutable std::optional<const mapped_type*> actor;
		mutable std::optional<const mapped_type*> npc;
		mutable std::optional<const mapped_type*> race;
	};

}

STD_SPECIALIZE_HASH(::IED::Data::configForm_t);

BOOST_CLASS_VERSION(
	::IED::Data::configCachedForm_t,
	::IED::Data::configCachedForm_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configFormFilterBase_t,
	::IED::Data::configFormFilterBase_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configFormFilter_t,
	::IED::Data::configFormFilter_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configFormSet_t,
	::IED::Data::configFormSet_t::Serialization::DataVersion1);

BOOST_CLASS_TEMPLATE_VERSION(
	class T,
	::IED::Data::configSexRoot_t<T>,
	::IED::Data::configSexRoot_t<T>::Serialization::DataVersion1);

BOOST_CLASS_TEMPLATE_VERSION(
	class T,
	::IED::Data::configStoreBase_t<T>,
	::IED::Data::configStoreBase_t<T>::Serialization::DataVersion1);
