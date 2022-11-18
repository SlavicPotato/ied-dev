#pragma once

#include "I3DIBoundObject.h"

#include "I3DIBoundingOrientedBox.h"

namespace IED
{
	class ActorObjectHolder;

	namespace UI
	{
		struct I3DICommonData;

		class I3DIActorObject :
			public I3DIBoundObject
		{
		public:
			I3DIActorObject(Game::FormID a_actor);

			virtual ~I3DIActorObject() noexcept override = default;

			virtual I3DIActorObject* AsActorObject() override
			{
				return this;
			};

			virtual void DrawObjectExtra(I3DICommonData& a_data) override;

			virtual void OnClick(I3DICommonData& a_data) override;

			virtual bool ObjectIntersects(
				I3DICommonData& a_data,
				const I3DIRay&  a_ray,
				float&          a_dist) override;

			virtual void UpdateBound() override;

			void Update(const ActorObjectHolder& a_holder);

			[[nodiscard]] inline constexpr auto& GetActorBound() const noexcept
			{
				return GetBoundingShape<I3DIBoundingOrientedBox>()->GetBound();
			}

			inline constexpr void SetLost() noexcept
			{
				m_lost = true;
			}

			[[nodiscard]] inline constexpr bool IsActorLost() const noexcept
			{
				return m_lost;
			}

			[[nodiscard]] inline constexpr auto& GetActorFormID() const noexcept
			{
				return m_actor;
			}

		private:
			Game::FormID m_actor;
			std::string  m_name;

			bool m_lost{ false };
		};

	}
}