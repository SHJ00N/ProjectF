#pragma once

#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include <unordered_map>
#include <string>

class SoundManager
{
public:
    static SoundManager& GetInstance()
    {
        static SoundManager instance;
        return instance;
    }

    bool Init();
    void LoadSound(const std::string &path, const std::string &name, bool loop = false);
    void PlaySFX(const std::string &name, float pitch = 1.0f, float volume = 1.0f);
    void PlayLoop(const std::string &name, float pitch = 1.0f, float volume = 1.0f);
    void StopLoop(const std::string &name);
    void Update();
    void Release();

private:
    SoundManager() = default;
    ~SoundManager();
    // delete copy
    SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator=(SoundManager&&) = delete;

    FMOD::System *m_system = nullptr;
    std::unordered_map<std::string, FMOD::Sound*> m_sounds;
    std::unordered_map<std::string, FMOD::Channel*> m_channels;
};