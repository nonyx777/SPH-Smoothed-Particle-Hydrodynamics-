#pragma once

#include "../GameObject.hpp"
#include "../Entities/Entities.hpp"
#include "../Util/Gizmo.hpp"
#include "../Util/Grid.hpp"
#include "../Globals.hpp"
#include "../Util/Collision.hpp"
#include <random>

namespace Constants
{
    //solver parameters
    const sf::Vector2f GRAVITY = sf::Vector2f(0.f, 1.f);
    const float REST_DENS = 10.f;
    const float GAS_CONST = 500.f;
    const float H = 16.f;
    const float HSQ = H * H;
    const float MASS = 2.5f;
    const float VISC = 50.f;
    const float DT = 0.007f;
    //smoothing kernels
    const float POLY6 = 4.f / (M_PI * pow(H, 8.f));
    const float SPIKY_GRAD = -10.f / (M_PI * pow(H, 5.f));
    const float VISC_LAP = 40.f / (M_PI * pow(H, 5.f));
    //simulation parameters
    const float EPS = H;
    const float BOUND_DAMPING = -0.5f;
};

class Scene : public GameObject
{
private:
    static Scene *instance;
    std::vector<Circle> particles;

private:
    Scene();
    ~Scene();

public:
    // Delete copy constructor and assignment operator to prevent cloning
    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    static Scene *getInstance();

    void update(float dt) override;
    void update(sf::Vector2f &vec, float dt);
    void render(sf::RenderTarget *target) override;

    void initSPH(int amount);
    void integrate();
    void computeDensityPressure();
    void computeForce();
    void pullParticles(sf::Vector2f &mousePos);

    // helper
    float randomFloat(const float min, const float max)
    {
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(generator);
    }
    void clampVector(sf::Vector2f &vec, float maxSpeed)
    {
        if(Math::_length(vec) < maxSpeed)
            return;
        sf::Vector2f norm = Math::_normalize(vec);
        vec = norm * maxSpeed;
    }
};