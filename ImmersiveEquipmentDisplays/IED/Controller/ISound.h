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

			BGSSoundDescriptorForm* Get(std::uint8_t a_formType, bool a_equip) const;

			soundPair_t weapon;
			soundPair_t arrow;
			soundPair_t armor;
			soundPair_t gen;
		};

		void SoundPlay(std::uint8_t a_formType, NiAVObject* a_object, bool a_equip) const;

		template <class... Args>
		void SetSounds(Args&&... a_args)
		{
			m_sounds = { std::forward<Args>(a_args)... };
		}

	protected:
		static BGSSoundDescriptorForm* GetSoundForm(
			const IPluginInfo& a_pinfo,
			const SetObjectWrapper<Data::ConfigForm>& a_form);

		static ISound::SoundRefHolder::soundPair_t MakeSoundPair(
			const IPluginInfo& a_pinfo,
			const Data::ConfigEntrySound::soundPair_t& a_in);

	private:
		SoundRefHolder m_sounds;
	};
}  // namespace IED