#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"
//////////////////////////////////////////////////////////////////////////

AudioSystem* g_theAudioSystem = nullptr;

//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------
// To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any game,
//	#define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//
#include "Game/Main/EngineBuildPreferences.hpp"
#include "../Core/Utils/XmlUtilities.hpp"
#include "../Math/MathUtils.hpp"
#include "../Core/Tools/Command.hpp"
#include "../Core/Tools/DevConsole.hpp"
#if !defined( ENGINE_DISABLE_AUDIO )


//-----------------------------------------------------------------------------------------------
// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "Engine/ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "Engine/ThirdParty/fmod/fmod_vc.lib" )
#endif


//====================================================================================
void DevConsolePlayOneShot(Command& thecommand)
{
	if(thecommand.m_commandArguements.size() <= 1)
	{
		DevConsole::AddErrorMessage("Please enter a clip name or hit help to see all clips");
		return;
	}

	std::string input = thecommand.m_commandArguements.at(1);

	if(input == "help")
	{
		DevConsole::AddConsoleDialogue(ConsoleDialogue("All Audio clips names", Rgba::WHITE));
		
		// print out all the sound names
		Strings clipNames = AudioSystem::GetInstance()->GetAllAudioClipNames();

		for(uint i = 0; i < clipNames.size(); i++)
		{
			DevConsole::AddConsoleDialogue(ConsoleDialogue(clipNames.at(i), GetRainbowColor(i, (int) clipNames.size())));
		}
	}
	else
	{
		// Since we have a default sound, play one shot uses that if its an invalid name so no need to check
		PlayOneShot(input);
	}

}

void ToggleMute(Command& theCommand)
{
	UNUSED(theCommand);

	AudioSystem::GetInstance()->ToggleMasterMute();
}

//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem()
	: m_fmodSystem( nullptr )
{
	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );


	//====================================================================================
	//									NOTES:
	//____________________________________________________________________________________
	// 
	// I COMMENTED OUT THIS CAUSE IT WAS FAILING AND I NEED TO WORK
	//
	//====================================================================================
	//result = m_fmodSystem->init( 512, FMOD_INIT_NORMAL, nullptr );
	ValidateResult( result );

	CreateDefaultSound();

	CommandRegister("oneShot","Type: help","Play a one shot of a sound", DevConsolePlayOneShot);
	CommandRegister("mute", "", "Toggles mute of sound", ToggleMute);
	g_theAudioSystem = this;
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	DestroyClips();
	
	FMOD_RESULT result = m_fmodSystem->release();
	ValidateResult( result );


	ReleaseFModAndSounds();

}


void AudioSystem::DestroyClips()
{
	for(std::vector<AudioClip*>::iterator it = m_audioClips.begin(); it != m_audioClips.end(); ++it)
	{
		AudioClip* current = *it;

		delete current;
		current = nullptr;
	}

	m_audioClips.clear();
}

//--------------------------------------------------------------------------
void AudioSystem::ReleaseFModAndSounds()
{
	// I think this is how you free up fmod. Release all sounds we made and then Release the system
	
	for(uint i = 0; i < m_registeredSounds.size(); i++)
	{
		//TODO figure out how this works :(
		//FMOD_RESULT result = m_registeredSounds.at(i)->release();
	}

	m_registeredSounds.clear();

	m_fmodSystem->release();
	m_fmodSystem = nullptr; 
}

AudioSystem* AudioSystem::GetInstance()
{
	return g_theAudioSystem;
}

void AudioSystem::StartUp()
{
	LoadFromXML("Data/Audio/AudioClips.xml");
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::CreateDefaultSound()
{
	std::string name = "default";
	std::string group = "NoGroup";
	std::string path = "error.wav";
	uint weight = 1U;
	std::string fullPath = relativePath + path;

	SoundID newID = CreateOrGetSound(fullPath);

	AudioClip* newAudioClip = new AudioClip(name, path, group, weight, newID);

	m_audioClips.push_back(newAudioClip);
}

void AudioSystem::LoadFromXML(std::string path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( path.c_str() );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: " + path);

	tinyxml2::XMLElement* current = rootElement->FirstChildElement();

	//---------------------------------------------------------
	while (current)
	{
		CreateAndStoreAudioClip(*current);

		current = current->NextSiblingElement();
	}
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFilePath )
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFilePath );
	if( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFilePath.c_str(), FMOD_DEFAULT, nullptr, &newSound );
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredSoundIDs[ soundFilePath ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


void AudioSystem::CreateAndStoreAudioClip(tinyxml2::XMLElement& node)
{
	std::string name = ParseXmlAttribute(node, "name", "NoName");
	std::string group = ParseXmlAttribute(node, "group", "NoGroup");
	std::string path = ParseXmlAttribute(node, "path", "ERROR");
	uint weight = (uint) ParseXmlAttribute(node, "weight", 0);
	std::string fullPath = relativePath + path;

	SoundID newID = CreateOrGetSound(fullPath);
	

	AudioClip* newAudioClip = new AudioClip(name, path, group, weight, newID);

	m_audioClips.push_back(newAudioClip);
}

AudioClip* AudioSystem::GetAudioClipByName(std::string name)
{
	for(uint i = 0; i < m_audioClips.size(); i++)
	{
		AudioClip* current = m_audioClips.at(i);

		if(current->m_name == name)
			return current;
	}

	// just in case it is deleted or not defined
	if(name == "default")
		return nullptr;

	// Return the error sound
	return GetAudioClipByName("default");
}

std::vector<AudioClip*> AudioSystem::GetAudioClipsByGroupName(std::string groupName, bool weighted)
{
	std::vector<AudioClip*>	clips;
	
	for(uint i = 0; i < m_audioClips.size(); i++)
	{
		AudioClip* current = m_audioClips.at(i);

		if(weighted)
		{
			uint theWeight = current->m_weight;

			// This is in whole number percents so by pushing that many back we can create weight
			for(uint j = 0; j < theWeight; j++)
			{
				clips.push_back(current);
			}

		}
		else
		{
			if(current->m_group == groupName)
				clips.push_back(current);
		}
		
	}

	// If we didn't find anything just return the default
	if(clips.size() == 0U)
	{
		AudioClip* d = GetAudioClipByName("default");
		clips.push_back(d);
	}
	

	return clips;

}

//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySound( SoundID soundID, bool isLooped, float volume, float balance, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, isPaused, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
	}

	return (SoundPlaybackID) channelAssignedToSound;
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound( SoundPlaybackID soundPlaybackID )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->stop();
}


