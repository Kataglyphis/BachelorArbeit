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
//texture to get our motion vector from
RWTexture2D<float2> input_average_motion_vector;

//all the textures for the temporal reprojection!
//HELP: Is there a better way than hardcoding them ?????????????????????????????????????????????????
// ??????????????????????????????????????????????????????????????????????????????????????????????

Texture2D<float4> temporal_projecting_4x_4;
Texture2D<float4> temporal_projecting_4x_3;
Texture2D<float4> temporal_projecting_4x_2;
Texture2D<float4> temporal_projecting_4x_1;
Texture2D<float4> temporal_projecting_4x0;

Texture2D<float4> temporal_projecting_3x_4;
Texture2D<float4> temporal_projecting_3x_3;
Texture2D<float4> temporal_projecting_3x_2;
Texture2D<float4> temporal_projecting_3x_1;
Texture2D<float4> temporal_projecting_3x0;

Texture2D<float4> temporal_projecting_2x_4;
Texture2D<float4> temporal_projecting_2x_3;
Texture2D<float4> temporal_projecting_2x_2;
Texture2D<float4> temporal_projecting_2x_1;
Texture2D<float4> temporal_projecting_2x0;

Texture2D<float4> temporal_projecting_1x_4;
Texture2D<float4> temporal_projecting_1x_3;
Texture2D<float4> temporal_projecting_1x_2;
Texture2D<float4> temporal_projecting_1x_1;
Texture2D<float4> temporal_projecting_1x0;

Texture2D<float4> temporal_projecting_4x4;
Texture2D<float4> temporal_projecting_4x3;
Texture2D<float4> temporal_projecting_4x2;
Texture2D<float4> temporal_projecting_4x1;

Texture2D<float4> temporal_projecting_3x4;
Texture2D<float4> temporal_projecting_3x3;
Texture2D<float4> temporal_projecting_3x2;
Texture2D<float4> temporal_projecting_3x1;

Texture2D<float4> temporal_projecting_2x4;
Texture2D<float4> temporal_projecting_2x3;
Texture2D<float4> temporal_projecting_2x2;
Texture2D<float4> temporal_projecting_2x1;

Texture2D<float4> temporal_projecting_1x4;
Texture2D<float4> temporal_projecting_1x3;
Texture2D<float4> temporal_projecting_1x2;
Texture2D<float4> temporal_projecting_1x1;

Texture2D<float4> temporal_projecting4x_4;
Texture2D<float4> temporal_projecting4x_3;
Texture2D<float4> temporal_projecting4x_2;
Texture2D<float4> temporal_projecting4x_1;
Texture2D<float4> temporal_projecting4x0;

Texture2D<float4> temporal_projecting3x_4;
Texture2D<float4> temporal_projecting3x_3;
Texture2D<float4> temporal_projecting3x_2;
Texture2D<float4> temporal_projecting3x_1;
Texture2D<float4> temporal_projecting3x0;

Texture2D<float4> temporal_projecting2x_4;
Texture2D<float4> temporal_projecting2x_3;
Texture2D<float4> temporal_projecting2x_2;
Texture2D<float4> temporal_projecting2x_1;
Texture2D<float4> temporal_projecting2x0;

Texture2D<float4> temporal_projecting1x_4;
Texture2D<float4> temporal_projecting1x_3;
Texture2D<float4> temporal_projecting1x_2;
Texture2D<float4> temporal_projecting1x_1;
Texture2D<float4> temporal_projecting1x0;

Texture2D<float4> temporal_projecting4x4;
Texture2D<float4> temporal_projecting4x3;
Texture2D<float4> temporal_projecting4x2;
Texture2D<float4> temporal_projecting4x1;

Texture2D<float4> temporal_projecting3x4;
Texture2D<float4> temporal_projecting3x3;
Texture2D<float4> temporal_projecting3x2;
Texture2D<float4> temporal_projecting3x1;

Texture2D<float4> temporal_projecting2x4;
Texture2D<float4> temporal_projecting2x3;
Texture2D<float4> temporal_projecting2x2;
Texture2D<float4> temporal_projecting2x1;

Texture2D<float4> temporal_projecting1x4;
Texture2D<float4> temporal_projecting1x3;
Texture2D<float4> temporal_projecting1x2;
Texture2D<float4> temporal_projecting1x1;

