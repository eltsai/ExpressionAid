#include "imagewidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include <GL/glu.h>
#include <QKeyEvent>
using std::cout;
using std::endl;
#define START 1

ImageWidget::ImageWidget(void)
{
    is_dealing_=false;
    is_img_on_=false;
    geter=new dmm_getface();
    capture=VideoCapture(0);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(cap_one_frame()));
    framecnt=0;
    capmode=0;
    rf=new refine();
    gl_show_ = new view3d();
    gl_show2_=new rview3d();
//    std::cout<<"here0"<<std::endl;
    QString mesh_path = "./data/s/s0.obj";
    if(!OpenMesh::IO::read_mesh(mesh,mesh_path.toStdString().c_str()))
        std::cout<<"Not load mesh"<<std::endl;
    for(TriMesh::VertexIter it=mesh.vertices_begin();it!=mesh.vertices_end();++it)
    {
        mesh.point(*it)[0]/=150000.0;
        mesh.point(*it)[1]/=150000.0;
        mesh.point(*it)[2]/=150000.0;
    }

    if(!OpenMesh::IO::read_mesh(gl_show2_->examiner_->mesh_show_,"./data/d/d0.obj"))
    {
        cout<<"no dog"<<endl;
    }
    for(TriMesh::VertexIter it=gl_show2_->examiner_->mesh_show_.vertices_begin();it!=gl_show2_->examiner_->mesh_show_.vertices_end();++it)
    {
        gl_show2_->examiner_->mesh_show_.point(*it)[0]/=3.0;
        gl_show2_->examiner_->mesh_show_.point(*it)[1]/=3.0;
        gl_show2_->examiner_->mesh_show_.point(*it)[2]/=3.0;
    }



    gl_show_->updateMesh(mesh);
    gl_show_->resetMesh(mesh,1);

    gl_show2_->updateMesh(gl_show2_->examiner_->mesh_show_);
    gl_show2_->resetMesh(gl_show2_->examiner_->mesh_show_,1);

    sparse_a.resize(4168,3);
    dense_a.resize(4168,3);
    keypoint_a_2d.resize(68,2);

    rt=new retargeting();

    facealignment.load_model(EAGLE_MODEL_DIR);
}


ImageWidget::~ImageWidget(void)
{
    
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
    QPainter painter;
    painter.begin(this);
    
    // Draw background
    painter.setBrush(Qt::lightGray);
    QRect back_rect(0, 0, width(), height());
    painter.drawRect(back_rect);
    


    // Draw image
    if(is_img_on_)
    {

        QImage image_show = QImage((unsigned char *)(geter->frame.data), geter->frame.cols, geter->frame.rows, geter->frame.step, QImage::Format_RGB888 );
        QRect rect = QRect(size().rwidth()/2-320, size().rheight()/2-240, image_show.width(), image_show.height());
        //QRect rect = QRect(20, 200, image_show.width(), image_show.height());
        painter.drawImage(rect,image_show);
    }
    
    /*if(is_dealing_==true&&geter->coarse_rec.render_.empty()!=true)
    {

        QImage image_show2 = QImage((unsigned char *)(geter->coarse_rec.render_.data), geter->coarse_rec.render_.cols, geter->coarse_rec.render_.rows, geter->coarse_rec.render_.step, QImage::Format_RGB888 );
        //QRect rect = QRect(size().rwidth()/2-320, size().rheight()/2-240, image_show.width(), image_show.height());
        QRect rect2 = QRect(0, 480, image_show2.width(), image_show2.height());
        painter.drawImage(rect2,image_show2);
    }*/
    
    
    painter.end();
    update();
}


void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    
}
void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    
}
void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    
}


void ImageWidget::to_turnimg()
{
    if(!is_img_on_)
    {
        is_img_on_=true;
        is_dealing_=false;
        timer->start(40);
    }
    else
    {
        is_img_on_=false;
        timer->stop();
        repaint();
    }
}

void ImageWidget::to_turndetect()
{
    if(!is_dealing_)
    {
        is_dealing_=true;
        geter->reinit();
    }else
    {
        is_dealing_=false;
        geter->reinit();
    }
}

void ImageWidget::to_catchmean()
{
    framecnt=0;
    capmode=1;
    is_dealing_=true;
}

void ImageWidget::to_catchtrain()
{
    framecnt=0;
    capmode=2;
    is_dealing_=true;
}

