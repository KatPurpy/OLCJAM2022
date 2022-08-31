#pragma once
namespace Constants
{
    struct GpuBufferVertex
    {

        float x, y, z;			  //	4	*	3	=	12
        unsigned char r, g, b, a; //	4
        unsigned short u, v;	  //	2*2	=	4
                                //	12	+	4	+	4	=	20

    };

    using GpuBufferIndex = uint32_t;

    inline float fireLifetime = 10;
    inline float fireSmokeThreshold = 7;
    inline void* particleFireTag = (void*)(0xBABEBABE);
    inline void* particleWaterTag = (void*)(0xBABABABA);
    inline void* particleSmokeTag = (void*)(0xD00DD00D);
    enum PhysicsCategory
    {
        PC_INVALID = 0,
        PC_PLAYERUNIT = 1 << 1,
        PC_GROUND = 1 << 2,
        PC_BUILDING = 1 << 3,
        PC_ANIMAL = 1 << 4,
        PC_FOREST = 1 << 5,
        PC_ANIMALSENSOR = 1 << 6,
        PC_WATER = 1 << 7,
        PC_CLOUD = 1 << 8
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
