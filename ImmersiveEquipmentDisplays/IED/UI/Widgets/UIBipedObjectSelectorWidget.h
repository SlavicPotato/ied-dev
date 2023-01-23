#pragma once

namespace IED
{
	namespace UI
	{
		class UIBipedObjectSelectorWidget
		{
		public:
			static bool DrawBipedObjectSelector(const char* a_label, BIPED_OBJECT& a_data, bool a_allowNone = false);

			[[nodiscard]] static constexpr const char* GetBipedSlotDesc(BIPED_OBJECT a_slot) noexcept
			{
				return a_slot < BIPED_OBJECT::kSpecTotal ? m_desc[stl::underlying(a_slot)] : nullptr;
			}

		private:
			using data_type = std::array<const char*, stl::underlying(BIPED_OBJECT::kSpecTotal)>;

			static data_type m_desc;
		};
	}
}