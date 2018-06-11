#pragma once
#include "Engine/ThirdParty/fmod/fmod.hpp"
#include "Engine/ThirdParty/tinyxml/tinyxml2.h"
#include <string>
#include <vector>
#include <map>
#include "../Core/General/EngineCommon.hpp"

//=============================================================
// Forward Declare
//=============================================================
class AudioSystem;

//=============================================================
// Type Defs + Defines
//=============================================================
typedef size_t SoundID;
typedef size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs

#define relativePath "Data/Audio/"

//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================
struct AudioClip
{
	AudioClip(std::string name, std::string path, std::string group, uint weight, SoundID id)
	:	m_name(name),
		m_path(path),
		m_group(group),
		m_weight(weight),
		m_soundID(id),
		m_playbackID(MISSING_SOUND_ID) {}


	std::string			m_name;
	std::string			m_path;
	std::string			m_group;
	uint				m_weight;

	SoundID				m_soundID; // used to play a sound

	// This is used to turn off sound because it is the channel
	// that is currently assigned to the playing sound.
	SoundPlaybackID		m_playbackID;
};

//=============================================================
// Classes
//=============================================================
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

	static AudioSystem* GetInstance();

public:
	void						StartUp(); // Load from XML
	void						LoadFromXML(std::string path);
	virtual void				BeginFrame();
	virtual void				EndFrame();

	virtual SoundID				CreateOrGetSound( const std::string& soundFilePath );
	virtual void				CreateAndStoreAudioClip(tinyxml2::XMLElement& node);
	AudioClip*					GetAudioClipByName(std::string name);
	std::vector<AudioClip*>		GetAudioClipsByGroupName(std::string groupName, bool weighted = true);

	virtual SoundPlaybackID		PlaySound( SoundID soundID, bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	virtual void				StopSound( SoundPlaybackID soundPlaybackID );
	virtual void				SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume );	// volume is in [0,1]
	virtual void				SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance );	// balance is in [-1,1], where 0 is L/R centered
	virtual void				SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed );		// speed is frequency multiplier (1.0 == normal)

	virtual void				ValidateResult( FMOD_RESULT result );

protected:
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
	
	std::vector<AudioClip*>				m_audioClips;
};

//=============================================================
// Standalone C Functions
//=============================================================
void PlayLoopingSound(std::string name, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false);
void StopSound(std::string name);

void PlayOneShot(std::string name, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false);
void PlayOneShotFromGroup(std::string groupName, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false);

//=============================================================
// Externs
//=============================================================
extern AudioSystem* g_theAudioSystem; // engine reference

