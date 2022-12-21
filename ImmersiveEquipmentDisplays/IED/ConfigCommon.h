#pragma once

#include "ConfigData.h"
#include "ConfigLUIDTag.h"

namespace IED
{
	namespace Data
	{
		Game::FormID resolve_form(Game::FormID a_form);
		Game::FormID resolve_form_zero_missing(Game::FormID a_form);

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

		struct configFormZeroMissing_t :
			Game::FormID
		{
			friend class boost::serialization::access;

		public:
			using Game::FormID::FormID;
			using Game::FormID::operator=;

			inline constexpr configFormZeroMissing_t(
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
					*this = resolve_form_zero_missing(*this);
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		static_assert(sizeof(configFormZeroMissing_t) == sizeof(Game::FormID));

		struct configCachedForm_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configCachedForm_t() noexcept = default;

			inline constexpr configCachedForm_t(
				const Game::FormID& a_id) noexcept :
				id(a_id)
			{
			}

			inline constexpr configCachedForm_t(
				const configCachedForm_t& a_rhs) noexcept :
				id(a_rhs.id)
			{
			}

			inline constexpr configCachedForm_t(
				configCachedForm_t&& a_rhs) noexcept :
				id(a_rhs.id)
			{
				a_rhs.form = nullptr;
			}

			inline constexpr configCachedForm_t& operator=(
				const Game::FormID& a_id) noexcept
			{
				id   = a_id;
				form = nullptr;

				return *this;
			}

			inline constexpr configCachedForm_t& operator=(
				const configCachedForm_t& a_rhs) noexcept
			{
				id   = a_rhs.id;
				form = nullptr;

				return *this;
			}

			inline constexpr configCachedForm_t& operator=(
				configCachedForm_t&& a_rhs) noexcept
			{
				id   = a_rhs.id;
				form = nullptr;

				a_rhs.form = nullptr;

				return *this;
			}

			inline constexpr TESForm* get_form() const noexcept
			{
				if (!id)
				{
					return nullptr;
				}

				if (!form || form->formID != id)
				{
					form = lookup_form(id);
				}

				if (form && form->IsDeleted())
				{
					return nullptr;
				}

				return form;
			}

			template <
				class T,
				class form_type = stl::strip_type<T>>
			inline constexpr form_type* get_form() const noexcept
			{
				if (const auto f = get_form())
				{
					return f->As<form_type>();
				}
				else
				{
					return nullptr;
				}
			}

			inline constexpr auto& get_id() const noexcept
			{
				return id;
			}

			inline constexpr auto& get_id() noexcept
			{
				return id;
			}

			[[nodiscard]] inline constexpr friend bool operator==(
				const configCachedForm_t& a_lhs,
				const Game::FormID&       a_rhs) noexcept
			{
				return a_lhs.id == a_rhs;
			}

			[[nodiscard]] inline constexpr friend bool operator<=(
				const configCachedForm_t& a_lhs,
				const Game::FormID&       a_rhs) noexcept
			{
				return a_lhs.id <= a_rhs;
			}

			[[nodiscard]] inline constexpr friend bool operator==(
				const configCachedForm_t& a_lhs,
				const configCachedForm_t& a_rhs) noexcept
			{
				return a_lhs.id == a_rhs.id;
			}

			[[nodiscard]] inline constexpr friend bool operator<=(
				const configCachedForm_t& a_lhs,
				const configCachedForm_t& a_rhs) noexcept
			{
				return a_lhs.id <= a_rhs.id;
			}

			/*inline void reset() const
			{
				form = nullptr;
			}*/

			void zero_missing_or_deleted();

		private:
			static TESForm* lookup_form(const Game::FormID a_form) noexcept;

			Game::FormID     id;
			mutable TESForm* form{ nullptr };

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

			kCanDualWield            = 0,
			kIsDead                  = 1,
			kInInterior              = 2,
			kIsPlayerTeammate        = 3,
			kIsGuard                 = 4,
			kIsMount                 = 5,
			kShoutEquipped           = 6,
			kInMerchantFaction       = 7,  // unused
			kCombatStyle             = 8,
			kClass                   = 9,
			kTimeOfDay               = 10,
			kIsInFirstPerson         = 11,
			kInCombat                = 12,
			kIsFemale                = 13,
			kPlayerEnemiesNearby     = 14,
			kInWater                 = 15,
			kUnderwater              = 16,
			kSwimming                = 17,
			kBleedingOut             = 18,
			kTresspassing            = 19,
			kIsCommanded             = 20,
			kParalyzed               = 21,
			kIsRidingMount           = 22,
			kHumanoidSkeleton        = 23,
			kIsPlayer                = 24,
			kBribedByPlayer          = 25,
			kAngryWithPlayer         = 26,
			kEssential               = 27,
			kProtected               = 28,
			kSitting                 = 29,
			kSleeping                = 30,
			kBeingRidden             = 31,
			kWeaponDrawn             = 32,
			kRandomPercent           = 33,
			kNodeMonitor             = 34,
			kArrested                = 35,
			kIsChild                 = 36,
			kInKillmove              = 37,
			kIsUnconscious           = 38,
			kIsPlayerLastRiddenMount = 39,
			kSDSShieldOnBackEnabled  = 40,
			kIsFlying                = 41,
			kIsLayingDown            = 42,
			kInPlayerEnemyFaction    = 43,
			kIsHorse                 = 44,
		};

		enum class ComparisonOperator : std::uint32_t
		{
			kEqual          = static_cast<std::underlying_type_t<ExtraConditionType>>(-1),
			kNotEqual       = 1,
			kGreater        = 2,
			kLower          = 3,
			kGreaterOrEqual = 4,
			kLowerOrEqual   = 5
		};

		template <class T>
		concept AcceptDataClear = requires(T a_data) {
									  a_data.clear();
								  };

		template <AcceptDataClear T>
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
				return data;
			}

