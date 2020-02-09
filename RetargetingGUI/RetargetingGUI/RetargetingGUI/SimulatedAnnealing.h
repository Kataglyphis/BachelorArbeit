#pragma once
#include "helpers.h"
#include "SimulatedAnnealingVisualizer.h"
#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <random>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "AnnealingSchedule.h"
#include "Hajek.h"
#include "ExponentialCoolDown.h"
#include "Inverse.h"
#include "Lineary.h"
#include "Kirkpatrick.h"
#include <fstream>
#include <sstream>

#include <time.h>


using namespace std;

typedef vector<int> Energy;
typedef std::vector<double> Deltas;
typedef std::vector<int> Probabilities;

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

        SimulatedAnnealing(int number_steps, AnnealingSchedule* schedule, Energy& energy, bool visualize_single_annealing);
        SimulatedAnnealing(int number_steps, AnnealingSchedule* schedule, Energy& energy, bool visualize_single_annealing, QObject* q_object);
        SimulatedAnnealing();
		Image execute(const char* filename, int& good_swaps);
        Energy getEnergy();
        AnnealingSchedule* getSchedule();
        int getNumSwaps();

private:

        SimulatedAnnealingVisualizer visualizer;
        AnnealingSchedule* schedule;
        int good_swaps;
        int image_width;
        int image_height;
        Energy energy;
        Deltas deltas;
        Probabilities probs;
        float max_energy_difference = 40.f;
        int number_steps;
        float temperature;
        bool visualize;
        std::string folder_permutation_textures = "pictures/Permutations/";
        
        //for Qt
        QProgressBar* progress_bar;

        helpers helper;
		float calculateEnergy(Image image_t, Image image_next, Image permutation);
        bool acceptanceProbabilityFunction(const float energy_old_condition, const float energy_new_condition, const float ratio_steps);
        bool applyOneRandomPermutation(Image& permutation_data_output, Image& permutation_positions);
        bool isApplicablePermutation(Image& permutation_data_step, Image& permutation_positions, const int random_x, const int random_y, const int random_step_x, const int random_step_y);
		
};