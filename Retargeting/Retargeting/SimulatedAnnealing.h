#pragma once
#include "helpers.h"
#include <array>
#include <stdlib.h>
#include <vector>

//for moredimensional arrays!!
typedef std::vector<std::vector<std::vector<int>>> Image;
typedef std::vector<std::vector<int>> Row;
//pixel structure with r,g,b,a as ints
typedef std::vector<int> Pixel;
//permutation, stores (i,j) - pair
/**
	stores permutation in this pixel manner; numbers representing indices
   0 --- 1 --- 2 --- 3 --- 4 --- 5 --- 6 --- 7 --- 8 --- 9 --- 10 --- 11 --- 12
   |                                                                                              |
   1                                                                                            |
   |                                                                                              |
   2                                                                                             |   
   |                                                                                              |
   3                                                                                             |
   |                                                                                               |
   4                                                                                              |
   |                                                                                               |
   5                                                                                              | 
   |                                                                                               |
   6                                                                                              |
   |                                                                                               |
   7                                                                                              |
   |                                                                                               |
   8                                                                                              |
   |                                                                                               |
   9                                                                                              |
   |                                                                                               |
   10                                                                                            |
   |                                                                                               |
   11                                                                                            |
   |                                                                                                |
   12  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | 
*/
typedef std::vector<int> Permutation;

/**
	Let s = s0
	For k = 0 through kmax (exclusive):
	T ← temperature( kmax/(k+1) )
	Pick a random neighbour, snew ← neighbour(s)
	If P(E(s), E(snew), T) ≥ random(0, 1):
	s ← snew
	Output: the final state s
	*/

class SimulatedAnnealing {
	public:
		SimulatedAnnealing() {};
		bool execute(uint32_t  number_steps, const char* filename, uint32_t image_width, uint32_t image_height);
private:
		bool fromBlueNoiseToRetarget(const char* blue_noise_filename, uint32_t image_width, uint32_t image_height, const char* retarget_filename);
		std::vector<int> toroidallyShift(unsigned int oldFrameDitherX, unsigned int oldFrameDitherY, uint32_t frame_width, uint32_t frame_height);
		bool loadPNGinArray(const char* fileName, Image* img_data);
		float calcPixelDifference(int pixelA[4], int pixelB[4], int numChannelUsed);
		int calculateEnergy(int*** image_t, int*** image_next, int** permutation, int width, int height, int numChannelUsed);
		bool saveRetargetImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap);
		bool selectRandomNeighbor(int*** candidate, int* neighbor_index_x, int* neighbor_index_y, int index_x, int index_y, int image_width, int image_height);
		bool fromArrayToBitmap(Image* img_data, FIBITMAP* bitmap, uint32_t image_height, uint32_t image_width);
		bool getNextDither(Image* dither_data, Image* next_dither_data, uint32_t frame_width, uint32_t frame_height);
		helpers helper;
};