#pragma once
#include "AnnealingSchedule.h"
#include <stdlib.h>
#include <cmath>
class Hajek :
	public AnnealingSchedule
{
public:
	Hajek();
	double getTemperature(int step);
	std::string getName();
private:
	double T_0;
};

