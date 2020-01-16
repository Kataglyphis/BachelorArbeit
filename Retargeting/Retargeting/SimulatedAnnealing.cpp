﻿#include "SimulatedAnnealing.h"

SimulatedAnnealing::SimulatedAnnealing(int number_steps) : helper(), image_width(64), image_height(64), visualizer() {
	this->number_steps = number_steps;
}

Image SimulatedAnnealing::execute(const char* filename, int& good_swaps) {
	
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

	helper.loadPNGinArray(filename, dither_data);
	helper.getNextDither(dither_data, next_dither_data, image_width, image_height);

	for (unsigned int i = 0; i < this->number_steps; i++) {

		//calc the energy of our permutation
		float energy_old_condition = calculateEnergy(dither_data, next_dither_data, permutation_data_output);
		if (i == 0) {
			energy.push_back(energy_old_condition);
		}
		//std::cout << "Energy of the old condition: " << energy_old_condition << std::endl;
		//std::cout << "Step: " << i << std::endl;
		//first we will go with the previous calculated permutation
		helper.deepCopyImage(permutation_data_output, permutation_data_step, image_width, image_height);
		helper.deepCopyImage(permutation_positions_output, permutation_positions_step, image_width, image_height);
		//now permute and have a look whether it is better
		//here we actually apply one permutation!
		applyOneRandomPermutation(permutation_data_step, permutation_positions_step);
	
		float energy_new_condition = calculateEnergy(dither_data, next_dither_data, permutation_data_step);
		float ratio_steps = ((float)(number_steps))/((float)(i+1));

		if (acceptanceProbabilityFunction(energy_old_condition, energy_new_condition, ratio_steps)) {
			//we will have a new condition
			helper.deepCopyImage(permutation_data_step, permutation_data_output, image_width, image_height);
			helper.deepCopyImage(permutation_positions_step, permutation_positions_output, image_width, image_height);
			good_swaps++;
			//std::cout << "Swap has been successful. \n" << "#Gute Tausche: " << good_swaps << endl;
			//push it in the energy array
			energy.push_back((int)energy_new_condition);
		}

	}



	FIBITMAP* retarget_bitmap = FreeImage_Allocate(image_width, image_height, 32);

	helper.fromPermuteToBitmap(permutation_data_output, retarget_bitmap, image_width, image_height);

	stringstream ss;
	ss << "retargeted_texture_" << good_swaps << "_swaps" <<".png";
	helper.saveImageToFile(ss.str().c_str(), retarget_bitmap);

	visualizer.visualizeEnergyOverSteps(energy);

	return permutation_data_output;
}


bool SimulatedAnnealing::applyOneRandomPermutation(Image& permutation_data_step, Image& permutation_positions) {
	
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

		if (isApplicablePermutation(permutation_data_step, permutation_positions, random_x, random_y, random_step_x, random_step_y)) {
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

	//std::cout << "Dies sind die Permutationsschritte x = " << permutation_data_step[position_x][position_y][0] << " und y = " << permutation_data_step[position_x][position_y][1] << endl;
	//std::cout << "Position x = " << position_x << "y = " << position_y << endl;
	return true;
}

bool SimulatedAnnealing::isApplicablePermutation(Image& permutation_data_step, Image& permutation_positions, const int random_x, const int random_y, const int random_step_x, const int random_step_y) {
	
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

//https://www.arnoldrenderer.com/research/dither_abstract.pdf
float SimulatedAnnealing::calculateEnergy(Image image_t, Image image_next, Image permutation) {

	float energy = 0;

	for (int i = 0; i < image_width; i++) {
		for (int j = 0; j < image_height; j++) {
			int permutation_coordinates_x = i + permutation[i][j][0];
			int permutation_coordinates_y = j + permutation[i][j][1];

			for (int m = 0; m < 1; m++) {
				energy += std::abs(image_t[permutation_coordinates_x][permutation_coordinates_y][m] - image_next[i][j][m]);
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