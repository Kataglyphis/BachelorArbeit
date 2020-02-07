#pragma once
#include "AnnealingSchedule.h"

#pragma once
#include "AnnealingSchedule.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <sstream>

/**
The initial temperature value is established by starting with quite a high temperature and gradually increasing
it until the ratio between the numbers of accepted and proposed transitions is practically one.
The iteration at temperature Tk should be long enough to permit the quasiequilibrium to be reached
*/

class Kirkpatrick :
	public AnnealingSchedule
{
public:
	Kirkpatrick();
	Kirkpatrick(double T_0, double mu, double quasieq);
	double getTemperature(int step);
	std::string getName();
	std::string getFunction();
	float getQuasiEq();
private:
	double T_0;
	double currentTemperature;
	float quasiequilibrium;
	/**
	values of a fluctuate between 0.8 and 0.99.!!
	*/
	double mu;
};

