#pragma once
#include "AnnealingSchedule.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <sstream>

class Inverse :
	public AnnealingSchedule
{
public:

	Inverse();
	Inverse(double T_0, double alpha);

	double getTemperature(int step);
	std::string getName();
	std::string getFunction();

private:

	double T_0;
	/**
		alpha > 0 !
	*/
	double alpha;

};

