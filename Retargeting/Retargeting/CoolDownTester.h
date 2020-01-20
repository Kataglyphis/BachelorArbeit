#pragma once
#include "matplotlib/matplotlibcpp.h"
#include <sstream>
#include <string>
#include "AnnealingSchedule.h"
#include "Hajek.h"
#include "ExponentialCoolDown.h"
#include "SimulatedAnnealing.h"

typedef std::vector<int> Energy;

class CoolDownTester
{
public:

	CoolDownTester();
	CoolDownTester(int number_steps);
	void compareDifferentCoolDownSchedules();

private:
	std::vector<Energy> energies;
	std::vector<SimulatedAnnealing> sas;
	const char* dither_texture_path = "LDR_RGBA_0_64.png";
	int number_steps;
};

