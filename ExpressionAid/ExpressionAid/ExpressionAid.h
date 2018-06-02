#pragma once

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include "ui_ExpressionAid.h"

class ExpressionAid : public QMainWindow
{
	Q_OBJECT

public:
	ExpressionAid(QWidget *parent = Q_NULLPTR);
	QMenu		*pMenu;
	QToolBar	*pToolBar;
	//Actions
	QAction		*actionImport;
	QAction		*actionRealtime;
	QAction		*actionHelp;
	QAction		*actionRun;
	QAction		*actionStop;

	//void Creat_Menu();
	void Creat_ToolBar();
	void Creat_Action();


	void Help();

private:
	Ui::ExpressionAidClass ui;
};
