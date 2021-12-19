#pragma once

#include "WindowLayoutData.h"
#include "UIWindowBase.h"

namespace IED
{
	namespace UI
	{
		class UIWindow : 
			public UIWindowBase
		{
		protected:
			void SetWindowDimensions(
				float a_offsetX = 0.0f,
				float a_sizeX = -1.0f,
				float a_sizeY = -1.0f,
				bool a_centered = false);

			inline void SetWindowDimensions(const WindowLayoutData& a_data)
			{
				SetWindowDimensions(
					a_data.m_offset,
					a_data.m_width,
					a_data.m_height,
					a_data.m_centered);
			}

			bool CanClip() const;

		private:
			struct
			{
				bool initialized{ false };
				ImVec2 sizeMin;
				ImVec2 sizeMax;
				ImVec2 pos;
				ImVec2 pivot;
				ImVec2 size;
			} m_sizeData;
		};

	}
}