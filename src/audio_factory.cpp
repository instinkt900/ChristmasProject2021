#include "game_pch.h"
#include "audio_factory.h"

AudioFactory::AudioFactory() {
}

AudioFactory::~AudioFactory() {
    if (m_loadingThread.joinable()) {
        m_loadingThread.join();
    }
}

void AudioFactory::LoadAll(std::function<void()> const& onLoadedCallback) {
    if (m_loadingThread.joinable()) {
        m_loadingThread.join();
    }
    m_loadingThread = std::thread([this, onLoadedCallback]() {
        using namespace std::chrono_literals;
        m_music = CreateMusicRef("resources/audio/OutThere.ogg");
        m_weaponSFX = CreateAudioRef("resources/audio/Pew__006.wav");
        m_explosionSFX = CreateAudioRef("resources/audio/Explosion2__006.wav");
        m_playerDiedSFX = CreateAudioRef("resources/audio/Explosion2__007.wav");
        m_countSFX = CreateAudioRef("resources/audio/Pickup__010.wav");
        m_startSFX = CreateAudioRef("resources/audio/Pickup__003.wav");
        Mix_Volume(-1, 32);
        Mix_VolumeChunk(m_weaponSFX.get(), 32);
        //Mix_VolumeChunk(m_playerDiedSFX.get(), 16);
        if (onLoadedCallback) {
            onLoadedCallback();
        }
    });
}
