#pragma once

class AudioFactory {
public:
    AudioFactory();
    ~AudioFactory();

    void LoadAll(std::function<void()> const& onLoadedCallback = nullptr);

    auto GetMusic() const { return m_music; }
    auto GetWeaponSFX() const { return m_weaponSFX; }
    auto GetExplosionSFX() const { return m_explosionSFX; }
    auto GetPlayerDiedSFX() const { return m_playerDiedSFX; }
    auto GetCountSFX() const { return m_countSFX; }
    auto GetStartSFX() const { return m_startSFX; }

private:
    MusicRef m_music;
    AudioRef m_weaponSFX;
    AudioRef m_explosionSFX;
    AudioRef m_playerDiedSFX;
    AudioRef m_countSFX;
    AudioRef m_startSFX;

    std::thread m_loadingThread;
};
