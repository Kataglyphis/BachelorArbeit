#pragma once
#include "matplotlib/matplotlibcpp.h"
#include <sstream>
#include <string>
#include "AnnealingSchedule.h"
#include "Hajek.h"
#include "ExponentialCoolDown.h"
#include "SimulatedAnnealing.h"
#include "Lineary.h"
#include "Inverse.h"

typedef std::vector<int> Energy;

class CoolDownTester
{
public:

	CoolDownTester();
	CoolDownTester(int number_steps, int image_width, int image_height, const char* filename);
	void compareDifferentCoolDownSchedules();

private:
	std::vector<Energy> energies;
	std::vector<SimulatedAnnealing> sas;
	const char* dither_texture_path;
	int number_steps;
	std::string folder_energy = "pictures/Energy/";
	int image_width;
	int image_height;
};

