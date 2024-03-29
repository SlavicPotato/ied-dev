#include "pch.h"

#include "ISound.h"

namespace IED
{
	void ISound::SoundPlay(
		std::uint8_t a_formType,
		TESObjectREFR* a_actor,
		bool         a_equip) const noexcept
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
				soundForm))
		{
			handle.SetPosition(a_actor->pos.x, a_actor->pos.y, a_actor->pos.z);
			if (const auto followObject = a_actor->Get3D1(false))
			{
				handle.SetObjectToFollow(followObject);
			}
			handle.Play();
		}
	}

	BGSSoundDescriptorForm*
		ISound::SoundRefHolder::Get(
			std::uint8_t a_formType,
			bool         a_equip) const noexcept
	{
		auto it = data.find(a_formType);
		if (it == data.end())
		{
			it = data.find(TESForm::kTypeID);
		}

		if (it != data.end())
		{
			return a_equip ?
			           it->second.equip :
                       it->second.unequip;
		}
		else
		{
			return nullptr;
		}
	}

	void ISound::ClearSounds()
	{
		m_sounds.data.clear();
	}

	BGSSoundDescriptorForm* ISound::GetSoundForm(
		Game::FormID a_formid)
	{
		if (a_formid.IsTemporary())
		{
			return nullptr;
		}

		auto form = a_formid.As<BGSSoundDescriptorForm>();
		if (!form)
		{
			return nullptr;
		}

		if (form->IsDeleted())
		{
			return nullptr;
		}

		return form;
	}

	BGSSoundDescriptorForm* ISound::GetSoundForm(
		const IPluginInfoA&                    a_pinfo,
		const std::optional<Data::ConfigForm>& a_form)
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

		return GetSoundForm(formid);
	}

	ISound::SoundRefHolder::soundPair_t ISound::MakeSoundPair(
		const IPluginInfoA&                                     a_pinfo,
		const Data::ConfigSound<Data::ConfigForm>::soundPair_t& a_in)
	{
		return {
			GetSoundForm(a_pinfo, a_in.first),
			GetSoundForm(a_pinfo, a_in.second)
		};
	}

	ISound::SoundRefHolder::soundPair_t ISound::MakeSoundPair(
		const Data::ConfigSound<Game::FormID>::soundPair_t& a_in)
	{
		return {
			a_in.first ?
				GetSoundForm(*a_in.first) :
                nullptr,
			a_in.second ?
				GetSoundForm(*a_in.second) :
                nullptr
		};
	}

}