#pragma once

namespace IED
{
	class SkeletonCache
	{
	public:
		using actor_entry_type       = std::shared_ptr<stl::unordered_map<stl::fixed_string, NiTransform>>;
		using const_actor_entry_type = std::shared_ptr<const stl::unordered_map<stl::fixed_string, NiTransform>>;
		using data_type              = stl::unordered_map<stl::fixed_string, actor_entry_type>;

		[[nodiscard]] inline static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		/*const Entry* GetNode(
			TESObjectREFR*           a_refr,
			const stl::fixed_string& a_name);*/

		const const_actor_entry_type Get(
			TESObjectREFR* a_refr,
			bool           a_firstPerson = false);

		[[nodiscard]] std::size_t GetSize() const noexcept;
		[[nodiscard]] std::size_t GetTotalEntries() const noexcept;

	private:
		SkeletonCache() = default;

		static stl::fixed_string mk_key(
			TESObjectREFR* a_refr,
			bool           a_firstPerson);

		const const_actor_entry_type get_or_create(
			const stl::fixed_string& a_key);

		static void fill(
			const stl::fixed_string&        a_key,
			actor_entry_type::element_type& a_entry);

		mutable boost::mutex m_lock;
		data_type            m_data;

		static SkeletonCache m_Instance;
	};
}