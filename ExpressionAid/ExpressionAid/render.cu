#include "render.h"

namespace FaceModeling {

template<typename T>
__device__ void Normal_base_f(const T* normal,
                            T* normal_b)
{
    normal_b[0] = T(1);
    normal_b[1] = normal[1];
    normal_b[2] = normal[2];
    normal_b[3] = normal[0];
    normal_b[4] = normal[0] * normal[1];
    normal_b[5] = normal[1] * normal[2];
    normal_b[6] = T(3)*normal[2]*normal[2] - T(1);
    normal_b[7] = normal[0]*normal[2];
    normal_b[8] = normal[0]*normal[0] - normal[1]*normal[1];
}

template<typename T>
__device__ void Syn_color_f(const T* normal_base, const T* diffuse, const T* texture,
                          T* syn_c)
{
    for(int channel=0; channel<3; channel++)
    {
        T sum = T(0);
        for(int i=0; i<9 ;i++)
        {
            sum = sum+normal_base[i]*diffuse[9*channel+i];
        }
        syn_c[channel] = texture[channel]*sum;
    }
}

template <typename T>
__global__ void rendermesh(int nthread, const T* ori_img, const pixel_info* pixel_gpu, const int* inds, const T* texture,
                           const T* diffuse_sh, unsigned char* dimg, int width, int height)
{
    CUDA_KERNEL_LOOP(index, nthread)
    {
        int yy = index/width;
        int xx = index%width;
        for(int c=0; c<3; c++)
        {
            dimg[3*(width*yy+xx)+2-c] = ori_img[c*width*height+yy*width+xx];
        }

        int tri_ind = pixel_gpu[index].tri_index;
        if(tri_ind != -1)
        {
            if(inds[tri_ind] == 1)
            {
                T syn_color[3];
                int x = pixel_gpu[index].x;
                int y = pixel_gpu[index].y;
                T normal_base[9];
                Normal_base_f(pixel_gpu[index].normal, normal_base);
                Syn_color_f(normal_base, diffuse_sh, texture, syn_color);
                for(int c=0; c<3; c++)
                {
                    T rend_c = 0.0*syn_color[2-c] + 1.0*dimg[3*(width*(height-1-y)+x)+c];
                    if(rend_c < 0)
                    {
                        rend_c = 0;
                    }
                    if(rend_c > 255)
                    {
                        rend_c = 255;
                    }
                    dimg[3*(width*(height-1-y)+x)+c] = (unsigned char)rend_c;
                }
            }
        }
    }
}

template <typename T>
__global__ void rendermesh_uchar(int nthread, const unsigned char* ori_img, const pixel_info* pixel_gpu, const int* inds, const T* texture,
                           const T* diffuse_sh, unsigned char* dimg, int width, int height)
{
    CUDA_KERNEL_LOOP(index, nthread)
    {
        int yy = index/width;
        int xx = index%width;
        for(int c=0; c<3; c++)
        {
            dimg[3*(width*yy+xx)+2-c] = ori_img[3*(width*yy+xx)+2-c];
        }

        int tri_ind = pixel_gpu[index].tri_index;
        if(tri_ind != -1)
        {
            if(inds[tri_ind] == 1)
            {
                T syn_color[3];
                int x = pixel_gpu[index].x;
                int y = pixel_gpu[index].y;
                T normal_base[9];
                Normal_base_f(pixel_gpu[index].normal, normal_base);
                Syn_color_f(normal_base, diffuse_sh, texture, syn_color);
                for(int c=0; c<3; c++)
                {
                    T rend_c = 1.0*syn_color[2-c] + 0.0 *dimg[3*(width*(height-1-y)+x)+c];
                    if(rend_c < 0)
                    {
                       rend_c = 0;
                    }
                    if(rend_c > 255)
                    {
                        rend_c = 255;
                    }
                    dimg[3*(width*(height-1-y)+x)+c] = (unsigned char)rend_c;
                }
            }
        }
    }
}


template <typename T>
__global__ void render_3dmm(int nthread, const unsigned char* ori_img, const pixel_info* pixel_gpu,
                            const int* inds, const T* texture, const T* diffuse_sh,
                            unsigned char* dimg, int width, int height)
{
    CUDA_KERNEL_LOOP(index, nthread)
    {
        int yy = index/width;
        int xx = index%width;
        for(int c=0; c<3; c++)
        {
            dimg[3*(width*yy+xx)+2-c] = ori_img[3*(width*yy+xx)+2-c];
        }

        int tri_ind = pixel_gpu[index].tri_index;
        if(tri_ind != -1)
        {
            if(inds[tri_ind] == 1)
            {
                int vinds[3];
                vinds[0] = pixel_gpu[index].v1index;
                vinds[1] = pixel_gpu[index].v2index;
                vinds[2] = pixel_gpu[index].v3index;
                T syn_color[3];
                int x = pixel_gpu[index].x;
                int y = pixel_gpu[index].y;
                T normal_base[9], tex[3];
                for(int c=0; c<3; c++)
                {
                    tex[c] = 0;
                    for(int j=0; j<3; j++)
                    {
                        tex[c] += pixel_gpu[index].coord[j]*texture[3*vinds[j]+c];
                    }
                }
                Normal_base_f(pixel_gpu[index].normal, normal_base);
                Syn_color_f(normal_base, diffuse_sh, tex, syn_color);
                for(int c=0; c<3; c++)
                {
                    T rend_c = 1.0*syn_color[2-c] + 0.0 *dimg[3*(width*(height-1-y)+x)+c];
                    if(rend_c < 0)
                    {
                       rend_c = 0;
                    }
                    if(rend_c > 255)
                    {
                        rend_c = 255;
                    }
                    dimg[3*(width*(height-1-y)+x)+c] = (unsigned char)rend_c;
                }
            }
        }
    }
}


void Render::render_mesh(const T* ori_img, const pixel_info *pixels, unsigned char *d_img, int width, int height)
{
    int nthread = width*height;
    T* d_tex;
    cudaMalloc(&d_tex, sizeof(T)*3);
    float c_tex[3] = {125, 125, 125};
    cudaMemcpy(d_tex, c_tex, sizeof(T)*3, cudaMemcpyHostToDevice);
    rendermesh<<< FaceModeling_GET_BLOCKS(nthread), FaceModeling_CUDA_NUM_THREADS >>>
    (nthread, ori_img, pixels, d_ind_without_eye, d_tex, d_light, d_img, width, height);
    safefree(d_tex);
}

void Render::render_mesh(const unsigned char* ori_img, const pixel_info *pixels, unsigned char *d_img, int width, int height)
{
    int nthread = width*height;
    T* d_tex;
    cudaMalloc(&d_tex, sizeof(T)*3);
    float c_tex[3] = {125, 125, 125};
    cudaMemcpy(d_tex, c_tex, sizeof(T)*3, cudaMemcpyHostToDevice);
    rendermesh_uchar<<< FaceModeling_GET_BLOCKS(nthread), FaceModeling_CUDA_NUM_THREADS >>>
    (nthread, ori_img, pixels, d_ind_without_eye, d_tex, d_light, d_img, width, height);
    safefree(d_tex);
}

void Render::render(const unsigned char *ori_img, const pixel_info *pixels, const T* paras, const T *texture,
                    unsigned char *d_img, int width, int height)
{
    int nthread = width*height;
    cudaMemcpy(d_diffuse_light, paras+285, sizeof(T)*27, cudaMemcpyDeviceToDevice);
    render_3dmm<<< FaceModeling_GET_BLOCKS(nthread), FaceModeling_CUDA_NUM_THREADS >>>
    (nthread, ori_img, pixels, d_ind_without_eye, texture, d_diffuse_light, d_img, width, height);
}

}  // namespace FaceModeling

