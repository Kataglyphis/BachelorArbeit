// here we are defining our line size for the reprojection
#define DIMENSION_SIZE 8

#define BLOCK_SIZE 64

//input of our ray traced frame stored in a texture; from previous render step
Texture2D<float4> input_frame_texture;
//incoming buffer with the given seeds; StructuredBuffer<uint>?
Texture2D<float4> src_seed_texture;
//output to render our new frame t + 1 to
RWTexture2D<float4> output_seed_texture;
//the previous calculated depth buffer from gbuffer state
Texture2D<float> depth;

//given variables for our frame
struct perFrameData
{
    
    uint tile_width; // width of the frame
    uint tile_height; // height of the frame
    uint frame_width; // width of the current frame
    uint frame_height; // height of the current frame
    uint frame_count; // the actual index of the frame
    uint enable; //0: is disabled; 1: retarget seeds
    bool camera_moved; //temporal reprojection when camera has moved otherwise not, 0: is disabled, 1: reproject seeds
    float4x4 Inverse_VP_prev_frame; //the view projection matrix for our previous frame
    float4x4 VP_curr_frame; //the view projection matrix for our current frame
    
};

//structure containing our frame data
StructuredBuffer<perFrameData> data;


//fetching precomputed permutation and applying it to the seeds
[numthreads(DIMENSION_SIZE, DIMENSION_SIZE, 1)]
void main(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID, uint2 group_thread_ID : SV_GroupThreadID)
{
    if (thread_ID.x >= data[0].frame_width || thread_ID.y >= data[0].frame_height || thread_ID.x < 0 || thread_ID.y < 0)
        return;
    
    uint tile_width = data[0].tile_width;
    uint tile_height = data[0].tile_height;
    uint frame_count = data[0].frame_count;
    uint frame_width = data[0].frame_width;
    uint frame_height = data[0].frame_height;
    uint enable_reprojection_pass = data[0].enable; //0=FALSE, 1=TRUE
    bool camera_moved = data[0].camera_moved;
    float4x4 Inverse_VP_prev_frame = data[0].Inverse_VP_prev_frame;
    float4x4 VP_curr_frame = data[0].VP_curr_frame;

    float g = 1.32471795724474602596f;
    uint offset_x = (1.0f / g) * tile_width * (frame_count);
    uint offset_y = (1.0f / (g * g)) * tile_height * (frame_count);
    uint2 offset = (offset_x, offset_y);
    uint2 bluenoise_index = (offset + thread_ID);
    bluenoise_index.x = bluenoise_index.x % tile_width;
    bluenoise_index.y = bluenoise_index.y % tile_height;

    // when the camera has not moved or the pass is in generel turned off
    // just pass the seeds normally 
    if ((enable_reprojection_pass == 0) || (camera_moved == false)) {
        
        output_seed_texture[thread_ID] = src_seed_texture[thread_ID];
        
    } else {
        
        // otherwise we will additionally temporally reproject!!
        float2 screen_space = float2(thread_ID.x / (float) (frame_width), thread_ID.y / (float) (frame_height));
        float2 clip_coord;
        clip_coord.x = (screen_space.x * 2.f) - 1.f;
        clip_coord.y = 1.f - (screen_space.y * 2.f); //cause here is left handed coord system!!!

        float frag_depth = depth[thread_ID];
        float z = (frag_depth * 2.f) - 1.f;
        
        float4 clip_space_pos = float4(clip_coord, z, 1.f);

        float4 world_position_normalized = mul(clip_space_pos, Inverse_VP_prev_frame);

        float4 reprojected_position_clip = mul(world_position_normalized, VP_curr_frame);
        reprojected_position_clip.w = 1.f / reprojected_position_clip.w;
        
        float4 coord_clip_space_tplus1;
        coord_clip_space_tplus1.x = reprojected_position_clip.x * reprojected_position_clip.w;
        coord_clip_space_tplus1.y = reprojected_position_clip.y * reprojected_position_clip.w;
        coord_clip_space_tplus1.z = reprojected_position_clip.z * reprojected_position_clip.w;

        uint2 screen_coord;
        screen_coord.x = ((coord_clip_space_tplus1.x + 1.f) / 2.f) * (frame_width);
        screen_coord.y = ((coord_clip_space_tplus1.y - 1.f) / -2.f) * (frame_height);
        
        //output_seed_texture[screen_coord] = src_seed_texture[thread_ID];
        int2 diff = screen_coord - thread_ID;
        diff.x = abs(diff.x);
        diff.y = abs(diff.y);
        //output_seed_texture[thread_ID] = float4(0.f, z, 0.f, 1.f);
        //output_seed_texture[thread_ID] = float4(screen_coord.x / (float) frame_width, screen_coord.y / (float) frame_height, 0.f, 1.f);
        //output_seed_texture[thread_ID] = float4(1.f, frame_count / 128.f, 0.f, 1.f);
        output_seed_texture[thread_ID] = float4(diff.x / (float) frame_width, diff.y / (float) frame_height, 0.f, 1.f);

    }
}
