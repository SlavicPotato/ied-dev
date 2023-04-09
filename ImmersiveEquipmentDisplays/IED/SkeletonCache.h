#pragma once

namespace IED
{
	class SkeletonCache
	{
		struct actor_entry_data :
			stl::intrusive_ref_counted
		{
			SKMP_REDEFINE_NEW_PREF();

			actor_entry_data(
				bool        a_nativeLoader,
				const char* a_modelPath);

			stl::unordered_map<stl::fixed_string, NiTransform> data;
		};

	public:
		using KeyPathPair = std::pair<stl::fixed_string, std::string>;

		class ActorEntry
		{
			friend class SkeletonCache;

		public:
			ActorEntry() = default;
			ActorEntry(
				bool        a_nativeLoader,
				const char* a_modelPath);

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

		inline void EnableNativeLoader(bool a_switch) noexcept
		{
			m_useNativeLoader.store(a_switch, std::memory_order_relaxed);
		}

	private:
		SkeletonCache() = default;

		static KeyPathPair make_key(
			TESObjectREFR* a_refr,
			bool           a_firstPerson);

		ActorEntry get_or_create(
			const KeyPathPair& a_key);

		ActorEntry try_get(
			const stl::fixed_string& a_key) const;

		data_type                 m_data;
		mutable stl::shared_mutex m_lock;

		std::atomic_bool m_useNativeLoader{ false };

		static SkeletonCache m_Instance;
	};
}