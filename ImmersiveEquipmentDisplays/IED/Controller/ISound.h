#pragma once

#include "IED/ConfigCommon.h"

namespace IED
{
	class ISound
	{
	public:
		struct SoundRefHolder
		{
			struct soundPair_t
			{
				BGSSoundDescriptorForm* equip{ nullptr };
				BGSSoundDescriptorForm* unequip{ nullptr };
			};

			BGSSoundDescriptorForm* Get(
				std::uint8_t a_formType,
				bool         a_equip) const;

			stl::unordered_map<std::uint8_t, soundPair_t> data;
		};

		void SoundPlay(std::uint8_t a_formType, NiAVObject* a_object, bool a_equip) const;

	protected:
		template <class Tr>
		void AddSound(std::uint8_t a_formType, Tr&& a_value)
		{
			m_sounds.data.emplace(a_formType, std::forward<Tr>(a_value));
		}

		void ClearSounds();

		static BGSSoundDescriptorForm* GetSoundForm(Game::FormID a_formid);

		static BGSSoundDescriptorForm* GetSoundForm(
			const IPluginInfo&                     a_pinfo,
			const stl::optional<Data::ConfigForm>& a_form);

		static ISound::SoundRefHolder::soundPair_t MakeSoundPair(
			const IPluginInfo&                                      a_pinfo,
			const Data::ConfigSound<Data::ConfigForm>::soundPair_t& a_in);

		static ISound::SoundRefHolder::soundPair_t MakeSoundPair(
			const Data::ConfigSound<Game::FormID>::soundPair_t& a_in);

	private:
		SoundRefHolder m_sounds;
	};
}