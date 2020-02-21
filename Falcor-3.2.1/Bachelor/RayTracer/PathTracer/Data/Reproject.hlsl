// here we are defining our line size for the reprojection
#define DIMENSION_SIZE 8

#define BLOCK_SIZE 64

//input of our ray traced frame stored in a texture; from previous render step
Texture2D<float4> input_frame_texture;
//incoming buffer with the given seeds; StructuredBuffer<uint>?
Texture2D<float4> src_seed_texture;
//output to render our new frame t + 1 to
RWTexture2D<float4> output_seed_texture;

//given variables for our frame
struct perFrameData
{
    
    uint tile_width; // width of the frame
    uint tile_height; // height of the frame
    uint frame_width; // width of the current frame
    uint frame_height; // height of the current frame
    uint frame_count; // the actual index of the frame
    uint enable; //0: is disabled; 1: retarget seeds
    uint camera_moved; //temporal reprojection when camera has moved otherwise not, 0: is disabled, 1: reproject seeds
    float4x4 VP_prev_frame; //the view projection matrix for our previous frame
    
};

//structure containing our frame data
StructuredBuffer<perFrameData> data;


//fetching precomputed permutation and applying it to the seeds
[numthreads(DIMENSION_SIZE, DIMENSION_SIZE, 1)]
void main(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID, uint2 group_thread_ID : SV_GroupThreadID)
{
    
    uint tile_width = data[0].tile_width;
    uint tile_height = data[0].tile_height;
    uint frame_count = data[0].frame_count;
    uint frame_width = data[0].frame_width;
    uint frame_height = data[0].frame_height;
    uint enable_reprojection_pass = data[0].enable; //0=FALSE, 1=TRUE
    uint camera_moved = data[0].camera_moved;
    float4x4 VP_prev_frame = data[0].VP_prev_frame;

    float g = 1.32471795724474602596f;
    uint offset_x = (1.0f / g) * tile_width * (frame_count);
    uint offset_y = (1.0f / (g * g)) * tile_height * (frame_count);
    uint2 offset = (offset_x, offset_y);
    uint2 bluenoise_index = (offset + thread_ID);
    bluenoise_index.x = bluenoise_index.x % tile_width;
    bluenoise_index.y = bluenoise_index.y % tile_height;

    // when the camera has not moved or the pass is in generel turned off
    // just pass the seeds normally 
    if ((enable_reprojection_pass == 0) || (camera_moved == 0)) {
        
        output_seed_texture[thread_ID] = src_seed_texture[thread_ID];
        
    }
    // otherwise we will temporally reproject!
    else {
        
        //now reproject here!!
        float4 position_ID = float4(thread_ID, 0, 1);
        uint2 reprojected_position = mul(VP_prev_frame, position_ID).aa;
        //output_seed_texture[thread_ID] = src_seed_texture[thread_ID];
        output_seed_texture[reprojected_position] = src_seed_texture[thread_ID];
        
    }
}
