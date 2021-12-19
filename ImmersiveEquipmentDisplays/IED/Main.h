#pragma once

namespace IED
{
	class Controller;

	extern bool Initialize(const SKSEInterface* a_skse);

	extern std::shared_ptr<Controller> g_controller;
}  // namespace IED