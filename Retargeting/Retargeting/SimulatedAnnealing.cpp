#include "SimulatedAnnealing.h"

bool SimulatedAnnealing::execute(const uint32_t  number_steps, const char* filename, const uint32_t image_width, const uint32_t image_height) {
	
	//generate a state with starting permutation: this means
	//nothing will be switched; just apply where everything will stay
	//we have a 2D-Array with each entry possessing 2 ints for permutation reasins
	Image dither_data;

	//we are computing retarget_0; this is retargeting dither 0 into dither 1
	//the next are computed on the fly with offsets
	Image next_dither_data;

	//permutation data structures
	Image permutation_data_output;
	Image permutation_data_step;

	//data structure for the new positions
	Image permutation_positions_step;
	Image permutation_positions_output;

	for (int i = 0; i < image_width; i++) {

		Column column_perm;
		Column column_dither;
		Column column_pos;

		for (int j = 0; j < image_height; j++) {

			//just assign the standard distribution
			//how this looks like; look at th etop for it 

			using namespace std;
			Values start_values_perm(2,0);
			Values start_values_dither(4,0);
			Values start_values_pos(2,0);
			start_values_pos[0] = i;
			start_values_pos[1] = j;
			column_perm.push_back(start_values_perm);
			column_dither.push_back(start_values_dither);
			column_pos.push_back(start_values_pos);

		}
		permutation_data_output.push_back(column_perm);
		permutation_data_step.push_back(column_perm);
		dither_data.push_back(column_dither);
		next_dither_data.push_back(column_dither);
		permutation_positions_step.push_back(column_pos);
		permutation_positions_output.push_back(column_pos);
	}

	loadPNGinArray(filename, dither_data);
	getNextDither(dither_data, next_dither_data, image_width, image_height);

	//help var for counting the good swaps
	uint32_t goodswaps = 0;

	for (int i = 0; i < number_steps; i++) {

		//calc the energy of our permutation
		float energy_old_condition = calculateEnergy(dither_data, next_dither_data, permutation_data_output, image_width, image_height);
		std::cout << energy_old_condition << std::endl;
		std::cout << i << std::endl;
		//first we will go with the previous calculated permutation
		deepCopyImage(permutation_data_output, permutation_data_step, image_width, image_height);
		deepCopyImage(permutation_positions_output, permutation_positions_step, image_width, image_height);
		//now permute and have a look whether it is better
		//here we actually apply one permutation!
		applyOneRandomPermutation(permutation_data_step, permutation_positions_step, image_width, image_height);
	
		float energy_new_condition = calculateEnergy(dither_data, next_dither_data, permutation_data_step, image_width, image_height);
		float ratio_steps = number_steps/(i+1);

		if (acceptanceProbabilityFunction(energy_old_condition, energy_new_condition, ratio_steps)) {
			//we will have a new condition
			//https://www.boost.org/doc/libs/1_63_0/libs/multi_array/doc/user.html docs are garanteing deep copying!!
			deepCopyImage(permutation_data_step, permutation_data_output, image_width, image_height);
			deepCopyImage(permutation_positions_step, permutation_positions_output, image_width, image_height);
			goodswaps++;
			std::cout << "#Gute Tausche: " << goodswaps << endl;
		}

	}

	FIBITMAP* retarget_bitmap = FreeImage_Allocate(image_width, image_height, 32);
	fromArrayToBitmap(permutation_data_output, retarget_bitmap, image_width, image_height);

	saveRetargetImageToFile("retargeted_texture.png", retarget_bitmap);

	return true;
}


bool SimulatedAnnealing::applyOneRandomPermutation(Image& permutation_data_step, Image& permutation_positions, const uint32_t image_width, const uint32_t image_height) {
	
	bool no_permutation_found = true;

	//random position on the texture
	int32_t random_x = std::rand() % image_width;
	int32_t random_y = std::rand() % image_height;
	int32_t random_x_VZ = (std::rand() % 2);
	int32_t random_y_VZ = (std::rand() % 2);
	int32_t random_step_x = (std::rand() % 7);
	int32_t random_step_y = (std::rand() % 7);
	if (random_x_VZ) {
		random_step_x *= -1;
	}
	if (random_y_VZ) {
		random_step_y *= -1;
	}

	//we have to check, whether this all happens in a radius of 6!
	do {

		//random position on the texture
		random_x = std::rand() % image_width;
		random_y = std::rand() % image_height;
		random_x_VZ = (std::rand() % 2);
		random_y_VZ = (std::rand() % 2);
		random_step_x = (std::rand() % 7);
		random_step_y = (std::rand() % 7);

		if (random_x_VZ) {
			random_step_x *= -1;
		}
		if (random_y_VZ) {
			random_step_y *= -1;
		}

		if (isApplicablePermutation(permutation_data_step, permutation_positions, random_x, random_y, random_step_x, random_step_y, image_width, image_height)) {
			no_permutation_found = false;
		}

	} while (no_permutation_found);

	//get the original position where the permuted value comes from
	int position_x = permutation_positions[random_x][random_y][0];
	int position_y = permutation_positions[random_x][random_y][1];

	int permute_x = permutation_data_step[position_x][position_y][0];
	int permute_y = permutation_data_step[position_x][position_y][1];

	permutation_data_step[position_x][position_y][0] = permute_x + random_step_x;
	permutation_data_step[position_x][position_y][1] = permute_y + random_step_y;

	int index_swap_position_x = position_x + permute_x + random_step_x;
	int index_swap_position_y = position_y + permute_y + random_step_y;

	int swap_position_x = permutation_positions[index_swap_position_x][index_swap_position_y][0];
	int swap_position_y = permutation_positions[index_swap_position_x][index_swap_position_y][1];

	int swap_permute_x_value = permutation_data_step[swap_position_x][swap_position_y][0];
	int swap_permute_y_value = permutation_data_step[swap_position_x][swap_position_y][1];

	permutation_data_step[swap_position_x][swap_position_y][0] = swap_permute_x_value - random_step_x;
	permutation_data_step[swap_position_x][swap_position_y][1] = swap_permute_y_value - random_step_y;

	//update position data; normal swap...
	permutation_positions[random_x][random_y][0] = swap_position_x;
	permutation_positions[random_x][random_y][1] = swap_position_y;

	permutation_positions[index_swap_position_x][index_swap_position_y][0] = position_x;
	permutation_positions[index_swap_position_x][index_swap_position_y][1] = position_y;

	std::cout << "Dies sind die Permutationen x = " << permutation_data_step[position_x][position_y][0] << " und y = " << permutation_data_step[position_x][position_y][1] << endl;
	std::cout << "Position x = " << position_x << "y = " << position_y << endl;
	return true;
}

