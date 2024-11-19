#include "../../include/Game/Scene.hpp"

Scene *Scene::instance = nullptr;

Scene::Scene()
{
    initSPH(GLOBAL::amount);
}

Scene::~Scene()
{
    delete instance;
}

Scene *Scene::getInstance()
{
    if (!instance)
        instance = new Scene();

    return instance;
}

void Scene::update(float dt)
{
}

void Scene::update(sf::Vector2f &mousePos, float dt)
{
    pushParticles(mousePos);
    computeDensityPressure();
    computeForce();
    integrate();
}

void Scene::render(sf::RenderTarget *target)
{
    for (Circle &particle : particles)
        particle.render(target);
}

void Scene::initSPH(int amount)
{
    for (int i = 0; i < amount; i++)
    {
        float x = randomFloat(0.f, GLOBAL::window_width);
        float y = randomFloat(0.f, GLOBAL::window_height);

        Circle particle = Circle(1.f, sf::Vector2f(x, y));
        particles.push_back(particle);
    }
}

void Scene::computeDensityPressure()
{
    for (Circle &pi : particles)
    {
        pi.density = 0.f;
        for (Circle &pj : particles)
        {
            sf::Vector2f rij = pj.property.getPosition() - pi.property.getPosition();
            float r2 = Math::_dot(rij, rij);

            if (r2 < Constants::HSQ)
            {
                pi.density += Constants::MASS * Constants::POLY6 * pow(Constants::HSQ - r2, 3.f);
            }
        }
        pi.pressure = Constants::GAS_CONST * (pi.density - Constants::REST_DENS);
    }
}

void Scene::computeForce()
{
    for (Circle &pi : particles)
    {
        pi.force = sf::Vector2f(0.f, 0.f);
        sf::Vector2f fpress(0.f, 0.f);
        sf::Vector2f fvisc(0.f, 0.f);
        for (Circle &pj : particles)
        {
            if (&pi == &pj)
                continue;

            sf::Vector2f rij = pj.property.getPosition() - pi.property.getPosition();
            float r = Math::_length(rij);

            if (r < Constants::H)
            {
                fpress += -Math::_normalize(rij) * Constants::MASS * (pi.pressure + pj.pressure) / (2.f * pj.density) * Constants::SPIKY_GRAD * pow(Constants::H - r, 3.f);
                fvisc += Constants::VISC * Constants::MASS * (pj.linearVelocity - pi.linearVelocity) / pj.density * Constants::VISC_LAP * (Constants::H - r);
            }
        }

        sf::Vector2f fgrav = Constants::GRAVITY * Constants::MASS / pi.density;
        pi.force = fpress + fvisc + fgrav;
    }
}

void Scene::integrate()
{
    for (Circle &p : particles)
    {
        p.linearVelocity += (p.force / p.density) * Constants::DT;
        p.property.move(p.linearVelocity * Constants::DT);

        // boundary
        if (p.property.getPosition().x - Constants::EPS < 0.f)
        {
            p.linearVelocity.x *= Constants::BOUND_DAMPING;
            p.property.setPosition(sf::Vector2f(Constants::EPS, p.property.getPosition().y));
        }

        if (p.property.getPosition().x + Constants::EPS > GLOBAL::window_width)
        {
            p.linearVelocity.x *= Constants::BOUND_DAMPING;
            p.property.setPosition(sf::Vector2f(GLOBAL::window_width - Constants::EPS, p.property.getPosition().y));
        }

        if (p.property.getPosition().y - Constants::EPS < 0.f)
        {
            p.linearVelocity.y *= Constants::BOUND_DAMPING;
            p.property.setPosition(sf::Vector2f(p.property.getPosition().x, Constants::EPS));
        }

        if (p.property.getPosition().y + Constants::EPS > GLOBAL::window_height)
        {
            p.linearVelocity.y *= Constants::BOUND_DAMPING;
            p.property.setPosition(sf::Vector2f(p.property.getPosition().x, GLOBAL::window_height - Constants::EPS));
        }
    }
}

void Scene::pushParticles(sf::Vector2f &mousePos)
{
    for (Circle &p : particles)
    {
        if (Math::_length(mousePos - p.property.getPosition()) > Constants::H)
            continue;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            p.property.move(sf::Vector2f(0.f, 1.f) * -3.f);
        }
    }
}