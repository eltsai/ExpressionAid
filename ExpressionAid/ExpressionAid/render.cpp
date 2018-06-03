#include "render.h"

namespace FaceModeling {

Render::Render()
{
    InitLighting("data/light.txt");
    LoadInd("data/sfs_index.txt");
}

Render::~Render()
{
    safefree(d_diffuse_light);
    safefree(d_light);
    safefree(d_ind_without_eye);
}

void Render::InitLighting(std::string light_path)
{
    std::ifstream fin(light_path);
    int sh_dim = 27;
    cudaMalloc(&d_diffuse_light, sizeof(T)*sh_dim);
    cudaMalloc(&d_light, sizeof(T)*sh_dim);
    T c_diffuse[27];
    for(int i=0; i<sh_dim; i++)
    {
        fin >> c_diffuse[i];
    }
    fin.close();
    cudaMemcpy(d_light, c_diffuse, sizeof(T)*sh_dim, cudaMemcpyHostToDevice);
}

void Render::LoadInd(std::string ind_path)
{
    int tri_num = 105840;
    std::ifstream fin(ind_path);
    int ind;
    Eigen::VectorXi inds(tri_num);
    inds.setZero();
    while(fin >> ind)
    {
        inds(ind) = 1;
        fin >> ind;
    }
    fin.close();
    cudaMalloc(&d_ind_without_eye, sizeof(int)*tri_num);
    cudaMemcpy(d_ind_without_eye, inds.data(), sizeof(int)*tri_num, cudaMemcpyHostToDevice);
}

}  // namespace FaceModeling
