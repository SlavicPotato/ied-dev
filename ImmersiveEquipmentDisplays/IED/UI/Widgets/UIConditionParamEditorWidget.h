#pragma once

#include "Form/UIFormPickerWidget.h"
#include "UIBipedObjectSelectorWidget.h"
#include "UICMNodeSelector.h"
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
		Extra,

		Total
	};

	struct ConditionParamItemExtraArgs
	{
		void* p1;
		const void* p2;
		void* p3;
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
				ConditionParamItemExtraArgs &a_args);
		};

		class UIConditionParamEditorWidget :
			public UIObjectSlotSelectorWidget,
			public UIBipedObjectSelectorWidget,
			public UICMNodeSelectorWidget,
			public UIFormLookupInterface,
			public virtual UILocalizationInterface
		{
			struct entry_t
			{
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
			void SetNext(T& a_p1);

			template <ConditionParamItem Ap, class T, class U>
			void SetNext(T& a_p1, const U& a_p2);

			const char* GetItemDesc(ConditionParamItem a_item);

		private:
			void GetFormDesc(Game::FormID a_form);
			const char* GetFormKeywordExtraDesc(const char* a_idesc) const noexcept;

			entry_t m_entries[stl::underlying(ConditionParamItem::Total)];

			inline constexpr auto& get(ConditionParamItem a_item) noexcept
			{
				return m_entries[stl::underlying(a_item)];
			}

			inline constexpr const auto& get(ConditionParamItem a_item) const noexcept
			{
				return m_entries[stl::underlying(a_item)];
			}

			struct
			{
				Game::FormID form;
				Game::FormID keyword;
				stl::fixed_string cmeNode;
				Biped::BIPED_OBJECT biped{ Biped::BIPED_OBJECT::kNone };
				Data::ObjectSlot slot{ Data::ObjectSlot::kMax };
				Data::ObjectSlotExtra slotExtra{ Data::ObjectSlotExtra::kNone };
				Data::QuestConditionType qcondType{ Data::QuestConditionType::kNone };

				static_assert(std::is_same_v<std::underlying_type_t<Biped::BIPED_OBJECT>, std::uint32_t>);
			} m_tempData;

			mutable char m_descBuffer[256]{ 0 };

			UIConditionParamExtraInterface* m_extraInterface{ nullptr };

			UIFormPickerWidget m_formPickerForm;
			UIFormPickerWidget m_formPickerKeyword;
		};

		template <ConditionParamItem Ap, class T>
		void UIConditionParamEditorWidget::SetNext(T& a_p1)
		{
			auto& e = get(Ap);

			if constexpr (
				Ap == ConditionParamItem::Form ||
				Ap == ConditionParamItem::Keyword)
			{
				if constexpr (std::is_convertible_v<T, Game::FormID>)
				{
					e.p1 = static_cast<void*>(std::addressof(static_cast<Game::FormID&>(a_p1)));
					e.p2 = nullptr;

					return;
				}
				else
				{
					static_assert(false);
				}
			}
			else if constexpr (
				Ap == ConditionParamItem::BipedSlot)
			{
				if constexpr (!std::is_same_v<T, std::uint32_t>)
				{
					static_assert(false);
				}
			}
			else if constexpr (
				Ap == ConditionParamItem::EquipmentSlot)
			{
				if constexpr (!std::is_same_v<T, Data::ObjectSlot>)
				{
					static_assert(false);
				}
			}
			else if constexpr (
				Ap == ConditionParamItem::EquipmentSlotExtra)
			{
				if constexpr (!std::is_same_v<T, Data::ObjectSlotExtra>)
				{
					static_assert(false);
				}
			}
			else if constexpr (
				Ap == ConditionParamItem::QuestCondType)
			{
				if constexpr (!std::is_same_v<T, Data::QuestConditionType>)
				{
					static_assert(false);
				}
			}
			else if constexpr (
				Ap == ConditionParamItem::Extra)
			{
			}
			else
			{
				static_assert(false);
			}

			e.p1 = static_cast<void*>(std::addressof(a_p1));
			e.p2 = nullptr;
		}

		template <ConditionParamItem Ap, class T, class U>
		void UIConditionParamEditorWidget::SetNext(T& a_p1, const U& a_p2)
		{
			auto& e = get(Ap);

			if constexpr (
				Ap == ConditionParamItem::CMENode)
			{
				if constexpr (
					std::is_convertible_v<T, stl::fixed_string> &&
					std::is_convertible_v<U, stl::fixed_string>)
				{
					e.p1 = static_cast<void*>(std::addressof(static_cast<stl::fixed_string&>(a_p1)));
					e.p2 = static_cast<const void*>(std::addressof(static_cast<const stl::fixed_string&>(a_p2)));
					return;
				}
				else
				{
					static_assert(false);
				}
			}
			else if constexpr (
				Ap == ConditionParamItem::Extra)
			{
			}
			else
			{
				static_assert(false);
			}

			e.p1 = static_cast<void*>(std::addressof(a_p1));
			e.p2 = static_cast<const void*>(std::addressof(a_p2));
		}
	}
}