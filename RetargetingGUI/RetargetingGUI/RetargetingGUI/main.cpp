#include "stdafx.h"
#include "retargetinggui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RetargetingGUI w;
	w.show();
	return a.exec();
}
