#pragma once
#include "helpers.h"
#include <array>
#include <stdlib.h>
#define Pixel struct{int r, int g; int b, int a};

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
		bool execute(uint32_t  number_steps, uint32_t image_width, uint32_t image_height);
private:
		bool fromBlueNoiseToRetarget(const char* blue_noise_filename, uint32_t image_width, uint32_t image_height, const char* retarget_filename);
		bool toroidallyShift(unsigned int oldFrameDitherX, unsigned int oldFrameDitherY, int* next_dither_x, int* next_dither_y, uint32_t frame_width, uint32_t frame_height);
		bool loadPNGinArray(const char* fileName, int*** img_data);
		float calcPixelDifference(int pixelA[4], int pixelB[4], int numChannelUsed);
		int calculateEnergy(int*** image_t, int*** image_next, int** permutation, int width, int height, int numChannelUsed);
		bool saveRetargetImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap);
		bool selectRandomNeighbor(int*** candidate, int* neighbor_index_x, int* neighbor_index_y, int index_x, int index_y, int image_width, int image_height);
		bool fromArrayToBitmap(int*** img_data, FIBITMAP* bitmap, uint32_t image_height, uint32_t image_width);
		bool getNextDither(int*** frame, int*** next_frame, uint32_t frame_width, uint32_t frame_height);
		helpers helper;
};