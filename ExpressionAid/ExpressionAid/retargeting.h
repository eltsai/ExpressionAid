#ifndef RETARGETING_H
#define RETARGETING_H
#include <stdio.h>
#include <math.h>
#include <vector>
#include <time.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <igl/readOFF.h>
#include <igl/writeOFF.h>
#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <igl/cotmatrix.h>
#include <fstream>
#include "mesh.h"


using namespace Eigen;
using namespace std;




class retargeting
{
public:
    retargeting();
    ~retargeting();

public:
    MatrixXd a;
    MatrixXd a_retargeted;
    MatrixXd a_dog;
    vector <Eigen::MatrixXd> p;   //personlized blendshapes
    vector <Eigen::MatrixXd> s;   //target blendshapes (non-sparse)
    vector <Eigen::MatrixXd> d;   //dog blendshapes
    MatrixXd p0;                  //neutral personal expression
    MatrixXd s0;                  //neutral avatar   expression
    MatrixXd d0;                  //neutral dog expression
    MatrixXi F0;                  //edge relationship of sparse meshes
    MatrixXi F1;                  //edge relationship of non-sparse meshes
    MatrixXi F2;                  //edge re  of dog
    //MatrixXd A=MatrixXd::Zero(6*M1,K);
    //VectorXd b=VectorXd::Zero(6*M1);
    //VectorXd w=VectorXd::Zero(K);
    MatrixXd A;
    VectorXd b;
    VectorXd w;
    vector<VectorXd> w_series;
    vector<SparseMatrix<double>> L;
    vector<MatrixXd> L_p;
    SparseMatrix<double> L0;
    MatrixXd L_p0;
    ofstream ouf;

public:
    void init_rt(MatrixXd &slo);
    void animate(MatrixXd a,MatrixXd keypoint_a_2d,MatrixXd rott, float pose_para0, float pose_para1, float scale, MatrixXd &a_retargeted, MatrixXd &a_dog);


private:
    void load_mesh(vector<MatrixXd> &p, vector<MatrixXd> &s, vector<MatrixXd> &d, MatrixXd &p0, MatrixXd &s0, MatrixXd &d0, Eigen::MatrixXi &F0, Eigen::MatrixXi &F1);
    void load_g(vector<MatrixXd> &p,  MatrixXd &p0,  Eigen::MatrixXi &F0,MatrixXd &slo);
    void E_Fit(vector<MatrixXd> p, MatrixXd &A);
    void E_Prior(vector<MatrixXd> p, MatrixXd p0, MatrixXi F0, MatrixXd &A);
    void E_Keypoint(vector<MatrixXd> p, MatrixXd p0,MatrixXd &A,VectorXd &b,MatrixXd keypoint_a_2d,MatrixXd rott, float pose_para3, float pose_para4, float scale);
    void load_vector_a(MatrixXd a, VectorXd &b);
    void Solve_Lasso(VectorXd &w, MatrixXd &A, VectorXd &b);
    void Retarget(VectorXd &w, vector<MatrixXd> s, MatrixXd s0, MatrixXd &a_retargeted,MatrixXd &a_dog);

};

#endif // RETARGETING_H
