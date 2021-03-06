#include "game_pch.h"
#include "game_layer.h"
#include "layer_stack.h"
#include "tile_map.h"
#include "audio_factory.h"
#include "ecs/components/components.h"
#include "ecs/systems/sprite_system.h"
#include "states/state_pre_game.h"
#include "states/state_game.h"
#include "states/state_pause.h"
#include "states/state_post_game.h"
#include "debug/inspectors.h"
#include "moth_ui/group.h"
#include "moth_ui/node_text.h"

GameLayer::GameLayer(Game& game)
    : UILayer("resources/layouts/hud.mothui")
    , m_game(game)
    , m_random(0) {
    auto renderer = m_game.GetRenderer();
    m_backgroundTexture = CreateTextureRef(renderer, "resources/images/background.jpg");
    m_explosionTexture = CreateTextureRef(renderer, "resources/images/exp2_0.png");

    m_tileMap = std::make_unique<TileMap>(*renderer, 8, 8);

    m_stateMachine.AddState<StatePreGame>(*this);
    m_stateMachine.AddState<StateGame>(*this);
    m_stateMachine.AddState<StatePause>(*this);
    m_stateMachine.AddState<StatePostGame>(*this);

    if (m_root) {
        m_scoreNode = m_root->FindChild<moth_ui::NodeText>("txt_score");
        m_highScoreNode = m_root->FindChild<moth_ui::NodeText>("txt_highscore");
        m_countNode = m_root->FindChild<moth_ui::NodeText>("txt_count");
        if (m_countNode) {
            m_countNode->SetVisible(false);
        }
    }

    LoadScore();
}

bool GameLayer::OnEvent(moth_ui::Event const& event) {
    if (!UILayer::OnEvent(event)) {
        moth_ui::EventDispatch dispatch(event);
        //dispatch.Dispatch<EventRenderDeviceReset>([this](auto event) { FC_ResetFontFromRendererReset(m_scoreFont.get(), m_game.GetRenderer(), SDL_RENDER_DEVICE_RESET); return true; });
        //dispatch.Dispatch<EventRenderTargetReset>([this](auto event) { FC_ResetFontFromRendererReset(m_scoreFont.get(), m_game.GetRenderer(), SDL_RENDER_TARGETS_RESET); return true; });
        dispatch.Dispatch(&m_stateMachine);
        dispatch.Dispatch(this, &GameLayer::OnKey);
        return dispatch.GetHandled();
    }
    return true;
}

void GameLayer::Update(uint32_t ticks) {
    UILayer::Update(ticks);
    m_stateMachine.Update(ticks, m_registry);
}

void GameLayer::Draw(SDL_Renderer& renderer) {
    SDL_RenderCopy(&renderer, m_backgroundTexture.get(), nullptr, nullptr);

    auto const& cameraPositionComponent = m_registry.get<PositionComponent>(m_cameraEntity);
    auto const& cameraCameraComponent = m_registry.get<CameraComponent>(m_cameraEntity);
    ViewParameters viewParameters;
    viewParameters.m_width = GetWidth();
    viewParameters.m_height = GetHeight();
    viewParameters.m_offsetX = static_cast<int>(cameraPositionComponent.x) - viewParameters.m_width / 2;
    viewParameters.m_offsetY = static_cast<int>(cameraPositionComponent.y) - viewParameters.m_height / 2;

    m_tileMap->Draw(viewParameters);

    SpriteSystem::Draw(renderer, m_registry, viewParameters);

    m_stateMachine.Draw(renderer);

    m_scoreNode->SetText(fmt::format("{}", m_worldState.m_score));
    m_highScoreNode->SetText(fmt::format("{}", m_worldState.m_highScore));

    UILayer::Draw(renderer);

    if (m_game.IsEditorMode()) {
        DrawDebugUI();
    }
}

void GameLayer::OnAddedToStack(LayerStack* layerStack) {
    UILayer::OnAddedToStack(layerStack);
    m_stateMachine.StateTransition<StatePreGame>();
    auto& audioFactory = m_game.GetAudioFactory();
    Mix_PlayMusic(audioFactory.GetMusic().get(), -1);
}

void GameLayer::OnRemovedFromStack() {
    UILayer::OnRemovedFromStack();
    Mix_PauseMusic();
}

entt::entity GameLayer::SpawnCamera(int x, int y) {
    auto entity = m_registry.create();

    m_registry.emplace<VelocityComponent>(entity);
    auto& cameraPosition = m_registry.emplace<PositionComponent>(entity);
    auto& cameraDetails = m_registry.emplace<CameraComponent>(entity);

    cameraPosition.x = static_cast<float>(x);
    cameraPosition.y = static_cast<float>(y);

    cameraDetails.active = true;

    return entity;
}

