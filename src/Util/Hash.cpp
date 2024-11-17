#include "../../include/Util/Hash.hpp"

Hash::Hash()
{
    //...
}

Hash::Hash(uint maxObjects, uint spacing, std::vector<Circle> &particles)
{
    this->spacing = spacing;
    this->tableSize = 2 * maxObjects;
    this->countTable.resize(this->tableSize + 1);
    this->fill(this->countTable);
    this->particleTable.resize(maxObjects);
    this->fill(this->particleTable);
    this->queryIds.resize(maxObjects);
    this->querySize = 0;
    this->particles = particles;
}

sf::Vector2i Hash::gridCoords(sf::Vector2f &pos)
{
    sf::Vector2i gridcoord = sf::Vector2i(
        floor(pos.x / this->spacing),
        floor(pos.y / this->spacing));

    return gridcoord;
}

uint Hash::hashFunc(sf::Vector2i &pos)
{
    int h = (pos.x * 92837111) ^ (pos.y * 689287499);
    return abs(h) % (tableSize / 2);
}

uint Hash::hashFunc(int x, int y)
{
    int h = (x * 92837111) ^ (y * 689287499);
    return abs(h) % (tableSize / 2);
}

uint Hash::hashPos(sf::Vector2f pos)
{
    sf::Vector2i gridcoord = this->gridCoords(pos);
    return hashFunc(gridcoord);
}

void Hash::populateTables()
{
    this->resetTables();

    // count the particles
    for (Circle &particle : this->particles)
    {
        uint i = this->hashPos(particle.property.getPosition());
        this->countTable[i]++;
    }

    uint sum = 0;
    // partial sum
    for (uint i = 0; i < this->countTable.size(); i++)
    {
        sum += this->countTable[i];
        this->countTable[i] = sum;
    }

    // fill the particle table
    for (int i = 0; i < this->particles.size(); i++)
    {
        uint index = this->hashPos(this->particles[i].property.getPosition());
        this->countTable[index]--;
        this->particleTable[this->countTable[index]] = i;
    }
}

void Hash::queryParticles(uint particleId, float maxDist)
{
    int x0 = floor((this->particles[particleId].property.getPosition().x - maxDist) / this->spacing);
    int y0 = floor((this->particles[particleId].property.getPosition().y - maxDist) / this->spacing);

    int x1 = floor((this->particles[particleId].property.getPosition().x + maxDist) / this->spacing);
    int y1 = floor((this->particles[particleId].property.getPosition().y + maxDist) / this->spacing);

    this->querySize = 0;

    for (int xi = x0; xi <= x1; xi++)
    {
        for (int yi = y0; yi <= y1; yi++)
        {
            int h = this->hashFunc(xi, yi);
            int start = this->countTable[h];
            int end = this->countTable[h + 1];

            for (int i = start; i < end; i++)
            {
                this->queryIds[this->querySize] = this->particleTable[i];
                this->querySize++;
            }
        }
    }
}

void Hash::resetTables()
{
    fill(countTable);
    fill(particleTable);
    fill(queryIds);
}