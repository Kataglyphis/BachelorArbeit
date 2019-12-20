#include "SimulatedAnnealing.h"

bool SimulatedAnnealing::execute(uint32_t  number_steps, const char* filename, const uint32_t image_width, const uint32_t image_height) {

	//generate a state with starting permutation: this means
	//nothing will be switched; just apply where everything will stay
	//we have a 2D-Array with each entry possessing 2 ints for permutation reasins
	Image dither_data(boost::extents[image_width][image_height][4]);
	SimulatedAnnealing::loadPNGinArray(filename, dither_data);

	//we are computing retarget_0; this is retargeting dither 0 into dither 1
	//the next are computed on the fly with offsets
	Image next_dither_data(boost::extents[image_width][image_height][4]);
	SimulatedAnnealing::getNextDither(dither_data, next_dither_data, image_width, image_height);

	Image permutation_data_output(boost::extents[image_width][image_height][2]);
	for (int i = 0; i < image_width; i++) {

		for (int j = 0; j < image_height; j++) {

			//just assign the standard distribution
			//how this looks like; look at th etop for it 

			permutation_data_output[i][j][0] = 0;
			permutation_data_output[i][j][1] = 0;

		}
	}

	Image permutation_data_step(boost::extents[image_width][image_height][2]);

	for (int i = 0; i < number_steps; i++) {

		//calc the energy of our permutation
		float energy_old_condition = SimulatedAnnealing::calculateEnergy(dither_data, next_dither_data, permutation_data_output, image_width, image_height);
		std::cout << energy_old_condition << std::endl;
		//first we will go with the previous calculated permutation
		permutation_data_step = permutation_data_output;
		//now permute and have a look whether it is better
		//here we actually apply one permutation!
		SimulatedAnnealing::applyOneRandomPermutation(dither_data, next_dither_data, permutation_data_step, image_width, image_height);
	
		float energy_new_condition = SimulatedAnnealing::calculateEnergy(dither_data, next_dither_data, permutation_data_step, image_width, image_height);
		float ratio_steps = number_steps/(i+1);

		if (SimulatedAnnealing::acceptanceProbabilityFunction(energy_old_condition, energy_new_condition, ratio_steps)) {
			//we will have a new condition
			//https://www.boost.org/doc/libs/1_63_0/libs/multi_array/doc/user.html docs are garanteing deep copying!!
			permutation_data_output = permutation_data_step;

		}

	}

	FIBITMAP* retarget_bitmap = FreeImage_Allocate(image_width, image_height, 32);
	SimulatedAnnealing::fromArrayToBitmap(permutation_data_output, retarget_bitmap, image_width, image_height);

	SimulatedAnnealing::saveRetargetImageToFile("retargeted_texture.png", retarget_bitmap);

	return true;
}

std::vector<int> SimulatedAnnealing::toroidallyShift(unsigned int oldFrameDitherX, unsigned int oldFrameDitherY, uint32_t frame_width, uint32_t frame_height) {

	std::vector<int> new_positions(2, 0);
	using namespace std;
	double g = 1.32471795724474602596;
	double a1 = 1.0 / g;
	double a2 = 1.0 / (g * g);
	
	unsigned int xOffest = a1 * frame_width;
	unsigned int yOffset = a2 * frame_height;

	//x[n] = (0.5 + a1 * n) % 1;
	//y[n] = (0.5 + a2 * n) % 1;
	//thats happening right here next!!
	unsigned int oldFrameDitherXOffset = oldFrameDitherX + xOffest;
	unsigned int oldFrameDitherYOffset = oldFrameDitherY + yOffset;
	// now here the modulo 1!
	new_positions[0] = oldFrameDitherXOffset % frame_width;
	new_positions[1] = oldFrameDitherYOffset % frame_height;

	return new_positions;
}

bool SimulatedAnnealing::loadPNGinArray(const char* fileName, Image& image_data) {

	FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, fileName, PNG_DEFAULT);
	int image_height = FreeImage_GetHeight(bitmap);
    int image_width = FreeImage_GetWidth(bitmap);
	//allocate store in appropriate size
	RGBQUAD color;
	for (int i = 0; i < image_width; i++) {

		for (int j = 0; j < image_height; j++) {

			if(!FreeImage_GetPixelColor(bitmap, i, j, &color)) exit(1);

			image_data[i][j][0] = color.rgbRed;
			image_data[i][j][1] = color.rgbGreen;
			image_data[i][j][2] = color.rgbBlue;
			image_data[i][j][3] = color.rgbReserved;

		}
	}

	return true;
}

