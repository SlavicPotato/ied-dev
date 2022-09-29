#include "pch.h"

#include "Input.h"
#include "Render.h"
#include "UI.h"

#include "IED/Parsers/JSONFontInfoMapParser.h"

#define SPECTRUM_USE_DARK_THEME

#include "ImGui/Styles/all.h"

namespace IED
{
	namespace Drivers
	{
		using namespace Handlers;
		using namespace Tasks;
		using namespace Events;

		UI UI::m_Instance;

		void UI::Initialize()
		{
			m_Instance.InitializeImpl();
		}

		void UI::InitializeImpl()
		{
			Input::RegisterForPriorityKeyEvents(this);
#if defined(IED_ENABLE_I3DI)
			Input::RegisterForPriorityMouseMoveEvents(this);
#endif

			auto& r = Render::GetSingleton();

			r.GetEventDispatcher<Events::D3D11CreateEventPost>().AddSink(this);
			r.GetEventDispatcher<Events::IDXGISwapChainPresent>().AddSink(this);
#if defined(IED_ENABLE_I3DI)
			r.GetEventDispatcher<Events::PrepareGameDataEvent>().AddSink(this);
#endif
		}

		void UI::Receive(const D3D11CreateEventPost& a_evn)
		{
			ASSERT(a_evn.m_pSwapChainDesc->OutputWindow != nullptr);

			stl::scoped_lock lock(m_lock);

			m_info.bufferSize = {
				static_cast<float>(a_evn.m_pSwapChainDesc->BufferDesc.Width),
				static_cast<float>(a_evn.m_pSwapChainDesc->BufferDesc.Height)
			};

			m_info.hWnd = a_evn.m_pSwapChainDesc->OutputWindow;

			RECT rect{};
			if (::GetClientRect(
					a_evn.m_pSwapChainDesc->OutputWindow,
					std::addressof(rect)) == TRUE)
			{
				m_ioUserData.btsRatio = {
					static_cast<float>(a_evn.m_pSwapChainDesc->BufferDesc.Width) /
						static_cast<float>(rect.right),
					static_cast<float>(a_evn.m_pSwapChainDesc->BufferDesc.Height) /
						static_cast<float>(rect.bottom)
				};
			}
			else
			{
				m_ioUserData.btsRatio = { 1.0f, 1.0f };
			}

			ASSERT(IMGUI_CHECKVERSION());
			ImGui::CreateContext();

			auto& io                             = ImGui::GetIO();
			io.MouseDrawCursor                   = true;
			io.WantSetMousePos                   = true;
			io.ConfigWindowsMoveFromTitleBarOnly = true;
			io.DisplaySize                       = { m_info.bufferSize.width, m_info.bufferSize.height };
			io.MousePos                          = { io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f };
			io.UserData                          = static_cast<void*>(std::addressof(m_ioUserData));
			io.IniFilename                       = !m_conf.imgui_ini.empty() ? m_conf.imgui_ini.c_str() : nullptr;

			ImGui::StyleColorsDark();

			if (!::ImGui_ImplWin32_Init(
					a_evn.m_pSwapChainDesc->OutputWindow))
			{
				Error("ImGui initialization failed (Win32)");
				return;
			}

			if (!LoadFonts(m_fontData, m_fontUpdateData.font))
			{
				Error("UI unavailable due to missing font atlas");
				return;
			}

			ASSERT(!m_fontData.empty());

			SetCurrentFont(m_sDefaultFont);
			UpdateStyleAlpha();

			if (!::ImGui_ImplDX11_Init(
					a_evn.m_pDevice,
					a_evn.m_pImmediateContext))
			{
				Error("ImGui initialization failed (DX11)");
				return;
			}

			m_imInitialized = true;

			Message("ImGui initialized");

			m_pfnWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtrA(
				a_evn.m_pSwapChainDesc->OutputWindow,
				GWLP_WNDPROC,
				reinterpret_cast<LONG_PTR>(WndProc_Hook)));