			[[nodiscard]] inline constexpr auto& operator()() const noexcept
			{
				return data;
			}

			[[nodiscard]] inline constexpr auto& get(ConfigSex a_sex) noexcept
			{
				return data[stl::underlying(a_sex)];
			}

			[[nodiscard]] inline constexpr auto& get(ConfigSex a_sex) const noexcept
			{
				return data[stl::underlying(a_sex)];
			}

			[[nodiscard]] inline constexpr auto& operator()(ConfigSex a_sex) noexcept
			{
				return get(a_sex);
			}

			[[nodiscard]] inline constexpr auto& operator()(ConfigSex a_sex) const noexcept
			{
				return get(a_sex);
			}

			constexpr void clear()
			{
				for (auto& e : data)
				{
					e.clear();
				}
			}

			template <class Tf>
			constexpr void visit(Tf a_func)
			{
				for (auto& e : data)
				{
					a_func(e);
				}
			}

			template <class Tf>
			constexpr void visit(Tf a_func) const
			{
				for (auto& e : data)
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

			SKMP_REDEFINE_NEW_PREF();

		private:
			T data[2]{};

			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				for (auto& e : data)
				{
					a_ar& e;
				}
			}
		};

		template <class T>
		using configFormMap_t = stl::boost_unordered_map<configForm_t, T>;

		template <AcceptDataClear T>
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

			inline constexpr auto& GetActorData() const noexcept
			{
				return data[stl::underlying(ConfigClass::Actor)];
			}

			inline constexpr auto& GetRaceData() noexcept
			{
				return data[stl::underlying(ConfigClass::Race)];
			}

			inline constexpr auto& GetRaceData() const noexcept
			{
				return data[stl::underlying(ConfigClass::Race)];
			}

			inline constexpr auto& GetNPCData() noexcept
			{
				return data[stl::underlying(ConfigClass::NPC)];
			}

			inline constexpr auto& GetNPCData() const noexcept
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

			inline constexpr auto& GetGlobalData() const noexcept
			{
				return global;
			}

			inline constexpr auto& GetGlobalData(GlobalConfigType a_type) noexcept
			{
				return global[stl::underlying(a_type)];
			}

			inline constexpr auto& GetGlobalData(GlobalConfigType a_type) const noexcept
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

