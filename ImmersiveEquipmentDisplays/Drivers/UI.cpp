#include "pch.h"

#include "Input.h"
#include "Render.h"
#include "UI.h"

#include "IED/Parsers/JSONFontInfoMapParser.h"

//#include "IED/UI/Widgets/UIStylePresetSelectorWidget.h"

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
			ITaskPool::AddTaskFixed(this);
#endif
		}

		void UI::Receive(const D3D11CreateEventPost& a_evn)
		{
			ASSERT(a_evn.m_pSwapChainDesc.OutputWindow != nullptr);

			const stl::lock_guard lock(m_lock);

			m_info.bufferSize = {
				static_cast<float>(a_evn.m_pSwapChainDesc.BufferDesc.Width),
				static_cast<float>(a_evn.m_pSwapChainDesc.BufferDesc.Height)
			};

			m_info.hWnd = a_evn.m_pSwapChainDesc.OutputWindow;

			RECT rect{};
			if (::GetClientRect(
					a_evn.m_pSwapChainDesc.OutputWindow,
					std::addressof(rect)) == TRUE)
			{
				m_ioUserData.btsRatio = {
					static_cast<float>(a_evn.m_pSwapChainDesc.BufferDesc.Width) /
						static_cast<float>(rect.right),
					static_cast<float>(a_evn.m_pSwapChainDesc.BufferDesc.Height) /
						static_cast<float>(rect.bottom)
				};
			}
			else
			{
				m_ioUserData.btsRatio = { 1.0f, 1.0f };
			}

			ASSERT(IMGUI_CHECKVERSION());
			ImGui::CreateContext();

			auto& io = ImGui::GetIO();

			io.MouseDrawCursor                   = true;
			io.ConfigWindowsMoveFromTitleBarOnly = true;
			io.DisplaySize                       = { m_info.bufferSize.width, m_info.bufferSize.height };
			io.MousePos                          = { io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f };
			io.UserData                          = static_cast<void*>(std::addressof(m_ioUserData));
			io.IniFilename                       = !m_conf.imgui_ini.empty() ? m_conf.imgui_ini.c_str() : nullptr;

			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

			ImGui::StyleColorsDark();

			if (!::ImGui_ImplWin32_Init(
					a_evn.m_pSwapChainDesc.OutputWindow))
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

			m_styleProfileManager.Load(PATHS::PROFILE_MANAGER_STYLES);

			m_pfnWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtrA(
				a_evn.m_pSwapChainDesc.OutputWindow,
				GWLP_WNDPROC,
				reinterpret_cast<LONG_PTR>(WndProc_Hook)));

			if (!m_pfnWndProc)
			{
				Warning(
					"[0x%llX] SetWindowLongPtrA failed",
					a_evn.m_pSwapChainDesc.OutputWindow);
			}

			m_imInitialized.store(true);

			Message("ImGui initialized");
		}

		void UI::Receive(const IDXGISwapChainPresent& a_evn)
		{
			if (m_suspended.load(std::memory_order_relaxed))
			{
				return;
			}

			const stl::lock_guard lock(m_lock);

			if (!m_imInitialized.load())
			{
				return;
			}

			m_drawing = true;

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

				const bool res = it->second->Run();

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

			m_drawing = false;

			if (!m_addQueue.empty())
			{
				for (auto& e : m_addQueue)
				{
					auto r = m_Instance.m_drawTasks.emplace(e.first, std::move(e.second));
					if (r.second)
					{
						m_Instance.OnTaskAdd(r.first->second.get());
					}
				}

				m_addQueue.clear();
			}

			m_uiRenderPerf.timer.End(m_uiRenderPerf.current);
		}

		// runs preps when paused
		void UI::Run()
		{
			RunPreps();
		}

		void UI::RunPreps()
		{
			if (m_suspended.load(std::memory_order_relaxed))
			{
				return;
			}

			const stl::lock_guard lock(m_lock);

			if (!m_imInitialized.load())
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
			if (m_Instance.m_suspended.load(std::memory_order_relaxed))
			{
				return;
			}

			const stl::lock_guard lock(m_lock);

			if (!m_imInitialized.load())
			{
				return;
			}

			for (auto& e : m_drawTasks)
			{
				e.second->OnKeyEvent(a_evn);
			}

			if (!m_state.blockInputImGuiCounter)
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

			/*if (Game::IsPaused())
			{
				return;
			}*/

			const stl::lock_guard lock(m_lock);

			if (!m_imInitialized.load())
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
				const stl::lock_guard lock(m_lock);

				if (a_switch)
				{
					if (!m_state.autoVanityAllowState)
					{
						if (auto pc = PlayerCamera::GetSingleton())
						{
							m_state.autoVanityAllowState.emplace(pc->allowAutoVanityMode);
							pc->allowAutoVanityMode = false;
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
						m_state.autoVanityAllowState.reset();
					}
				}
			});
		}

		void UI::FreezeTime(bool a_switch)
		{
			m_state.timeFrozen = a_switch;

			ITaskPool::AddTask([a_switch] {
				Game::Main::GetSingleton()->freezeTime = a_switch;
			});
		}

		stl::smart_ptr<Tasks::UIRenderTaskBase> UI::GetTask(std::int32_t a_id)
		{
			const stl::lock_guard lock(m_Instance.m_lock);

			if (!m_Instance.m_imInitialized.load())
			{
				return {};
			}

			auto it = m_Instance.m_drawTasks.find(a_id);
			if (it != m_Instance.m_drawTasks.end())
			{
				return it->second;
			}
			else
			{
				return {};
			}
		}

		void UI::QueueRemoveTask(std::int32_t a_id)
		{
			const stl::lock_guard lock(m_Instance.m_lock);

			auto it = m_Instance.m_drawTasks.find(a_id);
			if (it != m_Instance.m_drawTasks.end())
			{
				it->second->m_stopMe = true;
			}

			m_Instance.m_addQueue.erase(a_id);
		}

		void UI::EvaluateTaskState()
		{
			m_Instance.EvaluateTaskStateImpl();
		}

		void UI::QueueEvaluateTaskState()
		{
			ITaskPool::AddTask([] {
				EvaluateTaskState();
			});
		}

		template <class T, class U>
		static constexpr void eval_opt(
			const T& a_opt,
			T&       a_state,
			U&       a_counter)
		{
			if (a_opt != a_state)
			{
				a_state = a_opt;

				if (a_state)
				{
					a_counter++;
				}
				else
				{
					a_counter--;
				}
			}
		}

		void UI::EvaluateTaskStateImpl()
		{
			const stl::lock_guard lock(m_lock);

			for (auto& [i, e] : m_drawTasks)
			{
				eval_opt(e->m_options.lockControls, e->m_state.holdsControlLock, m_state.lockCounter);
				eval_opt(e->m_options.freeze, e->m_state.holdsFreeze, m_state.freezeCounter);
				eval_opt(e->m_options.wantCursor, e->m_state.holdsWantCursor, m_state.wantCursorCounter);
				eval_opt(e->m_options.blockCursor, e->m_state.holdsBlockCursor, m_state.blockCursorCounter);
				eval_opt(e->m_options.blockImGuiInput, e->m_state.holdsBlockImGuiInput, m_state.blockInputImGuiCounter);
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

			ApplyControlLockChanges();
		}

		void UI::ApplyControlLockChanges() const
		{
			auto& io = ImGui::GetIO();

			io.MouseDrawCursor =
				m_state.wantCursorCounter > 0 &&
				!m_state.blockCursorCounter;

			if (m_state.blockCursorCounter > 0)
			{
				io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
			}
			else
			{
				io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			}
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

		void UI::QueueSetLanguageGlyphData(const stl::smart_ptr<FontGlyphData>& a_data)
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
			auto info = m_Instance.MakeFontMetadata();

			const stl::lock_guard lock(m_Instance.m_lock);

			m_Instance.UpdateAvailableFontsImpl(info);
		}

		void UI::MarkFontUpdateDataDirty()
		{
			m_Instance.MarkFontUpdateDataDirtyImpl();
		}

		void UI::QueueImGuiSettingsSave()
		{
			ITaskPool::AddTask([]() {
				const stl::lock_guard lock(m_Instance.m_lock);

				ImGui::SaveIniSettingsToDisk(PATHS::IMGUI_INI);
			});
		}

		bool UI::AddTask(
			std::int32_t                                   a_id,
			const stl::smart_ptr<Tasks::UIRenderTaskBase>& a_task)
		{
			return m_Instance.AddTaskImpl(a_id, a_task);
		}

		void UI::OnTaskAdd(Tasks::UIRenderTaskBase* a_task)
		{
			a_task->m_state.holdsControlLock     = a_task->m_options.lockControls;
			a_task->m_state.holdsFreeze          = a_task->m_options.freeze;
			a_task->m_state.holdsWantCursor      = a_task->m_options.wantCursor;
			a_task->m_state.holdsBlockCursor     = a_task->m_options.blockCursor;
			a_task->m_state.holdsBlockImGuiInput = a_task->m_options.blockImGuiInput;

			if (a_task->m_state.holdsControlLock)
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

			if (a_task->m_state.holdsBlockCursor)
			{
				m_state.blockCursorCounter++;
			}

			if (a_task->m_state.holdsBlockImGuiInput)
			{
				m_state.blockInputImGuiCounter++;
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

			ApplyControlLockChanges();

			if (m_suspended.load(std::memory_order_relaxed))
			{
				ImGui_ImplWin32_ResetFrameTimer();
			}

			m_suspended.store(false, std::memory_order_relaxed);
		}

		void UI::OnTaskRemove(UIRenderTaskBase* a_task)
		{
			if (a_task->m_state.holdsControlLock)
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

			if (a_task->m_state.holdsBlockCursor)
			{
				m_state.blockCursorCounter--;
			}

			if (a_task->m_state.holdsBlockImGuiInput)
			{
				m_state.blockInputImGuiCounter--;
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

			ApplyControlLockChanges();
		}

		void UI::QueueSetScaleImpl(float a_scale)
		{
			const stl::lock_guard lock(m_lock);

			m_fontUpdateData.scale = std::clamp(a_scale, 0.2f, 8.0f);
			m_fontUpdateData.dirty = true;
		}

		void UI::QueueSetExtraGlyphsImpl(GlyphPresetFlags a_flags)
		{
			const stl::lock_guard lock(m_lock);

			m_fontUpdateData.extraGlyphPresets = a_flags;
			m_fontUpdateData.dirty             = true;
		}

		void UI::QueueSetLanguageGlyphDataImpl(
			const stl::smart_ptr<FontGlyphData>& a_data)
		{
			const stl::lock_guard lock(m_lock);

			if (a_data != m_fontUpdateData.langGlyphData)
			{
				m_fontUpdateData.langGlyphData = a_data;
				m_fontUpdateData.dirty         = true;
			}
		}

		void UI::QueueFontChangeImpl(const stl::fixed_string& a_font)
		{
			const stl::lock_guard lock(m_lock);

			m_fontUpdateData.font  = a_font;
			m_fontUpdateData.dirty = true;
		}

		void UI::QueueSetFontSizeImpl(float a_size)
		{
			const stl::lock_guard lock(m_lock);

			m_fontUpdateData.fontsize.emplace(a_size);
			m_fontUpdateData.dirty = true;
		}

		void UI::QueueResetFontSizeImpl()
		{
			const stl::lock_guard lock(m_lock);

			m_fontUpdateData.fontsize.reset();
			m_fontUpdateData.dirty = true;
		}

		void UI::MarkFontUpdateDataDirtyImpl()
		{
			const stl::lock_guard lock(m_lock);

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

			auto info = MakeFontMetadata();
			UpdateAvailableFontsImpl(info);

			auto requestedfont(
				m_fontUpdateData.font.empty() ?
					m_currentFont->first :
					m_fontUpdateData.font);

			auto& io = ImGui::GetIO();

			io.Fonts->Clear();

			ASSERT(BuildFonts(*info, m_fontData, requestedfont));
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
			const auto& data = m_styleProfileManager.Data();

			const ImGuiStyle* presetStyle = nullptr;

			if (!m_currentStyle.empty())
			{
				const auto it = data.find(m_currentStyle);
				if (it != data.end())
				{
					presetStyle = std::addressof(it->second.Data().style);
				}
				else
				{
					m_currentStyle.clear();
				}
			}

			if (presetStyle)
			{
				ImGui::GetStyle() = *presetStyle;
			}
			else
			{
				ImGui::GetStyle() = {};
			}

			ScaleStyle(ImGui::GetStyle(), m_fontUpdateData.scale);
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
			const auto info = MakeFontMetadata();
			UpdateAvailableFontsImpl(info);

			if (!BuildFonts(*info, a_data, a_font))
			{
				Error("%s: failed building font atlas", __FUNCTION__);
				return false;
			}

			return true;
		}

		std::unique_ptr<FontInfoMap> UI::MakeFontMetadata()
		{
			auto result = LoadFontMetadata(PATHS::FONT_META);

			if (!result)
			{
				result = std::make_unique_for_overwrite<FontInfoMap>();
			}

			try
			{
				const fs::path allowedExt{ ".json" };

				for (const auto& entry : fs::directory_iterator(PATHS::FONT_META_USER_PATH))
				{
					if (!entry.is_regular_file())
					{
						continue;
					}

					auto& path = entry.path();

					if (!path.has_extension() ||
					    path.extension() != allowedExt)
					{
						continue;
					}

					const auto tmp = LoadFontMetadata(entry.path());

					if (!tmp)
					{
						continue;
					}

					for (auto& e : tmp->fonts)
					{
						auto r = result->fonts.emplace(std::move(e));

						if (!r.second)
						{
							Warning(
								"%s: [%s] duplicate entry '%s'",
								__FUNCTION__,
								Serialization::SafeGetPath(path).c_str(),
								e.first.c_str());
						}
					}
				}
			}
			catch (const std::exception& e)
			{
				Exception(e, "%s", __FUNCTION__);
			}

			return result;
		}

		std::unique_ptr<FontInfoMap> UI::LoadFontMetadata(
			const fs::path& a_path)
		{
			using namespace Serialization;

			try
			{
				Json::Value root;

				ReadData(a_path, root);

				auto result = std::make_unique_for_overwrite<FontInfoMap>();

				ParserState         state;
				Parser<FontInfoMap> parser(state);

				if (!parser.Parse(root, *result))
				{
					throw parser_exception("parse failed");
				}

				return result;
			}
			catch (const std::exception& e)
			{
				Exception(
					e,
					"%s: [%s]",
					__FUNCTION__,
					SafeGetPath(a_path).c_str());

				return {};
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
			ImFontGlyphRangesBuilder&            a_builder,
			const stl::smart_ptr<FontGlyphData>& a_data)
		{
			auto& io = ImGui::GetIO();

			a_builder.AddRanges(io.Fonts->GetGlyphRangesDefault());

			auto langFlags = m_fontUpdateData.langGlyphData ?
			                     m_fontUpdateData.langGlyphData->glyph_preset_flags :
			                     stl::flag<GlyphPresetFlags>(GlyphPresetFlags::kNone);

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kCyrilic)) ||
			    langFlags.test(GlyphPresetFlags::kCyrilic) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kCyrilic))
			{
				a_builder.AddRanges(IGlyphData::get_glyph_ranges_cyrilic());
			}

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kJapanese)) ||
			    langFlags.test(GlyphPresetFlags::kJapanese) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kJapanese))
			{
				a_builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
			}

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kChineseSimplifiedCommon)) ||
			    langFlags.test(GlyphPresetFlags::kChineseSimplifiedCommon) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kChineseSimplifiedCommon))
			{
				a_builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
			}

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kChineseFull)) ||
			    langFlags.test(GlyphPresetFlags::kChineseFull) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kChineseFull))
			{
				a_builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
			}

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kKorean)) ||
			    langFlags.test(GlyphPresetFlags::kKorean) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kKorean))
			{
				a_builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
			}

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kLatinFull)) ||
			    langFlags.test(GlyphPresetFlags::kLatinFull) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kLatinFull))
			{
				a_builder.AddRanges(IGlyphData::get_glyph_ranges_latin_full());
			}

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kGreek)) ||
			    langFlags.test(GlyphPresetFlags::kGreek) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kGreek))
			{
				a_builder.AddRanges(IGlyphData::get_glyph_ranges_greek());
			}

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kArabic)) ||
			    langFlags.test(GlyphPresetFlags::kArabic) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kArabic))
			{
				a_builder.AddRanges(IGlyphData::get_glyph_ranges_arabic());
			}

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kArrows)) ||
			    langFlags.test(GlyphPresetFlags::kArrows) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kArrows))
			{
				a_builder.AddRanges(IGlyphData::get_glyph_ranges_arrows());
			}

			if ((a_data && a_data->glyph_preset_flags.test(GlyphPresetFlags::kCommon)) ||
			    langFlags.test(GlyphPresetFlags::kCommon) ||
			    m_fontUpdateData.extraGlyphPresets.test(GlyphPresetFlags::kCommon))
			{
				a_builder.AddRanges(IGlyphData::get_glyph_ranges_common());
			}

			if (a_data)
			{
				AddFontRanges(a_builder, a_data->glyph_ranges);
			}

			if (m_fontUpdateData.langGlyphData)
			{
				AddFontRanges(a_builder, m_fontUpdateData.langGlyphData->glyph_ranges);
			}

			if (a_data && !a_data->extra_glyphs.empty())
			{
				a_builder.AddText(a_data->extra_glyphs.c_str());
			}

			if (m_fontUpdateData.langGlyphData &&
			    !m_fontUpdateData.langGlyphData->extra_glyphs.empty())
			{
				a_builder.AddText(m_fontUpdateData.langGlyphData->extra_glyphs.c_str());
			}
		}

		bool UI::BuildFonts(
			const FontInfoMap&       a_info,
			font_data_container&     a_out,
			const stl::fixed_string& a_font)
		{
			a_out.clear();

			auto& io = ImGui::GetIO();

			{
				ImFontGlyphRangesBuilder builder;

				builder.AddRanges(io.Fonts->GetGlyphRangesDefault());

				if (auto& data = a_info.default_glyph_data)
				{
					AddFontRanges(builder, data->glyph_ranges);
				}

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
					if (const auto fontPath = GetFontPath(it->second))
					{
						ImFontGlyphRangesBuilder builder;

						AddFontRanges(builder, it->second.glyph_data);

						const auto& fontsize = m_fontUpdateData.fontsize ?
						                           *m_fontUpdateData.fontsize :
						                           it->second.size;

						auto r = a_out.try_emplace(it->first, nullptr, fontsize);

						builder.BuildRanges(std::addressof(r.first->second.ranges));

						auto font = io.Fonts->AddFontFromFileTTF(
							Serialization::SafeGetPath(*fontPath).c_str(),
							r.first->second.size * m_fontUpdateData.scale,
							nullptr,
							r.first->second.ranges.Data);

						if (!font)
						{
							Error(
								"%s: failed loading font '%s' from '%s'",
								__FUNCTION__,
								it->first.c_str(),
								Serialization::SafeGetPath(*fontPath).c_str());

							a_out.erase(r.first);
						}
						else
						{
							r.first->second.font = font;
						}
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

		static bool try_build_valid_path(
			const fs::path&                a_file,
			fs::path&                      a_out,
			std::function<bool(fs::path&)> a_func)
		{
			fs::path tmp;

			if (!a_func(tmp))
			{
				return false;
			}

			tmp /= a_file;

			if (!Serialization::FileExists(tmp))
			{
				return false;
			}

			a_out = std::move(tmp);

			return true;
		}

		std::optional<fs::path> UI::GetFontPath(const FontInfoEntry& a_in)
		{
			try
			{
				const fs::path file(
					stl::str_to_wstr(
						a_in.path,
						boost::locale::conv::method_type::stop));

				if (file.is_absolute() || !a_in.win_font)
				{
					return file;
				}

				fs::path result;

				if (try_build_valid_path(
						file,
						result,
						[](fs::path& a_out) {
							return WinApi::get_known_folder_path(
								::FOLDERID_Fonts,
								a_out);
						}))
				{
					return result;
				}

				if (try_build_valid_path(
						file,
						result,
						[](fs::path& a_out) {
							if (WinApi::get_known_folder_path(
									::FOLDERID_LocalAppData,
									a_out))
							{
								a_out /= "Microsoft";
								a_out /= "Windows";
								a_out /= "Fonts";

								return true;
							}
							else
							{
								return false;
							}
						}))
				{
					return result;
				}

				throw std::runtime_error("font not found");
			}
			catch (const std::exception& e)
			{
				Exception(
					e,
					"%s: [%s]",
					__FUNCTION__,
					a_in.path.c_str());

				return {};
			}
		}

		void UI::UpdateAvailableFontsImpl(const std::unique_ptr<FontInfoMap>& a_data)
		{
			m_availableFonts.clear();

			if (a_data)
			{
				for (auto& e : a_data->fonts)
				{
					m_availableFonts.emplace(e.first);
				}
			}

			m_availableFonts.emplace(m_sDefaultFont);
		}

		void UI::Suspend()
		{
			assert(m_state.lockCounter == 0);
			assert(m_state.freezeCounter == 0);
			assert(m_state.wantCursorCounter == 0);
			assert(m_state.blockCursorCounter == 0);
			assert(m_state.blockInputImGuiCounter == 0);

			ResetInput();

			m_suspended.store(true, std::memory_order_relaxed);
		}

		bool UI::AddTaskImpl(
			std::int32_t                                   a_id,
			const stl::smart_ptr<Tasks::UIRenderTaskBase>& a_task)
		{
			assert(a_task);

			const stl::lock_guard lock(m_lock);

			if (!m_imInitialized.load())
			{
				return false;
			}

			if (!a_task->RunEnableChecks())
			{
				return false;
			}

			if (m_drawing)
			{
				if (m_drawTasks.contains(a_id))
				{
					return false;
				}
				else
				{
					m_addQueue.insert_or_assign(
						a_id,
						a_task);

					return true;
				}
			}

			const auto r = m_drawTasks.emplace(
				a_id,
				a_task);

			if (!r.second)
			{
				return false;
			}

			OnTaskAdd(r.first->second.get());

			return true;
		}

	}
}