entt::entity GameLayer::SpawnPlayer(int x, int y) {
    auto entity = m_registry.create();

    m_registry.emplace<HealthComponent>(entity);
    m_registry.emplace<VelocityComponent>(entity);
    auto& playerPositionComponent = m_registry.emplace<PositionComponent>(entity);
    auto& playerCollisionComponent = m_registry.emplace<CollisionComponent>(entity);
    auto& playerSpriteComponent = m_registry.emplace<SpriteComponent>(entity);
    auto& playerWeaponComponent = m_registry.emplace<WeaponComponent>(entity);

    playerSpriteComponent.texture = CreateTextureRef(m_game.GetRenderer(), m_worldParameters.m_playerSpritePath.c_str());
    playerSpriteComponent.width = m_worldParameters.m_playerSpriteWidth;
    playerSpriteComponent.height = m_worldParameters.m_playerSpriteHeight;

    playerPositionComponent.x = static_cast<float>(x);
    playerPositionComponent.y = static_cast<float>(y);
    playerPositionComponent.parent = m_cameraEntity;

    playerCollisionComponent.width = m_worldParameters.m_playerCollisionWidth;
    playerCollisionComponent.height = m_worldParameters.m_playerCollisionHeight;
    playerCollisionComponent.flags = COLLISION_FLAG_PLAYER;
    playerCollisionComponent.flag_mask = COLLISION_FLAG_MAP | COLLISION_FLAG_ENEMY;
    playerCollisionComponent.on_collision = [this, entity](entt::entity otherEntity) {
        auto& healthComponent = m_registry.get<HealthComponent>(entity);
        healthComponent.alive = false;
        auto const position = ResolvePosition(m_registry, entity);
        SpawnExplosion(static_cast<int>(position.x), static_cast<int>(position.y), false);
    };

    playerWeaponComponent.fire_delay = m_worldParameters.m_playerFireDelay;
    playerWeaponComponent.velocity = m_worldParameters.m_playerBulletSpeed;

    return entity;
}

entt::entity GameLayer::SpawnExplosion(int x, int y, bool playSound) {
    auto entity = m_registry.create();
    auto& positionComponent = m_registry.emplace<PositionComponent>(entity);
    auto& spriteComponent = m_registry.emplace<SpriteComponent>(entity);
    auto& animComponent = m_registry.emplace<AnimationComponent>(entity);
    auto& lifetimeComponent = m_registry.emplace<LifetimeComponent>(entity);

    positionComponent.x = static_cast<float>(x);
    positionComponent.y = static_cast<float>(y);

    spriteComponent.texture = m_explosionTexture;
    spriteComponent.width = 64;
    spriteComponent.height = 64;

    int const frameCount = 16;
    animComponent.ticks_per_frame = 1000 / frameCount;
    animComponent.playing = true;
    animComponent.anim_type = AnimType::Stop;
    int srcTop = 0;
    int srcLeft = 0;
    animComponent.frames.resize(frameCount);
    for (int i = 0; i < frameCount; ++i) {
        auto& rect = animComponent.frames[i];
        rect.x = srcLeft;
        rect.y = srcTop;
        rect.w = 64;
        rect.h = 64;

        srcLeft += 64;
        if (((i + 1) % 4) == 0) {
            srcLeft = 0;
            srcTop += 64;
        }
    }

    lifetimeComponent.lifetime = animComponent.ticks_per_frame * frameCount;

    if (playSound) {
        Mix_PlayChannel(-1, m_game.GetAudioFactory().GetExplosionSFX().get(), 0);
    }

    return entity;
}

void GameLayer::SetupLevel() {
    m_registry.clear();
    m_random.Reset(m_worldParameters.m_seed);

    m_cameraEntity = SpawnCamera(0, 0);
    m_playerEntity = SpawnPlayer(0, 0);

    m_worldState.m_levelSpeed = m_worldParameters.m_levelSpeedInit;
    m_worldState.m_enemySpawnDelayMin = static_cast<float>(m_worldParameters.m_enemySpawnDelayMinInit);
    m_worldState.m_enemySpawnDelayMax = static_cast<float>(m_worldParameters.m_enemySpawnDelayMaxInit);
    m_worldState.m_enemySpawnTimer = static_cast<uint32_t>(m_random.Range(m_worldState.m_enemySpawnDelayMin, m_worldState.m_enemySpawnDelayMax));
    m_worldState.m_enemyCurrentSpeed = m_worldParameters.m_enemySpeedInit;
    m_worldState.m_score = 0;
}

void GameLayer::LoadScore() {
    if (std::filesystem::exists(m_worldParameters.m_dataFile)) {
        std::ifstream scoreData(m_worldParameters.m_dataFile, std::ios_base::binary);
        scoreData.read(reinterpret_cast<char*>(&m_worldState.m_highScore), sizeof(m_worldState.m_highScore));
        scoreData.close();
    }
}

void GameLayer::SaveScore() {
    std::ofstream scoreData(m_worldParameters.m_dataFile, std::fstream::binary | std::fstream::trunc);
    if (scoreData.is_open()) {
        scoreData.write(reinterpret_cast<char const*>(&m_worldState.m_highScore), sizeof(m_worldState.m_highScore));
        scoreData.close();
    }
}

void GameLayer::DrawDebugUI() {
    if (ImGui::Begin("registry")) {
        m_registry.each([this](auto entity) {
            auto const label = fmt::format("{}", static_cast<uint32_t>(entity));
            if (ImGui::TreeNode(label.c_str())) {
                ImGuiInspectAll(m_registry, entity);
                ImGui::TreePop();
            }
        });
    }
    ImGui::End();
}

bool GameLayer::OnKey(moth_ui::EventKey const& event) {
    if (event.GetAction() == moth_ui::KeyAction::Down) {
        if (event.GetKey() == moth_ui::Key::Escape) {
            if (m_stateMachine.IsInState<StateGame>()) {
                m_stateMachine.StateTransition<StatePause>();
            }
            return true;
        }
    }
    return false;
}
