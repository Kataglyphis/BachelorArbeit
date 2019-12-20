#pragma once
#include "helpers.h"
#include <stdlib.h>
#include"boost/boost/multi_array.hpp"
#include <cassert>
#include <iostream>


//for moredimensional arrays!!
/**typedef std::vector<std::vector<std::vector<int>>> Image;
typedef std::vector<std::vector<int>> Row;
//pixel structure with r,g,b,a as ints
typedef std::vector<int> Pixel;
//permutation, stores (i,j) - pair*/
typedef boost::multi_array<double, 3> Image;
typedef Image::index index;
//typedef array_type::index index;
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
		std::vector<int> toroidallyShift(unsigned int oldFrameDitherX, unsigned int oldFrameDitherY, uint32_t frame_width, uint32_t frame_height);
		bool loadPNGinArray(const char* fileName, Image& img_data);
		float calculateEnergy(Image& image_t, Image& image_next, Image& permutation, int width, int height);
		bool saveRetargetImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap);
		bool fromArrayToBitmap(Image& img_data, FIBITMAP* bitmap, uint32_t image_width, uint32_t image_height);
		bool getNextDither(Image& dither_data, Image& next_dither_data, uint32_t frame_width, uint32_t frame_height);
        std::vector<int> selectRandomPixelIndices(int image_width, int image_height);
        bool acceptanceProbabilityFunction(float energy_old_condition, float energy_new_condition, float ratio_steps);
        bool applyOneRandomPermutation(Image& dither_data, Image& next_dither_data, Image& permutation_data_output, uint32_t image_width, uint32_t image_height);
        bool isApplicablePermutation(Image& permutation_data_step, int random_x, int random_y, int random_step_x, int random_step_y, int image_width, int image_height);
		helpers helper;
};