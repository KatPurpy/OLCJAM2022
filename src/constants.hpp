namespace Constants
{
    enum PhysicsCategories
    {
        PC_PLAYERWEATHER = 1 << 1,
        PC_GROUND = 1 << 2,
        PC_BUILDING = 1 << 3,
        PC_ANIMAL = 1 << 4,
        PC_FOREST = 1 << 5,
        PC_PLAYERUNIT = 1 << 6,
    };

    enum COLLIDE
    {
        GROUND_COLLIDESWITH = PC_PLAYERWEATHER | PC_ANIMAL | PC_BUILDING | PC_FOREST,
        PLAYERWEATHER_COLLIDESWITH = PC_PLAYERWEATHER | PC_ANIMAL | PC_GROUND | PC_BUILDING | PC_FOREST,
        ANIMAL_COLLIDESSWITH = PC_PLAYERWEATHER | PC_GROUND | PC_FOREST,
        BUILDING_COLLIDESSWITH = PC_PLAYERWEATHER | PC_GROUND | PC_FOREST, //third one is to make sure fire spreads to forest as well
        PLAYERUNIT_COLLIDESWITH = PC_GROUND
    };
} 
