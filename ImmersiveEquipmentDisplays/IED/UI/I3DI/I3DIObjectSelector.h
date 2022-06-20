#pragma once

namespace IED
{
	namespace UI
	{
		template <class K, class V>
		class I3DIObjectSelector
		{
		public:
			struct Entry
			{
				DirectX::BoundingOrientedBox bound;
				std::optional<float>         dist;
				V                            data;
			};

			using data_type  = stl::vectormap<K, Entry>;
			using value_type = std::pair<K, Entry&>;

			std::optional<value_type> XM_CALLCONV GetHovered(
				DirectX::XMVECTOR a_rayOrigin,
				DirectX::XMVECTOR a_rayDir);

			[[nodiscard]] inline constexpr auto& GetData() const noexcept
			{
				return m_data;
			}

		protected:
			stl::vectormap<K, Entry> m_data;
		};

		template <class K, class V>
		auto XM_CALLCONV I3DIObjectSelector<K, V>::GetHovered(
			DirectX::XMVECTOR a_rayOrigin,
			DirectX::XMVECTOR a_rayDir)
			-> std::optional<value_type>
		{
			auto& data = m_data.getvec();

			for (auto& e : data)
			{
				float dist;

				if (e->second.bound.Intersects(a_rayOrigin, a_rayDir, dist))
				{
					e->second.dist = dist;
				}
				else
				{
					e->second.dist.reset();
				}
			}

			m_data.sortvec([](auto& a_lhs, auto& a_rhs) {
				return a_lhs->second.dist < a_rhs->second.dist;
			});

			auto it = std::find_if(
				data.begin(),
				data.end(),
				[&](auto& a_v) {
					return a_v->second.dist.has_value();
				});

			return it != data.end() ?
			           std::make_optional<value_type>((*it)->first, (*it)->second) :
                       std::optional<value_type>{};
		}

	}
}