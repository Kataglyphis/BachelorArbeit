#pragma once
#pragma warning(disable : 4996)

#include "SimulatedAnnealing.h"

class SimulatedAnnealingTest {
public: 
	  SimulatedAnnealingTest();
	  SimulatedAnnealingTest(QObject* q_object);
	  void testPermutation(const char* filename);
	  Image applyPermutationToOriginal(Image original, Image applied_perm);
	  //pointer on progress in %PERCENT%

private:

	  SimulatedAnnealing sa;
	  int number_steps;
	  AnnealingSchedule* schedule;

	  helpers helper;
	  const char* dither_texture_path = "LDR_RGBA_0_64.png";
	  std::string folder_permuted_images = "pictures/AppliedPermutation/";
	  std::string folder_energy = "pictures/Energy/";
	  QProgressBar* progress_bar;

};