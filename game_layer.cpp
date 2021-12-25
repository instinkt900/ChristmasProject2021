#include "game_layer.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

#include "components.h"
#include "layer_stack.h"

#include "enemy_system.h"
#include "weapon_system.h"
#include "velocity_system.h"
#include "lifetime_system.h"

GameLayer::GameLayer(SDL_Renderer* renderer)
:m_renderer(renderer) {
    m_registry.on_destroy<SpriteComponent>().connect<&GameLayer::DestroySprite>(this);

    SDL_Surface* bg = IMG_Load("background.jpg");
    m_backgroundTexture = SDL_CreateTextureFromSurface(m_renderer, bg);
    SDL_FreeSurface(bg);

    m_tileMap = std::make_unique<TileMap>(m_renderer, 8, 8);

    TTF_Font* font = TTF_OpenFont("pilotcommand.ttf", 80);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SDL_Surface* countText[3] = { nullptr, nullptr, nullptr };
    SDL_Surface* gameOverText = nullptr;
    countText[0] = TTF_RenderText_Solid(font, "1", textColor);
    countText[1] = TTF_RenderText_Solid(font, "2", textColor);
    countText[2] = TTF_RenderText_Solid(font, "3", textColor);
    gameOverText = TTF_RenderText_Solid(font, "Game Over", textColor);
    for (int i = 0; i < 3; ++i) {
        m_countDownText[i] = SDL_CreateTextureFromSurface(m_renderer, countText[i]);
        m_countDownTextDim[i] = std::make_tuple(countText[i]->w, countText[i]->h);
        SDL_FreeSurface(countText[i]);
    }
    m_gameOverText = SDL_CreateTextureFromSurface(m_renderer, gameOverText);
    m_gameOverTextDim = std::make_tuple(gameOverText->w, gameOverText->h);
    SDL_FreeSurface(gameOverText);
}

GameLayer::~GameLayer() {
    SDL_DestroyTexture(m_backgroundTexture);
    SDL_DestroyTexture(m_gameOverText);
    for (int i = 0; i < 3; ++i) {
        SDL_DestroyTexture(m_countDownText[i]);
    }
}

bool GameLayer::OnEvent(SDL_Event& event) {
    if (event.type == SDL_WINDOWEVENT) {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            m_registry.view<CameraComponent>().each([&event](auto& cameraComponent) {
                cameraComponent.width = event.window.data1;
                cameraComponent.height = event.window.data2;
                });
            break;
        }
    }

    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_w: m_controlState[ControlKey::Up] = true; return true;
        case SDLK_s: m_controlState[ControlKey::Down] = true; return true;
        case SDLK_a: m_controlState[ControlKey::Left] = true; return true;
        case SDLK_d: m_controlState[ControlKey::Right] = true; return true;
        case SDLK_SPACE: m_controlState[ControlKey::Fire] = true; return true;
        }
    }
    else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
        case SDLK_w: m_controlState[ControlKey::Up] = false; return true;
        case SDLK_s: m_controlState[ControlKey::Down] = false; return true;
        case SDLK_a: m_controlState[ControlKey::Left] = false; return true;
        case SDLK_d: m_controlState[ControlKey::Right] = false; return true;
        case SDLK_SPACE: m_controlState[ControlKey::Fire] = false; return true;
        }
    }

    return false;
}

void GameLayer::Update(uint32_t ticks) {
    switch (m_currentState) {
    case GameState::PreGame:
        m_pregameTimer -= ticks;
        if (m_pregameTimer <= 0) {
            StateTransition(GameState::Game);
        }
        break;
    case GameState::Game:

        auto& playerPositionComponent = m_registry.get<PositionComponent>(m_playerEntity);
        if (m_controlState[ControlKey::Up]) playerPositionComponent.y -= ticks * 0.1f;
        if (m_controlState[ControlKey::Down]) playerPositionComponent.y += ticks * 0.1f;
        if (m_controlState[ControlKey::Left]) playerPositionComponent.x -= ticks * 0.1f;
        if (m_controlState[ControlKey::Right]) playerPositionComponent.x += ticks * 0.1f;
        if (auto weaponComponent = m_registry.try_get<WeaponComponent>(m_playerEntity)) {
            weaponComponent->firing = m_controlState[ControlKey::Fire];
        }

        EnemySystem::Update(ticks, m_registry, m_renderer, *m_tileMap);
        WeaponSystem::Update(ticks, m_registry, m_renderer);
        VelocitySystem::Update(ticks, m_registry, *m_tileMap);
        LifetimeSystem::Update(ticks, m_registry);
        break;
    }
}

