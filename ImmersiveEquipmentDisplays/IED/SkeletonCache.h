#pragma once

namespace IED
{
	class SkeletonCache
	{
	public:
		class ActorEntry;

		SkeletonCache(const SkeletonCache&)            = delete;
		SkeletonCache& operator=(const SkeletonCache&) = delete;

	private:
		struct actor_entry_data :
			stl::intrusive_ref_counted
		{
			friend class ActorEntry;

		public:
			SKMP_REDEFINE_NEW_PREF();

			actor_entry_data(
				const char* a_modelPath);

			actor_entry_data(
				NiAVObject* a_root);

		private:
			void make_node_entries(NiAVObject* a_root);

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
				const char* a_modelPath);

			ActorEntry(NiAVObject* a_root);

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

			[[nodiscard]] constexpr const auto& operator*() const noexcept
			{
				return ptr->data;
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
			Actor* a_refr,
			bool   a_firstPerson = false);

	private:
		void MakeFrom(
			Actor*      a_refr,
			NiAVObject* a_root,
			bool        a_firstPerson = false);

	public:
		[[nodiscard]] std::size_t GetSize() const;
		[[nodiscard]] std::size_t GetTotalEntries() const;

		constexpr void EnableMakeOnLoad(bool a_switch) noexcept
		{
			m_makeOnLoad = a_switch;
		}

		void OnLoad3D(Actor* a_actor, NiAVObject* a_root, bool a_firstPerson);

	private:
		SkeletonCache() = default;

		static KeyPathPair make_key(
			Actor* a_refr,
			bool   a_firstPerson);

		ActorEntry get_or_create(
			const KeyPathPair& a_key);

		ActorEntry try_get(
			const stl::fixed_string& a_key) const;

		bool has(const stl::fixed_string& a_key) const;

		data_type                 m_data;
		mutable stl::shared_mutex m_lock;

		bool             m_makeOnLoad{ true };

		static SkeletonCache m_Instance;
	};
}