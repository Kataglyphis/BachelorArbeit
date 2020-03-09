//for there is no given hlsl function ... 

RWTexture2D<float2> output_average_motion_vector;
//the previous calculated depth buffer from gbuffer state
Texture2D<float> depth;

//given variables for our frame
struct perFrameData
{
    uint frame_width; // width of the current frame
    uint frame_height; // height of the current frame
    float4x4 Inverse_VP_prev_frame; //the view projection matrix for our previous frame
    float4x4 VP_curr_frame; //the view projection matrix for our current frame
    bool camera_moved; //temporal reprojection when camera has moved otherwise not, 0: is disabled, 1: reproject seeds
};

//structure containing our frame data
RWStructuredBuffer<perFrameData> data;

//fetching precomputed permutation and applying it to the seeds
[numthreads(1, 1, 1)]
void main(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID, uint2 group_thread_ID : SV_GroupThreadID)
{

    //falcor has some wrong thread id 's!!! stop them here for preventing errors!
    if (thread_ID.x >= data[0].frame_width || thread_ID.y >= data[0].frame_height || thread_ID.x < 0 || thread_ID.y < 0)
        return;
    
    uint frame_width = data[0].frame_width;
    uint frame_height = data[0].frame_height;
    float4x4 Inverse_VP_prev_frame = data[0].Inverse_VP_prev_frame;
    float4x4 VP_curr_frame = data[0].VP_curr_frame;
    bool camera_moved = data[0].camera_moved;

    output_average_motion_vector[int2(0)] = float2(-1.f);
    // if enabled read retarget coords from textures
    // if normal retargeting enabled or temporal is activated and camera ist not moving :)
    
    if (camera_moved) {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // otherwise we will temporally reproject!!
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        float2 screen_space = float2(thread_ID.x / (float) (frame_width), thread_ID.y / (float) (frame_height));
        float2 clip_coord;
        clip_coord.x = (screen_space.x * 2.f) - 1.f;
        clip_coord.y = 1.f - (screen_space.y * 2.f); //cause here is left handed coord system!!!

        float frag_depth = depth[thread_ID];
        float z = (frag_depth * 2.f) - 1.f;
        float4 clip_space_pos = float4(clip_coord, z, 1.f);

        //apply projection!!!
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float2 average_diff = (screen_coord - thread_ID) / (float2) (frame_width * frame_height);

        //garantee an atomic access!
        output_average_motion_vector[int2(0)] += average_diff;
        
        }

}
