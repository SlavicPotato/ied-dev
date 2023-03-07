#pragma once

#include "ConfigOutfitEntryHolder.h"

#include "IED/ConfigCommon.h"

namespace IED
{
	namespace Data
	{
		namespace OM
		{
			struct configStoreOutfit_t :
				configStoreBase_t<configOutfitEntryHolder_t>
			{
				const data_type& GetNPC(
					Game::FormID a_npc,
					Game::FormID a_race) const noexcept;
				
				const data_type& GetRace(
					Game::FormID a_race) const noexcept;
				
				const data_type& GetGlobal() const noexcept;

				template <class Tf>
				void visit(Tf a_func)                              //
					noexcept(noexcept(data_type().visit(a_func)))  //
					requires(requires(data_type a_v, Tf a_func) { a_v.visit(a_func); })
				{
					for (auto& e : data)
					{
						for (auto& f : e)
						{
							f.second.visit(a_func);
						}
					}

					for (auto& e : global)
					{
						e.visit(a_func);
					}
				}
			};
		}
	}
}