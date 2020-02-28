//here we are aiming for retarget our pixels;
//accumulate improvements over time 
//we are using a precomputed permutation

// here we are defining our line size for the retargeting
#define DIMENSION_SIZE 8

#define BLOCK_SIZE 64


//for there is no given hlsl function ... 
bool vectors_are_equal(uint2 vector1, uint2 vector2)
{
    if (((vector1.x - vector2.x) == 0) &&
        ((vector1.y - vector2.y) == 0))
    {
        return true;
    }
    else
    {
        return false;
    }

}


//retarget texture simulates t + 1;
//each <float i,float j> position stores its retargeted coordinates <float k,float l>;vertical and horizontal offset
Texture2D<float4> retarget_texture;
//incoming buffer with the given seeds; StructuredBuffer<uint>?
Texture2D<float4> src_seed_texture;
//output to render our new frame t + 1 to
RWTexture2D<float4> output_seed_texture;
//the previous calculated depth buffer from gbuffer state
Texture2D<float> depth;

//all the textures for the temporal reprojection!
Texture2D<float4> retarget_texture0x1;
Texture2D<float4> retarget_texture0x2;
Texture2D<float4> retarget_texture0x3;
Texture2D<float4> retarget_texture1x0;
Texture2D<float4> retarget_texture1x1;
Texture2D<float4> retarget_texture1x2;
Texture2D<float4> retarget_texture1x3;
Texture2D<float4> retarget_texture2x0;
Texture2D<float4> retarget_texture2x1;
Texture2D<float4> retarget_texture2x2;
Texture2D<float4> retarget_texture2x3;


//given variables for our frame
struct perFrameData {
    
    uint tile_width; // width of the frame
    uint tile_height; // height of the frame
    uint frame_width; // width of the current frame
    uint frame_height; // height of the current frame
    int frame_count; // the actual index of the frame
    uint enable_retargeting; //0: is disabled; 1: retarget seeds
    uint enable_temporal_reprojection; // in addition reprojct seeds!
    float4x4 Inverse_VP_prev_frame; //the view projection matrix for our previous frame
    float4x4 VP_curr_frame; //the view projection matrix for our current frame
    bool camera_moved; //temporal reprojection when camera has moved otherwise not, 0: is disabled, 1: reproject seeds
    
};

//structure containing our frame data
StructuredBuffer<perFrameData> data;

groupshared uint2 difference = uint2(0);

