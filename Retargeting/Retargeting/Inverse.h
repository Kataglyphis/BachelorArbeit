#pragma once
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <sstream>

#include "AnnealingSchedule.h"

class Inverse :
	public AnnealingSchedule
{
public:

	Inverse();
	Inverse(double T_0, double alpha);

	double getTemperature(int step);
	std::string getName();
	std::string getFunction();
	float getQuasiEq();

private:

	double T_0;
	/**
		alpha > 0 !
	*/
	double alpha;

};

