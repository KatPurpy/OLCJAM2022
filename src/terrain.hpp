#pragma once
#include "Box2D.h"
#include "stb_ds.h"
#include "HandmadeMath.h"
#include "constants.hpp"
#include "earcut.hpp"
#include "BZZRE/subsystems/graphics/vibuf.tcc"
//#include "delaunator.hpp"
using TerrainVertexBuffer = BZZRE::Graphics::VIBuf<Constants::GpuBufferVertex, Constants::GpuBufferIndex>;
class Terrain
{
	struct WriteInfo
	{
		float localoffset;
		int begin;
		int end;

		WriteInfo(Terrain* terrain, float begin_x, int count)
		{
			localoffset = begin_x - terrain->m_offset.X;
			begin = localoffset / terrain->m_distBetweenVecs;
			if(begin < 0)
			{
				begin = 0;
			}
			end = begin + count;
			printf("BEGIN %i\n", begin);

			if(end >= arrlen(terrain->m_heightmap))
			{
				end = arrlen(terrain->m_heightmap);
			}
		}
	};
	friend struct WriteInfo;

  protected:
	b2World* m_world;
	float* m_heightmap;
	sg_color* m_heigthmap_colors;
	float m_distBetweenVecs;
	b2Body** chunkBodies;
	hmm_v2 m_offset;
	float m_max_depth;

  public:
	TerrainVertexBuffer* buffer;
	bool* m_chunk_is_dirty;
	float chunk_length;
	float m_num_chunks;
	int m_verts_per_chunk;

	float
	GetDistance(int vertex)
	{
		return (float)vertex * m_distBetweenVecs;
	}

	int
	GetVertsForDistance(float distance)
	{
		return distance / m_distBetweenVecs;
	}

	float
	GetHeightAt(float place)
	{
		int localoffset = place - m_offset.X;
		int index = localoffset / m_distBetweenVecs;
		return m_heightmap[index];
	}

	void
	Initialize(b2World* world, float start_x, float start_y, float max_depth, float length, int num_verts,
			   int verts_per_chunk)
	{
		assert(max_depth > 0);
		m_max_depth = max_depth;
		m_offset = { start_x, start_y };
		assert(world);
		m_world = world;
		assert(num_verts % verts_per_chunk == 0);
		m_num_chunks = num_verts / verts_per_chunk;

		m_distBetweenVecs = length / num_verts;
		m_verts_per_chunk = verts_per_chunk;
		assert(m_verts_per_chunk % 8 == 0);
		this->chunk_length = m_distBetweenVecs * verts_per_chunk;
		arrsetlen(m_chunk_is_dirty, m_num_chunks);
		arrsetlen(m_heightmap, num_verts);
		arrsetlen(chunkBodies, m_num_chunks);
		memset(chunkBodies, 0, sizeof(b2Body*) * m_num_chunks);
	}
	void
	RegenerateChunks()
	{
		for(int chunk = 0; chunk < m_num_chunks; chunk++)
		{
			if(!m_chunk_is_dirty[chunk])
				continue;
			printf("REGENERATE CHUNK %i\n", chunk);
			b2Vec2* points = NULL;
			int chunnkverts_begin = chunk * m_verts_per_chunk;
			int chunkverts_end = (chunnkverts_begin + m_verts_per_chunk);
			float chunkworldpos_x = chunnkverts_begin * m_distBetweenVecs;
			float chunkworldposend_x = chunkverts_end * m_distBetweenVecs;
			// last chunk must not connect to first point of the next one as it simply does not exist
			if(chunk != (m_num_chunks - 1))
			{
				chunkverts_end++;
			}
			for(int i = chunnkverts_begin; i < chunkverts_end; i++)
			{
				b2Vec2 point;
				point.x = (i - chunnkverts_begin) * m_distBetweenVecs;
				point.y = m_heightmap[i];
				arrpush(points, point);
			}
			if(chunkBodies[chunk])
				m_world->DestroyBody(chunkBodies[chunk]);
			b2BodyDef bodydef;
			bodydef.type = b2_staticBody;
			bodydef.position.Set(m_offset.X + chunkworldpos_x, m_offset.Y);
			b2Body* body = m_world->CreateBody(&bodydef);

			for(int i = 0; i < m_verts_per_chunk - 1; i += 4)
			{
				int lastPointIdx = i + 4;
				if(chunk == (m_num_chunks - 1) && (i == (m_verts_per_chunk - 4)))
				{
					lastPointIdx--;
				}
				// BUG: when using wireframe rendering in debug, the bottom lines of the polys are not renderered, if
				// something breaks, blame this
				b2Vec2 quad[7] = {
					points[i],
					points[i + 1],
					points[i + 2],
					points[i + 3],
					points[lastPointIdx],
					{ (lastPointIdx)*m_distBetweenVecs, -m_max_depth - 5 },
					{ i * m_distBetweenVecs, -m_max_depth - 5 },
				};

				b2PolygonShape shape;
				shape.Set(quad, 7);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &shape;
				fixtureDef.filter.categoryBits = Constants::PC_GROUND;
				fixtureDef.filter.maskBits = Constants::GROUND_COLLIDESWITH;
				body->CreateFixture(&fixtureDef);
			}

			chunkBodies[chunk] = body;
			m_chunk_is_dirty[chunk] = false;

			// arrfree(doubles_ptr);
			arrfree(points);
		}
	}

	// offset relative to world
	void
	WriteHeightmapProcedural(float (*heightFunc)(int, float), int count, float begin_x)
	{
		WriteInfo info(this, begin_x, count);

		int i = 0;
		for(int vertex = info.begin; vertex < info.end; vertex++, i++)
		{
			m_heightmap[vertex] = heightFunc(vertex, i * m_distBetweenVecs);
			m_chunk_is_dirty[vertex / m_verts_per_chunk] = true;
		}
	}

	// offset relative to world
	void
	WriteHeightmap(float* data, int count, float begin_x)
	{
		// HACK: this ain't threadsafe!
		static float* tmp_data;
		tmp_data = data;
		WriteHeightmapProcedural([](int, float) { return *(tmp_data++); }, count, begin_x);
	}

	void
	WriteHeightmapProceduralOffset(float (*heightFunc)(int, float), int count, float begin_x)
	{
		// HACK: this ain't threadsafe!
		static float* tmp_heightmap;
		static float (*tmp_heightFunc)(int, float);
		tmp_heightmap = m_heightmap;
		tmp_heightFunc = heightFunc;
		WriteHeightmapProcedural([](int vertex_index, float x)
								 { return tmp_heightmap[vertex_index] + tmp_heightFunc(vertex_index, x); },
								 count, begin_x);
	}

	void
	WriteHeightMapOffset(float* data, int count, float begin_x)
	{
		// HACK: this ain't threadsafe!
		static float* tmp_data;
		static float* tmp_heightmap;
		tmp_heightmap = m_heightmap;
		tmp_data = data;
		WriteHeightmapProceduralOffset([](int vertex_index, float) { return *(tmp_data++); }, count, begin_x);
	}

	void
	Generate(b2World* world, float (*heightFunc)(int, float))
	{
		WriteHeightmapProcedural(heightFunc, arrlen(m_heightmap), m_offset.X);
		RegenerateChunks();
	}

	void BuildBuffer()
	{
		Constants::GpuBufferVertex* verticies = nullptr;
		
	}
};