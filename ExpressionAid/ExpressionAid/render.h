#ifndef RENDER_H
#define RENDER_H

#include "device_config.h"
#include "typedef.h"
#include <fstream>


namespace FaceModeling {

class Render
{
public:
    Render();
    ~Render();
    void render_mesh(const T *ori_img, const pixel_info* pixels, unsigned char* d_img, int width, int height);
    void render_mesh(const unsigned char *ori_img, const pixel_info* pixels,  unsigned char* d_img,
                     int width, int height);
    void render(const unsigned char *ori_img, const pixel_info* pixels, const T* paras,
                const T* texture, unsigned char* d_img, int width, int height);
    void InitLighting(std::string light_path);
    void LoadInd(std::string ind_path);

private:
    T* d_diffuse_light;
    T* d_light;
    int* d_ind_without_eye;
};

}  // namespace FaceModeling

#endif // RENDER_H
