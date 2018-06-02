#pragma once

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QSplitter>
#include <QListView>
#include <QTreeView>
#include <QTextEdit>
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

	//Splitter
	QSplitter	*splitter;
	QListView	*left;
	QListView	*right;


	//void Creat_Menu();
	void Create_ToolBar();
	void Create_Action();
	//void Create_Splitter();
	

	void Help();

private:
	Ui::ExpressionAidClass ui;
};
