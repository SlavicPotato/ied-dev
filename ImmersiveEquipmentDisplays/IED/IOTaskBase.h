#pragma once

#include <ext/IOTask.h>

namespace IED
{
	template <class _Super>
	class IOTaskBase :
		public RE::IOTask
	{
	private:
		void Unk_01() override
		{
		}

		void Unk_02() override
		{
		}

		bool Run() override
		{
			static_cast<_Super*>(this)->RunTask();
			return false;
		}
	};
}