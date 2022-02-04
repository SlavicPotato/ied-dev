#pragma once

#include "Form/UIFormPickerWidget.h"
#include "UIBipedObjectSelectorWidget.h"
#include "UICMNodeSelector.h"
#include "UIConditionExtraSelectorWidget.h"
#include "UIObjectTypeSelectorWidget.h"

#include "IED/UI/UILocalizationInterface.h"

#include "IED/ConfigOverrideCommon.h"

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
		Extra,

		Total
	};

	struct ConditionParamItemExtraArgs
	{
		void* p1{ nullptr };
		const void* p2{ nullptr };
		void* p3{ nullptr };
		bool disable{ false };
	};

	namespace UI
	{
		class UIConditionParamExtraInterface
		{
		public:
			virtual bool DrawConditionParamExtra(void* a_p1, const void* a_p2) = 0;
			virtual bool DrawConditionItemExtra(
				ConditionParamItem a_item,
				ConditionParamItemExtraArgs& a_args);
		};

		class UIConditionParamEditorWidget :
			public UIObjectSlotSelectorWidget,
			public UIBipedObjectSelectorWidget,
			public UICMNodeSelectorWidget,
			public UIFormLookupInterface,
			public UIConditionExtraSelectorWidget,
			public virtual UILocalizationInterface
		{
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

				void* p1{ nullptr };
				const void* p2{ nullptr };
			};

		public:
			UIConditionParamEditorWidget(Controller& a_controller);

			void OpenConditionParamEditorPopup();
			bool DrawConditionParamEditorPopup();

			inline constexpr auto& GetFormPicker() noexcept
			{
				return m_formPickerForm;
			}

			inline constexpr auto& GetKeywordPicker() noexcept
			{
				return m_formPickerKeyword;
			}

			inline void SetExtraInterface(
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

			mutable char m_descBuffer[256]{ 0 };
			mutable char m_descBuffer2[256]{ 0 };

			UIConditionParamExtraInterface* m_extraInterface{ nullptr };

			UIFormPickerWidget m_formPickerForm;
			UIFormPickerWidget m_formPickerKeyword;
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
				static_assert(std::is_same_v<T, Biped::BIPED_OBJECT>);
			}
			else if constexpr (
				Ap == ConditionParamItem::EquipmentSlot)
			{
				static_assert(std::is_same_v<T, Data::ObjectSlot>);
			}
			else if constexpr (
				Ap == ConditionParamItem::EquipmentSlotExtra)
			{
				static_assert(std::is_same_v<T, Data::ObjectSlotExtra>);
			}
			else if constexpr (
				Ap == ConditionParamItem::QuestCondType)
			{
				static_assert(std::is_same_v<T, Data::QuestConditionType>);
			}
			else if constexpr (
				Ap == ConditionParamItem::CondExtra)
			{
				static_assert(std::is_same_v<T, Data::ExtraConditionType>);
			}
			else if constexpr (
				Ap == ConditionParamItem::Extra)
			{
			}
			else
			{
				static_assert(false);
			}

			e = {
				static_cast<void*>(std::addressof(a_p1)),
				nullptr
			};
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
			}
			else
			{
				static_assert(false);
			}

			e = {
				static_cast<void*>(std::addressof(a_p1)),
				static_cast<const void*>(std::addressof(a_p2))
			};
		}
	}
}