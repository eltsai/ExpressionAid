#ifndef LANDMARK_DETECT_H
#define LANDMARK_DETECT_H

#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>
#include <dlib/opencv.h>
#include <typedef.h>


namespace FaceModeling {

class landmark_detector
{
public:
    landmark_detector();
    bool Detect(cv::Mat &cvimg, T* lands);

private:
    dlib::frontal_face_detector detector;
    dlib::shape_predictor sp;
};


}  // namespace FaceModeling


#endif // LANDMARK_DETECT_H
