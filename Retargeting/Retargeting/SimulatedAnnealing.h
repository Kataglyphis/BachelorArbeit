#pragma once
#include "helpers.h"
#include "SimulatedAnnealingVisualizer.h"
#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <stdio.h>

using namespace std;

typedef vector<int> Energy;

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
        SimulatedAnnealing(int number_steps);
        int image_width;
        int image_height;
        Energy energy;
        int number_steps;
		Image execute(const char* filename, int& good_swaps);
        SimulatedAnnealingVisualizer visualizer;
private:
        helpers helper;
		float calculateEnergy(Image image_t, Image image_next, Image permutation);
        bool acceptanceProbabilityFunction(const float energy_old_condition, const float energy_new_condition, const float ratio_steps);
        bool applyOneRandomPermutation(Image& permutation_data_output, Image& permutation_positions);
        bool isApplicablePermutation(Image& permutation_data_step, Image& permutation_positions, const int random_x, const int random_y, const int random_step_x, const int random_step_y);
		
};