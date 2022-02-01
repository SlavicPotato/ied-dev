#pragma once

#include "Events/Dispatcher.h"
#include "ImGUI/MouseEventQueue.h"
#include "Input/Handlers.h"
#include "Render/Events.h"
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
			::Events::EventSink<Handlers::KeyEvent>,
			::Events::EventSink<Events::D3D11CreateEventPost>,
			::Events::EventSink<Events::IDXGISwapChainPresent>
		{
			enum class UIInputEventType : std::uint32_t
			{
				MouseButton,
				MouseWheel,
				Keyboard
			};

			struct wchars_t
			{
				WCHAR b[4];
				int n{ 0 };
			};

			class KeyEventTaskPress
			{
			public:
				KeyEventTaskPress(
					UINT a_uval,
					const wchars_t& a_chars) :
					m_uval(a_uval),
					m_chars(a_chars)
				{
				}

				void Run();

			private:
				UINT m_uval;
				wchars_t m_chars;
			};

			class KeyEventTaskRelease
			{
			public:
				KeyEventTaskRelease(
					UINT a_uval) :
					m_uval(a_uval)
				{
				}

				void Run();

			private:
				UINT m_uval;
			};

			struct UIFontUpdateData
			{
				float scale{ 1.0f };
				stl::fixed_string font;
				stl::optional<float> fontsize;
				stl::flag<GlyphPresetFlags> extraGlyphPresets{ GlyphPresetFlags::kNone };
				std::shared_ptr<fontGlyphData_t> langGlyphData;

				bool dirty{ false };
			};

			struct FontEntry
			{
				FontEntry(
					ImFont* a_font,
					float a_default_size) :
					font(a_font),
					size(a_default_size)
				{
				}

				FontEntry(const FontEntry&) = delete;
				FontEntry(FontEntry&&) = delete;

				FontEntry& operator=(const FontEntry&) = delete;
				FontEntry& operator=(FontEntry&&) = delete;

				ImFont* font;
				float size;
				ImVector<ImWchar> ranges;
			};

			using font_data_container = std::unordered_map<stl::fixed_string, FontEntry>;

			inline static constexpr auto DEFAULT_FONT_NAME = "Default";

		public:
			static void Initialize();

			inline static constexpr bool HasCallbacks() noexcept
			{
				return !m_Instance.m_drawTasks.empty();
			}

			[[nodiscard]] static bool AddTask(
				std::uint32_t a_id,
				Tasks::UIRenderTaskBase* a_task);

			static void RemoveTask(std::uint32_t a_id);

			inline static constexpr bool HasCallback(std::uint32_t a_id)
			{
				return m_Instance.m_drawTasks.contains(a_id);
			}

			inline constexpr static const UIRect& GetBufferSize() noexcept
			{
				return m_Instance.m_info.bufferSize;
			}

			void ResetImGuiIO();

			inline static void QueueResetIO() noexcept
			{
				IScopedLock lock(m_Instance.m_lock);
				m_Instance.m_nextResetIO = true;
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

			inline static constexpr auto GetPerf() noexcept
			{
				return m_Instance.m_uiRenderPerf.current;
			}

			inline static constexpr auto GetFrameCount() noexcept
			{
				return m_Instance.m_frameCount;
			}

			/*inline static void EnableScaling(bool a_switch) noexcept
			{
				m_Instance.m_scalingEnabled = a_switch;
			}*/

			inline static void SetCurrentFont(font_data_container::const_iterator a_it) noexcept
			{
				m_Instance.m_currentFont = std::addressof(*a_it);
			}

			inline static constexpr const auto& GetAvailableFonts() noexcept
			{
				return m_Instance.m_availableFonts;
			}

			inline static constexpr const auto GetCurrentFont() noexcept
			{
				return m_Instance.m_currentFont;
			}

			static void QueueSetExtraGlyphs(GlyphPresetFlags a_flags);
			static void QueueSetLanguageGlyphData(const std::shared_ptr<fontGlyphData_t>& a_data);
			static void QueueFontChange(const stl::fixed_string& a_font);
			static void QueueSetFontSize(float a_size);
			static void QueueResetFontSize();

			static void UpdateAvailableFonts();

			static void QueueSetScale(float a_scale);
			static void MarkFontUpdateDataDirty();

			static inline constexpr auto IsImInitialized() noexcept
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
				HWND hWnd,
				UINT uMsg,
				WPARAM wParam,
				LPARAM lParam);

			void Suspend();

			void LockControls(bool a_switch);
			void FreezeTime(bool a_switch);

			void OnTaskRemove(Tasks::UIRenderTaskBase* a_task);

			inline constexpr auto& GetKeyPressQueue() noexcept
			{
				return m_keyPressQueue;
			}

			inline constexpr auto& GetKeyReleaseQueue() noexcept
			{
				return m_keyReleaseQueue;
			}

			inline constexpr auto& GetMousePressEventQueue() noexcept
			{
				return m_mousePressEventQueue;
			}

			inline constexpr auto& GetMouseReleaseEventQueue() noexcept
			{
				return m_mouseReleaseEventQueue;
			}

			bool SetCurrentFont(const stl::fixed_string& a_font);

			void QueueSetScaleImpl(float a_scale);
			void QueueSetExtraGlyphsImpl(GlyphPresetFlags a_flags);
			void QueueSetLanguageGlyphDataImpl(const std::shared_ptr<fontGlyphData_t>& a_data);
			void QueueFontChangeImpl(const stl::fixed_string& a_font);
			void QueueSetFontSizeImpl(float a_size);
			void QueueResetFontSizeImpl();

			void MarkFontUpdateDataDirtyImpl();
			void UpdateFontData(bool a_force = false);

			bool LoadFonts(
				font_data_container& a_data,
				const stl::fixed_string& a_font);
			bool LoadFontMetadata(fontInfoMap_t& a_out);

			void AddFontRanges(
				ImFontGlyphRangesBuilder& a_builder,
				const fontGlyphRange_t& a_range);

			void AddFontRanges(
				ImFontGlyphRangesBuilder& a_builder,
				const fontGlyphData_t& a_data);

			bool BuildFonts(
				const fontInfoMap_t& a_in,
				font_data_container& a_out,
				const stl::fixed_string& a_font);

			void UpdateAvailableFontsImpl(const fontInfoMap_t& a_data);

			WNDPROC m_pfnWndProc{ nullptr };

			struct
			{
				std::uint32_t lockCounter{ 0 };
				std::uint32_t freezeCounter{ 0 };

				bool controlsLocked{ false };
				stl::optional<bool> autoVanityAllowState;
				bool timeFrozen{ false };
			} m_state;

			struct
			{
				UIRect bufferSize;
			} m_info;

			struct
			{
				std::string imgui_ini;
			} m_conf;

			struct
			{
				PerfTimerInt timer{ 1000000LL };
				long long current{ 0L };
			} m_uiRenderPerf;

			stl::map<std::uint32_t, Tasks::UIRenderTaskBase*> m_drawTasks;

			bool m_imInitialized{ false };
			bool m_suspended{ true };
			HWND m_windowHandle{ nullptr };

			IOUserData m_ioUserData;
			UIFontUpdateData m_fontUpdateData;

			TaskQueueStatic<KeyEventTaskPress> m_keyPressQueue;
			TaskQueueStatic<KeyEventTaskRelease> m_keyReleaseQueue;
			ImGuiMouseEventQueue m_mousePressEventQueue;
			ImGuiMouseEventQueue m_mouseReleaseEventQueue;

			font_data_container m_fontData;
			const font_data_container::value_type* m_currentFont{ nullptr };

			stl::set<stl::fixed_string> m_availableFonts;
			stl::fixed_string m_sDefaultFont{ DEFAULT_FONT_NAME };

			std::uint64_t m_frameCount{ 0 };

			bool m_nextResetIO{ false };

			BYTE m_keyState[256]{ 0 };

			WCriticalSection m_lock;

			static UI m_Instance;
		};
	}  // namespace Drivers
}  // namespace IED