			if (!m_pfnWndProc)
			{
				Warning(
					"[0x%llX] SetWindowLongPtrA failed",
					a_evn.m_pSwapChainDesc->OutputWindow);
			}
		}

		void UI::Receive(const IDXGISwapChainPresent& a_evn)
		{
			if (m_suspended.load(std::memory_order_relaxed))
			{
				return;
			}

			stl::scoped_lock lock(m_lock);

			if (!m_imInitialized)
			{
				return;
			}

			m_uiRenderPerf.timer.Begin();

			if (m_updateFlags.consume(UpdateFlags::kResetInput))
			{
				ResetInput();
			}

			UpdateFontData();

			if (m_conf.releaseFontData)
			{
				if (m_frameCount && !::ImGui_ImplDX11_HasFontSampler())
				{
					::ImGui_ImplDX11_ReCreateFontsTexture();
				}
			}

			if (m_currentStyle != m_conf.style ||
			    m_updateFlags.test(UpdateFlags::kStyle))
			{
				m_updateFlags.clear(UpdateFlags::kStyleMask);
				m_currentStyle = m_conf.style;
				UpdateStyle();
			}

			if (m_updateFlags.consume(UpdateFlags::kStyleAlpha))
			{
				UpdateStyleAlpha();
			}

			ProcessPressQueue();

			::ImGui_ImplDX11_NewFrame();
			::ImGui_ImplWin32_NewFrame();

			ImGui::NewFrame();

			ImGui::PushFont(m_currentFont->second.font);

			for (auto it = m_drawTasks.begin();
			     it != m_drawTasks.end();)
			{
				ImGui::PushID(it->first);
				bool res = it->second->Run();
				ImGui::PopID();

				if (!res || it->second->m_stopMe)
				{
					OnTaskRemove(it->second.get());
					it = m_drawTasks.erase(it);
				}
				else
				{
					++it;
				}
			}

			ImGui::PopFont();

			for (auto& e : m_drawTasks)
			{
				e.second->Render();
			}

			ImGui::Render();
			::ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			m_frameCount++;

			if (m_drawTasks.empty())
			{
				Suspend();

				if (m_conf.releaseFontData)
				{
					::ImGui_ImplDX11_ReleaseFontsTexture();
				}
			}
			else
			{
				ProcessReleaseQueue();
			}

			m_uiRenderPerf.timer.End(m_uiRenderPerf.current);
		}

		void UI::Receive(const Events::PrepareGameDataEvent&)
		{
			if (m_suspended.load(std::memory_order_relaxed))
			{
				return;
			}

			// *should* never be true
			if (Game::IsPaused())
			{
				return;
			}

			stl::scoped_lock lock(m_lock);

			if (!m_imInitialized)
			{
				return;
			}

			for (auto& e : m_drawTasks)
			{
				e.second->PrepareGameData();
			}
		}

		// invokes preps when paused
		void UI::Run()
		{
			if (m_suspended.load(std::memory_order_relaxed))
			{
				return;
			}

			if (!Game::IsPaused())
			{
				return;
			}

			stl::scoped_lock lock(m_lock);

			if (!m_imInitialized)
			{
				return;
			}

			for (auto& e : m_drawTasks)
			{
				e.second->PrepareGameData();
			}
		}

		LRESULT CALLBACK UI::WndProc_Hook(
			HWND   hWnd,
			UINT   uMsg,
			WPARAM wParam,
			LPARAM lParam)
		{
			auto result = ::CallWindowProcA(
				m_Instance.m_pfnWndProc,
				hWnd,
				uMsg,
				wParam,
				lParam);

			if (uMsg == WM_KILLFOCUS)
			{
				QueueResetInput();
			}

			return result;
		}

		void UI::Receive(const Handlers::KeyEvent& a_evn)
		{
			if (!m_Instance.m_suspended.load(std::memory_order_relaxed))
			{
				ProcessEvent(a_evn);
			}
		}

