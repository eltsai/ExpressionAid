#include "ExpressionAid.h"

ExpressionAid::ExpressionAid(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	Create_Action();
	Create_ToolBar();
	//Create_Splitter();
	splitter = new QSplitter(this);
	left = new QListView(this);
	right = new QListView(this);
	//设置QTextEdit的值
	left->setBaseSize(400, 600);
	right->setBaseSize(400, 600);

	//设置到分离器
	splitter->addWidget(left);
	splitter->addWidget(right);
	//设置中心控件
	setCentralWidget(splitter);

}
void ExpressionAid::Create_Action()
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

void ExpressionAid::Create_ToolBar()
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

/*
void ExpressionAid::Create_Splitter()
{
	splitter = new QSplitter(this);
	listview = new QListView;
	treeview = new QTreeView;
	textedit = new QTextEdit;
	splitter->addWidget(listview);
	splitter->addWidget(treeview);
	splitter->addWidget(textedit);
}
*/


