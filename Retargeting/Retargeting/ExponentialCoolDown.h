#pragma once
#include "AnnealingSchedule.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <sstream>

class ExponentialCoolDown : public AnnealingSchedule
{
public:

	ExponentialCoolDown();
	ExponentialCoolDown(double T_0, double alpha);
	double getTemperature(int step);
	std::string getName();
	std::string getFunction();
	float getQuasiEq();

private:
	double T_0;
	double alpha;
};

