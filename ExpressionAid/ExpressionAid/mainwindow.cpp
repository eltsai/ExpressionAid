#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowIcon(QIcon(QStringLiteral("./ddemodel/black.jpg")));
    setGeometry(100, 100, 1400, 1100);
    imagewidget_ = new ImageWidget();

    setCentralWidget(imagewidget_);
    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();

    td_model_show_ = new QDockWidget(tr("&3D_model"),this);
    td_model_show_ -> setFeatures(QDockWidget::DockWidgetFloatable);
    td_model_show_ -> setFixedSize(480,480);
    td_model_show_ -> setAllowedAreas(Qt::RightDockWidgetArea);
    td_model_show_ -> setWidget(imagewidget_->gl_show_);
    addDockWidget(Qt::RightDockWidgetArea, td_model_show_);

    td_model_show2_ = new QDockWidget(tr("&avatar_model"),this);
    td_model_show2_ -> setFeatures(QDockWidget::DockWidgetFloatable);
    td_model_show2_ -> setFixedSize(480,480);
    td_model_show2_ -> setAllowedAreas(Qt::RightDockWidgetArea);
    td_model_show2_ -> setWidget(imagewidget_->gl_show2_);
    addDockWidget(Qt::RightDockWidgetArea, td_model_show2_);
}

MainWindow::~MainWindow()
{

}


void MainWindow::closeEvent(QCloseEvent *e)
{

}

void MainWindow::paintEvent(QPaintEvent* paintevent)
{

}

void MainWindow::CreateActions()
{
    action_img_switch_= new QAction(tr("&ImgSwitch"), this);
    action_img_switch_->setShortcut(QKeySequence(tr("ctrl+ ")));
    action_img_switch_->setStatusTip(tr("To turn on or off the img"));
    connect(action_img_switch_, SIGNAL(triggered()), imagewidget_, SLOT(to_turnimg()));

    action_detect_switch_ = new QAction(tr("&DetectSwitch"), this);
    action_detect_switch_->setShortcut(QKeySequence(tr("s")));
    action_detect_switch_->setStatusTip(tr("To start decting your face"));
    connect(action_detect_switch_, SIGNAL(triggered()), imagewidget_, SLOT(to_turndetect()));

    action_meanface_ = new QAction(tr("&Meanface"), this);
    action_meanface_->setShortcut(QKeySequence(tr("m")));
    action_meanface_->setStatusTip(tr("To record your mean face"));
    connect(action_meanface_, SIGNAL(triggered()), imagewidget_, SLOT(to_catchmean()));

    action_train_ = new QAction(tr("&Trainframes"), this);
    action_train_->setShortcut(QKeySequence(tr("t")));
    action_train_->setStatusTip(tr("To record your training sequence"));
    connect(action_train_, SIGNAL(triggered()), imagewidget_, SLOT(to_catchtrain()));

    //action_retargeting
    action_retargeting_ = new QAction(tr("&Retarget"), this);
    action_retargeting_->setShortcut(QKeySequence(tr("r")));
    action_retargeting_->setStatusTip(tr("To retargrt"));
    connect(action_retargeting_, SIGNAL(triggered()), imagewidget_, SLOT(to_retarget()));

}

void MainWindow::CreateMenus()
{
    menu_ctrl_= menuBar()->addMenu(tr("&Control"));
    menu_ctrl_->setStatusTip(tr("Control menu"));
    menu_ctrl_->addAction(action_img_switch_);
    menu_ctrl_->addAction(action_detect_switch_);
    menu_ctrl_->addAction(action_meanface_);
    menu_ctrl_->addAction(action_train_);
    menu_ctrl_->addAction(action_retargeting_);


}

void MainWindow::CreateToolBars()
{
    toolbar_ctrl_ = addToolBar(tr("Control"));
    toolbar_ctrl_->addAction(action_img_switch_);
    toolbar_ctrl_->addAction(action_detect_switch_);
    toolbar_ctrl_->addAction(action_meanface_);
    toolbar_ctrl_->addAction(action_train_);
    toolbar_ctrl_->addAction(action_retargeting_);
}

void MainWindow::CreateStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
