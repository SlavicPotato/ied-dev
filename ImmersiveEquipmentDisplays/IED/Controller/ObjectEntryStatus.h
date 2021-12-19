#pragma once

namespace IED
{
	enum class ObjectEntryStatusCode : std::uint32_t
	{
		kNone,
		kLoaded,
		kModelParamsError,
		kTargetNodeError,
		kModelLoadError,
		kTempForm,
	};

	struct ObjectEntryStatusInfo
	{
		Game::FormID actor;
		Game::FormID npc;
		Game::FormID race;
	};

	struct processParams_t;

	class ObjectEntryStatus
	{
	public:
		void set(
			ObjectEntryStatusCode a_code,
			const processParams_t& a_params,
			Game::FormID a_form);

		void clear() noexcept;

		[[nodiscard]] inline constexpr auto get_status() const noexcept
		{
			return m_lastStatus;
		}

		[[nodiscard]] inline constexpr const auto& get_info() const noexcept
		{
			return m_info;
		}

	private:
		ObjectEntryStatusCode m_lastStatus{ ObjectEntryStatusCode::kNone };
		std::unique_ptr<ObjectEntryStatusInfo> m_info;
	};
}