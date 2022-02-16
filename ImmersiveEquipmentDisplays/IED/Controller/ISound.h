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

			soundPair_t arrow;
			soundPair_t armor;
			soundPair_t weapon;
			soundPair_t gen;
		};

		void SoundPlay(std::uint8_t a_formType, NiAVObject* a_object, bool a_equip) const;

		template <class... Args>
		void SetSounds(Args&&... a_args)
		{
			m_sounds = { std::forward<Args>(a_args)... };
		}

	protected:
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
}  // namespace IED