#include "SimulatedAnnealing.h"

//do not use
SimulatedAnnealing::SimulatedAnnealing() /*: helper(), image_width(64), image_height(64), visualizer(), good_swaps(0), visualize(false)*/ {
	
	/*this->number_steps = number_steps;
	this->schedule = new Hajek();
	this->temperature = schedule->getTemperature(0);
	visualizer = SimulatedAnnealingVisualizer(this->schedule);*/

}

SimulatedAnnealing::SimulatedAnnealing(int number_steps, AnnealingSchedule* schedule, Energy& energy, bool visualize_single_annealing, int image_width, int image_height, 
	helpers helper, const char* filename) : visualizer(), good_swaps(0), intermediate_step_count(0), energy(0.f) {
	
	this->filename = filename;
	this->helper = helper;
	this->visualize = visualize_single_annealing;
	this->number_steps = number_steps;
	this->temperature = schedule->getTemperature(0);
	//decide here which cooldown function
	this->schedule = schedule;
	this->energy = energy;
	//this->temerature_step = this->max_energy_difference / (number_steps + 1);
	visualizer = SimulatedAnnealingVisualizer(schedule);
	this->image_width = image_width; 
	this->image_height = image_height;

	helper.initializeImage(dither_data);
	helper.initializeImage(next_dither_data);
	helper.initializePermutation(permutation_data_output);
	helper.initializePositions(permutation_positions_output);

}

Image SimulatedAnnealing::execute(Image org, const char* temp_rep_filename, int offset_x, int offset_y, int& good_swaps, float& progress_temp, int dim) {

	helper.getNextDither(org, next_dither_data, image_width, image_height);
	this->current_energy = calculateStartingEnergy(dither_data, next_dither_data, permutation_data_output);

	//int number_of_dim = 64;
	int number_of_intermediate_snapshots = 10;
	int when_taking_snapshot = this->number_steps / number_of_intermediate_snapshots;
	float progress_prev = progress_temp;

	for (unsigned int i = 0; i < this->number_steps; i++) {

		progress_temp = progress_prev +  (float)i / ((float) (number_steps * dim));
		//if ((i % when_taking_snapshot) == 0) takeIntermediateSnapshot(permutation_data_output, dither_data);

		this->temperature = schedule->getTemperature(good_swaps);
		temperatures.push_back(this->temperature);

#ifdef _DEBUG 
		std::cout << "Aktuelle Temperatur ist " << this->temperature << "\n";
#endif

		//calc the energy of our permutation
		//float energy_old_condition = calculatePermutationEnergy(dither_data, next_dither_data, permutation_data_output);
		if (i == 0) {
			energy.push_back(current_energy);
		}

		//first we will go with the previous calculated permutation
		//now permute and have a look whether it is better
		//here we actually apply one permutation!
		permutation new_pair;
		permutation old_pair;
		old_indices indices;
		old_indices swap_indices;
		//float energy_old_condition = applyOneRandomPermutation(permutation_data_step, permutation_positions_step, pair);
		float energy_old_condition = applyOneRandomPermutation(permutation_data_output, permutation_positions_output, new_pair, old_pair, indices, swap_indices);

#ifdef _DEBUG
		std::cout << "Current energy: " << current_energy << std::endl;
		std::cout << "Step: " << i << std::endl;
#endif

		float energy_new_condition = calculatePermutationEnergy(dither_data, next_dither_data, new_pair);

		//std::cout << "Energy of the new condition: " << energy_new_condition << std::endl;

		if (acceptanceProbabilityFunction(energy_old_condition, energy_new_condition, this->temperature)) {

			//we will have a new condition
			good_swaps++;
#ifdef _DEBUG
			std::cout << "Swap has been successful. \n" << "#Gute Tausche: " << good_swaps << endl;
#endif
			//push it in the energy array
			current_energy -= (energy_old_condition - energy_new_condition);
			energy.push_back(this->current_energy);

		}
		else {

			withdraw_permutation(permutation_data_output, permutation_positions_output, old_pair, indices, swap_indices);

		}

	}

	this->good_swaps = good_swaps;

	FIBITMAP* retarget_bitmap = FreeImage_Allocate(image_width, image_height, 32);

	helper.fromPermuteToBitmap(permutation_data_output, retarget_bitmap, image_width, image_height);

	//save the texture in its corresponding file
	stringstream ss;
	ss << temp_rep_filename << offset_x << "x" << offset_y << "_"<< "permutation_texture" << ".png";
	helper.saveImageToFile(ss.str().c_str(), retarget_bitmap);

	if (visualize) {
		visualizer.visualizeEnergyOverSteps(energy);
		visualizer.visualizeTemperatureOverSteps(temperatures);
		visualizer.visualizeAcceptanceProbabilities(this->deltas, this->probs);
	}

	return permutation_data_output;
}

