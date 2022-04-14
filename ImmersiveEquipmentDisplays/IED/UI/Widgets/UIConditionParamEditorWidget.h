#pragma once

#include "Form/UIFormPickerWidget.h"
#include "UIBipedObjectSelectorWidget.h"
#include "UICMNodeSelector.h"
#include "UIComparisonOperatorSelector.h"
#include "UIConditionExtraSelectorWidget.h"
#include "UIObjectTypeSelectorWidget.h"
#include "UIPackageTypeSelectorWidget.h"
#include "UITimeOfDaySelectorWidget.h"
#include "UIWeatherClassSelectorWidget.h"

#include "IED/UI/UILocalizationInterface.h"

#include "IED/ConfigCommon.h"

namespace IED
{
	class Controller;

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
		TimeOfDay,
		Extra,

		Total
	};

	struct ConditionParamItemExtraArgs
	{
		void*       p1{ nullptr };
		const void* p2{ nullptr };
		void*       p3{ nullptr };
		bool        disable{ false };
		bool        hide{ false };
	};

	namespace UI
	{
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
			public UIObjectSlotSelectorWidget,
			public UIBipedObjectSelectorWidget,
			public UICMNodeSelectorWidget,
			public UIFormLookupInterface,
			public UIConditionExtraSelectorWidget,
			public UIPackageTypeSelectorWidget,
			public UIWeatherClassSelectorWidget,
			public UIComparisonOperatorSelector,
			public UITimeOfDaySelectorWidget,
			public virtual UILocalizationInterface
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
			const char* GetFormKeywordExtraDesc(const char* a_idesc) const noexcept;

		private:
			bool DrawExtra(
				const entry_t&               a_entry,
				ConditionParamItemExtraArgs& a_args,
				ConditionParamItem           a_item);

			void GetFormDesc(Game::FormID a_form);

			entry_t m_entries[stl::underlying(ConditionParamItem::Total)];

			inline constexpr auto& get(ConditionParamItem a_item) noexcept
			{
				return m_entries[stl::underlying(a_item)];
			}

			inline constexpr const auto& get(ConditionParamItem a_item) const noexcept
			{
				return m_entries[stl::underlying(a_item)];
			}

			mutable char m_descBuffer[COND_PE_DESC_BUFFER_SIZE]{ 0 };
			mutable char m_descBuffer2[COND_PE_DESC_BUFFER_SIZE]{ 0 };

			UIConditionParamExtraInterface* m_extraInterface{ nullptr };

			UIFormPickerWidget m_formPickerForm;
			UIFormPickerWidget m_formPickerKeyword;

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
				Ap == ConditionParamItem::Keyword)
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
				Ap == ConditionParamItem::Float)
			{
				static_assert(std::is_same_v<T, float>);

				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else if constexpr (
				Ap == ConditionParamItem::UInt32)
			{
				static_assert(std::is_same_v<T, std::uint32_t>);

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
				Ap == ConditionParamItem::Extra)
			{
				e = {
					static_cast<void*>(std::addressof(a_p1)),
					nullptr
				};
			}
			else
			{
				static_assert(false);
			}
		}

		template <ConditionParamItem Ap, class Tp1, class Tp2>
		constexpr void UIConditionParamEditorWidget::SetNext(Tp1& a_p1, const Tp2& a_p2) noexcept
		{
			static_assert(Ap < ConditionParamItem::Total);

			auto& e = get(Ap);

			if constexpr (
				Ap == ConditionParamItem::CMENode)
			{
				static_assert(
					std::is_convertible_v<Tp1, stl::fixed_string> &&
					std::is_convertible_v<Tp2, stl::fixed_string>);

				e = {
					static_cast<void*>(std::addressof(static_cast<stl::fixed_string&>(a_p1))),
					static_cast<const void*>(std::addressof(static_cast<const stl::fixed_string&>(a_p2)))
				};

				return;
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
				static_assert(false);
			}
		}
	}
}