		void UI::Receive(const Handlers::MouseMoveEvent& a_evn)
		{
			if (m_Instance.m_suspended.load(std::memory_order_relaxed))
			{
				return;
			}

			if (Game::IsPaused())
			{
				return;
			}

			stl::scoped_lock lock(m_lock);

			if (!m_imInitialized)
			{
				return;
			}

			for (auto& e : m_drawTasks)
			{
				e.second->OnMouseMove(a_evn);
			}
		}

		void UI::LockControls(bool a_switch)
		{
			m_state.controlsLocked = a_switch;
			Input::SetInputBlocked(a_switch);

			ITaskPool::AddTask([this, a_switch]() {
				stl::scoped_lock lock(m_lock);

				if (a_switch)
				{
					if (!m_state.autoVanityAllowState)
					{
						if (auto pc = PlayerCamera::GetSingleton())
						{
							m_state.autoVanityAllowState = pc->allowAutoVanityMode;
							pc->allowAutoVanityMode      = false;
						}
					}
				}
				else
				{
					if (m_state.autoVanityAllowState)
					{
						if (auto pc = PlayerCamera::GetSingleton())
						{
							pc->allowAutoVanityMode = *m_state.autoVanityAllowState;
						}
						m_state.autoVanityAllowState.clear();
					}
				}
			});
		}

		void UI::FreezeTime(bool a_switch)
		{
			m_state.timeFrozen = a_switch;

			ITaskPool::AddTask([a_switch]() {
				Game::Main::GetSingleton()->freezeTime = a_switch;
			});
		}

		void UI::QueueRemoveTask(std::int32_t a_id)
		{
			stl::scoped_lock lock(m_Instance.m_lock);

			auto it = m_Instance.m_drawTasks.find(a_id);
			if (it != m_Instance.m_drawTasks.end())
			{
				it->second->m_stopMe = true;
			}
		}

		void UI::EvaluateTaskState()
		{
			m_Instance.EvaluateTaskStateImpl();
		}

		void UI::EvaluateTaskStateImpl()
		{
			stl::scoped_lock lock(m_lock);

			for (auto& [i, e] : m_drawTasks)
			{
				if (e->m_options.lock != e->m_state.holdsLock)
				{
					e->m_state.holdsLock = e->m_options.lock;

					if (e->m_state.holdsLock)
					{
						m_state.lockCounter++;
					}
					else
					{
						m_state.lockCounter--;
					}
				}

				if (e->m_options.freeze != e->m_state.holdsFreeze)
				{
					e->m_state.holdsFreeze = e->m_options.freeze;

					if (e->m_state.holdsFreeze)
					{
						m_state.freezeCounter++;
					}
					else
					{
						m_state.freezeCounter--;
					}
				}

				if (e->m_options.wantCursor != e->m_state.holdsWantCursor)
				{
					e->m_state.holdsWantCursor = e->m_options.wantCursor;

					if (e->m_state.holdsWantCursor)
					{
						m_state.wantCursorCounter++;
					}
					else
					{
						m_state.wantCursorCounter--;
					}
				}
			}

			if (m_state.controlsLocked)
			{
				if (m_state.lockCounter == 0)
				{
					LockControls(false);
				}
			}
			else
			{
				if (m_state.lockCounter > 0)
				{
					LockControls(true);
				}
			}

			if (m_state.timeFrozen)
			{
				if (m_state.freezeCounter == 0)
				{
					FreezeTime(false);
				}
			}
			else
			{
				if (m_state.freezeCounter > 0)
				{
					FreezeTime(true);
				}
			}

			ImGui::GetIO().MouseDrawCursor = static_cast<bool>(m_state.wantCursorCounter);
		}

