//we are aiming to sort our pixels new

/** Pseudocode from paper
Algorithm 1 The sorting pass permutes pixel seeds by blocks.
. Create lists of Pixel(float value, int i, int j)
1: for each (i, j) in block do
2:   F.add( Pixel(intensity(frame(i, j)), i, j) ) . frame pixel
3:   D.add( Pixel(dither(i, j), i, j) ) . dither pixel
4: end for
. Sort lists by pixel values
5: sort(F)
6: sort(D)
. Permute seeds
7: for each n in 1..size(F) do
8:  seeds_sorted(D(n).i, D(n). j) = seeds(F(n).i, F(n). j)
9: end for
*/
//__import ShaderCommon;
//__import Helpers;
// number of pixels we group togehter and we are sorting for itself

//sorting 4 pixel blocks each for itself
#define DIMENSION_SIZE 8

#define BLOCK_SIZE 64

//some helper functions; make coding easier
#define Swap(A,B) {pixel temp = A; A = B; B = temp;}

//fmod(x, y) function := return the floating-point reminder f of x/y
//#define Even(x) (fmod((x), 2) == 0)
//#define Odd(x)  (fmod((x), 2) != 0)

//helpfull functions
float getIntensity(float3 pixel) {
    return (pixel.x + pixel.y + pixel.z) / 3.0f;
}

uint getSeedFromTex(float4 pixelValue) {
    return ((((uint) pixelValue.x) << 24) | (((uint) pixelValue.y) << 16) | (((uint) pixelValue.z << 8)) | pixelValue.w);
}

float4 fromSeedToTexture(float seed)
{
    return float4((((uint) seed & 0xFF000000) >> 24) / 255.f, (((uint) seed & 0x00FF0000) >> 16) / 255.f, (((uint) seed & 0x0000FF00) >> 8) / 255.f, (((uint) seed & 0x000000FF)) / 255.f);
}

//central struct for the sorting; each pixel has a value and an index in our
//4x4 block; sorting like the pair data structure in C++-Library; sorting
//the value by simultaneously keeping track of their indices!!  
struct pixel {
    float value;
    float index;
};

//input of our ray traced frame stored in a texture
Texture2D<float4> input_frame_texture;
//needed as comparisson for sorting
Texture2D<float4> input_blue_noise_texture;
//texture we are becoming and will again put out filled with sorted seeds;
RWTexture2D<float4> input_seed_texture;

//given variables for our frame
struct perFrameData
{
    uint tile_width; // width of the frame
    uint tile_height; // height of the frame
    uint frame_width; // width of the current frame
    uint frame_height; // height of the current frame
    int frame_count; // the actual index of the frame
};

StructuredBuffer<perFrameData> data;

//shared beneath all threads of the group
//must be shared! wen want to sort all the pixels
//of a block
groupshared pixel sortedImage[BLOCK_SIZE];
groupshared pixel sortedBlueNoise[BLOCK_SIZE];

//we are performing a 1-dimensional-sorting of our seeds
//[numthreads(DIMENSION_SIZE, DIMENSION_SIZE, 1)]
[numthreads(DIMENSION_SIZE, DIMENSION_SIZE, 1)]
void main(uint group_Index : SV_GROUPINDEX, uint2 group_ID : SV_GROUPID, uint2 thread_ID : SV_DISPATCHTHREADID, uint2 group_thread_id : SV_GroupThreadID)
{
    //if (thread_ID.x >= data[0].frame_width || thread_ID.y >= data[0].frame_height)
     //   return;
    uint tile_width = data[0].tile_width;
    uint tile_height = data[0].tile_height;
    uint frame_count = data[0].frame_count;

    //target blue noise tile should change after each frame --> each pixel has a different error in each frame
    //This is important for temporel filtering algorithms to reduce errors by averaging them over multiple frames!!
    float g = 1.32471795724474602596f;
    uint offset_x = (1.0f / g) * tile_width * frame_count; //multiply with index for changes frame by frame!
    uint offset_y = (1.0f / (g * g)) * tile_height * frame_count; //multiply with index for changes frame by frame!
    uint2 offset = (offset_x, offset_y);
    uint2 bluenoise_index = (offset + thread_ID);
    bluenoise_index.x = bluenoise_index.x % tile_width;
    bluenoise_index.y = bluenoise_index.y % tile_height;
    
    //we have the values shared beneath all threads of a groupshared
    //before we start to sort we have to firstly simply copy
    //we need all threads to reach this point
    sortedImage[group_Index].value = getIntensity(input_frame_texture[thread_ID].xyz);
    //.x is hard coded for compiling reason; please correct it later
    sortedImage[group_Index].index = getSeedFromTex(input_seed_texture[thread_ID] * 255.f);

    //blue noise value; we use only one value; R-Channel
    sortedBlueNoise[group_Index].value = input_blue_noise_texture[bluenoise_index].r * 255.f;
    //save the group_index as inital value before sorting
    sortedBlueNoise[group_Index].index = group_Index;

    GroupMemoryBarrierWithGroupSync();
    //we have to sort the pixels in the block by their color intensities
    //and the blue noise pixels by their greyscal intesities

    for (int i = 0; i < (BLOCK_SIZE / 2); i++)
    {
        //first round
        if (group_Index < (BLOCK_SIZE - 1) && (group_Index % 2 != 0))
        {
            //sorting pixels of the frame
            if (sortedImage[group_Index].value > sortedImage[group_Index + 1].value) {
                
                Swap(sortedImage[group_Index], sortedImage[group_Index + 1]);
                    
             }
            //sorting pixels of gryscal
             if (sortedBlueNoise[group_Index].value > sortedBlueNoise[group_Index + 1].value) {
                Swap(sortedBlueNoise[group_Index], sortedBlueNoise[group_Index + 1]);
             }
         }
         GroupMemoryBarrierWithGroupSync();
        //wait for all,then second round!!
        if ((group_Index < BLOCK_SIZE - 1) && (group_Index % 2 == 0))
        {
            //first round
            if (sortedImage[group_Index].value > sortedImage[group_Index + 1].value)  {
                Swap(sortedImage[group_Index], sortedImage[group_Index + 1]);
             }
            //second round
             if (sortedBlueNoise[group_Index + 1].value < sortedBlueNoise[group_Index].value) {
                 Swap(sortedBlueNoise[group_Index], sortedBlueNoise[group_Index + 1]);
             }
        }
        //wait for all before continuing!
        GroupMemoryBarrierWithGroupSync();   
     }
  
    //save the new sorted seeds correctly!
    //we have sorted the texture by their greyscales and saved the now fetched index
    uint local_bluenoise_index = sortedBlueNoise[group_Index].index;
    //we need a global index for writing data into the seed texture, calc global indices
    uint x = local_bluenoise_index % DIMENSION_SIZE;
    uint y = local_bluenoise_index / DIMENSION_SIZE;
    uint2 global_seed_index = uint2(x, y) + group_ID * DIMENSION_SIZE; //NOT BLOCK_SIZE!! we will be 2 dimensional here first
    //get the the 1-dimensional index into the texture
    //new seed index in result of sorting blue noise texture
    //uint new_seed_index = global_seed_index.y * width + global_seed_index.x;
    //fed the input_seed_texture with the now sorted seeds!!!!
    input_seed_texture[global_seed_index] = fromSeedToTexture(sortedImage[group_Index].index); //float4(x, y, 0, 1); float4(x,y,0,1);////float4(1.0,0,0,1); //we've copied the global position above; so just enter with group_Index
     
}

