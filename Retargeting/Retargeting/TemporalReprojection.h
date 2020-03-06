#pragma once
/*
This class produces a whole set of retarget textures;
The seeds will therefore be permuted with an extra 
offsetvector; we choose the retarget texture as an average   
over all pixel differences between two frames when 
the camera is in motion
*/
#include "SimulatedAnnealing.h"

class TemporalReprojection
{
public:

	TemporalReprojection(int number_of_steps_temp);
	void generateRetargetTextureSet(int vector_offest_x, int vector_offset_y, float& progress_temp);

private:

	void calcOriginalWithOffset(Image original, Image& original_with_offset, int offset_x, int offset_y);

	SimulatedAnnealing sa;
	int number_steps;
	AnnealingSchedule* schedule;
	helpers helper;

	int image_width = 64;
	int image_height = 64;
	const char* filename = "LDR_RGBA_0_64.png";
	const char* temp_rep_base_folder = "pictures/TemporalReprojection/";
};