Texture2D<float4> temporal_projecting0x_4;
Texture2D<float4> temporal_projecting0x_3;
Texture2D<float4> temporal_projecting0x_2;
Texture2D<float4> temporal_projecting0x_1;
Texture2D<float4> temporal_projecting0x0;

Texture2D<float4> temporal_projecting0x4;
Texture2D<float4> temporal_projecting0x3;
Texture2D<float4> temporal_projecting0x2;
Texture2D<float4> temporal_projecting0x1;

//given variables for our frame
struct perFrameData {
    
    uint tile_width; // width of the frame
    uint tile_height; // height of the frame
    uint frame_width; // width of the current frame
    uint frame_height; // height of the current frame
    int frame_count; // the actual index of the frame
    uint enable_retargeting; //0: is disabled; 1: retarget seeds
    uint enable_temporal_reprojection; // in addition reprojct seeds!
    bool camera_moved;
    
};

//structure containing our frame data
StructuredBuffer<perFrameData> data;

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
    bool camera_moved = data[0].camera_moved;

    float2 prev_calc_sreen_space_distance = input_average_motion_vector[int2(0)]; // / (float2)(frame_width * frame_height);
    
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
    // if normal retargeting enabled or temporal is activated and camera ist not moving :)
    if (((enable_retargeting_pass == 1) && (enable_temporal_reprojection == 0)) ||
        ((enable_temporal_reprojection == 1) && (camera_moved == false)))
    {
        
        retarget += int2(round(retarget_texture[bluenoise_index].rg * 12.f - float2(6.f)));
        
    } else if ((enable_temporal_reprojection == 1) && (camera_moved == true)) {

        int2 additional_reprojection = int2(0);
        int2 difference = prev_calc_sreen_space_distance;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //what happend in this block: we calculate the average screen position difference!
        // thus we choose the retarget value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        
        if (vectors_are_equal(difference, int2(-4, -4)))
        {
            additional_reprojection = temporal_projecting_4x_4[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-4, -3)))
        {
            additional_reprojection = temporal_projecting_4x_3[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-4, -2)))
        {
            additional_reprojection = temporal_projecting_4x_2[bluenoise_index].rg;
        } else if (vectors_are_equal(difference, int2(-4, -1)))
        {
            additional_reprojection = temporal_projecting_4x_1[bluenoise_index].rg;
        } else if (vectors_are_equal(difference, int2(-4, 0)))
        {
            additional_reprojection = temporal_projecting_4x0[bluenoise_index].rg;
        } else if (vectors_are_equal(difference, int2(-3, -4)))
        {
            additional_reprojection = temporal_projecting_3x_4[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-3, -3)))
        {
            additional_reprojection = temporal_projecting_3x_3[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-3, -2)))
        {
            additional_reprojection = temporal_projecting_3x_2[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-3, -1)))
        {
            additional_reprojection = temporal_projecting_3x_1[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-3, 0)))
        {
            additional_reprojection = temporal_projecting_3x0[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-2, -4)))
        {
            additional_reprojection = temporal_projecting_2x_4[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-2, -3)))
        {
            additional_reprojection = temporal_projecting_2x_3[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-2, -2)))
        {
            additional_reprojection = temporal_projecting_2x_2[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-2, -1)))
        {
            additional_reprojection = temporal_projecting_2x_1[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-2, 0)))
        {
            additional_reprojection = temporal_projecting_2x0[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-1, -4)))
        {
            additional_reprojection = temporal_projecting_1x_4[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-1, -3)))
        {
            additional_reprojection = temporal_projecting_1x_3[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-1, -2)))
        {
            additional_reprojection = temporal_projecting_1x_2[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-1, -1)))
        {
            additional_reprojection = temporal_projecting_1x_1[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-1, 0)))
        {
            additional_reprojection = temporal_projecting_1x0[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-4, 4)))
        {
            additional_reprojection = temporal_projecting_4x4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-4, 3)))
        {
            additional_reprojection = temporal_projecting_4x3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-4, 2)))
        {
            additional_reprojection = temporal_projecting_4x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-4, 1)))
        {
            additional_reprojection = temporal_projecting_4x1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-3, 4)))
        {
            additional_reprojection = temporal_projecting_3x4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-3, 3)))
        {
            additional_reprojection = temporal_projecting_3x3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-3, 2)))
        {
            additional_reprojection = temporal_projecting_3x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-3, 1)))
        {
            additional_reprojection = temporal_projecting_3x1[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(-2, 4)))
        {
            additional_reprojection = temporal_projecting_2x4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-2, 3)))
        {
            additional_reprojection = temporal_projecting_2x3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-2, 2)))
        {
            additional_reprojection = temporal_projecting_2x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-2, 1)))
        {
            additional_reprojection = temporal_projecting_2x1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-1, 4)))
        {
            additional_reprojection = temporal_projecting_1x4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-1, 3)))
        {
            additional_reprojection = temporal_projecting_1x3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-1, 2)))
        {
            additional_reprojection = temporal_projecting_1x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(-1, 1)))
        {
            additional_reprojection = temporal_projecting_1x1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(4, -4)))
        {
            additional_reprojection = temporal_projecting4x_4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(4, -3)))
        {
            additional_reprojection = temporal_projecting4x_3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(4, -2)))
        {
            additional_reprojection = temporal_projecting4x_2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(4, -1)))
        {
            additional_reprojection = temporal_projecting4x_1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(3, -4)))
        {
            additional_reprojection = temporal_projecting3x_4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(3, -3)))
        {
            additional_reprojection = temporal_projecting3x_3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(3, -2)))
        {
            additional_reprojection = temporal_projecting3x_2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(3, -1)))
        {
            additional_reprojection = temporal_projecting3x_1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(2, -4)))
        {
            additional_reprojection = temporal_projecting2x_4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(2, -3)))
        {
            additional_reprojection = temporal_projecting2x_3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(2, -2)))
        {
            additional_reprojection = temporal_projecting2x_2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(2, -1)))
        {
            additional_reprojection = temporal_projecting2x_1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(1, -4)))
        {
            additional_reprojection = temporal_projecting1x_4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(1, -3)))
        {
            additional_reprojection = temporal_projecting1x_3[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(1, -2)))
        {
            additional_reprojection = temporal_projecting1x_2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(1, -1)))
        {
            additional_reprojection = temporal_projecting1x_1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(0,0)))
        {
            additional_reprojection = retarget_texture[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(0, 1)))
        {
            additional_reprojection = temporal_projecting0x1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(0, 2)))
        {
            additional_reprojection = temporal_projecting0x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(0, 3)))
        {
            additional_reprojection = temporal_projecting0x3[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(0, 4)))
        {
            additional_reprojection = temporal_projecting0x4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(1, 0)))
        {
            additional_reprojection = temporal_projecting1x0[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(1, 1)))
        {
            additional_reprojection = temporal_projecting1x1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(1, 2)))
        {
            additional_reprojection = temporal_projecting1x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(1, 3)))
        {
            additional_reprojection = temporal_projecting1x3[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(1, 4)))
        {
            additional_reprojection = temporal_projecting1x4[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(2, 0)))
        {
            additional_reprojection = temporal_projecting2x0[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(2, 1)))
        {
            additional_reprojection = temporal_projecting2x1[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(2, 2)))
        {
            additional_reprojection = temporal_projecting2x2[bluenoise_index].rg;
        }
        else if (vectors_are_equal(difference, int2(2, 3)))
        {
            additional_reprojection = temporal_projecting2x3[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(2, 4)))
        {
            additional_reprojection = temporal_projecting2x4[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(3, 0)))
        {
            additional_reprojection = temporal_projecting3x0[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(3, 1)))
        {
            additional_reprojection = temporal_projecting3x1[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(3, 2)))
        {
            additional_reprojection = temporal_projecting3x2[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(3, 3)))
        {
            additional_reprojection = temporal_projecting3x3[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(3, 4)))
        {
            additional_reprojection = temporal_projecting3x4[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(4, 0)))
        {
            additional_reprojection = temporal_projecting4x0[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(4, 1)))
        {
            additional_reprojection = temporal_projecting4x0[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(4, 2)))
        {
            additional_reprojection = temporal_projecting4x2[bluenoise_index].rg;
        }else if (vectors_are_equal(difference, int2(4, 3)))
        {
            additional_reprojection = temporal_projecting4x3[bluenoise_index].rg;
        }
        else
        {
            additional_reprojection = temporal_projecting4x4[bluenoise_index].rg;
        }
                 
        retarget += int2(round(additional_reprojection * 12.f - float2(6.f)));
        
    }

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
        //output_seed_texture[thread_ID] = float4(prev_calc_sreen_space_distance, 0, 1);
    }
    else
    {
        output_seed_texture[thread_ID] = src_seed_texture[thread_ID];
    }
}
