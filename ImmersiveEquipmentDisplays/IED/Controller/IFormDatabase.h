#pragma once

#include "../FormCommon.h"

namespace IED
{
	class IFormDatabase : public IFormCommon
	{
	public:
		inline static std::uint32_t constexpr EXTRA_TYPE_ARMOR = 0xFFFF;

		struct entry_t
		{
			Game::FormID             formid;
			stl::flag<FormInfoFlags> flags{ FormInfoFlags::kNone };
			std::string              name;

			inline friend bool operator<(
				const entry_t& a_lhs,
				const entry_t& a_rhs) noexcept
			{
				return a_lhs.formid < a_rhs.formid;
			}

			inline friend bool operator==(
				const entry_t& a_lhs,
				const entry_t& a_rhs) noexcept
			{
				return a_lhs.formid == a_rhs.formid;
			}
		};

		using entry_holder_type = std::vector<entry_t>;

		using data_type   = std::unordered_map<std::uint32_t, entry_holder_type>;
		using value_type  = typename data_type::value_type;
		using result_type = std::shared_ptr<data_type>;

	protected:
		using form_db_get_func_t = std::function<void(result_type)>;

		result_type GetFormDatabase();

	private:
		template <class T, class Tf = T>
		constexpr static void Populate(
			data_type&       a_data,
			const tArray<T>& a_fromData);

		template <class T, class Tf>
		constexpr static void Populate2(
			data_type&       a_data,
			const tArray<T>& a_fromData,
			std::uint32_t    a_type,
			Tf               a_func);

		static result_type Create();

		std::weak_ptr<data_type> m_data;
	};
}  // namespace IED