		bool UI::SetCurrentFont(const stl::fixed_string& a_font)
		{
			bool ok = false;

			auto it = m_fontData.find(a_font);

			if (it == m_fontData.end())
			{
				Error("%s: font '%s' not found", __FUNCTION__, a_font.c_str());
			}
			else if (!it->second.font->IsLoaded())
			{
				Error("%s: font '%s' could not be loaded", __FUNCTION__, a_font.c_str());
			}
			else
			{
				ok = true;
			}

			if (ok)
			{
				m_currentFont = std::addressof(*it);
			}
			else
			{
				it = m_fontData.find(m_sDefaultFont);

				ASSERT(it != m_fontData.end());
				ASSERT(it->second.font->IsLoaded());

				m_currentFont = std::addressof(*it);
			}

			return ok;
		}

		void UI::QueueSetExtraGlyphs(GlyphPresetFlags a_flags)
		{
			m_Instance.QueueSetExtraGlyphsImpl(a_flags);
		}

		void UI::QueueSetLanguageGlyphData(const std::shared_ptr<fontGlyphData_t>& a_data)
		{
			m_Instance.QueueSetLanguageGlyphDataImpl(a_data);
		}

		void UI::QueueFontChange(const stl::fixed_string& a_font)
		{
			m_Instance.QueueFontChangeImpl(a_font);
		}

		void UI::QueueSetFontSize(float a_size)
		{
			m_Instance.QueueSetFontSizeImpl(a_size);
		}

		void UI::QueueResetFontSize()
		{
			m_Instance.QueueResetFontSizeImpl();
		}

		void UI::QueueSetScale(float a_scale)
		{
			m_Instance.QueueSetScaleImpl(a_scale);
		}

		void UI::UpdateAvailableFonts()
		{
			fontInfoMap_t info;
			m_Instance.LoadFontMetadata(info);

			stl::scoped_lock lock(m_Instance.m_lock);

			m_Instance.UpdateAvailableFontsImpl(info);
		}

		void UI::MarkFontUpdateDataDirty()
		{
			m_Instance.MarkFontUpdateDataDirtyImpl();
		}

		void UI::QueueImGuiSettingsSave()
		{
			ITaskPool::AddTask([]() {
				stl::scoped_lock lock(m_Instance.m_lock);

				ImGui::SaveIniSettingsToDisk(PATHS::IMGUI_INI);
			});
		}

		void UI::OnTaskAdd(Tasks::UIRenderTaskBase* a_task)
		{
			a_task->m_state.holdsLock       = a_task->m_options.lock;
			a_task->m_state.holdsFreeze     = a_task->m_options.freeze;
			a_task->m_state.holdsWantCursor = a_task->m_options.wantCursor;

			if (a_task->m_state.holdsLock)
			{
				m_state.lockCounter++;
			}

			if (a_task->m_state.holdsFreeze)
			{
				m_state.freezeCounter++;
			}

			if (a_task->m_state.holdsWantCursor)
			{
				m_state.wantCursorCounter++;
			}

			a_task->m_state.running   = true;
			a_task->m_state.startTime = IPerfCounter::Query();

			a_task->OnTaskStart();

			if (!m_state.controlsLocked &&
			    m_state.lockCounter > 0)
			{
				LockControls(true);
			}

			if (!m_state.timeFrozen &&
			    m_state.freezeCounter > 0)
			{
				FreezeTime(true);
			}

			ImGui::GetIO().MouseDrawCursor = static_cast<bool>(m_state.wantCursorCounter);

			if (m_suspended.load(std::memory_order_relaxed))
			{
				ImGui_ImplWin32_ResetFrameTimer();
			}

			m_suspended.store(false, std::memory_order_relaxed);
		}

		void UI::OnTaskRemove(UIRenderTaskBase* a_task)
		{
			if (a_task->m_state.holdsLock)
			{
				m_state.lockCounter--;
			}

			if (a_task->m_state.holdsFreeze)
			{
				m_state.freezeCounter--;
			}

			if (a_task->m_state.holdsWantCursor)
			{
				m_state.wantCursorCounter--;
			}

			a_task->m_state.running = false;
			a_task->m_stopMe        = false;

			a_task->OnTaskStop();

			if (m_state.controlsLocked && m_state.lockCounter == 0)
			{
				LockControls(false);
			}

			if (m_state.timeFrozen && m_state.freezeCounter == 0)
			{
				FreezeTime(false);
			}

			ImGui::GetIO().MouseDrawCursor = static_cast<bool>(m_state.wantCursorCounter);
		}