void GameLayer::Draw(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, m_backgroundTexture, nullptr, nullptr);

    int viewOffsetX = 0;
    int viewOffsetY = 0;
    int viewWidth = 0;
    int viewHeight = 0;
    bool cameraFound = false;
    m_registry.view<PositionComponent, CameraComponent>().each([&](auto const& positionComponent, auto const& cameraComponent) {
        if (!cameraFound && cameraComponent.active) {
            viewWidth = cameraComponent.width;
            viewHeight = cameraComponent.height;
            viewOffsetX = static_cast<int>(positionComponent.x) - viewWidth / 2;
            viewOffsetY = static_cast<int>(positionComponent.y) - viewHeight / 2;
            cameraFound = true;
        }
    });

    m_tileMap->Draw(m_renderer, viewOffsetX, viewOffsetY, GetLayerWidth(), GetLayerHeight());

    m_registry.view<PositionComponent, SpriteComponent>().each([&renderer, viewOffsetX, viewOffsetY](auto entity, auto const& position, auto const& sprite) {
        SDL_Rect destRect {
            static_cast<int>(position.x) - viewOffsetX - static_cast<int>(sprite.width / 2),
            static_cast<int>(position.y) - viewOffsetY - static_cast<int>(sprite.height / 2),
            static_cast<int>(sprite.width),
            static_cast<int>(sprite.height)
        };
        SDL_RenderCopyEx(renderer, sprite.texture, sprite.source_rect, &destRect, 0.0f, nullptr, sprite.flip);
    });

    if (m_currentState == GameState::PreGame) {
        int const index = static_cast<int>(std::floor(m_pregameTimer / 1000));
        SDL_Rect destRect{
            (GetLayerWidth() - std::get<0>(m_countDownTextDim[index])) / 2,
            (GetLayerHeight() - std::get<1>(m_countDownTextDim[index])) / 2,
            std::get<0>(m_countDownTextDim[index]),
            std::get<1>(m_countDownTextDim[index])
        };
        SDL_RenderCopy(renderer, m_countDownText[index], nullptr, &destRect);
    }
    else if (m_currentState == GameState::PostGame) {
        SDL_Rect destRect{
            (GetLayerWidth() - std::get<0>(m_gameOverTextDim)) / 2,
            (GetLayerHeight() - std::get<1>(m_gameOverTextDim)) / 2,
            std::get<0>(m_gameOverTextDim),
            std::get<1>(m_gameOverTextDim)
        };
        SDL_RenderCopy(renderer, m_gameOverText, nullptr, &destRect);
    }
}

void GameLayer::OnAddedToStack(LayerStack* layerStack) {
    m_layerStack = layerStack;
    StateTransition(GameState::PreGame);
}

void GameLayer::OnRemovedFromStack() {
    m_layerStack = nullptr;
}

void GameLayer::StateTransition(GameState newState) {
    m_currentState = newState;
    switch (m_currentState) {
    case GameState::PreGame: {
            m_pregameTimer = 3000;
            Setup();
        }
        break;
    case GameState::Game: {
            auto& cameraVelocityComponent = m_registry.get<VelocityComponent>(m_cameraEntity);
            auto& playerVelocityComponent = m_registry.get<VelocityComponent>(m_playerEntity);
            cameraVelocityComponent.x = playerVelocityComponent.x = 100.0f;
        }
        break;
    case GameState::PostGame: {
            auto& cameraVelocityComponent = m_registry.get<VelocityComponent>(m_cameraEntity);
            cameraVelocityComponent.x = 0.0f;
        }
        break;
    }
}

void GameLayer::Setup() {
    m_registry.clear();

    // camera
    m_cameraEntity = m_registry.create();
    auto& cameraPosition = m_registry.emplace<PositionComponent>(m_cameraEntity);
    auto& cameraDetails = m_registry.emplace<CameraComponent>(m_cameraEntity);
    auto& cameraVelocity = m_registry.emplace<VelocityComponent>(m_cameraEntity);

    cameraPosition.x = 0;
    cameraPosition.y = 0;
    
    cameraDetails.width = GetLayerWidth();
    cameraDetails.height = GetLayerHeight();
    cameraDetails.active = true;

    // player
    m_playerEntity = m_registry.create();
    m_registry.emplace<VelocityComponent>(m_playerEntity);
    auto& playerPositionComponent = m_registry.emplace<PositionComponent>(m_playerEntity);
    auto& playerCollisionComponent = m_registry.emplace<CollisionComponent>(m_playerEntity);
    auto& playerSpriteComponent = m_registry.emplace<SpriteComponent>(m_playerEntity);
    auto& playerWeaponComponent = m_registry.emplace<WeaponComponent>(m_playerEntity);

    SDL_Surface* playerImage = IMG_Load("ship003.png");
    playerSpriteComponent.texture = SDL_CreateTextureFromSurface(m_renderer, playerImage);
    SDL_FreeSurface(playerImage);
    playerSpriteComponent.width = 62;
    playerSpriteComponent.height = 32;

    playerPositionComponent.x = 0;
    playerPositionComponent.y = 0;

    playerCollisionComponent.width = 50;
    playerCollisionComponent.height = 19;
    playerCollisionComponent.flags = COLLISION_FLAG_PLAYER;
    playerCollisionComponent.flag_mask = COLLISION_FLAG_MAP | COLLISION_FLAG_ENEMY;
    playerCollisionComponent.on_collision = [this](entt::entity otherEntity) {
        // game over
        StateTransition(GameState::PostGame);
    };

    playerWeaponComponent.fire_delay = 300;
    playerWeaponComponent.velocity = 600.0f;

    // enemy spawner state (probably move this to a world state object)
    auto enemyBrain = m_registry.create();
    auto& enemySpawnerComponent = m_registry.emplace<EnemySpawnerComponent>(enemyBrain);
    enemySpawnerComponent.spawn_delay = 6000;
    enemySpawnerComponent.speed = 100.0f;
    enemySpawnerComponent.speed_increase = 5.0f;
}

int GameLayer::GetLayerWidth() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return m_layerStack->GetWidth();
}

int GameLayer::GetLayerHeight() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return m_layerStack->GetHeight();
}

void GameLayer::DestroySprite(entt::registry& registry, entt::entity entity) {
    auto& spriteComponent = m_registry.get<SpriteComponent>(entity);
    if (!spriteComponent.managed_texture) {
        SDL_DestroyTexture(spriteComponent.texture);
    }
}