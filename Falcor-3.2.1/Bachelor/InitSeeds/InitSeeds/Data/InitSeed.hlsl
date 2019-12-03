//Initialization of seeds for ray tracing
//number of blocks we want to consider for our calculation
#define BLOCK_SIZE pow(LINE_SIZE, 2)
#define LINE_SIZE 8

//we need a texture to store our seeds to!
//Texture2D gInput;
RWTexture2D<uint3> gTexture;

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

[numthreads(LINE_SIZE, LINE_SIZE, 1)]
void InitSeeds(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID) {
    
    uint num_tiles_per_row = 240;
    uint wang_index = group_Index + (group_ID.y  * BLOCK_SIZE  * num_tiles_per_row + group_ID.x * BLOCK_SIZE);
    gTexture[1920 * thread_ID.y + thread_ID.x] = wang_hash(wang_index);
    
}

