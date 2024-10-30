#include "pch.h"
#include "SoundMgr.h"
#include "PathMgr.h"

SoundMgr::SoundMgr()
{
	FMOD_System_Create(&m_fmSystem, FMOD_VERSION);
	FMOD_System_Init(m_fmSystem, 32, FMOD_INIT_NORMAL, nullptr);
}

SoundMgr::~SoundMgr()
{
	for (auto& [name, sound] : m_mapSound)
	{
		FMOD_Sound_Release(sound);
	}
	for (auto& [name, BGMSound] : m_mapBGM)
	{
		FMOD_Sound_Release(BGMSound);
	}

	FMOD_System_Release(m_fmSystem);
}

void SoundMgr::Init()
{
	std::filesystem::recursive_directory_iterator SoundIter{ Mgr(PathMgr)->GetSoundPath() };

	for (const auto& sound : SoundIter)
	{
		if (sound.is_regular_file())
		{
			FMOD_SOUND* pSound = nullptr;
			const string pathName = wstring2string(sound.path().wstring());
			if ("BGM" == sound.path().parent_path().filename())
			{
				FMOD_System_CreateSound(m_fmSystem, pathName.data(), FMOD_LOOP_NORMAL, nullptr, &pSound);
				m_mapBGM.emplace(sound.path().filename().string(), pSound);
			}
			else
			{
				FMOD_System_CreateSound(m_fmSystem, pathName.data(), FMOD_LOOP_OFF, nullptr, &pSound);
				m_mapSound.emplace(sound.path().filename().string(), pSound);
			}
		}
	}
}

void SoundMgr::PlayEffect(string_view _strName, float _fVol)
{
	FMOD_System_PlaySound(m_fmSystem, m_mapSound[_strName.data()], 0, false, &m_arrChannel[m_iCurChannel]);
	FMOD_Channel_SetVolume(m_arrChannel[m_iCurChannel], _fVol);
	m_iCurChannel = (m_iCurChannel + 1) % 31;
}

void SoundMgr::PlayBGM(string_view _strName, float _fVol)
{
	if (m_fmBGMChannel)
	{
		FMOD_Channel_Stop(m_fmBGMChannel);
		m_fmBGMChannel = nullptr;
		return;
	}
	FMOD_System_PlaySound(m_fmSystem, m_mapBGM[_strName.data()], 0, false, &m_fmBGMChannel);
	FMOD_Channel_SetVolume(m_fmBGMChannel, _fVol);
}

void SoundMgr::BGMVolDown()
{
	if (m_fmBGMChannel)
	{
		float fCurVol;
		FMOD_Channel_GetVolume(m_fmBGMChannel, &fCurVol);
		if (0.f >= fCurVol)
		{
			FMOD_Channel_SetVolume(m_fmBGMChannel, 0.f);
			return;
		}
		FMOD_Channel_SetVolume(m_fmBGMChannel, fCurVol - 0.1f);
	}
}

void SoundMgr::Update()
{
	FMOD_System_Update(m_fmSystem);
}

void SoundMgr::BGMVolUp()
{
	if (m_fmBGMChannel)
	{
		float fCurVol;
		FMOD_Channel_GetVolume(m_fmBGMChannel, &fCurVol);
		if (1.f <= fCurVol)
		{
			FMOD_Channel_SetVolume(m_fmBGMChannel, 1.f);
			return;
		}
		FMOD_Channel_SetVolume(m_fmBGMChannel, fCurVol + 0.1f);
	}
}