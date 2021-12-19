#include "pch.h"

#include "ISound.h"

namespace IED
{
	void ISound::SoundPlay(
		std::uint8_t a_formType,
		NiAVObject* a_object,
		bool a_equip) const
	{
		auto audioManager = BSAudioManager::GetSingleton();
		if (!audioManager)
		{
			return;
		}

		auto soundForm = m_sounds.Get(a_formType, a_equip);
		if (!soundForm)
		{
			return;
		}

		BSSoundHandle handle;

		if (audioManager->BuildSoundDataFromDescriptor(
				handle,
				std::addressof(soundForm->soundDescriptor)))
		{
			handle.SetObjectToFollow(a_object);
			handle.Play();
		}
	}

	BGSSoundDescriptorForm*
		ISound::SoundRefHolder::Get(
			std::uint8_t a_formType,
			bool a_equip) const
	{
		const soundPair_t* pair;

		switch (a_formType)
		{
		case TESObjectWEAP::kTypeID:
			pair = std::addressof(weapon);
			break;
		case TESAmmo::kTypeID:
			pair = std::addressof(arrow);
			break;
		case TESObjectARMO::kTypeID:
			pair = std::addressof(armor);
			break;
		default:
			pair = std::addressof(gen);
			break;
		}

		return a_equip ? pair->equip : pair->unequip;
	}

	BGSSoundDescriptorForm* ISound::GetSoundForm(
		const IPluginInfo& a_pinfo,
		const SetObjectWrapper<Data::ConfigForm>& a_form)
	{
		if (!a_form)
		{
			return nullptr;
		}

		Game::FormID formid;

		if (!a_pinfo.ResolveFormID(*a_form, formid))
		{
			return nullptr;
		}

		auto res = formid.As<BGSSoundDescriptorForm>();
		if (!res)
		{
			return nullptr;
		}

		if (res->IsDeleted())
		{
			return nullptr;
		}

		return res;
	}

	[[nodiscard]] ISound::SoundRefHolder::soundPair_t ISound::MakeSoundPair(
		const IPluginInfo& a_pinfo,
		const Data::ConfigEntrySound::soundPair_t& a_in)
	{
		return {
			GetSoundForm(a_pinfo, a_in.first),
			GetSoundForm(a_pinfo, a_in.second)
		};
	}

}  // namespace IED