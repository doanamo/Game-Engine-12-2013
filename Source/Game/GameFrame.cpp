#include "Precompiled.hpp"
#include "GameFrame.hpp"
#include "MenuFrame.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"

#include "Transform.hpp"
#include "Script.hpp"
#include "Render.hpp"

namespace
{
    // Projectile script.
    class ScriptProjectile : public ScriptObject
    {
    public:
        ScriptProjectile() :
            m_lifeTime(0.0f)
        {
        }

        void Execute(Entity* entity, float timeDelta)
        {
            assert(entity != nullptr);

            // Check if entity has needed components.
            Transform* transform = entity->GetComponent<Transform>();
            if(transform == nullptr) return;

            // Check if the projectile reached it's lifetime.
            m_lifeTime += timeDelta;

            if(m_lifeTime >= 1.0f)
            {
                entity->Destroy();
                return;
            }

            // Move entity to the right.
            glm::vec2 position = transform->GetPosition();
            position.x += 700.0f * timeDelta;
            transform->SetPosition(position);
        }

    private:
        float m_lifeTime;
    };

    void CreateProjectile(EntitySystem* entitySystem, glm::vec2 position)
    {
        
    }

    // Player script.
    class ScriptPlayer : public ScriptObject
    {
    public:
        ScriptPlayer() :
            m_shootTime(0.0f)
        {
        }

        void Execute(Entity* entity, float timeDelta)
        {
            assert(entity != nullptr);

            // Check if entity has needed components.
            Transform* transform = entity->GetComponent<Transform>();
            if(transform == nullptr) return;

            // Get keyboard state.
            const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

            // Shoot a projectile.
            m_shootTime = std::max(0.0f, m_shootTime - timeDelta);

            if(keyboardState[SDL_SCANCODE_SPACE])
            {
                if(m_shootTime == 0.0f)
                {
                    // Projectile factory method.
                    auto CreateProjectile = [](EntitySystem* entitySystem, glm::vec2 position)
                    {
                        // Create an entity.
                        Entity* entity = entitySystem->CreateEntity();

                        // Create components.
                        std::unique_ptr<Transform> transform(new Transform());
                        transform->SetPosition(position);
                        transform->SetScale(glm::vec2(1.0f, 1.0f));
                        transform->SetRotation(0.0f);
                        entity->InsertComponent(transform);

                        std::unique_ptr<Script> script(new Script());
                        script->SetScript(std::make_unique<ScriptProjectile>());
                        entity->InsertComponent(script);

                        std::unique_ptr<Render> render(new Render());
                        render->SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
                        entity->InsertComponent(render);
                    };

                    // Create a projectile entity.
                    CreateProjectile(entity->GetEntitySystem(), transform->GetPosition());

                    m_shootTime = 0.2f;
                }
            }

            // Create a direction vector.
            glm::vec2 direction(0.0f, 0.0f);

            if(keyboardState[SDL_SCANCODE_D])
            {
                direction.x = 1.0f;
            }

            if(keyboardState[SDL_SCANCODE_A])
            {
                direction.x = -1.0f;
            }

            if(keyboardState[SDL_SCANCODE_W])
            {
                direction.y = 1.0f;
            }

            if(keyboardState[SDL_SCANCODE_S])
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

    // Initialize the script system.
    if(!m_scriptSystem.Initialize())
        return false;

    m_entitySystem.RegisterSubsystem(&m_scriptSystem);

    // Initialize the render system.
    if(!m_renderSystem.Initialize())
        return false;

    m_entitySystem.RegisterSubsystem(&m_renderSystem);

    // Create entities.
    {
        // Create an entity.
        Entity* entity = m_entitySystem.CreateEntity();
        m_playerHandle = entity->GetHandle();

        // Create components.
        std::unique_ptr<Transform> transform(new Transform());
        transform->SetPosition(glm::vec2(50.0f, 275.0f));
        transform->SetScale(glm::vec2(1.0f, 1.0f));
        transform->SetRotation(0.0f);
        entity->InsertComponent(transform);

        std::unique_ptr<Script> script(new Script());
        script->SetScript(std::make_unique<ScriptPlayer>());
        entity->InsertComponent(script);

        std::unique_ptr<Render> render(new Render());
        render->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        entity->InsertComponent(render);
    }

    for(int i = 0; i < 4; ++i)
    {
        // Create an entity.
        Entity* entity = m_entitySystem.CreateEntity();

        // Create components.
        std::unique_ptr<Transform> transform(new Transform());
        transform->SetPosition(glm::vec2(900.0f, 100.0f + i * 100.0f));
        transform->SetScale(glm::vec2(1.0f, 1.0f));
        transform->SetRotation(0.0f);
        entity->InsertComponent(transform);

        std::unique_ptr<Render> render(new Render());
        render->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        entity->InsertComponent(render);
    }

    // Success!
    m_initialized = true;

    return true;
}

void GameFrame::Cleanup()
{
    m_playerHandle = EntityHandle();

    m_entitySystem.Cleanup();
    m_scriptSystem.Cleanup();
    m_renderSystem.Cleanup();

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
        return true;
    }

    return false;
}

void GameFrame::Update(float dt)
{
    // Update entities.
    m_entitySystem.Update(dt);
}

void GameFrame::Draw()
{
    // Clear the screen.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepth(1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render entities.
    m_renderSystem.Draw();
}
