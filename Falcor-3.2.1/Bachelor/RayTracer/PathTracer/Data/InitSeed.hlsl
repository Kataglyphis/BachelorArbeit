//Initialization of seeds for ray tracing

//number of blocks we want to consider for our calculation
#define BLOCK_SIZE pow(DIMENSION, 2)
#define DIMENSION_SIZE 8

//necessary information about frame
int width;
int height;
int number_blocks_per_row;

//read/write buffer for storing our seeds
RWStructuredBuffer<uint> seed_Texture;

//function for generate random numbers
// http://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

//generating seeds in DIMENSION_SIZE * DIMENSION_SIZE blocks;
[numthreads(DIMENSION_SIZE, DIMENSION_SIZE, 1)]
void InitSeeds(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID) {
    uint wang_index = group_Index + (group_ID.y * number_blocks_per_row * BLOCK_SIZE  
                                        + group_ID.x * BLOCK_SIZE);
    seed_Texture[width * thread_ID.y + thread_ID.x] = wang_hash(wang_index);
}

