namespace LevelData
{
    struct CMD_InitTerrain
    {
        float length;
        int verts_per_chunk;
        float bumpiness;
    };
    
    struct CMD_GenPit
    {
        float x;
        float width;
        float depth;
        bool fillwater;
    }; 

    struct CMD_SpawnAnimal
    {
        float x;
    };

    struct CMD_SpawnSun
    {
        float x, y;
    };
} // namespace LevelData
