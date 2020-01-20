#pragma once
#pragma warning(disable : 4996)

#include "SimulatedAnnealing.h"

class SimulatedAnnealingTest {
public: 

	  SimulatedAnnealing sa;
	  const char* dither_texture_path = "LDR_RGBA_0_64.png";
	  helpers helper;
	  int number_steps;
	  AnnealingSchedule* schedule;

	  SimulatedAnnealingTest();
	  void testPermutation(const char* filename);
	  Image applyPermutationToOriginal(Image original, Image applied_perm);
private:

};