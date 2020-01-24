#pragma once

#include <QtWidgets/QWidget>
#include "ui_retargetinggui.h"

class RetargetingGUI : public QWidget
{
	Q_OBJECT

public:
	RetargetingGUI(QWidget *parent = Q_NULLPTR);

private:
	Ui::RetargetingGUIClass ui;
};
