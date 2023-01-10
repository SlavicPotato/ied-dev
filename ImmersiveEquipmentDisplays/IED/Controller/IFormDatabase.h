#pragma once

#include "IED/FormCommon.h"

namespace IED
{
	class IFormDatabase : public IFormCommon
	{
	public:
		inline static constexpr std::uint32_t EXTRA_TYPE_ARMOR = 0xFFFF;
		inline static constexpr std::uint32_t EXTRA_TYPE_LIGHT = 0xFFFE;

		struct entry_t
		{
			Game::FormID             formid;
			stl::flag<FormInfoFlags> flags;
			std::string              name;

			inline friend constexpr bool operator<(
				const entry_t& a_lhs,
				const entry_t& a_rhs) noexcept
			{
				return a_lhs.formid < a_rhs.formid;
			}

			inline friend constexpr bool operator==(
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

	protected:
		using form_db_get_func_t = std::function<void(result_type)>;

		result_type GetFormDatabase();

	private:
		template <class T, class Tf = T>
		constexpr static void Populate(
			container_type&        a_data,
			const RE::BSTArray<T>& a_fromData);

		template <class T, class Tf>
		constexpr static void Populate2(
			container_type&        a_data,
			const RE::BSTArray<T>& a_fromData,
			std::uint32_t          a_type,
			Tf                     a_func);

		static result_type Create();

		std::weak_ptr<container_type> m_data;
	};
}