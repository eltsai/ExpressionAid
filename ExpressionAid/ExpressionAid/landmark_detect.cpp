#include "landmark_detect.h"


namespace FaceModeling {


landmark_detector::landmark_detector()
{
    dlib::deserialize("data/shape_predictor_68_face_landmarks.dat") >> sp;
    detector = dlib::get_frontal_face_detector();
}

bool landmark_detector::Detect(cv::Mat &cvimg, T *lands)
{
    int height = cvimg.rows;
    dlib::array2d<dlib::rgb_pixel> img;
    dlib::assign_image(img, dlib::cv_image<dlib::bgr_pixel>(cvimg));
    std::vector<dlib::rectangle> dets = detector(img);
    if(dets.empty())
    {
        return false;
    }
    int size = 0;

    int max_id = 0;
    for(int i=0; i<dets.size(); i++)
    {
        dlib::rectangle det = dets[i];
        int cur_size = det.height();
        if(cur_size > size)
        {
            size = cur_size;
            max_id = i;
        }
    }
    dlib::full_object_detection shape = sp(img, dets[max_id]);
    for(int i=0; i<68; i++)
    {
        lands[2*i] = shape.part(i).x();
        lands[2*i+1] = height - 1 - shape.part(i).y();
    }
    return true;
}


}  // namespace FaceModeling


