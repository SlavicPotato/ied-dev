#pragma once

#include "ImGui/Styles/StylePreset.h"
#include "Input/Handlers.h"
#include "Render/Events.h"
#include "UI/InputHandler.h"
#include "UI/StyleProfileManager.h"
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

		namespace concepts
		{
			template <class T>
			concept is_render_task =
				std::is_convertible_v<
					T,
					stl::smart_ptr<Tasks::UIRenderTaskBase>>;
		}

		class UI :
			ILog,
			UIInputHandler,
			::Events::EventSink<Handlers::KeyEvent>,
			::Events::EventSink<Handlers::MouseMoveEvent>,
			::Events::EventSink<Events::D3D11CreateEventPost>,
			::Events::EventSink<Events::IDXGISwapChainPresent>,
			TaskDelegateFixed
		{
			struct UIFontUpdateData
			{
				float                         scale{ 1.0f };
				stl::fixed_string             font;
				std::optional<float>          fontsize;
				stl::flag<GlyphPresetFlags>   extraGlyphPresets{ GlyphPresetFlags::kNone };
				stl::smart_ptr<FontGlyphData> langGlyphData;

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

				FontEntry(const FontEntry&)            = delete;
				FontEntry& operator=(const FontEntry&) = delete;

				ImFont*           font;
				float             size;
				ImVector<ImWchar> ranges;
			};

			using font_data_container = stl::unordered_map<stl::fixed_string, FontEntry>;

			static constexpr auto DEFAULT_FONT_NAME = "Default";
			static constexpr auto DEFAULT_STYLE     = UIStylePreset::Dark;

		public:
			enum class UpdateFlags : std::uint8_t
			{
				kNone = 0,

				kResetInput        = 1ui8 << 0,
				kStyle             = 1ui8 << 1,
				kStyleAlpha        = 1ui8 << 2,
				kEvaluateTaskState = 1ui8 << 3,

				kStyleMask = kStyle |
				             kStyleAlpha
			};

			static void Initialize();

			[[nodiscard]] inline static bool HasCallbacks() noexcept
			{
				return !m_Instance.m_drawTasks.empty();
			}

			[[nodiscard]] static stl::smart_ptr<Tasks::UIRenderTaskBase> GetTask(std::int32_t a_id);

			static void QueueRemoveTask(std::int32_t a_id);

			static void EvaluateTaskState();

			[[nodiscard]] inline static bool HasCallback(std::int32_t a_id)
			{
				return m_Instance.m_drawTasks.contains(a_id);
			}

			[[nodiscard]] static constexpr const auto& GetBufferSize() noexcept
			{
				return m_Instance.m_info.bufferSize;
			}

			static void QueueResetInput() noexcept
			{
				const stl::lock_guard lock(m_Instance.m_lock);
				m_Instance.m_updateFlags.set(UpdateFlags::kResetInput);
			}

			static constexpr void SetImGuiIni(const std::string& a_path)
			{
				m_Instance.m_conf.imgui_ini = a_path;
			}

			static constexpr void SetImGuiIni(std::string&& a_path)
			{
				m_Instance.m_conf.imgui_ini = std::move(a_path);
			}

			static constexpr void SetImGuiIni(const char* a_path)
			{
				m_Instance.m_conf.imgui_ini = a_path;
			}

			[[nodiscard]] static constexpr auto GetPerf() noexcept
			{
				return m_Instance.m_uiRenderPerf.current;
			}

			[[nodiscard]] static constexpr auto GetFrameCount() noexcept
			{
				return m_Instance.m_frameCount;
			}

			[[nodiscard]] static constexpr const auto& GetAvailableFonts() noexcept
			{
				return m_Instance.m_availableFonts;
			}

			[[nodiscard]] static constexpr const auto GetCurrentFont() noexcept
			{
				return m_Instance.m_currentFont;
			}

			[[nodiscard]] static constexpr const auto& GetCurrentStyle() noexcept
			{
				return m_Instance.m_currentStyle;
			}

			[[nodiscard]] static constexpr const auto& GetStyleData() noexcept
			{
				return m_Instance.m_styleProfileManager.Data();
			}

			[[nodiscard]] static constexpr auto& GetStyleProfileManager() noexcept
			{
				return m_Instance.m_styleProfileManager;
			}

			static void SetStyle(const stl::fixed_string& a_style, bool a_forceUpdate = false) noexcept
			{
				const stl::lock_guard lock(m_Instance.m_lock);

				if (a_style != m_Instance.m_conf.style)
				{
					m_Instance.m_conf.style = a_style;
				}

				if (a_forceUpdate)
				{
					m_Instance.m_updateFlags.set(UpdateFlags::kStyle);
				}
			}

			static void SetReleaseFontData(bool a_switch) noexcept
			{
				const stl::lock_guard lock(m_Instance.m_lock);
				m_Instance.m_conf.releaseFontData = a_switch;
			}

			static void SetAlpha(float a_value) noexcept
			{
				const stl::lock_guard lock(m_Instance.m_lock);
				m_Instance.m_conf.alpha = a_value;
				m_Instance.m_updateFlags.set(UpdateFlags::kStyleAlpha);
			}

			static void SetBGAlpha(const stl::optional<float>& a_value) noexcept
			{
				const stl::lock_guard lock(m_Instance.m_lock);
				m_Instance.m_conf.bgAlpha = a_value;
				m_Instance.m_updateFlags.set(UpdateFlags::kStyle);
			}

			static void QueueSetExtraGlyphs(GlyphPresetFlags a_flags);
			static void QueueSetLanguageGlyphData(const stl::smart_ptr<FontGlyphData>& a_data);
			static void QueueFontChange(const stl::fixed_string& a_font);
			static void QueueSetFontSize(float a_size);
			static void QueueResetFontSize();

			static void UpdateAvailableFonts();

			static void QueueSetScale(float a_scale);
			static void MarkFontUpdateDataDirty();

			[[nodiscard]] static inline auto IsImInitialized() noexcept
			{
				return m_Instance.m_imInitialized.load();
			}

			[[nodiscard]] static constexpr auto GetWindowHandle() noexcept
			{
				return m_Instance.m_info.hWnd;
			}

			[[nodiscard]] static constexpr bool IsImInputBlocked() noexcept
			{
				return static_cast<bool>(m_Instance.m_state.blockInputImGuiCounter);
			}

			static void QueueImGuiSettingsSave();

			static bool AddTask(
				std::int32_t                                   a_id,
				const stl::smart_ptr<Tasks::UIRenderTaskBase>& a_task);

			static inline auto GetScopedLock()
			{
				return stl::lock_guard(m_Instance.m_lock);
			}

			FN_NAMEPROC("UI");

		private:
			UI() = default;

			void InitializeImpl();

			virtual void Receive(const Handlers::KeyEvent& a_evn) override;
			virtual void Receive(const Handlers::MouseMoveEvent& a_evn) override;
			virtual void Receive(const Events::D3D11CreateEventPost& a_evn) override;
			virtual void Receive(const Events::IDXGISwapChainPresent& a_evn) override;

			virtual void Run() override;

			void RunPreps();

			static LRESULT CALLBACK WndProc_Hook(
				HWND   hWnd,
				UINT   uMsg,
				WPARAM wParam,
				LPARAM lParam);

			void EvaluateTaskStateImpl();
			void ApplyControlLockChanges() const;

			void Suspend();

			bool AddTaskImpl(
				std::int32_t                                   a_id,
				const stl::smart_ptr<Tasks::UIRenderTaskBase>& a_task);

			void LockControls(bool a_switch);
			void FreezeTime(bool a_switch);

			void OnTaskAdd(Tasks::UIRenderTaskBase* a_task);
			void OnTaskRemove(Tasks::UIRenderTaskBase* a_task);

			bool SetCurrentFont(const stl::fixed_string& a_font);

			void QueueSetScaleImpl(float a_scale);
			void QueueSetExtraGlyphsImpl(GlyphPresetFlags a_flags);
			void QueueSetLanguageGlyphDataImpl(const stl::smart_ptr<FontGlyphData>& a_data);
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

			std::unique_ptr<FontInfoMap> MakeFontMetadata();
			std::unique_ptr<FontInfoMap> LoadFontMetadata(const fs::path& a_path);

			void AddFontRanges(
				ImFontGlyphRangesBuilder& a_builder,
				const fontGlyphRange_t&   a_range);

			void AddFontRanges(
				ImFontGlyphRangesBuilder&            a_builder,
				const stl::smart_ptr<FontGlyphData>& a_data);

			bool BuildFonts(
				const FontInfoMap&       a_in,
				font_data_container&     a_out,
				const stl::fixed_string& a_font);

			std::optional<fs::path> GetFontPath(const FontInfoEntry& a_in);

			void UpdateAvailableFontsImpl(const std::unique_ptr<FontInfoMap>& a_data);

			WNDPROC m_pfnWndProc{ nullptr };

			struct
			{
				std::uint64_t lockCounter{ 0 };
				std::uint64_t freezeCounter{ 0 };
				std::uint64_t wantCursorCounter{ 0 };
				std::uint64_t blockCursorCounter{ 0 };
				std::uint64_t blockInputImGuiCounter{ 0 };

				std::optional<bool> autoVanityAllowState;

				bool controlsLocked{ false };
				bool timeFrozen{ false };
			} m_state;

			struct
			{
				HWND   hWnd{ nullptr };
				UIRect bufferSize;
			} m_info;

			struct
			{
				std::string          imgui_ini;
				stl::fixed_string    style;
				float                alpha{ 1.0f };
				stl::optional<float> bgAlpha;
				bool                 releaseFontData{ false };
			} m_conf;

			struct
			{
				PerfTimerInt timer{ 1000000LL };
				long long    current{ 0L };
			} m_uiRenderPerf;

			using map_type = stl::map<
				std::int32_t,
				stl::smart_ptr<Tasks::UIRenderTaskBase>>;

			map_type m_drawTasks;

			stl::unordered_map<
				map_type::key_type,
				map_type::mapped_type>
				m_addQueue;

			std::atomic<bool> m_imInitialized{ false };
			bool              m_drawing{ false };
			std::atomic<bool> m_suspended{ true };

			SKMP_ImGuiUserData m_ioUserData;
			UIFontUpdateData   m_fontUpdateData;

			font_data_container                    m_fontData;
			const font_data_container::value_type* m_currentFont{ nullptr };

			stl::fixed_string m_currentStyle;

			stl::set<stl::fixed_string, stl::fixed_string::less_str> m_availableFonts;
			stl::fixed_string                                        m_sDefaultFont{ DEFAULT_FONT_NAME };

			std::uint64_t m_frameCount{ 0 };

			stl::flag<UpdateFlags> m_updateFlags{ UpdateFlags::kNone };

			StyleProfileManager m_styleProfileManager;

			mutable stl::recursive_mutex m_lock;

			static UI m_Instance;
		};

		DEFINE_ENUM_CLASS_BITWISE(UI::UpdateFlags);

	}
}