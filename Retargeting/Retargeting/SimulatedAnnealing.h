#pragma once
#include "helpers.h"
#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <vector>
using namespace std;


//typedef Image::index index;
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
		bool execute(const uint32_t  number_steps, const char* filename, const uint32_t image_width, const uint32_t image_height);
private:
        helpers helper;

		std::vector<int> toroidallyShift(const unsigned int oldFrameDitherX, const unsigned int oldFrameDitherY, const uint32_t frame_width, const uint32_t frame_height);
		bool loadPNGinArray(const char* fileName, Image& img_data);
		float calculateEnergy(Image& image_t, Image& image_next, Image& permutation, const int width, const int height);
		bool saveRetargetImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap);
		bool fromArrayToBitmap(Image& img_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height);
		bool getNextDither(Image& dither_data, Image& next_dither_data, const uint32_t frame_width, const uint32_t frame_height);
        bool acceptanceProbabilityFunction(const float energy_old_condition, const float energy_new_condition, const float ratio_steps);
        bool applyOneRandomPermutation(Image& permutation_data_output, Image& permutation_positions, const uint32_t image_width, const uint32_t image_height);
        bool isApplicablePermutation(Image& permutation_data_step, Image& permutation_positions, const int random_x, const int random_y, const int random_step_x, const int random_step_y, const int image_width, const int image_height);
        bool deepCopyImage(Image& source, Image& dest, const int image_width, const int image_height);
		
};