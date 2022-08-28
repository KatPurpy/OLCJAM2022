#pragma once
#include "Box2D.h"
#include "stb_ds.h"
#include "HandmadeMath.h"
#include "constants.hpp"
class Terrain {
  struct WriteInfo
  {
    float localoffset;
    int begin;
    int end;

    WriteInfo(Terrain* terrain, float begin_x, int count)
    {
      localoffset = begin_x - terrain->m_offset.X;
      begin = localoffset / terrain->m_distBetweenVecs;
      if(begin < 0) begin = 0;
      end = begin + count;
      printf("BEGIN %i\n",begin);
      assert(begin < arrlen(terrain->m_heightmap));
      assert(end <= arrlen(terrain->m_heightmap));
    }
  };
  friend struct WriteInfo;
  protected:
    b2World* m_world;
    float* m_heightmap;
    float m_distBetweenVecs;
    b2Body** chunkBodies;
    hmm_v2 m_offset;

  public:
   
    bool* m_chunk_is_dirty;
    float chunk_length;
    float m_num_chunks;
    int m_verts_per_chunk;
  void Initialize(b2World* world, float start_x, float start_y, float length, int num_verts, int verts_per_chunk)
  {
      m_offset = {start_x, start_y};
      assert(world);
      m_world = world;
      assert(num_verts % verts_per_chunk == 0);
      m_num_chunks = num_verts / verts_per_chunk;
      
      m_distBetweenVecs = length / num_verts;
      m_verts_per_chunk = verts_per_chunk;
      this->chunk_length = m_distBetweenVecs * verts_per_chunk;
      arrsetlen(m_chunk_is_dirty, m_num_chunks);
      arrsetlen(m_heightmap, num_verts);
      arrsetlen(chunkBodies, m_num_chunks);
      memset(chunkBodies, 0, sizeof(b2Body*) * m_num_chunks);
  }
  void RegenerateChunks()
  {
    for (int chunk = 0; chunk < m_num_chunks; chunk++) 
    {
        if(!m_chunk_is_dirty[chunk]) continue;
        printf("REGENERATE CHUNK %i\n", chunk);
        b2Vec2* points = NULL;
        int chunnkverts_begin = chunk * m_verts_per_chunk;
        int chunkverts_end = (chunnkverts_begin + m_verts_per_chunk);
        if(chunk != (m_num_chunks - 1)) //last chunk must not connect to first point of the next one as it simply does not exist
        {
          chunkverts_end++;
        }
        for(int i = chunnkverts_begin; i < chunkverts_end; i++)
        {
            b2Vec2 point;
            point.x = i * m_distBetweenVecs;
            point.y = m_heightmap[i];
            arrpush(points, point);
        }
        if(chunkBodies[chunk]) m_world->DestroyBody(chunkBodies[chunk]);
        b2BodyDef bodydef;
        bodydef.type = b2_staticBody;
        bodydef.position.Set(m_offset.X, m_offset.Y);
        b2Body* body = m_world->CreateBody(&bodydef);
        
        b2ChainShape chainShape;
        chainShape.CreateChain(points, arrlen(points));
        arrfree(points);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &chainShape;
        fixtureDef.filter.categoryBits = Constants::PC_GROUND;
                fixtureDef.filter.maskBits = Constants::GROUND_COLLIDESWITH;
        body->CreateFixture(&fixtureDef);
        chunkBodies[chunk] = body;
        m_chunk_is_dirty[chunk] = false;
    }
  }

   //offset relative to world
  void WriteHeightmapProcedural(float (*heightFunc)(int,float), int count, float begin_x)
  {
    WriteInfo info(this, begin_x, count);
    
    int i = 0;
    for(int vertex = info.begin; vertex < info.end; vertex++, i++)
    {
      m_heightmap[vertex] = heightFunc(vertex, i * m_distBetweenVecs);
      m_chunk_is_dirty[vertex / m_verts_per_chunk] = true;
    }
  }

  //offset relative to world
  void WriteHeightmap(float* data, int count, float begin_x)
  {
    //HACK: this ain't threadsafe!
    static float* tmp_data;
    tmp_data = data;
    WriteHeightmapProcedural([](int,float){return *(tmp_data++); }, count, begin_x);
  }

  void WriteHeightMapOffset(float* data, int count, float begin_x)
  {
    //HACK: this ain't threadsafe!
    static float* tmp_data;
    static float* tmp_heightmap;
    tmp_heightmap = m_heightmap;
    tmp_data = data;
    WriteHeightmapProcedural([](int vertex_index,float){return tmp_heightmap[vertex_index] + *(tmp_data++); }, count, begin_x);
  }



  void Generate(b2World* world, float (*heightFunc)(int,float)) 
    {
      WriteHeightmapProcedural(heightFunc, arrlen(m_heightmap), m_offset.X);
      RegenerateChunks();
  }
};