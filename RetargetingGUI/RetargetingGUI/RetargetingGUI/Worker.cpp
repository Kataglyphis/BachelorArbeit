#include "stdafx.h"
#include "Worker.h"

Worker::Worker(QObject* q_object) {

	test = SimulatedAnnealingTest(q_object);

}

Worker::~Worker() {

}

void Worker::process() {

	this->test.testPermutation(filename);
	emit finished();

}

int Worker::give_overall_progress() {
	return (*this->progress_in_percent);
}
