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
	Lineary(double T_0, double mu);
	double getTemperature(int step);
	std::string getName();
	std::string getFunction();
private:
	double T_0;
	double mu;
};

