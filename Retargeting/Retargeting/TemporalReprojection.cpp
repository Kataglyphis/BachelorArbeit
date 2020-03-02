#include "TemporalReprojection.h"

TemporalReprojection::TemporalReprojection() {

	this->helper = helpers(filename, image_width, image_height);
	//this->schedule = new ExponentialCoolDown();
	double T_0 = 511;
	double quasieq = 20000.f;
	double mu = 0.95;
	this->schedule = new Kirkpatrick(T_0, mu, quasieq);
	Energy energy;
	number_steps = 1000;
	this->sa = SimulatedAnnealing(number_steps, schedule, energy, false, image_width, image_height, helper, filename);
	this->filename = filename;
}

void TemporalReprojection::generateRetargetTextureSet(int vector_offset_x, int vector_offset_y) {

	Image next_dither;
	Image original;
	Image original_with_offset;

	helper.initializeImage(next_dither);
	helper.initializeImage(original);
	helper.initializeImage(original_with_offset);

	helper.loadPNGinArray(filename, original);

	helper.getNextDither(original, next_dither, helper.getDitherWidth(), helper.getDitherHeight());

	for (int i = 0; i < vector_offset_x; i++) {
		
		for (int j = 0; j < vector_offset_y; j++) {

			int32_t goodswaps = 0;
			calcOriginalWithOffset(original, original_with_offset, i, j);
			Image permutation = sa.execute(original_with_offset, this->temp_rep_base_folder, i, j, goodswaps);

		}
	}
}

void TemporalReprojection::calcOriginalWithOffset(Image original, Image& original_with_offset, int offset_x, int offset_y) {

	for (int i = 0; i < image_width; i++) {
		
		for (int j = 0; j < image_height; j++) {

			int new_index_x = (i + offset_x) % image_width;
			int new_index_y = (j + offset_y) % image_height;

			original_with_offset[new_index_x][new_index_y] = original[i][j];

		}
	}

}