		void UI::QueueSetScaleImpl(float a_scale)
		{
			stl::scoped_lock lock(m_lock);

			m_fontUpdateData.scale = std::clamp(a_scale, 0.2f, 8.0f);
			m_fontUpdateData.dirty = true;
		}

		void UI::QueueSetExtraGlyphsImpl(GlyphPresetFlags a_flags)
		{
			stl::scoped_lock lock(m_lock);

			m_fontUpdateData.extraGlyphPresets = a_flags;
			m_fontUpdateData.dirty             = true;
		}

		void UI::QueueSetLanguageGlyphDataImpl(
			const std::shared_ptr<fontGlyphData_t>& a_data)
		{
			stl::scoped_lock lock(m_lock);

			m_fontUpdateData.langGlyphData = a_data;
			m_fontUpdateData.dirty         = true;
		}

		void UI::QueueFontChangeImpl(const stl::fixed_string& a_font)
		{
			stl::scoped_lock lock(m_lock);

			m_fontUpdateData.font  = a_font;
			m_fontUpdateData.dirty = true;
		}

		void UI::QueueSetFontSizeImpl(float a_size)
		{
			stl::scoped_lock lock(m_lock);

			m_fontUpdateData.fontsize = a_size;
			m_fontUpdateData.dirty    = true;
		}

		void UI::QueueResetFontSizeImpl()
		{
			stl::scoped_lock lock(m_lock);

			m_fontUpdateData.fontsize.clear();
			m_fontUpdateData.dirty = true;
		}

		void UI::MarkFontUpdateDataDirtyImpl()
		{
			stl::scoped_lock lock(m_lock);

			m_fontUpdateData.dirty = true;
		}

		static constexpr void ScaleStyle(ImGuiStyle& a_style, float a_factor) noexcept
		{
			a_style.WindowPadding.x           = (a_style.WindowPadding.x * a_factor);
			a_style.WindowPadding.y           = (a_style.WindowPadding.y * a_factor);
			a_style.WindowRounding            = (a_style.WindowRounding * a_factor);
			a_style.WindowMinSize.x           = (a_style.WindowMinSize.x * a_factor);
			a_style.WindowMinSize.y           = (a_style.WindowMinSize.y * a_factor);
			a_style.ChildRounding             = (a_style.ChildRounding * a_factor);
			a_style.PopupRounding             = (a_style.PopupRounding * a_factor);
			a_style.FramePadding.x            = (a_style.FramePadding.x * a_factor);
			a_style.FramePadding.y            = (a_style.FramePadding.y * a_factor);
			a_style.FrameRounding             = (a_style.FrameRounding * a_factor);
			a_style.ItemSpacing.x             = (a_style.ItemSpacing.x * a_factor);
			a_style.ItemSpacing.y             = (a_style.ItemSpacing.y * a_factor);
			a_style.ItemInnerSpacing.x        = (a_style.ItemInnerSpacing.x * a_factor);
			a_style.ItemInnerSpacing.y        = (a_style.ItemInnerSpacing.y * a_factor);
			a_style.CellPadding.x             = (a_style.CellPadding.x * a_factor);
			a_style.CellPadding.y             = (a_style.CellPadding.y * a_factor);
			a_style.TouchExtraPadding.x       = (a_style.TouchExtraPadding.x * a_factor);
			a_style.TouchExtraPadding.y       = (a_style.TouchExtraPadding.y * a_factor);
			a_style.IndentSpacing             = (a_style.IndentSpacing * a_factor);
			a_style.ColumnsMinSpacing         = (a_style.ColumnsMinSpacing * a_factor);
			a_style.ScrollbarSize             = (a_style.ScrollbarSize * a_factor);
			a_style.ScrollbarRounding         = (a_style.ScrollbarRounding * a_factor);
			a_style.GrabMinSize               = (a_style.GrabMinSize * a_factor);
			a_style.GrabRounding              = (a_style.GrabRounding * a_factor);
			a_style.LogSliderDeadzone         = (a_style.LogSliderDeadzone * a_factor);
			a_style.TabRounding               = (a_style.TabRounding * a_factor);
			a_style.TabMinWidthForCloseButton = (a_style.TabMinWidthForCloseButton != FLT_MAX) ? (a_style.TabMinWidthForCloseButton * a_factor) : FLT_MAX;
			a_style.DisplayWindowPadding.x    = (a_style.DisplayWindowPadding.x * a_factor);
			a_style.DisplayWindowPadding.y    = (a_style.DisplayWindowPadding.y * a_factor);
			a_style.DisplaySafeAreaPadding.x  = (a_style.DisplaySafeAreaPadding.x * a_factor);
			a_style.DisplaySafeAreaPadding.y  = (a_style.DisplaySafeAreaPadding.y * a_factor);
			a_style.MouseCursorScale          = (a_style.MouseCursorScale * a_factor);
		}

