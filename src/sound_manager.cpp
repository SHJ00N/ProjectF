#include "sound_manager.h"

#include <iostream>

SoundManager::~SoundManager()
{
    this->Release();
}

bool SoundManager::Init()
{
    if(m_system) return true;

    FMOD_RESULT result;
    result = FMOD::System_Create(&m_system);      // Create the main system object.
    if (result != FMOD_OK)
    {
        std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        return false;
    }

    result = m_system->init(32, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        return false;
    }

    return true;
}

void SoundManager::LoadSound(const std::string &path, const std::string &name, bool loop)
{
    if(!m_system) return;   // check system
    if(m_sounds.find(name) != m_sounds.end()) return;   // prevent duplicate loads

    FMOD_RESULT result;
    FMOD::Sound *sound;
    FMOD_MODE mode = FMOD_DEFAULT;
    if(loop) mode |= FMOD_LOOP_NORMAL;
    result = m_system->createSound(path.c_str(), mode, nullptr, &sound);
    if(result != FMOD_OK)
    {
        std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
    }

    m_sounds[name] = sound;
}

void SoundManager::PlaySFX(const std::string &name, float pitch, float volume)
{
    auto it = m_sounds.find(name);
    if (it == m_sounds.end())
    {
        std::cout << name << " sound does not exist." << std::endl;
        return;
    }

    FMOD::Sound* sound = it->second;
    FMOD::Channel *channel;
    m_system->playSound(sound, nullptr, false, &channel);
    channel->setPitch(pitch);
    channel->setVolume(volume);
}

void SoundManager::PlayLoop(const std::string &name, float pitch, float volume)
{
    // already playing
    if (m_channels.find(name) != m_channels.end())
    {
        bool isPlaying = false;
        m_channels[name]->isPlaying(&isPlaying);
        if (isPlaying) 
        {
            m_channels[name]->setPitch(pitch);
            m_channels[name]->setVolume(volume);
            return;
        }
    }

    auto it = m_sounds.find(name);
    if (it == m_sounds.end())
    {
        std::cout << name << " sound does not exist." << std::endl;
        return;
    }

    FMOD::Sound* sound = it->second;
    FMOD::Channel* channel;
    m_system->playSound(sound, nullptr, false, &channel);
    channel->setPitch(pitch);
    channel->setVolume(volume);

    m_channels[name] = channel;
}

void SoundManager::StopLoop(const std::string &name)
{
    auto it = m_channels.find(name);
    if (it == m_channels.end()) return;

    if (it->second)
        it->second->stop();

    m_channels.erase(it);
}

void SoundManager::Update()
{
    if(m_system) m_system->update();
}

void SoundManager::Release()
{
    for (auto& [key, sound] : m_sounds)
    {
        if (sound)
            sound->release();
    }
    m_sounds.clear();

    if (m_system)
    {
        m_system->close();
        m_system->release();
        m_system = nullptr;
    }
}