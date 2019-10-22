#ifndef EIGHTPOINTALGO_H
#define EIGHTPOINTALGO_H
#include <vector>
#include <math.h>
#include <opencv2/opencv.hpp>
//#include <Eigen/Dense>

using namespace std;
using namespace cv;
//using namespace Eigen;
class EightPointAlgorithm
{
    public:
        EightPointAlgorithm();
        int getID();

        Mat F_Matrix_Eight_Point(vector<Point2d>&, vector<Point2d>&, bool norm = false);
        Mat F_Matrix_Normalized_Eight_Point(vector<Point2d>&, vector<Point2d>&);

    private:
        int id;

        Mat Get_Homogen(vector<Point2d>&);
        Mat Get_Y(Mat&,Mat&);
        Mat Get_NormMat2d(Mat&);
};
#endif