void ImageWidget::to_refine()
{
    rf->init();

}

void ImageWidget::to_retarget()
{

    framecnt=0;
    capmode=3;
    is_dealing_=true;
}

void ImageWidget::cap_one_frame()
{
    capture >> geter->frame;
    IplImage im;
    im=IplImage(geter->frame);
    cvFlip(&im, NULL,1);

    if(is_dealing_)
    {
        geter->getface();
        geter->getfacemesh();
    }
    if(capmode==1)
    {
        if(framecnt<30)
        {
            //the first 29 frames
            for(TriMesh::VertexIter it=geter->front_mesh.vertices_begin();it!=geter->front_mesh.vertices_end();++it)
            {
                geter->front_mesh.point(*it)[0]/=150000.0;
                geter->front_mesh.point(*it)[1]/=150000.0;
                geter->front_mesh.point(*it)[2]/=150000.0;
            }
            gl_show_->resetMesh(geter->front_mesh,true);
            gl_show_->updateGL();
            framecnt++;
        }else
        {
            geter->front_mesh.request_vertex_normals();
            OpenMesh::IO::Options opt;
            if (!opt.check(OpenMesh::IO::Options::VertexNormal))
            {
                geter->front_mesh.request_face_normals();

                geter->front_mesh.update_normals();

                geter->front_mesh.release_face_normals();
            }
            OpenMesh::IO::Options opt_write = OpenMesh::IO::Options::VertexNormal;
            IO::write_mesh(geter->front_mesh,"./data/g/g0.obj",opt_write);
            IO::write_mesh(geter->front_mesh,"./gmean.obj",opt_write);
            for(TriMesh::VertexIter it=geter->front_mesh.vertices_begin();it!=geter->front_mesh.vertices_end();++it)
            {
                geter->front_mesh.point(*it)[0]/=150000.0;
                geter->front_mesh.point(*it)[1]/=150000.0;
                geter->front_mesh.point(*it)[2]/=150000.0;
            }
            gl_show_->resetMesh(geter->front_mesh,1);
            gl_show_->updateGL();
            capmode=0;
            is_dealing_=false;
            cout<<"~~~start to refine"<<endl;
            to_refine();
            rt->init_rt(rf->solution);
        }
    }else if(capmode==2)
    {
    }
    else if(capmode==3)
    {
        geter->getfacemesh();
        cv::Rect face;
        eagle::Mat image = geter->frame;
        face.x=image.cols*0.2;
        face.y=image.rows*0.2;
        face.width=image.cols*0.6;
        face.height=image.rows*0.6;
        //detect keypoints using static_face_alignment library
        float center_x = face.x + face.width / 2;
        float center_y = face.y + face.height / 2;
        float ratio = 1.4;
        float width_height = (face.width + face.height) / 2 * ratio;
        cv::Rect new_rect(center_x - width_height / 2, center_y - width_height / 2, width_height, width_height);
        std::vector<cv::Rect> face_rects = { new_rect };
        eagle::StaticFaceAlignmentParams faparams;
        std::vector<eagle::StaticFaceAlignmentInfo> face_infos = facealignment.process(image, face_rects, faparams);
        int i=0;
        for (auto &face_info : face_infos){
            for (auto &pt : face_info.landmarks_){
                if(i<68){
                    keypoint_a_2d(i,0)=pt.x;
                    keypoint_a_2d(i,1)=pt.y;
                    cv::circle(geter->frame,pt, 1, cv::Scalar(0, 255, 0), 0.1);
                    i++;
                }

            }
        }




        int cnt=0;
        for (MyMesh::VertexIter v_it=geter->front_mesh.vertices_begin(); v_it!=geter->front_mesh.vertices_end(); ++v_it)
        {
            sparse_a(cnt,0)=geter->front_mesh.point(*v_it)[0];
            sparse_a(cnt,1)=geter->front_mesh.point(*v_it)[1];
            sparse_a(cnt,2)=geter->front_mesh.point(*v_it)[2];
            cnt++;
        }
        cout<<cnt<<endl;
        //rt->animate(sparse_a,dense_a,dog_a);

        //cout<<dog_a.rows()<<endl;
        //printf("!!!!!!!!!!!!!!\n");
        float x=geter->pose_para(0);
        float y=geter->pose_para(1);
        float z=geter->pose_para(2);
        float scale_=geter->pose_para(5);
        cnt=0;
        //
        MatrixXd rott(3,3);
        rott<<
               cos(y)*cos(z), sin(z)*cos(y), -sin(y),
                (sin(x)*sin(y)*cos(z) - sin(z)*cos(x)),(sin(x)*sin(y)*sin(z) + cos(x)*cos(z)),sin(x)*cos(y),
                (cos(x)*sin(y)*cos(z)+sin(x)*sin(z)),(cos(x)*sin(y)*sin(z)-sin(x)*cos(z)), cos(x)*cos(y);

        rt->animate(sparse_a,keypoint_a_2d,rott,geter->pose_para(3),geter->pose_para(4),scale_,dense_a,dog_a);

        //test 3d-point-2d-keypoint function
        int keypoint_list[24]={3330,618,3992,321,333,329,2103,2407,2042,2176,2175,1256,3223,957,3628,1562,1541,3047,3113,1598,1528,1553,962,3270};
        for(int i=0;i<24;i++){
            VectorXd tmp(3);
            tmp<<
                  sparse_a(keypoint_list[i],0),
                    sparse_a(keypoint_list[i],1),
                    sparse_a(keypoint_list[i],2);
            tmp=rott*tmp;

            tmp(0)=(scale_*tmp(0)+geter->pose_para(3));
            tmp(1)=(scale_*tmp(1)+geter->pose_para(4));
            tmp(2)=scale_*tmp(2);
            cv::Point pt(tmp(0),480-tmp(1));
            cv::circle(geter->frame,pt, 1, cv::Scalar(0, 0, 255), 0.1);
        }

        for (MyMesh::VertexIter v_it=geter->target_dense_mesh.vertices_begin(); v_it!=geter->target_dense_mesh.vertices_end(); ++v_it)
        {
            VectorXt tmp(3);
            tmp<<
                  dense_a(cnt,0),
                    dense_a(cnt,1),
                    dense_a(cnt,2);
            //            tmp=rott*tmp;
            //            geter->target_dense_mesh.point(*v_it)[0]=(scale_*tmp(0)+geter->pose_para(3));
            //            geter->target_dense_mesh.point(*v_it)[1]=(scale_*tmp(1)+geter->pose_para(4));
            //            geter->target_dense_mesh.point(*v_it)[2]=scale_*tmp(2);

            geter->target_dense_mesh.point(*v_it)[0]=dense_a(cnt,0)/150000.0;
            geter->target_dense_mesh.point(*v_it)[1]=dense_a(cnt,1)/150000.0;
            geter->target_dense_mesh.point(*v_it)[2]=dense_a(cnt,2)/150000.0;
            cnt++;
        }
        //cout<<"/"<<endl;
        cnt=0;
        for (MyMesh::VertexIter v_it=gl_show2_->examiner_->mesh_show_.vertices_begin(); v_it!=gl_show2_->examiner_->mesh_show_.vertices_end(); ++v_it)
        {
            VectorXt tmp(3);
            tmp<<
                  dog_a(cnt,0),
                    dog_a(cnt,1),
                    dog_a(cnt,2);
            //            tmp=rott*tmp;
            //            mesh2.point(*v_it)[0]=(scale_*tmp(0)+geter->pose_para(3));
            //            mesh2.point(*v_it)[1]=(scale_*tmp(1)+geter->pose_para(4));
            //            mesh2.point(*v_it)[2]=scale_*tmp(2);

            gl_show2_->examiner_->mesh_show_.point(*v_it)[0]=tmp(0)/3.0;
            gl_show2_->examiner_->mesh_show_.point(*v_it)[1]=tmp(1)/3.0;
            gl_show2_->examiner_->mesh_show_.point(*v_it)[2]=tmp(2)/3.0;
            cnt++;
        }
        //cout<<"???"<<endl;
        gl_show_->resetMesh(geter->target_dense_mesh,1);
        gl_show_->updateGL();
        gl_show2_->examiner_->mesh_show_.update_normals();
        gl_show2_->resetMesh(gl_show2_->examiner_->mesh_show_,1);
        gl_show2_->updateGL();
    }
    else
    {
        //        geter->getfacemesh();
        //        geter->getsparsemesh();
        //        gl_show_->resetMesh(geter->sparse_mesh,1);
        //        gl_show_->updateGL();
    }

    if(!geter->frame.empty())
    {
        cvtColor(geter->frame,geter->frame,CV_BGR2RGB );
    }

    repaint();
}





















