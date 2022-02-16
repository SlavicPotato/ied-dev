#pragma once

namespace IED
{
	namespace UI
	{
		class UIBipedObjectSelectorWidget
		{
		public:
			static bool DrawBipedObjectSelector(const char* a_label, Biped::BIPED_OBJECT& a_data);

			[[nodiscard]] inline static constexpr const char* GetBipedSlotDesc(Biped::BIPED_OBJECT a_slot) noexcept
			{
				if (a_slot < stl::underlying(Biped::BIPED_OBJECT::kTotal))
				{
					return m_desc[a_slot];
				}
				else
				{
					return nullptr;
				}
			}

		private:
			using data_type = std::array<const char*, stl::underlying(Biped::BIPED_OBJECT::kTotal)>;

			static data_type m_desc;
		};
	}
}