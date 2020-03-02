#pragma once
#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <random>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <time.h>

#include "helpers.h"
#include "SimulatedAnnealingVisualizer.h"
#include "AnnealingSchedule.h"
#include "Hajek.h"
#include "ExponentialCoolDown.h"
#include "Inverse.h"
#include "Lineary.h"
#include "Kirkpatrick.h"


using namespace std;

typedef vector<int> Energy;
typedef vector<int> Temperatures;
typedef std::vector<double> Deltas;
typedef std::vector<int> Probabilities;

typedef std::pair<int,int> position;
typedef std::pair<int,int> step;
typedef std::pair<position, step> permutation_pair;

typedef std::pair<permutation_pair, permutation_pair> permutation;

typedef std::pair<unsigned int,unsigned int> position_index;
typedef std::pair<unsigned int, unsigned int> step_index;
typedef std::pair<position_index, step_index> old_indices;

//typedef Image::index index;
/**
	stores permutation in this pixel manner; numbers representing indices
   0 --- 1 --- 2 --- 3 --- 4 --- 5 --- 6 --- 7 --- 8 --- 9 --- 10 --- 11 --- 12
   |                                                                                              |
   1                                                                                            |
   |                                                                                              |
   2                                                                                             |   
   |                                                                                              |
   3                                                                                             |
   |                                                                                               |
   4                                                                                              |
   |                                                                                               |
   5                                                                                              | 
   |                                                                                               |
   6                                                                                              |
   |                                                                                               |
   7                                                                                              |
   |                                                                                               |
   8                                                                                              |
   |                                                                                               |
   9                                                                                              |
   |                                                                                               |
   10                                                                                            |
   |                                                                                               |
   11                                                                                            |
   |                                                                                                |
   12  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | 
*/

/**
	Let s = s0
	For k = 0 through kmax (exclusive):
	T ← temperature( kmax/(k+1) )
	Pick a random neighbour, snew ← neighbour(s)
	If P(E(s), E(snew), T) ≥ random(0, 1):
	s ← snew
	Output: the final state s
*/

class SimulatedAnnealing {
	public:

        SimulatedAnnealing(int number_steps, AnnealingSchedule* schedule, Energy& energy, bool visualize_single_annealing, int image_width, int image_height, helpers helper, const char* filename);
        SimulatedAnnealing();
		Image execute(int& good_swaps);
        Image execute(Image org, const char* temp_rep_filename, int offset_x, int offset_y, int& good_swaps);
        Energy getEnergy();
        AnnealingSchedule* getSchedule();
        int getNumSwaps();

private:

        SimulatedAnnealingVisualizer visualizer;
        AnnealingSchedule* schedule;
        float current_energy;
        int good_swaps;
        int image_width;
        int image_height;
        Energy energy;
        Temperatures temperatures;
        Deltas deltas;
        Probabilities probs;
        float max_energy_difference = 40.f;
        int number_steps;
        float temperature;
        bool visualize;
        std::string folder_intermediate_steps = "pictures/AppliedPermutation/Intermediate_Steps/";
        uint32_t intermediate_step_count;
        uint32_t num_intermediate_shots = 10;
        std::string folder_permutation_textures = "pictures/Permutations/";
        const char* filename;

        //generate a state with starting permutation: this means
        //nothing will be switched; just apply where everything will stay
        //we have a 2D-Array with each entry possessing 2 ints for permutation reasins
        Image dither_data;

        //we are computing retarget_0; this is retargeting dither 0 into dither 1
        //the next are computed on the fly with offsets
        Image next_dither_data;

        //permutation data structures
        Image permutation_data_output;
        //Image permutation_data_step;

        //data structure for the new positions
        //Image permutation_positions_step;
        Image permutation_positions_output;

        helpers helper;
		float calculatePermutationEnergy(Image image_t, Image image_next, permutation perm_pair);
        bool acceptanceProbabilityFunction(const float energy_old_condition, const float energy_new_condition, const float ratio_steps);
        float applyOneRandomPermutation(Image& permutation_data_output, Image& permutation_positions, permutation& new_pair, permutation& old_pair, old_indices& indices, old_indices& swap_indices);
        bool isApplicablePermutation(Image& permutation_data_step, Image& permutation_positions, const int random_x, const int random_y, const int random_step_x, const int random_step_y);
        void takeIntermediateSnapshot(Image Permutation, Image Original);
        float calculateStartingEnergy(Image image_t, Image image_next, Image permutation);
        void withdraw_permutation(Image& permutation_data_output, Image& permutation_positions_output, permutation old_pair, old_indices indices, old_indices swap);
		
};