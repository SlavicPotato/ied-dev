#pragma once

namespace IED
{
	class Controller;
	struct FormInfoResult;

	namespace UI
	{
		class UIFormLookupInterface
		{
		public:
			UIFormLookupInterface(
				Controller& a_controller);

			const FormInfoResult* LookupForm(Game::FormID a_form) const;

		private:
			Controller& m_controller;
		};

	}
}