		bool UI::UpdateFontData(bool a_force)
		{
			if (!m_fontUpdateData.dirty && !a_force)
			{
				return false;
			}

			if (!m_frameCount)
			{
				return false;
			}

			fontInfoMap_t info;

			LoadFontMetadata(info);
			UpdateAvailableFontsImpl(info);

			auto requestedfont(
				m_fontUpdateData.font.empty() ?
					m_currentFont->first :
                    m_fontUpdateData.font);

			auto& io = ImGui::GetIO();

			io.Fonts->Clear();

			m_fontData.clear();

			ASSERT(BuildFonts(info, m_fontData, requestedfont));
			ASSERT(!m_fontData.empty());

			SetCurrentFont(requestedfont);

			ImGui_ImplDX11_ReCreateFontsTexture();

			UpdateStyle();

			m_fontUpdateData.dirty = false;
			m_fontUpdateData.font.clear();

			return true;
		}

		void UI::UpdateStyle()
		{
			using namespace IED::UI;

			auto newStyle = std::make_unique<ImGuiStyle>();

			switch (m_currentStyle)
			{
			case UIStylePreset::Light:
				ImGui::StyleColorsLight(newStyle.get());
				break;
			case UIStylePreset::Classic:
				ImGui::StyleColorsClassic(newStyle.get());
				break;
			case UIStylePreset::ItaDark:
				Styles::ITA::Setup(*newStyle, Styles::ITA::Template::Dark, 1.0f);
				break;
			case UIStylePreset::ItaLight:
				Styles::ITA::Setup(*newStyle, Styles::ITA::Template::Light, 1.0f);
				break;
			case UIStylePreset::ItaClassic:
				Styles::ITA::Setup(*newStyle, Styles::ITA::Template::Classic, 1.0f);
				break;
			case UIStylePreset::SteamClassic:
				Styles::SteamClassic::Setup(*newStyle);
				break;
			case UIStylePreset::DeepDark:
				Styles::DeepDark::Setup(*newStyle);
				break;
			case UIStylePreset::S56:
				Styles::S56::Setup(*newStyle);
				break;
			case UIStylePreset::CorpGrey:
				Styles::CorporateGrey::Setup(*newStyle, false);
				break;
			case UIStylePreset::CorpGreyFlat:
				Styles::CorporateGrey::Setup(*newStyle, true);
				break;
			case UIStylePreset::DarkRed:
				Styles::DarkRed::Setup(*newStyle);
				break;
			case UIStylePreset::SpectrumDark:
				Styles::SpectrumDark::Setup(*newStyle);
				break;
			case UIStylePreset::EnemyMouse:
				Styles::EnemyMouse::Setup(*newStyle);
				break;
			default:
				ImGui::StyleColorsDark(newStyle.get());
				break;
			}

			ScaleStyle(*newStyle, m_fontUpdateData.scale);
			ImGui::GetStyle() = *newStyle;

			UpdateStyleAlpha();
		}

