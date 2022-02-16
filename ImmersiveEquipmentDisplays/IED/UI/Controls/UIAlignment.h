#pragma once

namespace IED
{
	namespace UI
	{
		class UIAlignment
		{
		protected:
			float GetNextTextOffset(const stl::fixed_string& a_text, bool a_clear = false);
			void  ClearTextOffset();
			bool  ButtonRight(const stl::fixed_string& a_text, bool a_disabled = false);

		private:
			float                                        m_posOffset = 0.0f;
			std::unordered_map<stl::fixed_string, float> m_ctlPositions;
		};

	}
}