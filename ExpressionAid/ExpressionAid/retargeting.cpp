/*
 *Input:	p0,{p}---rbf-transformed user(human) blendshapes
 *        s0,{s}---normal people(3dmm-facewarehouse) blendshapes
 *        d0,{d}---pre-stored cartoon(dog) blendshapes
 *        a---current captured user face
 **********************************************************
 *Output:	{w}---retargeting weights
 *       w_series---instore series of constant frames' {w}
 *       a_retargeted---retargeted animation(based on {s})
 *       a_dog---retargeted animation(based on {d})
 **********************************************************
 *Brief Intro:
 *   Optimization Problem:  min{ ||A1*w-b1||^2 + ||A2*w-b2||^2 + ||A3*w-b3|| + ALPHA*||w||^1 }
 *   Solve the lasso problem to compute retargeting weights {w}.
 *   Note: since we have three terms of ||*||^2, we can combine them like ||A*w - b||^2,
 *         A=(A1,A2,A3)^T, b=(b1,b2,b3)^T
 *
 *
 *
 *---------------
 *PSEUDOCODE PART
 *---------------
 *
 *Step1-Initial
 *
 *1.Load-Mesh(p0,s0,d0,{p},{s},{d})   ->load_mesh(p,s,d,p0,s0,d0,F0,F1)
 *
 *2.Compute laplace matrix(cotangent weights) for each blendshape pi(based on libigl::cotmatrix), push back into L_p and L_p0
 *
 *3.Fill Matrix A (doesn't change for same user)   ->E_Fit(p,A), E_Prior(p,p0,F0,A)
 *
 *4.Initial w_series
 *
 *
 *Step2-Animate
 *
 *1.Input a
 *
 *2.Fill vector b (different for different a)  ->load_vector_a(a-p0,b)
 *
 *3.Solve the Lasso problem, return weights {w}   ->Solve_Lasso(w,A,b)
 *
 *4.Push {w} into w_series, compute the average {w} of latest 4 frames,return weights {w}
 *
 *5.Compute retargeted animation a_retargeted and a_dog based on {s} and {d} and {w}  ->Retarget(w,s,s0,a_retargeted,a_dog)
 *
 */

#include "retargeting.h"
#include <QCoreApplication>
#include <time.h>
#include <fstream>
#define K 46     //# of blendshape
#define M1 4168   //# of sparse keypoints
#define M2 4168  //# of all points (target blendshapes)
#define M3 24    //# of 2d detected keypoints
#define F 500     //# of animation
#define Gamma 1  //scalar E_Fit term
#define Mu 0.5     //scalar E_Prior term
#define Lamda 10  //scalar 2D keypoint term

int keypoint_list[M3]={3330,618,3992,321,333,329,2103,2407,2042,2176,2175,1256,3223,957,3628,1562,1541,3047,3113,1598,1528,1553,962,3270};

retargeting::retargeting()
{
    //load mesh
    load_mesh(p,s,d,p0,s0,d0,F0,F1);
}

retargeting::~retargeting()
{}


void retargeting::init_rt(MatrixXd &slo)
{
    clock_t start,end,suck;
    start=clock();
    load_g(p,p0,F0,slo);
    //compute laplace matrix(cotangent weights) based on libigl, for each blendshape
    for(int k=0;k<K;k++)
    {
        SparseMatrix<double> laplace;
        igl::cotmatrix(p.at(k)+p0,F0,laplace);
        L.push_back(laplace);
        MatrixXd laplace_p=0.5*laplace*p.at(k);
        L_p.push_back(laplace_p);
    }
    igl::cotmatrix(p0,F0,L0);
    L_p0=0.5*L0*p0;


    //Initial Matrix A
    A=MatrixXd::Zero(6*M1+2*M3,K);

    //Fill Matrix A (just E_Fit and E_Prior term)
    E_Fit(p,A);
    E_Prior(p,p0,F0,A);

    //Initial w
    w=VectorXd::Zero(K);
    w_series.push_back(w);
    w_series.push_back(w);
    w_series.push_back(w);

    end=clock();
    cout<<(double)(end-start)/CLOCKS_PER_SEC<<"ssss used to start retarget."<<endl;
}


void retargeting::animate(MatrixXd a, MatrixXd keypoint_a_2d,MatrixXd rott, float pose_para3, float pose_para4, float scale, MatrixXd &a_retargeted, MatrixXd &a_dog){
    clock_t start,finish;
    start=clock();
    b=VectorXd::Zero(6*M1+2*M3);
    w=VectorXd::Zero(K);

    //Fill vector b (just the first two terms)
    load_vector_a(a-p0,b);

    //Fill matrix A and vector b for 2d-keypoint part
    E_Keypoint(p,p0,A,b,keypoint_a_2d,rott,pose_para3,pose_para4,scale);

    //Solve optimization problem, return weights vector w
    Solve_Lasso(w,A,b);

    //Do retargeting part, return dog-like and human-like animation
    Retarget(w,s,s0,a_retargeted,a_dog);

    finish=clock();
    printf("Animate function time cost: %f s\n",(double)(finish-start)/CLOCKS_PER_SEC);

}