bool SimulatedAnnealing::isApplicablePermutation(Image& permutation_data_step, Image& permutation_positions, const int random_x, const int random_y, const int random_step_x, const int random_step_y, const int image_width, const int image_height) {
	
	int position_x = permutation_positions[random_x][random_y][0];
	int position_y = permutation_positions[random_x][random_y][1];

	int permute_x = permutation_data_step[position_x][position_y][0];
	int permute_y = permutation_data_step[position_x][position_y][1];

	if (((permute_x + random_step_x) > 6) |
		 ((permute_y + random_step_y) > 6) |
		((permute_x + random_step_x) < -6) |
	   ((permute_y + random_step_y) < -6)) {
		return false;
	}

	if (((position_x + permute_x + random_step_x) < 0) |
	     ((position_y + permute_y + random_step_y) < 0) |
		 ((position_x + permute_x +random_step_x) >= image_width) |
		((position_y + permute_y + random_step_y) >= image_height)) {
		return false;
	}

	int index_swap_position_x = position_x + permute_x + random_step_x;
	int index_swap_position_y = position_y + permute_y + random_step_y;

	int swap_position_x = permutation_positions[index_swap_position_x][index_swap_position_y][0];
	int swap_position_y = permutation_positions[index_swap_position_x][index_swap_position_y][1];

	int swap_permute_x_value = permutation_data_step[swap_position_x][swap_position_y][0];
	int swap_permute_y_value = permutation_data_step[swap_position_x][swap_position_y][1];

	if (((swap_permute_x_value - random_step_x) > 6) |
		((swap_permute_y_value - random_step_y) > 6) |
		((swap_permute_x_value - random_step_x) < -6) |
		((swap_permute_y_value - random_step_y) < -6)) {
		return false;
	}

	//hold the regional permutation!!
	return true;
}

// https://www.arnoldrenderer.com/research/dither_abstract.pdf
float SimulatedAnnealing::calculateEnergy(Image& image_t, Image& image_next, Image& permutation, const int width, const int height) {

	float energy = 0;

	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			int permutation_coordinates_x = i + permutation[i][j][0];
			int permutation_coordinates_y = j + permutation[i][j][1];

			for (int m = 0; m < 1; m++) {
				energy += std::abs(image_t[i][j][m] - image_next[permutation_coordinates_x][permutation_coordinates_y][m]);
			}
		}
	}

	return energy;
}


bool SimulatedAnnealing::acceptanceProbabilityFunction(const float energy_old_condition, const float energy_new_condition, const float ratio_steps) {
	//TODO: important!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! to make this a simulated annealing we have to bring in the temperature in the decision function
	//right now it is a simple hill climbing algorithm!!!!
	if (energy_new_condition < energy_old_condition) {
		return true;
	}
	else {
		return false;
	}
}

bool SimulatedAnnealing::saveRetargetImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap) {

	return FreeImage_Save(FIF_PNG, retargetBitMap, filenameToSave, PNG_Z_NO_COMPRESSION);

}

bool SimulatedAnnealing::fromArrayToBitmap(Image& image_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height) {

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

bool SimulatedAnnealing::getNextDither(Image& dither_data, Image& next_dither_data, const uint32_t frame_width, const uint32_t frame_height) {

	for (int i = 0; i < frame_width; i++) {
		for (int j = 0; j < frame_height; j++) {
			
			std::vector<int> new_dither_positions = toroidallyShift(i, j, frame_width, frame_height);
			int new_dither_x = new_dither_positions[0];
			int new_dither_y = new_dither_positions[1];
			next_dither_data[new_dither_x][new_dither_y] = dither_data[i][j];
		}
	}
	
	return true;
}

std::vector<int> SimulatedAnnealing::toroidallyShift(const unsigned int oldFrameDitherX, const unsigned int oldFrameDitherY, const uint32_t frame_width, const uint32_t frame_height) {

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

bool SimulatedAnnealing::deepCopyImage(Image& source, Image& dest, const int image_width, const int image_height) {
	
	for (int i = 0; i < image_width; i++) {
		for (int j = 0; j < image_height; j++) {

			int x_source = source[i][j][0];
			int y_source = source[i][j][1];
			dest[i][j][0] = x_source;
			dest[i][j][1] = y_source;
		}
	}
	return true;
}