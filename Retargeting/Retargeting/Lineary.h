#pragma once
#include "AnnealingSchedule.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <sstream>

class Lineary :
	public AnnealingSchedule
{
public:
	Lineary();
	Lineary(double T_0, int number_of_steps, double quasiequilibrium);
	Lineary(double T_0, int number_of_steps);
	double getTemperature(int step);
	std::string getName();
	std::string getFunction();
	float getQuasiEq();
private:
	double T_0;

	/**
	values of a fluctuate between 0.8 and 0.99.!!
	*/
	double mu;
	double quasiequilibrium;
	double current_temperature;
};

