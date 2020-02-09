#pragma once

#include <QtWidgets/QWidget>
#include "ui_retargetinggui.h"
#include "SimulatedAnnealingTest.h"
#include "Worker.h"

class RetargetingGUI : public QWidget
{
	Q_OBJECT
	QThread workerThread;
	QMutex progress_bar_mutex;

public:
	RetargetingGUI(QWidget *parent = Q_NULLPTR);
	~RetargetingGUI();

	int getValue() const { return progress_percent; }

signals:
	void startCalculatingRetargetTexture();
	void get_progress_percent(int);

private slots:

	void signal_process_bar();

private:

	Ui::RetargetingGUIClass* ui;

	int* progress;
	int progress_percent;
	//SimulatedAnnealingTest test;
	const char* filename = "pictures\BlueNoiseCode\FreeBlueNoiseTextures\Data\64_64\HDR_L_0.png";
};
