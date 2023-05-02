#pragma once

#include <ext/stl_typeid.h>

#define CLIP_TYPE_CASE(cl)     \
	case stl::type_hash<cl>(): \
		return std::addressof(data->To<cl>());

#define CLIP_TYPE_CASE_CUSTOM()                                                          \
	case stl::type_hash<Data::configCustomNameValue_t>():                                \
		{                                                                                \
			return std::addressof(get_value(data->To<Data::configCustomNameValue_t>())); \
		}

namespace IED
{
	namespace Data
	{
		struct configSlot_t;
		template <class T>
		struct configOverride_t;
		struct configBaseValues_t;
		using equipmentOverride_t = configOverride_t<configBaseValues_t>;
		struct configBase_t;
		struct configNodeOverrideTransformValues_t;
		struct configNodeOverrideOffset_t;
		struct configNodeOverrideTransform_t;
		struct configNodeOverridePlacementValues_t;
		struct configNodeOverridePlacementOverride_t;
		struct configNodeOverridePlacement_t;
		struct configNodePhysicsValues_t;
		struct configNodeOverridePhysicsOverride_t;
		struct configNodeOverridePhysics_t;
		struct configCustomNameValue_t;
		struct configCustom_t;
		struct configBaseFiltersHolder_t;
	}

	namespace UI
	{
		class UIClipboard
		{
		private:
			template <class T>
			class Entry;

			class EntryAdapter
			{
			protected:
				explicit EntryAdapter(
					std::uint32_t a_id) noexcept :
					id(a_id)
				{
				}

			public:
				virtual ~EntryAdapter() noexcept = default;

				template <class T>
				[[nodiscard]] const T& To() const noexcept
				{
					return static_cast<const Entry<T>*>(this)->data;
				}

				template <class T>
				[[nodiscard]] const T* As() const noexcept
				{
					return id == stl::type_hash<T>() ? std::addressof(To<T>()) : nullptr;
				}

				const std::uint32_t id;
			};

			template <class T>
			class Entry :
				public EntryAdapter
			{
			public:
				using value_type = stl::strip_type<T>;

				template <class... Args>
				explicit Entry(Args&&... a_args) :
					EntryAdapter(stl::type_hash<value_type>()),
					data(std::forward<Args>(a_args)...)
				{
				}

				~Entry() override = default;

				value_type data;
			};

			template <class T>
			const T* GetImpl() const noexcept
			{
				auto& data = m_data;

				if (!data)
				{
					return nullptr;
				}

				if constexpr (std::is_same_v<T, Data::configBaseValues_t>)
				{
					switch (data->id)
					{
						CLIP_TYPE_CASE_CUSTOM()
						CLIP_TYPE_CASE(Data::configSlot_t)
						CLIP_TYPE_CASE(Data::equipmentOverride_t)
						CLIP_TYPE_CASE(Data::configBase_t)
					}
				}

				if constexpr (std::is_same_v<T, Data::configBase_t>)
				{
					switch (data->id)
					{
						CLIP_TYPE_CASE_CUSTOM()
						CLIP_TYPE_CASE(Data::configSlot_t)
					}
				}

				if constexpr (std::is_same_v<T, Data::configNodeOverrideTransformValues_t>)
				{
					switch (data->id)
					{
						CLIP_TYPE_CASE(Data::configNodeOverrideOffset_t)
						CLIP_TYPE_CASE(Data::configNodeOverrideTransform_t)
					}
				}

				if constexpr (std::is_same_v<T, Data::configNodeOverridePlacementValues_t>)
				{
					switch (data->id)
					{
						CLIP_TYPE_CASE(Data::configNodeOverridePlacementOverride_t)
						CLIP_TYPE_CASE(Data::configNodeOverridePlacement_t)
					}
				}

				if constexpr (std::is_same_v<T, Data::configNodePhysicsValues_t>)
				{
					switch (data->id)
					{
						CLIP_TYPE_CASE(Data::configNodeOverridePhysicsOverride_t)
						CLIP_TYPE_CASE(Data::configNodeOverridePhysics_t)
					}
				}

				if constexpr (std::is_same_v<T, Data::configBaseFiltersHolder_t>)
				{
					switch (data->id)
					{
						CLIP_TYPE_CASE_CUSTOM()
						CLIP_TYPE_CASE(Data::configSlot_t)
						CLIP_TYPE_CASE(Data::configBase_t)
					}
				}

				return data->As<T>();
			}

			static const Data::configCustom_t& get_value(
				const Data::configCustomNameValue_t& a_data) noexcept;

		public:
			template <class T>
			[[nodiscard]] static const auto* Get() noexcept
			{
				using data_type = Entry<T>::value_type;

				return m_Instance.GetImpl<data_type>();
			}

			template <class T>
			static void Set(const T& a_data)
			{
				m_Instance.m_data = std::make_unique<const Entry<T>>(a_data);
			}

			template <class T, class... Args>
			static void Set(Args&&... a_data)
			{
				m_Instance.m_data = std::make_unique<const Entry<T>>(std::forward<Args>(a_data)...);
			}

			static void Clear();

		private:
			UIClipboard() = default;

			std::unique_ptr<const EntryAdapter> m_data;

			static UIClipboard m_Instance;
		};
	}
}

#undef CLIP_TYPE_CASE
#undef CLIP_TYPE_CASE_CUSTOM