Image SimulatedAnnealing::execute(int& good_swaps, float& progress) {

	helper.loadPNGinArray(this->filename, dither_data);
	helper.getNextDither(dither_data, next_dither_data, image_width, image_height);
	this->current_energy = calculateStartingEnergy(dither_data, next_dither_data, permutation_data_output);

	int number_of_intermediate_snapshots = 20;
	int when_taking_snapshot = this->number_steps / number_of_intermediate_snapshots;

	for (unsigned int i = 0; i < this->number_steps; i++) {

		progress = i / float(number_steps);

		//if ((i % when_taking_snapshot) == 0) takeIntermediateSnapshot(permutation_data_output, dither_data);

		this->temperature = schedule->getTemperature(good_swaps);
		temperatures.push_back(this->temperature);

		#ifdef _DEBUG 
		std::cout << "Aktuelle Temperatur ist " << this->temperature << "\n";
		#endif

		//calc the energy of our permutation
		//float energy_old_condition = calculatePermutationEnergy(dither_data, next_dither_data, permutation_data_output);
		if (i == 0) {
			energy.push_back(current_energy);
		}

		//first we will go with the previous calculated permutation
		//now permute and have a look whether it is better
		//here we actually apply one permutation!
		permutation new_pair;
		permutation old_pair;
		old_indices indices;
		old_indices swap_indices;
		//float energy_old_condition = applyOneRandomPermutation(permutation_data_step, permutation_positions_step, pair);
		float energy_old_condition = applyOneRandomPermutation(permutation_data_output, permutation_positions_output, new_pair, old_pair, indices, swap_indices);
		
		#ifdef _DEBUG
		std::cout << "Current energy: " << current_energy << std::endl;
		std::cout << "Step: " << i << std::endl;
		#endif
	
		float energy_new_condition = calculatePermutationEnergy(dither_data, next_dither_data, new_pair);

		//std::cout << "Energy of the new condition: " << energy_new_condition << std::endl;

		if (acceptanceProbabilityFunction(energy_old_condition, energy_new_condition, this->temperature)) {
			
			//we will have a new condition
			good_swaps++;
			#ifdef _DEBUG
			std::cout << "Swap has been successful. \n" << "#Gute Tausche: " << good_swaps << endl;
			#endif
			//push it in the energy array
			current_energy -= (energy_old_condition - energy_new_condition);
			energy.push_back(this->current_energy);
		
		} else {
  
			withdraw_permutation(permutation_data_output, permutation_positions_output, old_pair, indices, swap_indices);

		}

	}

	this->good_swaps = good_swaps;

	FIBITMAP* retarget_bitmap = FreeImage_Allocate(image_width, image_height, 32);

	helper.fromPermuteToBitmap(permutation_data_output, retarget_bitmap, image_width, image_height);

	stringstream ss;
	ss << this->folder_permutation_textures << "permutation_texture_" << good_swaps << "_swaps" << schedule->getName() <<".png";
	helper.saveImageToFile(ss.str().c_str(), retarget_bitmap);

	if (visualize) {
		visualizer.visualizeEnergyOverSteps(energy);
		visualizer.visualizeTemperatureOverSteps(temperatures);
		visualizer.visualizeAcceptanceProbabilities(this->deltas, this->probs);
	}

	return permutation_data_output;
}


