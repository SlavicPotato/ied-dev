#pragma once

#include "Events/Dispatcher.h"
#include "ImGui/Styles/StylePreset.h"
#include "Input/Handlers.h"
#include "Render/Events.h"
#include "UI/InputHandler.h"
#include "UI/Tasks.h"

#include "Fonts/FontInfo.h"
#include "Fonts/IGlyphData.h"

namespace IED
{
	namespace Drivers
	{
		struct UIRect
		{
			float width{ 0.0f };
			float height{ 0.0f };
		};

		class UI :
			ILog,
			IGlyphData,
			UIInputHandler,
			::Events::EventSink<Handlers::KeyEvent>,
			::Events::EventSink<Events::D3D11CreateEventPost>,
			::Events::EventSink<Events::IDXGISwapChainPresent>
		{
			struct UIFontUpdateData
			{
				float                            scale{ 1.0f };
				stl::fixed_string                font;
				stl::optional<float>             fontsize;
				stl::flag<GlyphPresetFlags>      extraGlyphPresets{ GlyphPresetFlags::kNone };
				std::shared_ptr<fontGlyphData_t> langGlyphData;

				bool dirty{ false };
			};

			struct FontEntry
			{
				FontEntry(
					ImFont* a_font,
					float   a_default_size) :
					font(a_font),
					size(a_default_size)
				{
				}

				FontEntry(const FontEntry&) = delete;
				FontEntry(FontEntry&&)      = delete;

				FontEntry& operator=(const FontEntry&) = delete;
				FontEntry& operator=(FontEntry&&) = delete;

				ImFont*           font;
				float             size;
				ImVector<ImWchar> ranges;
			};

			using font_data_container = std::unordered_map<stl::fixed_string, FontEntry>;

			inline static constexpr auto DEFAULT_FONT_NAME = "Default";
			inline static constexpr auto DEFAULT_STYLE     = UIStylePreset::Dark;

		public:
			enum class UpdateFlags : std::uint8_t
			{
				kNone = 0,

				kResetInput = 1ui8 << 0,
				kStyle      = 1ui8 << 1,
				kStyleAlpha = 1ui8 << 2,

				kStyleMask = kStyle |
				             kStyleAlpha
			};

			static void Initialize();

			[[nodiscard]] inline static constexpr bool HasCallbacks() noexcept
			{
				return !m_Instance.m_drawTasks.empty();
			}

			[[nodiscard]] static bool AddTask(
				std::uint32_t            a_id,
				Tasks::UIRenderTaskBase* a_task);

			static void RemoveTask(std::uint32_t a_id);

			static void EvaluateTaskState();

			[[nodiscard]] inline static constexpr bool HasCallback(std::uint32_t a_id)
			{
				return m_Instance.m_drawTasks.contains(a_id);
			}

			[[nodiscard]] inline static constexpr const auto& GetBufferSize() noexcept
			{
				return m_Instance.m_info.bufferSize;
			}

			static void QueueResetInput() noexcept
			{
				IScopedLock lock(m_Instance.m_lock);
				m_Instance.m_updateFlags.set(UpdateFlags::kResetInput);
			}

			inline static void SetImGuiIni(const std::string& a_path)
			{
				m_Instance.m_conf.imgui_ini = a_path;
			}

			inline static void SetImGuiIni(std::string&& a_path)
			{
				m_Instance.m_conf.imgui_ini = std::move(a_path);
			}

			inline static void SetImGuiIni(const char* a_path)
			{
				m_Instance.m_conf.imgui_ini = a_path;
			}

			[[nodiscard]] inline static constexpr auto GetPerf() noexcept
			{
				return m_Instance.m_uiRenderPerf.current;
			}

			[[nodiscard]] inline static constexpr auto GetFrameCount() noexcept
			{
				return m_Instance.m_frameCount;
			}

			[[nodiscard]] inline static constexpr const auto& GetAvailableFonts() noexcept
			{
				return m_Instance.m_availableFonts;
			}

			[[nodiscard]] inline static constexpr const auto GetCurrentFont() noexcept
			{
				return m_Instance.m_currentFont;
			}

			static void SetStyle(UIStylePreset a_style) noexcept
			{
				IScopedLock lock(m_Instance.m_lock);
				m_Instance.m_conf.style = a_style;
			}

			static void SetReleaseFontData(bool a_switch) noexcept
			{
				IScopedLock lock(m_Instance.m_lock);
				m_Instance.m_conf.releaseFontData = a_switch;
			}

			static void SetAlpha(float a_value) noexcept
			{
				IScopedLock lock(m_Instance.m_lock);
				m_Instance.m_conf.alpha = a_value;
				m_Instance.m_updateFlags.set(UpdateFlags::kStyleAlpha);
			}

			static void SetBGAlpha(const stl::optional<float>& a_value) noexcept
			{
				IScopedLock lock(m_Instance.m_lock);
				m_Instance.m_conf.bgAlpha = a_value;
				m_Instance.m_updateFlags.set(UpdateFlags::kStyle);
			}

			static void QueueSetExtraGlyphs(GlyphPresetFlags a_flags);
			static void QueueSetLanguageGlyphData(const std::shared_ptr<fontGlyphData_t>& a_data);
			static void QueueFontChange(const stl::fixed_string& a_font);
			static void QueueSetFontSize(float a_size);
			static void QueueResetFontSize();

			static void UpdateAvailableFonts();

			static void QueueSetScale(float a_scale);
			static void MarkFontUpdateDataDirty();

			[[nodiscard]] static inline constexpr auto IsImInitialized() noexcept
			{
				return m_Instance.m_imInitialized;
			}

			FN_NAMEPROC("UI");

		private:
			UI() = default;

			void InitializeImpl();

			virtual void Receive(const Handlers::KeyEvent& a_evn) override;
			virtual void Receive(const Events::D3D11CreateEventPost& a_evn) override;
			virtual void Receive(const Events::IDXGISwapChainPresent& a_evn) override;

			static LRESULT CALLBACK WndProc_Hook(
				HWND   hWnd,
				UINT   uMsg,
				WPARAM wParam,
				LPARAM lParam);

			void EvaluateTaskStateImpl();

			void Suspend();

			void LockControls(bool a_switch);
			void FreezeTime(bool a_switch);

			void OnTaskRemove(Tasks::UIRenderTaskBase* a_task);

			bool SetCurrentFont(const stl::fixed_string& a_font);

			void QueueSetScaleImpl(float a_scale);
			void QueueSetExtraGlyphsImpl(GlyphPresetFlags a_flags);
			void QueueSetLanguageGlyphDataImpl(const std::shared_ptr<fontGlyphData_t>& a_data);
			void QueueFontChangeImpl(const stl::fixed_string& a_font);
			void QueueSetFontSizeImpl(float a_size);
			void QueueResetFontSizeImpl();

			void MarkFontUpdateDataDirtyImpl();
			bool UpdateFontData(bool a_force = false);
			void UpdateStyle();
			void UpdateStyleAlpha();

			bool LoadFonts(
				font_data_container&     a_data,
				const stl::fixed_string& a_font);

			bool LoadFontMetadata(fontInfoMap_t& a_out);

			void AddFontRanges(
				ImFontGlyphRangesBuilder& a_builder,
				const fontGlyphRange_t&   a_range);

			void AddFontRanges(
				ImFontGlyphRangesBuilder& a_builder,
				const fontGlyphData_t&    a_data);

			bool BuildFonts(
				const fontInfoMap_t&     a_in,
				font_data_container&     a_out,
				const stl::fixed_string& a_font);

			void UpdateAvailableFontsImpl(const fontInfoMap_t& a_data);

			WNDPROC m_pfnWndProc{ nullptr };

			struct
			{
				std::uint64_t lockCounter{ 0 };
				std::uint64_t freezeCounter{ 0 };

				stl::optional<bool> autoVanityAllowState;

				bool controlsLocked{ false };
				bool timeFrozen{ false };
			} m_state;

			struct
			{
				UIRect bufferSize;
			} m_info;

			struct
			{
				std::string          imgui_ini;
				UIStylePreset        style{ DEFAULT_STYLE };
				float                alpha{ 1.0f };
				stl::optional<float> bgAlpha;
				bool                 releaseFontData{ false };
			} m_conf;

			struct
			{
				PerfTimerInt timer{ 1000000LL };
				long long    current{ 0L };
			} m_uiRenderPerf;

			stl::map<std::uint32_t, Tasks::UIRenderTaskBase*> m_drawTasks;

			bool m_imInitialized{ false };
			bool m_suspended{ true };

			SKMP_ImGuiUserData m_ioUserData;
			UIFontUpdateData   m_fontUpdateData;

			font_data_container                    m_fontData;
			const font_data_container::value_type* m_currentFont{ nullptr };

			UIStylePreset m_currentStyle{ DEFAULT_STYLE };

			stl::set<stl::fixed_string> m_availableFonts;
			stl::fixed_string           m_sDefaultFont{ DEFAULT_FONT_NAME };

			std::uint64_t m_frameCount{ 0 };

			stl::flag<UpdateFlags> m_updateFlags{ UpdateFlags::kNone };

			WCriticalSection m_lock;

			static UI m_Instance;
		};

		DEFINE_ENUM_CLASS_BITWISE(UI::UpdateFlags);

	}
}