		void UI::UpdateStyleAlpha()
		{
			auto& style = ImGui::GetStyle();

			style.Alpha = std::clamp(m_conf.alpha, 0.15f, 1.0f);

			if (m_conf.bgAlpha)
			{
				style.Colors[ImGuiCol_WindowBg].w = std::clamp(*m_conf.bgAlpha, 0.1f, 1.0f);
			}
		}

		bool UI::LoadFonts(
			font_data_container&     a_data,
			const stl::fixed_string& a_font)
		{
			fontInfoMap_t info;

			LoadFontMetadata(info);
			UpdateAvailableFontsImpl(info);

			a_data.clear();

			if (!BuildFonts(info, a_data, a_font))
			{
				Error("%s: failed building font atlas", __FUNCTION__);
				return false;
			}

			return true;
		}

		bool UI::LoadFontMetadata(fontInfoMap_t& a_out)
		{
			try
			{
				Json::Value root;

				Serialization::ReadData(PATHS::FONT_META, root);

				Serialization::ParserState           state;
				Serialization::Parser<fontInfoMap_t> parser(state);

				return parser.Parse(root, a_out);
			}
			catch (const std::exception& e)
			{
				Error("%s: %s", __FUNCTION__, e.what());
				return false;
			}
		}

		void UI::AddFontRanges(
			ImFontGlyphRangesBuilder& a_builder,
			const fontGlyphRange_t&   a_range)
		{
			using iter_type = std::conditional_t<
				(IM_UNICODE_CODEPOINT_MAX < std::numeric_limits<std::uint32_t>::max() &&
			     sizeof(ImWchar) < sizeof(std::uint32_t)),
				std::uint32_t,
				std::uint64_t>;

			for (auto& e : a_range)
			{
				for (iter_type i = e.first; i <= e.second && i <= IM_UNICODE_CODEPOINT_MAX; i++)
				{
					a_builder.AddChar(static_cast<ImWchar>(i));
				}
			}
		}

