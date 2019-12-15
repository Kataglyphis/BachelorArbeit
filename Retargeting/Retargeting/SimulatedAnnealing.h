#pragma once
#include "helpers.h"
#include <array>
#include <stdlib.h>
#define Pixel struct{int r, int g; int b, int a};
class SimulatedAnnealing {
	public:
		SimulatedAnnealing() {};
		bool fromBlueNoiseToRetarget(const char* filename);
		bool toroidallyShift(unsigned int oldFrameDitherX, unsigned int oldFrameDitherY);
		bool loadPNGinArray(const char* fileName, int*** img_data);
		float calcPixelDifference(int pixelA[4], int pixelB[4], int numChannelUsed);
		int calculateEnergy(int*** image_t, int*** image_next, int width, int height, int numChannelUsed);
	private:
		helpers helper;
};