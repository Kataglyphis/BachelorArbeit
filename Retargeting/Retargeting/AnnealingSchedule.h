#pragma once
#include <string>
class AnnealingSchedule
{
	public: 
		virtual double getTemperature(int step)=0;
		virtual std::string getName()=0;
		virtual std::string getFunction() = 0;
		virtual float getQuasiEq() = 0;
};

