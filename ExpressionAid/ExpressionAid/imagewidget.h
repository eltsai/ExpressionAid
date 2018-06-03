#pragma once
#include <QWidget>
#include <opencv2/opencv.hpp>
#include <vector>
#include <qpoint.h>
#include <qpainter.h>
#include <eigen3/Eigen/Dense>
#include <QThread>
#include <QGLWidget>
#include "dmm_getface.h"
#include "refine.h"
#include "retargeting.h"
#include "view3d.h"
#include "rview3d.h"
#include "data_simulation.h"
#include <iostream>
#include "core/core.hpp"
#include "face_detect/face_detect.hpp"
#include "static_face_alignment.hpp"
using namespace Eigen;
using namespace cv;
using namespace cv::dnn;
using namespace eagle_test;



#define EAGLE_MODEL_DIR "./model"
/*#define AVATATPNUM 11510
#define USERPNUM 53215
#define LANDMARKPNUM 105*/

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

class ImageWidget :
        public QGLWidget
{
    Q_OBJECT
public:
    ImageWidget(void);
    ~ImageWidget(void);

protected:
    void paintEvent(QPaintEvent *paintevent);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


public slots:
    void to_turnimg();
    void to_turndetect();
    void cap_one_frame();
    void to_catchmean();
    void to_catchtrain();
    void to_refine();
    void to_retarget();


public:
    Mat	image_mat_;
    bool is_img_on_;
    bool is_dealing_;
    VideoCapture capture;
    QTimer *timer;
    dmm_getface *geter;
    int capmode;
    int framecnt;
    refine *rf;
    retargeting *rt;
    view3d *gl_show_;
    rview3d *gl_show2_;
    TriMesh mesh;
    TriMesh mesh2;
    MatrixXd sparse_a;
    MatrixXd dense_a;
    MatrixXd dog_a;
    //DataSimulation ds;
    eagle::StaticFaceAlignment facealignment;
    MatrixXd keypoint_a_2d;






};

