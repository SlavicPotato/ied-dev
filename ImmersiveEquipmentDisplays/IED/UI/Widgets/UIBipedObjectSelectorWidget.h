#pragma once

namespace IED
{
	namespace UI
	{
		class UIBipedObjectSelectorWidget
		{
		public:
			static bool DrawBipedObjectSelector(const char* a_label, Biped::BIPED_OBJECT& a_data);

			inline static constexpr const char* GetBipedSlotDesc(Biped::BIPED_OBJECT a_slot) noexcept
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
			static const char* m_desc[stl::underlying(Biped::BIPED_OBJECT::kTotal)];
		};
	}
}