			constexpr void clear()
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
			public stl::flat_set<
				configForm_t,
				std::less_equal<configForm_t>,
				stl::boost_container_allocator<configForm_t>>
		{
			friend class boost::serialization::access;

			using super = stl::flat_set<
				configForm_t,
				std::less_equal<configForm_t>,
				stl::boost_container_allocator<configForm_t>>;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<super&>(*this);
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

			inline constexpr bool test(
				Game::FormID a_form) const noexcept
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

			bool test(Game::FormID a_form) const noexcept;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configFormFilterBase_t&>(*this);
				a_ar& filterFlags.value;
				a_ar& profile.name;
			}
		};

		using configFormList_t       = stl::boost_vector<configForm_t>;
		using configFixedStringSet_t = stl::flat_set<
			stl::fixed_string,
			stl::fixed_string_less_equal_ptr,
			stl::boost_container_allocator<stl::fixed_string>>;

		using configFixedStringList_t = stl::boost_vector<stl::fixed_string>;

	}

	template <class T>
	concept AcceptHolderCacheData =
		requires(T a_data) {
			{
				std::addressof(a_data.find(typename T::key_type())->second)
				} -> std::convertible_to<const typename T::mapped_type*>;
		};

	template <class T>
	struct configHolderCache_t
	{
	public:
		using mapped_type = typename T::mapped_type;
		using key_type    = typename T::key_type;

		static_assert(
			std::is_convertible_v<Game::FormID, typename T::key_type> &&
				std::is_convertible_v<typename T::key_type, Game::FormID>,
			"FormID <-> key_type must be convertible");

		constexpr const mapped_type* get_actor(
			const key_type& a_key,
			const T&        a_data) const noexcept
		{
			if (!actor)
			{
				auto it = a_data.find(a_key);

				actor.emplace(it != a_data.end() ? std::addressof(it->second) : nullptr);
			}

			return *actor;
		}

		constexpr const mapped_type* get_npc(
			const key_type& a_key,
			const T&        a_data) const noexcept
		{
			if (!npc)
			{
				auto it = a_data.find(a_key);

				npc.emplace(it != a_data.end() ? std::addressof(it->second) : nullptr);
			}

			return *npc;
		}

		constexpr const mapped_type* get_race(
			const key_type& a_key,
			const T&        a_data) const noexcept
		{
			if (!race)
			{
				auto it = a_data.find(a_key);

				race.emplace(it != a_data.end() ? std::addressof(it->second) : nullptr);
			}

			return *race;
		}

		template <AcceptHolderCacheData Td>
		SKMP_FORCEINLINE static constexpr const typename Td::mapped_type* get_entry(
			const Td&                    a_data,
			const typename Td::key_type& a_key) noexcept
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

	template <class T>
	struct configCopyableUniquePtr_t
	{
	public:
		configCopyableUniquePtr_t() = default;

		configCopyableUniquePtr_t(const configCopyableUniquePtr_t& a_rhs)
		{
			if (a_rhs.data)
			{
				data = std::make_unique<T>(*a_rhs.data);
			}
		}

		configCopyableUniquePtr_t& operator=(configCopyableUniquePtr_t a_rhs)
		{
			std::swap(data, a_rhs.data);
			return *this;
		}

		configCopyableUniquePtr_t(configCopyableUniquePtr_t&&)            = default;
		configCopyableUniquePtr_t& operator=(configCopyableUniquePtr_t&&) = default;

		[[nodiscard]] inline constexpr explicit operator bool() const noexcept
		{
			return static_cast<bool>(data.get());
		}

		[[nodiscard]] inline constexpr auto operator->() const noexcept
		{
			return data.get();
		}

		std::unique_ptr<T> data;
	};

}

namespace std
{
	template <>
	struct hash<::IED::Data::configForm_t>
	{
		inline constexpr std::size_t operator()(
			::IED::Data::configForm_t const& a_in) const noexcept
		{
			return hash<Game::FormID>()(a_in);
		}
	};

	template <>
	struct hash<::IED::Data::configCachedForm_t>
	{
		inline constexpr std::size_t operator()(
			::IED::Data::configCachedForm_t const& a_in) const noexcept
		{
			return hash<Game::FormID>()(a_in.get_id());
		}
	};

	template <>
	struct hash<::IED::Data::configFormZeroMissing_t>
	{
		inline constexpr std::size_t operator()(
			::IED::Data::configFormZeroMissing_t const& a_in) const noexcept
		{
			return hash<Game::FormID>()(a_in);
		}
	};
}

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
