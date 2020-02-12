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
#define DIMENSION_SIZE 4

#define BLOCK_SIZE 16

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
[numthreads(DIMENSION_SIZE, DIMENSION_SIZE, 1)]
void main(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID, uint2 group_thread_ID : SV_GroupThreadID) {
    
    uint tile_width = data[0].tile_width;
    uint tile_height = data[0].tile_height;
    uint frame_count = data[0].frame_count;
    uint frame_width = data[0].frame_width;
    uint frame_height = data[0].frame_height;
    uint enable_retargeting_pass = data[0].enable; //0=FALSE, 1=TRUE

    //Not finished yet! Same problem as in the sorting pass, how to access texture correctly with OFFSET ???
    //Big TODO: target blue noise tile should change after each frame --> each pixel has a different error in each frame
    //This is important for temporel filtering algorithms to reduce errors by averaging them over multiple frames!!
    float g = 1.32471795724474602596f;
    uint offset_x = (1.0f / g) * tile_width * (frame_count);
    uint offset_y = (1.0f / (g * g)) * tile_height * (frame_count);
    uint2 offset = (offset_x, offset_y);
    uint2 bluenoise_index = (offset + thread_ID);
    bluenoise_index.x = bluenoise_index.x % tile_width;
    bluenoise_index.y = bluenoise_index.y % tile_height;

    int2 retarget = int2(0);
    //retarget = int2(-6,-6);

    // if enabled read retarget coords from textures
    if (enable_retargeting_pass == 1)
    {

        retarget += int2(round(retarget_texture[bluenoise_index].rg * 12.f - float2(6.f)));

    }

    uint2 global_retarget_coordinates = thread_ID + retarget + uint2(frame_width, frame_height);
    global_retarget_coordinates.x = global_retarget_coordinates.x % frame_width;
    global_retarget_coordinates.y = global_retarget_coordinates.y % frame_height;
    
    
    if (thread_ID.x >= data[0].frame_width || thread_ID.y >= data[0].frame_height ||
        thread_ID.x < 0 || thread_ID.y < 0)
    {
    
        uint2 new_thread_coordinates = thread_ID + uint2(frame_width, frame_height);
        new_thread_coordinates.x = new_thread_coordinates.x % frame_width;
        new_thread_coordinates.y = new_thread_coordinates.y % frame_height;
        if (enable_retargeting_pass == 1)
        {
            output_seed_texture[global_retarget_coordinates] = src_seed_texture[new_thread_coordinates];
        }
        else
        {
            output_seed_texture[new_thread_coordinates] = src_seed_texture[new_thread_coordinates];
            
        }
        return;
        
    }
    
    //apply permutation to the seeds
    //output_seed_texture[3] = src_seed_texture[thread_ID];
    //output_seed_texture[thread_ID] = float4(0.047, 0.047, 0, 1);
    //output_seed_texture[thread_ID] = retarget_texture[int2(bluenoise_index)];
    //output_seed_texture[thread_ID] = src_seed_texture[global_retarget_coordinates];
    //output_seed_texture[thread_ID] = src_seed_texture[thread_ID];
    //output_seed_texture[thread_ID] = src_seed_texture[uint2(thread_ID.x + retarget.x, thread_ID.y + retarget.y)];//float4(global_retarget_coordinates.x / frame_width, global_retarget_coordinates.y / frame_height,0,1);
    //output_seed_texture[global_retarget_coordinates] = src_seed_texture[uint2((thread_ID.x + frame_width) % frame_width, (thread_ID.y + frame_height) % frame_height)];//float4(thread_ID.x/ (float)frame_width, thread_ID.y/(float)frame_height,0,1);//float4(global_retarget_coordinates.x / frame_width, global_retarget_coordinates.y / frame_height,0,1);
    //output_seed_texture[global_retarget_coordinates] = src_seed_texture[thread_ID];
    if (enable_retargeting_pass)
    {
        output_seed_texture[global_retarget_coordinates] = src_seed_texture[thread_ID];
    }
    else
    {
        output_seed_texture[thread_ID] = src_seed_texture[thread_ID];
    }
}
