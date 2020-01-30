//here we are aiming for retarget our pixels;
//accumulate improvements over time 
//we are using a precomputed permutation
/** Algorithm 2 The retargeting pass permutes pixel seeds.
. Fetch precomputed retargeting tile
1: (k,l) = retarget(i, j)
. Permute seeds
2: seeds_retargeted(k,l) = seeds(i, j)
*/
//__import ShaderCommon;
//__import Helpers;
// here we are defining our line size for the retargeting
#define BLOCK_SIZE 4

//retarget texture simulates t + 1;
//each <float i,float j> position stores its retargeted coordinates <float k,float l>;vertical and horizontal offset
Texture2D<float4> retarget_texture;
//incoming buffer with the given seeds; StructuredBuffer<uint>?
Texture2D<float4> src_seed_texture;
//output to render our new frame t + 1 to
RWTexture2D<float4> output_seed_texture;

//given variables for our frame
struct perFrameData {
    
    uint tile_width; // width of the frame
    uint tile_height; // height of the frame
    uint frame_width; // width of the current frame
    uint frame_height; // height of the current frame
    uint frame_count; // the actual index of the frame
    uint enable; //0: is disabled; 1: retarget seeds
    
};

//structure containing our frame data
StructuredBuffer<perFrameData> data;


//fetching precomputed permutation and applying it to the seeds
[numthreads(BLOCK_SIZE,BLOCK_SIZE,1)]
void main(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID) {

    uint tile_width = data[0].tile_width;
    uint tile_height = data[0].tile_height;
    uint frame_count = data[0].frame_count;
    uint frame_width = data[0].frame_width;
    uint frame_height = data[0].frame_height;
    
    //Not finished yet! Same problem as in the sorting pass, how to access texture correctly with OFFSET ???
    //Big TODO: target blue noise tile should change after each frame --> each pixel has a different error in each frame
    //This is important for temporel filtering algorithms to reduce errors by averaging them over multiple frames!!
    float g = 1.32471795724474602596f;
    float offset_x = (1.0f / g) * tile_width * frame_count;
    float offset_y = (1.0f / (g * g)) * tile_height * frame_count;
    float2 offset = (offset_x,offset_y);
    uint2 bluenoise_index = (offset + thread_ID);
    bluenoise_index.x = bluenoise_index.x % tile_width;
    bluenoise_index.y = bluenoise_index.y % tile_height;

    int2 retarget = int2(0);

    // if enabled read retarget coords from textures
    if (data[0].enable == 1) {

        //retarget = int2(retarget_texture[int2(bluenoise_index.x, tile_height - 1 - bluenoise_index.y)].rg * 255.f - float2(6.f));
        //retarget.y = -retarget.y;
        int retarget_x = retarget_texture[int2(bluenoise_index)].r;
        int retarget_y = retarget_texture[int2(bluenoise_index)].g;
        retarget = int2(int2(retarget_x, retarget_y)*255.f - float2(6.f));
    }

    //retargeting of the seeds
    uint2 retargetCoordinates = thread_ID + retarget;
    retargetCoordinates.x = retargetCoordinates.x % tile_width;
    retargetCoordinates.y = retargetCoordinates.y % tile_height;
    //apply permutation to the seeds
    //output_seed_texture[thread_ID] = float4(1,0,0,1);
    output_seed_texture[retargetCoordinates] = src_seed_texture[thread_ID];
    }
