#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QThread>
#include <QKeySequence>
#include "imagewidget.h"

QT_BEGIN_NAMESPACE
    class QAction;
    class QMenu;
    class ViewWidget;
    class QImage;
    class QPainter;
    class QRect;
    class ImageWidget;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *e);
    void paintEvent(QPaintEvent *paintevent);

private:
    void CreateActions();
    void CreateMenus();
    void CreateToolBars();
    void CreateStatusBar();
private:
    //***********************************
    QMenu		*menu_ctrl_;
    QToolBar	*toolbar_ctrl_;
    QAction		*action_img_switch_;
    QAction		*action_detect_switch_;
    QAction     *action_meanface_;
    QAction     *action_train_;
    QAction     *action_retargeting_;



    ImageWidget		*imagewidget_;

    QDockWidget *td_model_show_;
    QDockWidget *td_model_show2_;

    //QAction *dock_mesh_load_;

};

#endif // MAINWINDOW_H
