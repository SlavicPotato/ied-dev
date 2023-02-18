#pragma once

namespace IED
{
	class SkeletonCache
	{
		struct actor_entry_data :
			stl::intrusive_ref_counted
		{
			SKMP_REDEFINE_NEW_PREF();

			actor_entry_data(const stl::fixed_string& a_key);

			stl::unordered_map<stl::fixed_string, NiTransform> data;
		};

	public:
		class ActorEntry
		{
			friend class SkeletonCache;

		public:
			ActorEntry() = default;
			ActorEntry(const stl::fixed_string& a_key);

			[[nodiscard]] NiTransform GetCachedOrZeroTransform(
				const stl::fixed_string& a_name) const;

			[[nodiscard]] NiTransform GetCachedOrCurrentTransform(
				const stl::fixed_string& a_name,
				NiAVObject*              a_object) const;

			[[nodiscard]] std::optional<NiTransform> GetCachedTransform(
				const stl::fixed_string& a_name) const;

			[[nodiscard]] constexpr explicit operator bool() const noexcept
			{
				return static_cast<bool>(ptr);
			}

			[[nodiscard]] constexpr const auto* operator->() const noexcept
			{
				return std::addressof(ptr->data);
			}

		private:
			stl::smart_ptr<actor_entry_data> ptr;
		};

	private:
		using data_type = stl::unordered_map<stl::fixed_string, ActorEntry>;

	public:
		[[nodiscard]] static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		ActorEntry Get(
			TESObjectREFR* a_refr,
			bool           a_firstPerson = false);

		[[nodiscard]] std::size_t GetSize() const;
		[[nodiscard]] std::size_t GetTotalEntries() const;

	private:
		SkeletonCache() = default;

		static stl::fixed_string make_key(
			TESObjectREFR* a_refr,
			bool           a_firstPerson);

		ActorEntry get_or_create(
			const stl::fixed_string& a_key);

		ActorEntry try_get(
			const stl::fixed_string& a_key) const;

		mutable stl::shared_mutex m_lock;
		data_type                 m_data;

		static SkeletonCache m_Instance;
	};
}