		void UI::AddFontRanges(
			ImFontGlyphRangesBuilder& a_builder,
			const fontGlyphData_t&    a_data)
		{
			auto& io = ImGui::GetIO();

			a_builder.AddRanges(io.Fonts->GetGlyphRangesDefault());

			auto langFlags = m_fontUpdateData.langGlyphData ?
			                     m_fontUpdateData.langGlyphData->glyph_preset_flags :
                                 GlyphPresetFlags::kNone;

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kCyrilic) ||
			    langFlags.test(GlyphPresetFlags::kCyrilic) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kCyrilic))
			{
				a_builder.AddRanges(get_glyph_ranges_cyrilic());
			}

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kJapanese) ||
			    langFlags.test(GlyphPresetFlags::kJapanese) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kJapanese))
			{
				a_builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
			}

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kChineseSimplifiedCommon) ||
			    langFlags.test(GlyphPresetFlags::kChineseSimplifiedCommon) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kChineseSimplifiedCommon))
			{
				a_builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
			}

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kChineseFull) ||
			    langFlags.test(GlyphPresetFlags::kChineseFull) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kChineseFull))
			{
				a_builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
			}

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kKorean) ||
			    langFlags.test(GlyphPresetFlags::kKorean) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kKorean))
			{
				a_builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
			}

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kLatinFull) ||
			    langFlags.test(GlyphPresetFlags::kLatinFull) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kLatinFull))
			{
				a_builder.AddRanges(get_glyph_ranges_latin_full());
			}

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kGreek) ||
			    langFlags.test(GlyphPresetFlags::kGreek) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kGreek))
			{
				a_builder.AddRanges(get_glyph_ranges_greek());
			}

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kArabic) ||
			    langFlags.test(GlyphPresetFlags::kArabic) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kArabic))
			{
				a_builder.AddRanges(get_glyph_ranges_arabic());
			}

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kArrows) ||
			    langFlags.test(GlyphPresetFlags::kArrows) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kArrows))
			{
				a_builder.AddRanges(get_glyph_ranges_arrows());
			}

			if (a_data.glyph_preset_flags.test(GlyphPresetFlags::kCommon) ||
			    langFlags.test(GlyphPresetFlags::kCommon) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kCommon))
			{
				a_builder.AddRanges(get_glyph_ranges_common());
			}

			AddFontRanges(a_builder, a_data.glyph_ranges);

			if (m_fontUpdateData.langGlyphData)
			{
				AddFontRanges(a_builder, m_fontUpdateData.langGlyphData->glyph_ranges);
			}

			if (!a_data.extra_glyphs.empty())
			{
				a_builder.AddText(a_data.extra_glyphs.c_str());
			}

			if (m_fontUpdateData.langGlyphData &&
			    !m_fontUpdateData.langGlyphData->extra_glyphs.empty())
			{
				a_builder.AddText(m_fontUpdateData.langGlyphData->extra_glyphs.c_str());
			}
		}

		bool UI::BuildFonts(
			const fontInfoMap_t&     a_info,
			font_data_container&     a_out,
			const stl::fixed_string& a_font)
		{
			auto& io = ImGui::GetIO();

			{
				ImFontGlyphRangesBuilder builder;

				builder.AddRanges(io.Fonts->GetGlyphRangesDefault());

				AddFontRanges(builder, a_info.default_glyph_data.glyph_ranges);

				const auto& fontsize = m_fontUpdateData.fontsize ?
				                           *m_fontUpdateData.fontsize :
                                           a_info.default_font_size;

				auto r = a_out.try_emplace(
					m_sDefaultFont,
					nullptr,
					fontsize);

				builder.BuildRanges(std::addressof(r.first->second.ranges));

				ImFontConfig defaultConf;

				defaultConf.SizePixels  = r.first->second.size * m_fontUpdateData.scale;
				defaultConf.GlyphRanges = r.first->second.ranges.Data;

				r.first->second.font = io.Fonts->AddFontDefault(std::addressof(defaultConf));
			}

			if (!a_font.empty() && a_font != m_sDefaultFont)
			{
				if (auto it = a_info.fonts.find(a_font); it != a_info.fonts.end())
				{
					ImFontGlyphRangesBuilder builder;

					AddFontRanges(builder, it->second);

					const auto& fontsize = m_fontUpdateData.fontsize ?
					                           *m_fontUpdateData.fontsize :
                                               it->second.size;

					auto r = a_out.try_emplace(it->first, nullptr, fontsize);

					builder.BuildRanges(std::addressof(r.first->second.ranges));

					auto font = io.Fonts->AddFontFromFileTTF(
						it->second.path.c_str(),
						r.first->second.size * m_fontUpdateData.scale,
						nullptr,
						r.first->second.ranges.Data);

					if (!font)
					{
						Error(
							"%s: failed loading font '%s' from '%s'",
							__FUNCTION__,
							it->first.c_str(),
							it->second.path.c_str());

						a_out.erase(r.first);
					}
					else
					{
						r.first->second.font = font;
					}
				}
				else
				{
					Error(
						"%s: font '%s' not found",
						__FUNCTION__,
						a_font.c_str());
				}
			}

			return io.Fonts->Build();
		}

		void UI::UpdateAvailableFontsImpl(const fontInfoMap_t& a_data)
		{
			m_availableFonts.clear();

			for (auto& e : a_data.fonts)
			{
				m_availableFonts.emplace(e.first);
			}

			m_availableFonts.emplace(m_sDefaultFont);
		}

		void UI::Suspend()
		{
			ResetInput();

			if (m_state.controlsLocked)
			{
				m_state.lockCounter = 0;
				LockControls(false);
			}

			if (m_state.timeFrozen)
			{
				m_state.freezeCounter = 0;
				FreezeTime(false);
			}

			ImGui::GetIO().MouseDrawCursor = true;

			m_suspended.store(true, std::memory_order_relaxed);
		}

	}
}