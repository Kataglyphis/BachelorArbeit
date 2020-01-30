#pragma once
#pragma warning(disable : 4996)

#include "SimulatedAnnealing.h"

class SimulatedAnnealingTest {
public: 

	  SimulatedAnnealingTest(const char* filename, int image_width, int image_height);
	  SimulatedAnnealingTest();
	  void testPermutation();
	  Image applyPermutationToOriginal(Image original, Image applied_perm);

private:

	  SimulatedAnnealing sa;
	  int number_steps;
	  AnnealingSchedule* schedule;

	  helpers helper;
	  const char* dither_texture_path = "pictures/BlueNoiseCode/FreeBlueNoiseTextures/Data/16_16/LDR_RGBA_0.png";
	  std::string folder_permuted_images = "pictures/AppliedPermutation/";
	  std::string folder_energy = "pictures/Energy/";
	  int image_width;
	  int image_height;
	  const char* filename;

};