float SimulatedAnnealing::applyOneRandomPermutation(Image& permutation_data_step, Image& permutation_positions, permutation& new_pair, 
																	permutation& old_pair, old_indices& indices, old_indices& swap_indices) {
	
	bool no_permutation_found = true;

	std::random_device rd;
	std::mt19937_64 gen(rd());
	// initialize the random number generator with time-dependent seed
	// initialize a uniform distribution between 0 and 1
	std::uniform_real_distribution<> unif_step(-7., 7.);
	std::uniform_real_distribution<> unif_position_x(0, this->image_width);
	std::uniform_real_distribution<> unif_position_y(0, this->image_height);

	//random position on the texture
	int32_t random_x = 0;// (int32_t)unif_position(gen);
	int32_t random_y = 0; //(int32_t)unif_position(gen);
	int32_t random_step_x = 0;// (int32_t)unif_step(gen);
	int32_t random_step_y = 0;// (int32_t)unif_step(gen);

	//we have to check, whether this all happens in a radius of 6!
	do {

		//random position on the texture
		random_x = (int32_t)unif_position_x(gen);
		random_y = (int32_t)unif_position_y(gen);
		random_step_x = (int32_t)unif_step(gen);
		random_step_y = (int32_t)unif_step(gen);

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

	int index_swap_position_x = (position_x + permute_x + random_step_x + image_width) % image_width;
	int index_swap_position_y = (position_y + permute_y + random_step_y + image_height) % image_height;

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

#ifdef _DEBUG

	std::cout << "Dies sind die Permutationsschritte x = " << permutation_data_step[position_x][position_y][0] << " und y = " << permutation_data_step[position_x][position_y][1] << endl;
	std::cout << "Position x = " << position_x << "y = " << position_y << endl;

#endif // _DEBUG

	//save indices
	indices.first.first = random_x;
	indices.first.second = random_y;
	indices.second.first = permute_x;
	indices.second.second = permute_y;

	swap_indices.first.first = index_swap_position_x;
	swap_indices.first.second = index_swap_position_y;
	swap_indices.second.first = swap_permute_x_value;
	swap_indices.second.second = swap_permute_y_value;

	std::pair<int,int> old_pos_pair_one(position_x, position_y);
	std::pair<int, int> old_step_pair_one(permute_x , permute_y);
	permutation_pair old_perm_pair_one(old_pos_pair_one, old_step_pair_one);

	std::pair<int, int> old_pos_pair_two(swap_position_x, swap_position_y);
	std::pair<int, int> old_step_pair_two(swap_permute_x_value, swap_permute_y_value);
	permutation_pair old_perm_pair_two(old_pos_pair_two, old_step_pair_two);

	old_pair = permutation(old_perm_pair_one, old_perm_pair_two);

	float result = calculatePermutationEnergy(dither_data, next_dither_data, old_pair);

	//for the new permutation
	std::pair<int, int> new_pos_pair_one(position_x, position_y);
	std::pair<int, int> new_step_pair_one(permute_x + random_step_x, permute_y + random_step_y);
	permutation_pair new_perm_pair_one(new_pos_pair_one, new_step_pair_one);

	std::pair<int, int> new_pos_pair_two(swap_position_x, swap_position_y);
	std::pair<int, int> new_step_pair_two(swap_permute_x_value - random_step_x, swap_permute_y_value - random_step_y);
	permutation_pair new_perm_pair_two(new_pos_pair_two, new_step_pair_two);

	new_pair = permutation(new_perm_pair_one, new_perm_pair_two);

	return result;
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
	//removed for toroidally shift support !!!
	/**if (((position_x + permute_x + random_step_x) < 0) |
	     ((position_y + permute_y + random_step_y) < 0) |
		 ((position_x + permute_x +random_step_x) >= image_width) |
		((position_y + permute_y + random_step_y) >= image_height)) {
		return false;
	}*/

	//added toroidally shifting
	int index_swap_position_x = (position_x + permute_x + random_step_x + image_width) % image_width;
	int index_swap_position_y = (position_y + permute_y + random_step_y + image_height) % image_height;

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
float SimulatedAnnealing::calculatePermutationEnergy(Image image_t, Image image_next, permutation pair) {
	//float position_delta = () / std::pow(sigma_i, 2);

	float energy = 0.f;

	int first_permutation_coordinates_x = (pair.first.first.first + pair.first.second.first + image_width) % image_width;
	int first_permutation_coordinates_y = (pair.first.first.second + pair.first.second.second + image_height) % image_height;

	int second_permutation_coordinates_x = (pair.second.first.first + pair.second.second.first + image_width) % image_width;
	int second_permutation_coordinates_y = (pair.second.first.second + pair.second.second.second + image_height) % image_height;

	for (int m = 0; m < 1; m++) {
		//calc the direct difference;
		energy += (std::abs(image_t[pair.first.first.first][pair.first.first.second][m] - image_next[first_permutation_coordinates_x][first_permutation_coordinates_y][m]));
		energy += (std::abs(image_t[pair.second.first.first][pair.second.first.second][m] - image_next[second_permutation_coordinates_x][second_permutation_coordinates_y][m]));
	}

	return energy;
}

float SimulatedAnnealing::calculateStartingEnergy(Image image_t, Image image_next, Image permutation) {

	float energy = 0.f;

	for (int i = 0; i < image_width; i++) {
		for (int j = 0; j < image_height; j++) {

			for (int m = 0; m < 1; m++) {
				//calc the direct difference;
				energy += (std::abs(image_t[i][j][m] - image_next[i][j][m]));
				//for really making it look like a dither we will have to consider the direct neighborhood
			}
		}
	}

	return energy;
}

bool SimulatedAnnealing::acceptanceProbabilityFunction(const float energy_old_condition, const float energy_new_condition, const float temperature) {
	
	//right now it is a simple hill climbing algorithm!!!!
	bool result = false;
	float delta = energy_new_condition - energy_old_condition;
	
	#ifdef _DEBUG

	std::cout << "Energy delta is " << delta << "\n";

	#endif // _DEBUG


	if (delta <= 0) {

		result =  true;

	} else {

		std::random_device rd;
		std::mt19937_64 gen(rd());
		// initialize the random number generator with time-dependent seed
		// initialize a uniform distribution between 0 and 1
		std::uniform_real_distribution<> unif(0., 1.);
		float currentRandomNumber = unif(gen);
		//cout << "Current random number is: "<< currentRandomNumber << "\n";
		//prob from our syst
		float prob = std::exp((-delta)/temperature);

		#ifdef _DEBUG

		std::cout << "Decision Probability is " << prob << "\n";

		#endif // _DEBUG
		this->deltas.push_back(delta);
		this->probs.push_back((int)(prob * 100));		


		if (prob > currentRandomNumber) {
			result = true;
		//for plotting reasons
		}
	}


	return result;
}

Energy SimulatedAnnealing::getEnergy() {

	return this->energy;

}

AnnealingSchedule* SimulatedAnnealing::getSchedule() {

	return this->schedule;

}

int SimulatedAnnealing::getNumSwaps() {

	return this->good_swaps;

}

void SimulatedAnnealing::takeIntermediateSnapshot(Image permutation, Image original) {

	Image permutatedOriginal = helper.applyPermutationToOriginal(original, permutation);

	stringstream ss;
	ss << this->folder_intermediate_steps << "intermediate_applied_permutation_" << intermediate_step_count  << "_quasieqstep" << schedule->getName() <<
		"_" << "energy_"<< this->current_energy <<".png";
	helper.fromImageToFile(ss.str().c_str(), permutatedOriginal);
	++intermediate_step_count;
}

void SimulatedAnnealing::withdraw_permutation(Image& permutation_data_output, Image& permutation_positions_output, permutation old_pair, 
																															old_indices indices, old_indices swap) {

	permutation_positions_output[indices.first.first][indices.first.second][0] = old_pair.first.first.first;
	permutation_positions_output[indices.first.first][indices.first.second][1] = old_pair.first.first.second;

	permutation_positions_output[swap.first.first][swap.first.second][0] = old_pair.second.first.first;
	permutation_positions_output[swap.first.first][swap.first.second][1] = old_pair.second.first.second;

	permutation_data_output[old_pair.first.first.first][old_pair.first.first.second][0] = indices.second.first;
	permutation_data_output[old_pair.first.first.first][old_pair.first.first.second][1] = indices.second.second;

	permutation_data_output[old_pair.second.first.first][old_pair.second.first.second][0] = swap.second.first;
	permutation_data_output[old_pair.second.first.first][old_pair.second.first.second][1] = swap.second.second;
}