#pragma once

#include "Form/UIFormPickerWidget.h"
#include "UIBipedObjectSelectorWidget.h"
#include "UICMNodeSelector.h"
#include "UIComparisonOperatorSelector.h"
#include "UIConditionExtraSelectorWidget.h"
#include "UINodeMonitorSelectorWidget.h"
#include "UIObjectTypeSelectorWidget.h"
#include "UIPackageTypeSelectorWidget.h"
#include "UITimeOfDaySelectorWidget.h"
#include "UIVariableConditionSourceSelectorWidget.h"
#include "UIVariableTypeSelectorWidget.h"
#include "UIWeatherClassSelectorWidget.h"

#include "IED/UI/UIFormBrowserCommonFilters.h"
#include "IED/UI/UILocalizationInterface.h"

#include "IED/ConditionalVariableStorage.h"
#include "IED/ConfigCommon.h"
#include "IED/ConfigVariableConditionSource.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		enum class ConditionParamItem : std::uint8_t
		{
			Form,
			Keyword,
			CMENode,
			BipedSlot,
			EquipmentSlot,
			EquipmentSlotExtra,
			Furniture,
			QuestCondType,
			CondExtra,
			PackageType,
			WeatherClass,
			CompOper,
			Float,
			UInt32,
			Int32,
			TimeOfDay,
			Extra,
			Race,
			Percent,
			NodeMon,
			CondVarType,
			VarCondSource,
			FormAny,

			Total
		};

		struct ConditionParamItemExtraArgs
		{
			void*                              p1{ nullptr };
			const void*                        p2{ nullptr };
			void*                              p3{ nullptr };
			std::optional<UIFormBrowserFilter> formFilter;
			bool                               disable{ false };
			bool                               hide{ false };
		};

		struct ConditionParamItemOnChangeArgs
		{
			void*       p1{ nullptr };
			const void* p2{ nullptr };
			void*       p3{ nullptr };
		};

		inline static constexpr std::size_t COND_PE_DESC_BUFFER_SIZE = 256;

		class UIConditionParamExtraInterface
		{
		public:
			virtual bool DrawConditionParamExtra(
				void*       a_p1,
				const void* a_p2) = 0;

			virtual bool DrawConditionItemExtra(
				ConditionParamItem           a_item,
				ConditionParamItemExtraArgs& a_args);

			virtual void OnConditionItemChange(
				ConditionParamItem                    a_item,
				const ConditionParamItemOnChangeArgs& a_args);
		};

		enum class UIConditionParamEditorTempFlags : std::uint8_t
		{
			kNone = 0,

			kNoClearForm    = 1ui8 << 0,
			kNoClearKeyword = 1ui8 << 1,
			kAllowBipedNone = 1ui8 << 2,
		};

		DEFINE_ENUM_CLASS_BITWISE(UIConditionParamEditorTempFlags);

		class UIConditionParamEditorWidget :
			public UIFormLookupInterface,
			public UIWeatherClassSelectorWidget,
			public UITimeOfDaySelectorWidget,
			public UINodeMonitorSelectorWidget
		{
			inline static constexpr auto POPUP_ID = "mpr_ed";

			struct entry_t
			{
				template <class T>
				inline constexpr T& As1() const noexcept
				{
					return *static_cast<T*>(p1);
				}

				template <class T>
				inline constexpr const T& As2() const noexcept
				{
					return *static_cast<const T*>(p2);
				}

				void*       p1{ nullptr };
				const void* p2{ nullptr };
			};

		public:
			UIConditionParamEditorWidget(Controller& a_controller);

			void OpenConditionParamEditorPopup();
			bool DrawConditionParamEditorPopup();
			bool DrawConditionParamEditorPanel();

			inline constexpr auto& GetFormPicker() noexcept
			{
				return m_formPickerForm;
			}

			inline constexpr auto& GetKeywordPicker() noexcept
			{
				return m_formPickerKeyword;
			}

			inline constexpr auto& GetRacePicker() noexcept
			{
				return m_formPickerRace;
			}

			inline constexpr void SetTempFlags(
				UIConditionParamEditorTempFlags a_mask) noexcept
			{
				m_tempFlags.set(a_mask);
			}

			inline constexpr void SetExtraInterface(
				UIConditionParamExtraInterface* a_if) noexcept
			{
				m_extraInterface = a_if;
			}

			void Reset();

			template <ConditionParamItem Ap, class T>
			constexpr void SetNext(T& a_p1) noexcept;

			template <ConditionParamItem Ap, class Tp1, class Tp2>
			constexpr void SetNext(Tp1& a_p1, const Tp2& a_p2) noexcept;

			const char* GetItemDesc(ConditionParamItem a_item);
			const char* GetFormKeywordExtraDesc(const char* a_idesc, bool a_race = false) const noexcept;

			inline constexpr auto& GetDescBuffer() noexcept
			{
				return m_descBuffer;
			}

		private:
			bool DrawExtra(
				const entry_t&               a_entry,
				ConditionParamItemExtraArgs& a_args,
				ConditionParamItem           a_item);

			void OnChange(
				const entry_t&     a_entry,
				ConditionParamItem a_item);

			void GetFormDesc(Game::FormID a_form);

			inline constexpr auto& get(ConditionParamItem a_item) noexcept
			{
				return m_entries[stl::underlying(a_item)];
			}

			inline constexpr const auto& get(ConditionParamItem a_item) const noexcept
			{
				return m_entries[stl::underlying(a_item)];
			}

			entry_t m_entries[stl::underlying(ConditionParamItem::Total)];

			UIConditionParamExtraInterface* m_extraInterface{ nullptr };

			UIFormPickerWidget m_formPickerForm;
			UIFormPickerWidget m_formPickerKeyword;
			UIFormPickerWidget m_formPickerRace;
			UIFormPickerWidget m_formPickerAny;

			mutable char m_descBuffer[COND_PE_DESC_BUFFER_SIZE]{ 0 };
			mutable char m_descBuffer2[COND_PE_DESC_BUFFER_SIZE]{ 0 };

			stl::flag<UIConditionParamEditorTempFlags> m_tempFlags{
				UIConditionParamEditorTempFlags::kNone
			};
		};

		template <ConditionParamItem Ap, class T>
		constexpr void UIConditionParamEditorWidget::SetNext(T& a_p1) noexcept
		{
			static_assert(Ap < ConditionParamItem::Total);

			auto& e = get(Ap);

			if constexpr (
				Ap == ConditionParamItem::Form ||
				Ap == ConditionParamItem::Keyword ||
				Ap == ConditionParamItem::Race ||
				Ap == ConditionParamItem::FormAny)
			{
				static_assert(std::is_convertible_v<T, Game::FormID>);

				e = {
					static_cast<void*>(std::addressof(static_cast<Game::FormID&>(a_p1))),
					nullptr
				};

				return;
			}
			else if constexpr (
				Ap == ConditionParamItem::BipedSlot)
			{
				static_assert(std::is_same_v<T, BIPED_OBJECT>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::EquipmentSlot)
			{
				static_assert(std::is_same_v<T, Data::ObjectSlot>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::EquipmentSlotExtra)
			{
				static_assert(std::is_same_v<T, Data::ObjectSlotExtra>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::QuestCondType)
			{
				static_assert(std::is_same_v<T, Data::QuestConditionType>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::CondExtra)
			{
				static_assert(std::is_same_v<T, Data::ExtraConditionType>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::PackageType)
			{
				static_assert(std::is_same_v<T, PACKAGE_PROCEDURE_TYPE>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::WeatherClass)
			{
				static_assert(std::is_same_v<T, WeatherClassificationFlags>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::CompOper)
			{
				static_assert(std::is_same_v<T, Data::ComparisonOperator>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::Float ||
				Ap == ConditionParamItem::Percent)
			{
				static_assert(std::is_same_v<T, float>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::NodeMon ||
				Ap == ConditionParamItem::UInt32)
			{
				static_assert(std::is_same_v<T, std::uint32_t>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::Int32)
			{
				static_assert(std::is_same_v<T, std::int32_t>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::TimeOfDay)
			{
				static_assert(std::is_same_v<T, Data::TimeOfDay>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::VarCondSource)
			{
				static_assert(std::is_same_v<T, Data::VariableConditionSource>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::Extra)
			{
				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else
			{
				HALT("fixme");
			}
		}

		template <ConditionParamItem Ap, class Tp1, class Tp2>
		constexpr void UIConditionParamEditorWidget::SetNext(Tp1& a_p1, const Tp2& a_p2) noexcept
		{
			static_assert(Ap < ConditionParamItem::Total);

			auto& e = get(Ap);

			if constexpr (
				Ap == ConditionParamItem::CondVarType)
			{
				static_assert(
					std::is_same_v<Tp1, ConditionalVariableType> &&
					std::is_convertible_v<Tp2, stl::fixed_string>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					static_cast<const void*>(std::addressof(static_cast<const stl::fixed_string&>(a_p2)))
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::CMENode)
			{
				static_assert(
					std::is_convertible_v<Tp1, stl::fixed_string> &&
					std::is_convertible_v<Tp2, stl::fixed_string>);

				e = {
					static_cast<void*>(std::addressof(static_cast<stl::fixed_string&>(a_p1))),
					static_cast<const void*>(std::addressof(static_cast<const stl::fixed_string&>(a_p2)))
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::Form ||
				Ap == ConditionParamItem::FormAny)
			{
				static_assert(
					std::is_convertible_v<Tp1, Game::FormID> &&
					std::is_same_v<std::underlying_type_t<Tp2>, Localization::StringID>);

				e = {
					static_cast<void*>(std::addressof(static_cast<Game::FormID&>(a_p1))),
					reinterpret_cast<const void*>(a_p2)
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::Extra)
			{
				e = {
					static_cast<void*>(std::addressof(a_p1)),
					static_cast<const void*>(std::addressof(a_p2))
				};
			}
			else
			{
				HALT("fixme");
			}
		}
	}
}