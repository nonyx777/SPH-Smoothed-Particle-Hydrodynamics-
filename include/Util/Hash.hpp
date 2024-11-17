#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include "../Entities/Entities.hpp"

class Hash
{
public:
    uint spacing;
    uint tableSize;
    std::vector<uint> countTable;
    std::vector<uint> particleTable;
    std::vector<uint> queryIds;
    uint querySize;
    std::vector<Circle> particles;

public:
    Hash();
    Hash(uint maxObjects, uint spacing, std::vector<Circle> &particles);

public:
    sf::Vector2i gridCoords(sf::Vector2f &pos);
    uint hashFunc(sf::Vector2i &pos);
    uint hashFunc(int x, int y);
    uint hashPos(sf::Vector2f pos);
    void populateTables();
    void queryParticles(uint particleId, float maxDist);
    void resetTables();

//utility functions
public:
    void fill(std::vector<uint> &table)
    {
        for(int i = 0; i < table.size(); i++)
            table[i] = 0;
    }
};