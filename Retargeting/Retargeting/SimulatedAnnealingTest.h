#pragma once
#include "SimulatedAnnealing.h"

class SimulatedAnnealingTest {
public: 

	  SimulatedAnnealing sa;
	  const char* dither_texture_path = "LDR_RGBA_0_64.png";
	  helpers helper;

	  SimulatedAnnealingTest();
	  void testPermutation(const char* filename, const unsigned int steps);
	  Image applyPermutationToOriginal(Image original, Image applied_perm);

};