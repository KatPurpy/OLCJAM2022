#pragma once
namespace Constants
{
    enum PhysicsCategory
    {
        PC_INVALID = 0,
        PC_PLAYERUNIT = 1 << 1,
        PC_GROUND = 1 << 2,
        PC_BUILDING = 1 << 3,
        PC_ANIMAL = 1 << 4,
        PC_FOREST = 1 << 5,
        PC_ANIMALSENSOR = 1 << 6,
    };

    enum COLLIDE
    {
        GROUND_COLLIDESWITH = PC_PLAYERUNIT | PC_ANIMAL | PC_BUILDING | PC_FOREST,
        PLAYERWEATHER_COLLIDESWITH = PC_PLAYERUNIT | PC_ANIMAL | PC_GROUND | PC_BUILDING | PC_FOREST,
        ANIMAL_COLLIDESWITH = PC_PLAYERUNIT | PC_GROUND | PC_FOREST,
        BUILDING_COLLIDESSWITH = PC_PLAYERUNIT | PC_GROUND | PC_FOREST, //third one is to make sure fire spreads to forest as well
        PLAYERUNIT_COLLIDESWITH = PC_GROUND,
        ANIMALSENSOR_COLLIDESWITH = PC_ANIMAL
    };
} 
