#include "stdafx.h"
#include "retargetinggui.h"

RetargetingGUI::RetargetingGUI(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::RetargetingGUIClass)
{
	//first lets setup created ui properly!
	ui->setupUi(this);
	
	//connect(ui->calcTexture, SIGNAL(clicked()), this, SLOT(signal_process_bar()));
	//connect(this, SIGNAL(get_progress_percent(int)), ui->progressBar, SLOT(setValue(int)));

	//setup the thread instance for computing in background
	//QThread* thread = new QThread;
	Worker* worker = new Worker(this);
	worker->moveToThread(&workerThread);
	//connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
	connect(&workerThread, SIGNAL(started()), worker, SLOT(process()));
	connect(worker, SIGNAL(finished()), &workerThread, SLOT(quit()));
	connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
	connect(&workerThread, SIGNAL(finished()), &workerThread, SLOT(deleteLater()));
	workerThread.start();

}

RetargetingGUI::~RetargetingGUI() {
	
	workerThread.quit();
	workerThread.wait();

}

void RetargetingGUI::signal_process_bar() {

	this->progress_percent = 20;
	emit get_progress_percent(progress_percent);

}