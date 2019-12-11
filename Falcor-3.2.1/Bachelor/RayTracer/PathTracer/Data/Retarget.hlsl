//here we are aiming for retarget our pixels;
//accumulate improvements over time 
//we are using a precomputed permutation
/** Algorithm 2 The retargeting pass permutes pixel seeds.
. Fetch precomputed retargeting tile
1: (k,l) = retarget(i, j)
. Permute seeds
2: seeds_retargeted(k,l) = seeds(i, j)
*/

// here we are defining our line size for the retargeting
#define BLOCK_SIZE 4

//retarget texture simulates t + 1;
//each <float i,float j> position stores its retargeted coordinates <float k,float l>;vertical and horizontal offset
Texture2D<float2> retarget_texture;
//incoming buffer with the given seeds; StructuredBuffer<uint>?
Texture1D<uint> src_seed_texture;
//output to render our new frame t + 1 to
RWStructuredBuffer<uint> new_seed_texture;

//given variables for our frame
cbuffer PerFrameData {
    uint width; // width of the frame
    uint heigth; // height of the frame
    uint frame_count; // the actual index of the frame
};


//fetching precomputed permutation and applying it to the seeds
[numthreads(BLOCK_SIZE,BLOCK_SIZE,1)]
void main(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID) {

    //Not finished yet! Same problem as in the sorting pass, how to access texture correctly with OFFSET ???
    //Big TODO: target blue noise tile should change after each frame --> each pixel has a different error in each frame
    //This is important for temporel filtering algorithms to reduce errors by averaging them over multiple frames!!
    float g = 1.32471795724474602596;
    float offset_x = (1.0/g) * width * frame_count;
    float offset_y = (1.0/(pow(g,2))) * heigth * frame_count;
    float2 offset = (offset_x,offset_y);
    uint2 bluenoise_index = (offset + thread_ID);
    bluenoise_index.x = bluenoise_index.x % width;
    bluenoise_index.y = bluenoise_index.y % heigth;

    int2 retarget = int2(retarget_texture[bluenoise_index]);

    //retargeting of the seeds
    uint2 retargetCoordinates = thread_ID + uint2(width, heigth) + retarget;
    retargetCoordinates.x = retargetCoordinates.x % width;
    retargetCoordinates.y = retargetCoordinates.y % heigth;
    //apply permutation to the seeds
    new_seed_texture[retargetCoordinates.y * width + retargetCoordinates.x] = src_seed_texture[thread_ID.y * width + thread_ID.x];
}