void retargeting::load_mesh(vector<MatrixXd> &p, vector<MatrixXd> &s, vector<MatrixXd> &d, MatrixXd &p0, MatrixXd &s0, MatrixXd &d0,MatrixXi &F0, MatrixXi &F1){
    MatrixXd V0;
    MatrixXd V1;
    char off_path[50];
    memset(off_path,0,sizeof(off_path));

    char s_path[30]="./data/s/s";
    char d_path[30]="./data/d/d";
    char d0_path[50]="./data/d/d0.obj";
    char num[5];
    //load p0 as neutral face in blendshapes
    sprintf(num,"%d",0);
//printf("Retarget->1 function time cost: %f s\n",(double)(finish-start)/CLOCKS_PER_SEC);
    memset(off_path,0,sizeof(off_path));
    strcat(off_path,s_path);
    strcat(off_path,num);
    strcat(off_path,".obj");
    igl::readOBJ(off_path,s0,F1);
    igl::readOBJ(d0_path,d0,F2);
    //load other blendshapes and store them in vector<MatrixXd> p,s as pi-p0,si-s0
    for(int k=1;k<=K;k++){
        memset(off_path,0,sizeof(off_path));
        memset(num,0,sizeof(num));
        sprintf(num,"%d",k);
        strcat(off_path,s_path);
        strcat(off_path,num);
        strcat(off_path,".obj");
        igl::readOBJ(off_path,V1,F1);
        s.push_back(V1-s0);

        memset(off_path,0,sizeof(off_path));
        memset(num,0,sizeof(num));
        sprintf(num,"%d",k);
        strcat(off_path,d_path);
        strcat(off_path,num);
        strcat(off_path,".obj");
        igl::readOBJ(off_path,V1,F1);
        d.push_back(V1-d0);
    }
}

void retargeting::load_g(vector<MatrixXd> &p, MatrixXd &p0, MatrixXi &F0,MatrixXd &slo){
    MatrixXd V0(4168,3);
    //load p0 as neutral face in blendshapes
    igl::readOBJ("./data/g/g0.obj",p0,F0);

//    cout<<slo.rows()<<" "<<slo.cols()<<endl;
    for(int k=1;k<=K;k++){
//        cout<<k<<endl;
        for(int i=0;i<4168;i++)
        {
            for(int c=0;c<3;c++)
            {
                V0(i,c)=slo(3*i+c,k-1);
            }
        }
        p.push_back(V0-p0);
    }
}

void retargeting::E_Fit(vector<MatrixXd> p, MatrixXd &A){
    clock_t start,finish;
    start=clock();
    for(int k=0;k<K;k++){
        for(int m=0;m<M1;m++){
            for(int xyz=0;xyz<3;xyz++){
                A(3*m+xyz,k)+=Gamma*p.at(k)(m,xyz);
            }
        }
    }

    finish=clock();
}

void retargeting::E_Prior(vector<MatrixXd> p, MatrixXd p0, MatrixXi F0, MatrixXd &A){
    clock_t start,finish;
    start=clock();


    for(int k=0;k<K;k++){
        for(int m=0;m<M1;m++){
            for(int xyz=0;xyz<3;xyz++){
                A(3*(m+M1)+xyz,k)+=Mu*L_p.at(k)(m,xyz);
            }
        }
    }

    finish=clock();

}

void retargeting::E_Keypoint(vector<MatrixXd> p, MatrixXd p0,MatrixXd &A,VectorXd &b,MatrixXd keypoint_a_2d,MatrixXd rott, float pose_para3, float pose_para4, float scale){
    for(int i=0;i<M3;i++){
        for(int k=0;k<K;k++){
            VectorXd tmp(3);
            tmp<<
                 p.at(k)(keypoint_list[i],0)+p0(keypoint_list[i],0),
                 p.at(k)(keypoint_list[i],1)+p0(keypoint_list[i],1),
                 p.at(k)(keypoint_list[i],2)+p0(keypoint_list[i],2);
                 //p.at(k)(keypoint_list[i],0),
                 //p.at(k)(keypoint_list[i],1),
                 //p.at(k)(keypoint_list[i],2);
            tmp=rott*tmp;
            A(6*M1+2*i,k)=Lamda*(scale*tmp(0)+pose_para3);
            A(6*M1+2*i+1,k)=Lamda*(480-(scale*tmp(1)+pose_para4));

        }
    }
    for(int i=0;i<M3;i++){
        VectorXd tmp(3);
        tmp<<
             p0(keypoint_list[i],0),
             p0(keypoint_list[i],1),
             p0(keypoint_list[i],2);
        tmp=rott*tmp;
        tmp(0)=scale*tmp(0)+pose_para3;
        tmp(1)=scale*tmp(1)+pose_para4;
        b(6*M1+2*i)=Lamda*(keypoint_a_2d(i+36,0));
        b(6*M1+2*i+1)=Lamda*(keypoint_a_2d(i+36,1));
        //cout<<"BBB "<<keypoint_a_2d(i+36,0)<<' '<<keypoint_a_2d(i+36,1)<<endl;
    }


}

