#pragma once

namespace IED
{
	class Controller;
	struct formInfoResult_t;

	namespace UI
	{
		class UIFormLookupInterface
		{
		public:
			UIFormLookupInterface(
				Controller& a_controller);

			const formInfoResult_t* LookupForm(Game::FormID a_form) const;

		private:
			Controller& m_controller;
		};

	}
}