// https://www.arnoldrenderer.com/research/dither_abstract.pdf
float SimulatedAnnealing::calculateEnergy(Image& image_t, Image& image_next, Image& permutation, int width, int height) {
	
	float energy = 0;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int permutation_coordinates_x = i + permutation[i][j][0];
			int permutation_coordinates_y = j + permutation[i][j][1];
			float pixel_value_difference = 0;
				//pixel difference
				for (int m = 0; m < 4; m++) {
					energy += std::abs((image_t[permutation_coordinates_x][permutation_coordinates_y][m] - image_next[permutation_coordinates_x][permutation_coordinates_y][m]));
				}
		}
	}

	return energy;
}

bool SimulatedAnnealing::saveRetargetImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap) {

	return FreeImage_Save(FIF_PNG, retargetBitMap, filenameToSave, PNG_Z_NO_COMPRESSION);

}


/**
take in consideration, that only local swaps in a radius r = 6 is considered!
*/
std::vector<int> SimulatedAnnealing::selectRandomNeighborCondition(Image& image_data, int index_x, int index_y, int image_width, int image_height) {

	std::vector<int> result(2,0);

	int random_number_x = (rand() % 6) + 1;
	int random_number_y = (rand() % 6) + 1;

	//random number, either 0 or 1
	int sign_choosing_x = rand() % 2;
	int sign_choosing_y = rand() % 2;
	
	if (sign_choosing_x) {
		random_number_x *= -1;
	} 
	if (sign_choosing_y) {
		random_number_y *= -1;
	}

	int new_index_x = index_x + random_number_x;
	int new_index_y = index_y + random_number_y;

	return result;
}

bool SimulatedAnnealing::fromArrayToBitmap(Image& image_data, FIBITMAP* bitmap, uint32_t image_width, uint32_t image_height) {

	RGBQUAD color;

	for (int i = 0; i < image_width; i++) {

		for (int j = 0; j < image_height; j++) {

			color.rgbRed = image_data[i][j][0] + 6;
			color.rgbGreen = image_data[i][j][1] + 6;
			//to store permutation we will only need to save in rg - channel !!
			color.rgbBlue = 0x00;
			color.rgbReserved = 0xFF;

			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}

	return true;
}

bool SimulatedAnnealing::getNextDither(Image& dither_data, Image& next_dither_data, uint32_t frame_width, uint32_t frame_height) {

	for (int i = 0; i < frame_width; i++) {
		for (int j = 0; j < frame_height; j++) {
			
			std::vector<int> new_dither_positions = SimulatedAnnealing::toroidallyShift(i, j, frame_width, frame_height);
			int new_dither_x = new_dither_positions[0];
			int new_dither_y = new_dither_positions[1];
			next_dither_data[new_dither_x][new_dither_y] = dither_data[i][j];
		}
	}
	
	return true;
}

std::vector<int> SimulatedAnnealing::selectRandomPixelIndices(int image_width, int image_height) {

	std::vector<int> indices(2, 0);
	indices[0] = std::rand() % image_width;
	indices[1] = std::rand() % image_height;

	return indices;
}

bool SimulatedAnnealing::acceptanceProbabilityFunction(float energy_old_condition, float energy_new_condition, float ratio_steps) {
	//TODO: important!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! to make this a simulated annealing we have to bring in the temperature in the decision function
	//right now it is a simple hill climbing algorithm!!!!
	if (energy_new_condition < energy_old_condition) {
		return true;
	}
	else {
		return false;
	}
}

bool SimulatedAnnealing::applyOneRandomPermutation(Image& dither_data, Image& next_dither_data, Image& permutation_data_step, uint32_t image_width, uint32_t image_height) {
	
	bool no_permutation_found = true;
	int random_x = std::rand() % image_width;
	int random_y = std::rand() % image_height;
	int random_step_x = std::rand() % 7 - 6;
	int random_step_y = std::rand() % 7 - 6;

	//we have to check, whether this all happens in a radius of 6!
	while (no_permutation_found) {
		random_x = std::rand() % image_width;
		random_y = std::rand() % image_height;
		random_step_x = std::rand() % 7 - 6;
		random_step_y = std::rand() % 7 - 6;
		if (SimulatedAnnealing::isApplicablePermutation(random_x, random_y, random_step_x, random_step_y, image_width, image_height)) {
			no_permutation_found = false;
		}
	}

	permutation_data_step[random_x][random_y][0] += random_step_x;
	permutation_data_step[random_x][random_y][1] += random_step_y;
	permutation_data_step[random_x + random_step_x][random_y + random_step_y][0] += (-random_step_x);
	permutation_data_step[random_x + random_step_x][random_y + random_step_y][1] += (-random_step_y);

	return true;
}

bool SimulatedAnnealing::isApplicablePermutation(int random_x, int random_y, int random_step_x, int random_step_y, int image_width, int image_height) {
	if (((random_x + random_step_x) < 0) |
	     ((random_y + random_step_y) < 0) |
		 ((random_x + random_step_x) >= image_width) |
		((random_y + random_step_y) >= image_height)) {
		return false;
	}
	return true;
}