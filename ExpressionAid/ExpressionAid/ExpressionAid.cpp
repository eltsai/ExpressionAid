#include "ExpressionAid.h"

ExpressionAid::ExpressionAid(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	Creat_Action();
	Creat_ToolBar();
	//Creat_Menu();

}
void ExpressionAid::Creat_Action()
{

	actionImport = new QAction(QIcon("Resources/import.png"), tr("&Import"), this);
	actionImport->setStatusTip(tr("click to import"));

	actionRealtime = new QAction(QIcon("Resources/realtime.png"), tr("&Real Time"), this);
	actionRealtime->setStatusTip(tr("click to analyze realtime..."));

	actionImport = new QAction(QIcon("Resources/import.png"), tr("&Import"), this);
	actionImport->setStatusTip(tr("click to import"));

	actionRun = new QAction(QIcon("Resources/run.png"), tr("&Run"), this);
	actionRun->setStatusTip(tr("click to run"));

	actionStop = new QAction(QIcon("Resources/stop.png"), tr("&Stop"), this);
	actionStop->setStatusTip(tr("click to stop"));

	actionHelp = new QAction(QIcon("Resources/help.png"), tr("&Help"), this);
	connect(actionHelp, &QAction::triggered, this, &ExpressionAid::Help);
}

void ExpressionAid::Creat_ToolBar()
{
	pToolBar = addToolBar(tr("&Main"));
	pToolBar->addAction(actionImport);
	pToolBar->addAction(actionRealtime);
	pToolBar->addAction(actionRun);
	pToolBar->addAction(actionStop);
	pToolBar->addAction(actionHelp);

}

/*
void ExpressionAid::Creat_Menu()
{
	pMenu = menuBar()->addMenu(tr("&Start"));


	pMenu->addAction(actionImport);
	pMenu->addAction(actionRealtime);
	pMenu->addAction(actionRun);
	pMenu->addAction(actionStop);
	pMenu->addAction(actionHelp);

}
*/

void ExpressionAid::Help()
{
	QMessageBox::about(this, tr("Help"), tr("Expression Aid"));
}


