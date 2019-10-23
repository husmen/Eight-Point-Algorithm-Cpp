#include <iostream>
#include "EightPointAlgorithm.hpp"

using namespace std;
int main()
{
  vector<Point2d> points_left, points_right;
  double points_left_tmp[16] = {10, 20, 30, 50, 60, 80, 90, 120, 130, 170, 180, 230, 240, 300, 310, 380};
  double points_right_tmp[16] = {20, 30, 40, 60, 70, 90, 100, 130, 140, 180, 190, 240, 250, 310, 320, 390};
  for(int i = 0; i < 8; i++)
  {
    Point2d p1(points_left_tmp[2 * i], points_left_tmp[2 * i + 1]);
    Point2d p2(points_right_tmp[2 * i], points_right_tmp[2 * i + 1]);
    points_left.push_back(p1); 
    points_right.push_back(p2); 
  }
  
  cout << points_left << endl;
  cout << points_right << endl;

  EightPointAlgorithm epa = EightPointAlgorithm();
  Mat fMatrix = epa.F_Matrix_Eight_Point(points_left, points_right);
  Mat fMatrixNorm = epa.F_Matrix_Normalized_Eight_Point(points_left, points_right);

  cout << "\nFundamental Matrix is:" << endl;
  cout << fMatrix.size() << fMatrix.type() << endl << fMatrix << endl;

  cout << "\nNormalized Fundamental Matrix is:" << endl;
  cout << fMatrixNorm.size() << fMatrixNorm.type() << endl << fMatrixNorm << endl;

  //cout << "\nFundamental Matrix by OpenCV is:" << endl;
  //Mat fundamentalMatrix = findFundamentalMat(points_left, points_right, FM_8POINT);
  //cout << fundamentalMatrix.size() << fundamentalMatrix.type() << endl << fundamentalMatrix << endl;

  Mat image_left(200, 400, CV_8UC3, Scalar(0,0, 100));
  Mat image_right(200, 400, CV_8UC3, Scalar(0,100,0));
  epa.Plot_Epipolar_lines(points_left, points_right, image_left, image_right);
  
}