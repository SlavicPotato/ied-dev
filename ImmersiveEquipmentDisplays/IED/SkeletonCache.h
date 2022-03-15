#pragma once

namespace IED
{
	// third-person 3d only
	class SkeletonCache
	{
		struct Entry
		{
			NiTransform transform;
		};

		using actor_entry_type = std::unordered_map<stl::fixed_string, Entry>;
		using data_type        = std::unordered_map<stl::fixed_string, actor_entry_type>;

	public:
		[[nodiscard]] inline static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		const Entry* GetNode(
			TESObjectREFR*           a_refr,
			const stl::fixed_string& a_name);

		[[nodiscard]] inline auto GetSize() const noexcept
		{
			return m_data.size();
		}

	private:
		static stl::fixed_string mk_key(TESObjectREFR* a_refr);

		data_type::const_iterator get_or_create(const stl::fixed_string& a_key);
		void                      fill(const stl::fixed_string& a_key, data_type::iterator a_it);

		data_type m_data;

		static SkeletonCache m_Instance;
	};
}