void retargeting::load_vector_a(MatrixXd a, VectorXd &b){
    for(int m=0;m<M1;m++){
        for(int xyz=0;xyz<3;xyz++){
            b(3*m+xyz)+=Gamma*a(m,xyz);
            b(3*(m+M1)+xyz)+=Mu*L_p0(m,xyz);
        }
    }

}

void retargeting::Solve_Lasso(VectorXd &w, MatrixXd &A, VectorXd &b){
    /*
        * Fitting::Lasso --
        *     Solve the lasso problem:
        *     argmin(w)
        *        ||Aw-b||^2 + lambda*||w||_1
        *
        * Params:
        *            A [in]: input lhs matrix.
        *            b [in]: input rhs vector.
        *       lambda [in]: L1_norm weight.
        *           w [out]: blenshape coefficients.
        *     tolerance[in]: convergent tolerance.
        *
        * Returns:
        *     None
        *
        * Side Effects:
        *     None
        */
    clock_t start,finish;
    start=clock();
    double tolerance = 1.0e-5;
    double lambda=0.1;


    int p = A.cols();
    bool isFound = false;
    Eigen::VectorXd betaLast;
    Eigen::VectorXd ATb(A.cols(), 1);
    Eigen::MatrixXd ATA(A.cols(), A.cols());
    ATA.setZero();
    ATA.selfadjointView<Eigen::Upper>().rankUpdate(A.transpose(),1.0);
    ATA.triangularView<Eigen::StrictlyLower>() = ATA.transpose();
    ATb.noalias() = A.transpose()*b;

    // srand(time(0));
    int step=0;
    while (!isFound) {
        betaLast = w;
        for (int j = 0; j < p; j++) {
            int i = rand() % p;
            double deltai = ATb.coeff(i) - (ATA.col(i)).dot(w) + ATA(i, i)*w.coeff(i);
            if (deltai < -lambda) {
                w(i) = (deltai + lambda)/ATA(i, i);
            } else if (deltai > lambda) {
                w(i) = (deltai - lambda)/ATA(i, i);
            } else {
                w(i) = 0;
            }
            w(i) = (w.coeff(i) > 1.0)?1.0:w.coeff(i);
            w(i) = (w.coeff(i) < 0)?0:w.coeff(i);
        }

        if ((w - betaLast).cwiseAbs().maxCoeff() < tolerance) {
            isFound = true;
        }
        step++;
    }
    finish=clock();
    //printf("Solve_lasso function time cost: %f s\n",(double)(finish-start)/CLOCKS_PER_SEC);
}

void retargeting::Retarget(VectorXd &w, vector<MatrixXd> s, MatrixXd s0, MatrixXd &a_retargeted, MatrixXd &a_dog){
    clock_t start,finish;
    start=clock();
    double w_sum=0;
    a_retargeted=s0;
    a_dog=d0;
    finish=clock();

    for(int k=0;k<K;k++){
        w_sum+=w(k);
    }

    //reinforce some key blendshapes' w
    for(int i=0;i<K;i++){
        //Apply Sigmoid function
        if(w(i)>0.00001){
            if(i==0||i==1){
                w(i)=(w(i)-0.15)*32;
                w(i)=1.0/(1+exp(-w(i)));
            }
            else if(i>35&&i<44){
                w(i)=(w(i)-0.2)*32;
                w(i)=1.0/(1+exp(-w(i)));
            }
            else{
                w(i)=(w(i)-0.5)*16;
                w(i)=1.0/(1+exp(-w(i)));
            }
        }
    }

    for(int k=0;k<K;k++){
        a_retargeted+=w(k)*s.at(k);
        a_dog+=w(k)*d.at(k);
    }

    finish=clock();
    //printf("Retarget function time cost: %f s\n",(double)(finish-start)/CLOCKS_PER_SEC);
}