//fetching precomputed permutation and applying it to the seeds
[numthreads(DIMENSION_SIZE, DIMENSION_SIZE, 1)]
void main(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID, uint2 group_thread_ID : SV_GroupThreadID) {

    //falcor has some wrong thread id 's!!! stop them here for preventing errors!
    if (thread_ID.x >= data[0].frame_width || thread_ID.y >= data[0].frame_height || thread_ID.x < 0 || thread_ID.y < 0)
        return;
    
    uint tile_width = data[0].tile_width;
    uint tile_height = data[0].tile_height;
    uint frame_count = data[0].frame_count;
    uint frame_width = data[0].frame_width;
    uint frame_height = data[0].frame_height;
    uint enable_retargeting_pass = data[0].enable_retargeting; //0=FALSE, 1=TRUE
    uint enable_temporal_reprojection = data[0].enable_temporal_reprojection;
    float4x4 Inverse_VP_prev_frame = data[0].Inverse_VP_prev_frame;
    float4x4 VP_curr_frame = data[0].VP_curr_frame;
    bool camera_moved = data[0].camera_moved;
    
    //This is important for temporel filtering algorithms to reduce errors by averaging them over multiple frames!!
    float g = 1.32471795724474602596f;
    uint offset_x = (1.0f / g) * tile_width * (frame_count);
    uint offset_y = (1.0f / (g * g)) * tile_height * (frame_count);
    uint2 offset = (offset_x, offset_y);
    uint2 bluenoise_index = (offset + thread_ID);
    bluenoise_index.x = bluenoise_index.x % tile_width;
    bluenoise_index.y = bluenoise_index.y % tile_height;

    //if not enabled the additional retargeting will stay 0
    int2 retarget = int2(0);

    // if enabled read retarget coords from textures
    if (((enable_retargeting_pass == 1) && (enable_temporal_reprojection == 0))
        || ((enable_temporal_reprojection == 1) && (camera_moved == false)))
    {
        
        retarget += int2(round(retarget_texture[bluenoise_index].rg * 12.f - float2(6.f)));
        
    } else if ((enable_temporal_reprojection == 1) && (camera_moved == true)) {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         // otherwise we will temporally reproject!!
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        float2 screen_space = float2(thread_ID.x / (float) (frame_width), thread_ID.y / (float) (frame_height));
        float2 clip_coord;
        clip_coord.x = (screen_space.x * 2.f) - 1.f;
        clip_coord.y = 1.f - (screen_space.y * 2.f); //cause here is left handed coord system!!!

        float frag_depth = depth[thread_ID];
        //now reproject here!!
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
        int2 diff = screen_coord - thread_ID;
        diff.x = abs(diff.x);
        diff.y = abs(diff.y);
        
        difference += diff;

        float2 retarget_value;

        GroupMemoryBarrierWithGroupSync();

        difference /= BLOCK_SIZE;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //what happend in this block: we calculate the average screen position difference!
        // thus we choose the retarget value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        if (vectors_are_equal(difference, uint2(0,0)))
        {
            retarget_value = retarget_texture[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(0, 1)))
        {
            retarget_value = retarget_texture0x1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(0, 2)))
        {
            retarget_value = retarget_texture0x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(0, 3)))
        {
            retarget_value = retarget_texture0x3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(1, 0)))
        {
            retarget_value = retarget_texture1x0[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(1, 1)))
        {
            retarget_value = retarget_texture1x1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(1, 2)))
        {
            retarget_value = retarget_texture1x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(1, 3)))
        {
            retarget_value = retarget_texture1x3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(2, 0)))
        {
            retarget_value = retarget_texture2x0[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(2, 1)))
        {
            retarget_value = retarget_texture2x1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(2, 2)))
        {
            retarget_value = retarget_texture2x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, uint2(2, 3)))
        {
            retarget_value = retarget_texture2x3[bluenoise_index].rg;
        }
        else
        {
            retarget_value = retarget_texture2x3[bluenoise_index].rg;
        }
                 
        retarget += int2(round(retarget_value * 12.f - float2(6.f)));
        
    }

    //uint2 local_retarget_coordinates = group_thread_ID + retarget + uint2(tile_width, tile_height);
    //local_retarget_coordinates.x = local_retarget_coordinates.x % tile_width;
    //local_retarget_coordinates.y = local_retarget_coordinates.y % tile_height;

    //uint2 global_retarget_coordinates = local_retarget_coordinates + DIMENSION_SIZE * (group_ID) + uint2(frame_width, frame_height);
    //global_retarget_coordinates.x = global_retarget_coordinates.x % frame_width;
    //global_retarget_coordinates.y = global_retarget_coordinates.y % frame_height;

    uint2 global_retarget_coordinates = thread_ID + retarget + uint2(frame_width, frame_height);
    global_retarget_coordinates.x = global_retarget_coordinates.x % frame_width;
    global_retarget_coordinates.y = global_retarget_coordinates.y % frame_height;
    
    //apply permutation to the seeds
    //output_seed_texture[thread_ID] = retarget_texture[int2(bluenoise_index)];
    //output_seed_texture[thread_ID] = src_seed_texture[uint2(thread_ID.x + retarget.x, thread_ID.y + retarget.y)];//float4(global_retarget_coordinates.x / frame_width, global_retarget_coordinates.y / frame_height,0,1);
    //output_seed_texture[global_retarget_coordinates] = src_seed_texture[uint2((thread_ID.x + frame_width) % frame_width, (thread_ID.y + frame_height) % frame_height)];//float4(thread_ID.x/ (float)frame_width, thread_ID.y/(float)frame_height,0,1);//float4(global_retarget_coordinates.x / frame_width, global_retarget_coordinates.y / frame_height,0,1);
    //output_seed_texture[global_retarget_coordinates] = src_seed_texture[thread_ID];
    
    if ((enable_retargeting_pass == 1) || (enable_temporal_reprojection == 1))
    {
        output_seed_texture[global_retarget_coordinates] = src_seed_texture[thread_ID];
    }
    else
    {
        output_seed_texture[thread_ID] = src_seed_texture[thread_ID];
    }
}
