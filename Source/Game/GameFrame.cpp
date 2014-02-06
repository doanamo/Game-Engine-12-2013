#include "Precompiled.hpp"
#include "GameFrame.hpp"
#include "MenuFrame.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"

#include "InputState.hpp"
#include "Script.hpp"

#include "EntitySystem.hpp"
#include "CollisionSystem.hpp"
#include "ScriptSystem.hpp"
#include "RenderSystem.hpp"

#include "TransformComponent.hpp"
#include "CollisionComponent.hpp"
#include "InputComponent.hpp"
#include "ScriptComponent.hpp"
#include "RenderComponent.hpp"

namespace
{
    // Projectile script.
    class ScriptProjectile : public Script
    {
    public:
        ScriptProjectile() :
            m_lifeTime(0.0f)
        {
        }

        void OnUpdate(EntityHandle entity, float timeDelta)
        {
            // Check if entity has needed components.
            TransformComponent* transform = Game::TransformComponents().Lookup(entity);
            if(transform == nullptr) return;

            // Check if the projectile reached it's lifetime.
            m_lifeTime += timeDelta;

            if(m_lifeTime >= 1.0f)
            {
                Game::EntitySystem().DestroyEntity(entity);
                return;
            }

            // Move entity to the right.
            glm::vec2 position = transform->GetPosition();
            position.x += 700.0f * timeDelta;
            transform->SetPosition(position);
        }

        void OnCollision(CollisionObject& object, CollisionObject& other)
        {
            assert(object.collision != nullptr);
            assert(other.collision != nullptr);

            // Disable collisions.
            object.collision->Disable();
            other.collision->Disable();

            // Destroy both entities.
            Game::EntitySystem().DestroyEntity(object.entity);
            Game::EntitySystem().DestroyEntity(other.entity);
        }

    private:
        float m_lifeTime;
    };

    // Player script.
    class ScriptPlayer : public Script
    {
    public:
        ScriptPlayer() :
            m_shootTime(0.0f)
        {
        }

        void OnUpdate(EntityHandle entity, float timeDelta)
        {
            // Check if entity has needed components.
            TransformComponent* transform = Game::TransformComponents().Lookup(entity);
            if(transform == nullptr) return;

            InputComponent* input = Game::InputComponents().Lookup(entity);
            if(input == nullptr) return;

            // Shoot a projectile.
            m_shootTime = std::max(0.0f, m_shootTime - timeDelta);

            if(input->GetState()->IsKeyDown(SDL_SCANCODE_SPACE))
            {
                if(m_shootTime == 0.0f)
                {
                    // Projectile factory method.
                    auto CreateProjectile = [](glm::vec2 position)
                    {
                        EntityHandle entity = Game::EntitySystem().CreateEntity();

                        TransformComponent* transform = Game::TransformComponents().Create(entity);
                        transform->SetPosition(position);
                        transform->SetScale(glm::vec2(1.0f, 1.0f));
                        transform->SetRotation(0.0f);

                        CollisionComponent* collision = Game::CollisionComponents().Create(entity);
                        collision->SetBoundingBox(glm::vec4(0.0f, 0.0f, 50.0f, 50.0f));

                        ScriptComponent* script = Game::ScriptComponents().Create(entity);
                        script->SetScript(std::make_shared<ScriptProjectile>());

                        RenderComponent* render = Game::RenderComponents().Create(entity);
                        render->SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
                    };

                    // Create a projectile entity.
                    CreateProjectile(transform->GetPosition());

                    m_shootTime = 0.34f;
                }
            }

            // Create a direction vector.
            glm::vec2 direction(0.0f, 0.0f);

            if(input->GetState()->IsKeyDown(SDL_SCANCODE_D))
            {
                direction.x = 1.0f;
            }

            if(input->GetState()->IsKeyDown(SDL_SCANCODE_A))
            {
                direction.x = -1.0f;
            }

            if(input->GetState()->IsKeyDown(SDL_SCANCODE_W))
            {
                direction.y = 1.0f;
            }

            if(input->GetState()->IsKeyDown(SDL_SCANCODE_S))
            {
                direction.y = -1.0f;
            }

            // Update player position.
            if(direction != glm::vec2(0.0f))
            {
                glm::vec2 position = transform->GetPosition();
                position += glm::normalize(direction) * 400.0f * timeDelta;
                transform->SetPosition(position);
            }
        }

    public:
        float m_shootTime;
    };
}

GameFrame::GameFrame() :
    m_initialized(false)
{
}

GameFrame::~GameFrame()
{
    Cleanup();
}

bool GameFrame::Initialize()
{
    Cleanup();

    // Setup emergency cleanup.
    auto EmergenyCleanup = MakeScopeGuard([&]()
    {
        // Cleanup if initialization failed.
        if(!m_initialized)
        {
            Cleanup();
        }
    });

    // Create entities.
    {
        EntityHandle entity = Game::EntitySystem().CreateEntity();

        TransformComponent* transform = Game::TransformComponents().Create(entity);
        transform->SetPosition(glm::vec2(50.0f, 275.0f));
        transform->SetScale(glm::vec2(1.0f, 1.0f));
        transform->SetRotation(0.0f);

        InputComponent* input = Game::InputComponents().Create(entity);
        input->SetStateReference(&Game::InputState());

        ScriptComponent* script = Game::ScriptComponents().Create(entity);
        script->SetScript(std::make_shared<ScriptPlayer>());

        RenderComponent* render = Game::RenderComponents().Create(entity);
        render->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    }

    for(int x = 0; x < 10; ++x)
    for(int y = 0; y < 8; ++y)
    {
        EntityHandle entity = Game::EntitySystem().CreateEntity();

        TransformComponent* transform = Game::TransformComponents().Create(entity);
        transform->SetPosition(glm::vec2(320.0f + x * 70.0f, 20.0f + y * 70.0f));
        transform->SetScale(glm::vec2(1.0f, 1.0f));
        transform->SetRotation(0.0f);

        CollisionComponent* collision = Game::CollisionComponents().Create(entity);
        collision->SetBoundingBox(glm::vec4(0.0f, 0.0f, 50.0f, 50.0f));

        RenderComponent* render = Game::RenderComponents().Create(entity);
        render->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    // Success!
    m_initialized = true;

    return true;
}

void GameFrame::Cleanup()
{
    m_initialized = false;
}

bool GameFrame::Process(const SDL_Event& event)
{
    switch(event.type)
    {
    case SDL_KEYDOWN:
        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            Game::StateMachine().ChangeState(&Game::MenuFrame());
        }
        break;
    }

    // Process input events.
    Game::InputState().Process(event);

    return false;
}

void GameFrame::Update(float timeDelta)
{
    // Process entity commands.
    Game::EntitySystem().ProcessCommands();

    // Update collision system.
    Game::CollisionSystem().Update();

    // Update script system.
    Game::ScriptSystem().Update(timeDelta);

    // Update render system.
    Game::RenderSystem().Update();
}

void GameFrame::Draw()
{
    // Clear the screen.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepth(1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render entities.
    Game::RenderSystem().Draw();
}
