#pragma once

namespace IED
{
	namespace UI
	{
		class I3DIObject;
		struct I3DICommonData;

		class I3DIObjectController
		{
		public:
			struct Entry
			{
				inline constexpr Entry(I3DIObject* a_object) :
					object(a_object)
				{
				}

				std::optional<float> dist;
				I3DIObject*          object;
			};

			[[nodiscard]] inline constexpr auto& GetData() const noexcept
			{
				return m_data;
			}

			[[nodiscard]] inline constexpr auto& GetData() noexcept
			{
				return m_data;
			}

			void Update(I3DICommonData& a_data);

		protected:
			I3DIObject* XM_CALLCONV GetHovered(
				DirectX::XMVECTOR a_rayOrigin,
				DirectX::XMVECTOR a_rayDir);

			I3DIObject* m_hovered{ nullptr };
			I3DIObject* m_selected{ nullptr };

			stl::vector<Entry> m_data;
		};

	}
}