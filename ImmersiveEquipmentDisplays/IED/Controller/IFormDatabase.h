#pragma once

#include "IED/FormCommon.h"

namespace IED
{
	class IFormDatabase
	{
	public:
		static constexpr std::uint32_t EXTRA_TYPE_ARMOR = 0xFFFF;
		static constexpr std::uint32_t EXTRA_TYPE_LIGHT = 0xFFFE;

		struct entry_t
		{
			Game::FormID             formid;
			stl::flag<FormInfoFlags> flags;
			stl::fixed_string        name;

			[[nodiscard]] friend constexpr bool operator<(
				const entry_t& a_lhs,
				const entry_t& a_rhs) noexcept
			{
				return a_lhs.formid < a_rhs.formid;
			}

			[[nodiscard]] friend constexpr bool operator==(
				const entry_t& a_lhs,
				const entry_t& a_rhs) noexcept
			{
				return a_lhs.formid == a_rhs.formid;
			}
		};

		using entry_holder_type = stl::vector<entry_t>;

		using container_type = stl::unordered_map<std::uint32_t, entry_holder_type>;
		using value_type     = typename container_type::value_type;
		using result_type    = std::shared_ptr<container_type>;

		using form_db_get_func_t = std::function<void(result_type)>;

		[[nodiscard]] static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		void QueueGetFormDatabase(form_db_get_func_t a_func);

		result_type GetDatabase();

	private:
		static result_type Create();

		std::weak_ptr<container_type> m_data;
		mutable stl::mutex            m_lock;

		static IFormDatabase m_Instance;
	};
}