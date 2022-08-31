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

    //warning: lifetime during fire creation does not respect this value
    inline float fireLifetime = 10;
    //141 - color at which the fire is not of fire color
    //256 - color table length (yes it's all hardcoded)
    //10 - fire lifetime
    inline float fireSmokeThreshold = 141./256. * fireLifetime;
    inline void* particleFireTag = (void*)(0x1);
    inline void* particleWaterTag = (void*)(0x2);
    inline void* particleSmokeTag = (void*)(0x3);
    inline void* maxParticleTag = (void*)(0x4);
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
        PC_CLOUD = 1 << 8,
        PC_FIRE = 1 << 9,
        PC_BURNING = 1 << 10
    };

    enum COLLIDE
    {
        GROUND_COLLIDESWITH = PC_PLAYERUNIT | PC_ANIMAL | PC_BUILDING | PC_FOREST,
        PLAYERWEATHER_COLLIDESWITH = PC_PLAYERUNIT | PC_ANIMAL | PC_GROUND | PC_BUILDING | PC_FOREST,
        ANIMAL_COLLIDESWITH = PC_PLAYERUNIT | PC_ANIMALSENSOR | PC_GROUND | PC_FOREST,
        BUILDING_COLLIDESSWITH = PC_PLAYERUNIT | PC_GROUND | PC_FOREST, //third one is to make sure fire spreads to forest as well
        PLAYERUNIT_COLLIDESWITH = PC_GROUND,
        ANIMALSENSOR_COLLIDESWITH = PC_ANIMAL,
        FIRE_COLLIDESWITH = ~PC_BURNING
    };
} 
