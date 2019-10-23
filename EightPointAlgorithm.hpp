#ifndef EIGHTPOINTALGO_H
#define EIGHTPOINTALGO_H
#include <vector>
#include <string>
#include <math.h>
#include <opencv2/opencv.hpp>
//#include <Eigen/Dense>

using namespace std;
using namespace cv;
//using namespace Eigen;

class EightPointAlgorithm
{
    public:
        EightPointAlgorithm(bool d = false);
        int getID();

        Mat F_Matrix_Eight_Point(vector<Point2d>&, vector<Point2d>&, bool norm = false);
        Mat F_Matrix_Normalized_Eight_Point(vector<Point2d>&, vector<Point2d>&);
        void Plot_Epipolar_lines(vector<Point2d>&, vector<Point2d>&, Mat&, Mat&);
    private:
        int id;
        bool debug;

        Mat Get_Homogen(vector<Point2d>&);
        Mat Get_Y(Mat&,Mat&);
        Mat Get_NormMat2d(Mat&);
        Mat Get_DrawLines(vector<Point2d>&,Mat&,vector<cv::Vec3d>&, Mat&);
};
#endif