//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setVolume( volume );
}


//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPan( balance );
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound( &currentSound );
	if( !currentSound )
		return;

	int ignored = 0;
	currentSound->getDefaults( &frequency, &ignored );
	channelAssignedToSound->setFrequency( frequency * speed );
}


void AudioSystem::SetMasterVolume(float volume)
{
	// credit Michael Dale
	
	FMOD::ChannelGroup* master;
	m_fmodSystem->getMasterChannelGroup(&master);

	master->setVolume(volume);

	master = nullptr;
}

float AudioSystem::GetMasterVolume()
{
	// credit Michael Dale

	FMOD::ChannelGroup* master;
	m_fmodSystem->getMasterChannelGroup(&master);

	float volume = 1.f;

	master->getVolume(&volume);

	master = nullptr;
	return volume;
}

void AudioSystem::ToggleMasterMute()
{
	// credit Michael Dale

	m_isMuted = !m_isMuted;

	if (m_isMuted == true)
	{
		DevConsole::AddConsoleDialogue("AUDIO MUTED", Rgba::RED);
		m_tempPreviousMasterVolume = GetMasterVolume();
		SetMasterVolume(0.0f);
	}
	else
	{
		DevConsole::AddConsoleDialogue("AUDIO UNMUTED", Rgba::GREEN);
		SetMasterVolume(m_tempPreviousMasterVolume);
	}

}

//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	if( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
	}
}


Strings AudioSystem::GetAllAudioClipNames()
{
	Strings names;

	for(uint i = 0; i < m_audioClips.size(); i++)
	{
		names.push_back(m_audioClips.at(i)->m_name);
	}

	return names;
}

#endif // !defined( ENGINE_DISABLE_AUDIO )

//====================================================================================
void PlayLoopingSound(std::string name, float volume, float balance, float speed, bool isPaused)
{
	AudioClip*	soundToPlay = g_theAudioSystem->GetAudioClipByName(name);

	// Check to make sure it's not already playing?
	if(soundToPlay->m_playbackID == MISSING_SOUND_ID)
		soundToPlay->m_playbackID = g_theAudioSystem->PlaySound(soundToPlay->m_soundID, true, volume, balance, speed, isPaused);
}

void StopSound(std::string name)
{
	AudioClip*	soundToPlay = g_theAudioSystem->GetAudioClipByName(name);

	// Check to make sure it's not already playing?
	if(soundToPlay->m_playbackID != MISSING_SOUND_ID)
	{
		g_theAudioSystem->StopSound(soundToPlay->m_playbackID);
		soundToPlay->m_playbackID = MISSING_SOUND_ID;
	}
}

void PlayOneShot(std::string name, float volume, float balance, float speed, bool isPaused)
{
	AudioClip*	soundToPlay = g_theAudioSystem->GetAudioClipByName(name);

	if(soundToPlay == nullptr)
		return;

	// We don't check to see if its already playing cause a one shot can be spammed? (think bullets)
	soundToPlay->m_playbackID = g_theAudioSystem->PlaySound(soundToPlay->m_soundID, false, volume, balance, speed, isPaused);
}

void PlayOneShotFromGroup(std::string groupName, float volume, float balance, float speed, bool isPaused)
{
	std::vector<AudioClip*> clips = g_theAudioSystem->GetAudioClipsByGroupName(groupName);

	int indexToUse = GetRandomIntRange(0, ((int) clips.size()) - 1);

	AudioClip* oneToPlay = clips.at(indexToUse);

	oneToPlay->m_playbackID = g_theAudioSystem->PlaySound(oneToPlay->m_soundID, false, volume, balance, speed, isPaused);

}


