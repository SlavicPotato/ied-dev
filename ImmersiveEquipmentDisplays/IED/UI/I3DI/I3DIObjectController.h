#pragma once

namespace IED
{
	namespace UI
	{
		class I3DIObject;
		class I3DIModelObject;
		struct I3DICommonData;

		class I3DIObjectController
		{
		public:
			struct Entry
			{
				inline Entry(const std::shared_ptr<I3DIObject>& a_object) :
					object(a_object)
				{
				}

				std::optional<float>        dist;
				std::shared_ptr<I3DIObject> object;
			};

			[[nodiscard]] inline constexpr auto& GetData() const noexcept
			{
				return m_data;
			}

			void RegisterObject(const std::shared_ptr<I3DIObject>& a_object);
			void UnregisterObject(const std::shared_ptr<I3DIObject>& a_object);

			//template <class T, class... Args>
			//auto& CreateObject(Args&&... a_args)  //
			//	requires(std::is_base_of_v<I3DIObject, T>)
			//{
			//	return m_data.emplace_back(std::make_shared<T>(std::forward<Args>(a_args)...)).object;
			//}

			void Run(I3DICommonData& a_data);
			void RenderObjects(I3DICommonData& a_data);

		protected:
			std::shared_ptr<I3DIObject> GetHovered(
				I3DICommonData& a_data);

			std::shared_ptr<I3DIObject> m_hovered;
			std::shared_ptr<I3DIObject> m_selected;

			stl::vector<Entry>                              m_data;
			stl::vector<std::pair<float, I3DIModelObject*>> m_drawQueueOpaque;
			stl::vector<std::pair<float, I3DIModelObject*>> m_drawQueueAlpha;
		};

	}
}