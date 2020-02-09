#pragma once
#include <qobject.h>
#include "SimulatedAnnealingTest.h"

class Worker :
	public QObject
{
	Q_OBJECT

public:
	Worker(QObject* q_object);
	~Worker();

public slots:
	void process();
	int give_overall_progress();

signals:
	void finished();
	void error(QString err);

private:
	SimulatedAnnealingTest test;
	const char* filename = "pictures\BlueNoiseCode\FreeBlueNoiseTextures\Data\64_64\HDR_L_0.png";
	int* progress_in_percent;
};

