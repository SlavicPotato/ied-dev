#pragma once

namespace IED
{
	namespace UI
	{
		struct I3DICommonData;

		class I3DIPopupWindow
		{
		public:
			inline static constexpr auto WINDOW_FLAGS = ImGuiWindowFlags_NoFocusOnAppearing |
			                                            ImGuiWindowFlags_NoInputs |
			                                            ImGuiWindowFlags_NoMove |
			                                            ImGuiWindowFlags_AlwaysAutoResize |
			                                            ImGuiWindowFlags_NoCollapse |
			                                            ImGuiWindowFlags_NoDecoration |
			                                            ImGuiWindowFlags_NoTitleBar |
			                                            ImGuiWindowFlags_NoSavedSettings;

			inline constexpr void SetPosition(
				const ImVec2& a_pos,
				const ImVec2& a_pivot = ImVec2()) noexcept
			{
				m_pos   = a_pos;
				m_pivot = a_pivot;
			}

			inline constexpr void XM_CALLCONV SetPosition(
				DirectX::XMVECTOR a_pos,
				const ImVec2&     a_pivot = ImVec2()) noexcept
			{
				m_pos = {
					DirectX::XMVectorGetX(a_pos),
					DirectX::XMVectorGetY(a_pos)
				};

				m_pivot = a_pivot;
			}

			inline constexpr void XM_CALLCONV SetWorldOrigin(
				DirectX::XMVECTOR a_origin) noexcept
			{
			}

			inline constexpr void XM_CALLCONV SetLineWorldOrigin(
				DirectX::XMVECTOR a_origin) noexcept
			{
				m_worldOrigin = a_origin;
			}

			inline constexpr void XM_CALLCONV ClearLineWorldOrigin(
				DirectX::XMVECTOR a_origin) noexcept
			{
				m_worldOrigin.reset();
			}

			using func_type = std::function<void()>;

			void Draw(const char* a_id, I3DICommonData& a_data, func_type a_func);

		protected:
			void ApplyPosition(I3DICommonData& a_data);

		private:
			ImVec2 m_pos;
			ImVec2 m_pivot;

			std::optional<DirectX::XMVECTOR> m_worldOrigin;
		};
	}
}