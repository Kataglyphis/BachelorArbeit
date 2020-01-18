#pragma once
#include <string>
class AnnealingSchedule
{
	public: 
		virtual double getTemperature(int step);
		virtual std::string getName();
};

