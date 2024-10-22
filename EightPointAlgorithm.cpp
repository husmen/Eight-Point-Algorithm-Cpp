#include "EightPointAlgorithm.hpp"

EightPointAlgorithm::EightPointAlgorithm(bool d)
{
        id = 1;
        debug = d;
}

int EightPointAlgorithm::getID()
{
        return id;
};

Mat EightPointAlgorithm::F_Matrix_Eight_Point(vector<Point2d> &points_left, vector<Point2d> &points_right, bool norm)
{
        
        ///////////////Step 1: Formulating a homogeneous linear equation: e*y_tilde=0
        ///////////////e is the essential matrix, y_tilde is the vector representation of y_prime*y_t
        ///////////////for 8 points: e_t*Y=0 where each column in Y represents one y_tilde
        
        //vector<Point3d> y, y_prime;
        Mat y, y_norm, y_prime, y_prime_norm, Y;
        y = Get_Homogen(points_left);
        y_prime = Get_Homogen(points_left);
        if(norm)
        {
                //Get normalization matrix
                y_norm = Get_NormMat2d(y);
                y_prime_norm = Get_NormMat2d(y_prime);
                // Normalisation
                y = y_norm * y;
                y_prime = y_prime_norm * y_prime;

                //y = transpose(y ./ repmat(y(3, :), [3, 1]));
                //y_prime = transpose(y_prime ./ repmat(y_prime(3, :), [3, 1]));
                Mat tmp;
                tmp = y / repeat(y.row(2),3,1);
                //y = tmp.t();
                y = tmp.clone();
                tmp = y_prime / repeat(y_prime.row(2),3,1);
                //y_prime = tmp.t();
                y_prime = tmp.clone();
        }

        Y = Get_Y(y, y_prime);
        if(debug)
                clog << "Y Matrix is: " << endl << Y.size() << Y.type() << endl << Y << endl;
        
        ///////////////Step 2: Solving the equation

        //perform SVD for Y
        Mat W, U, VT, sigmaMatrix; //singular vec, left singular vec, transposed matrix of right singular vectors
        SVD::compute(Y.clone(), W, U, VT, SVD::FULL_UV); //FULL_UV to inforce square matrix
        if(debug)
        {
                clog << "W is " << W.size() << " of type " << W.type() << endl;
                clog << "U is " << U.size() << " of type " << U.type() << endl;
                clog << "VT is " << VT.size() << " of type " << VT.type() << endl;
        }

        if(true)
        {
                //Building Sigma Matrix 
                sigmaMatrix = Mat::zeros(U.cols, VT.rows, CV_64F);
                for (std::size_t i = 0; i < W.rows; i++)
                        sigmaMatrix.at<double>(i, i) = W.at<double>(i, 0);

                //Checking A=U*sigmaMatrix*Vt 
                if(debug)
                        cout << "checking Y-U*Sigma*VT=0" << endl << sum(Y - U * sigmaMatrix * VT).val[0] << endl;
        }

        /////Building F Matrix From F vector 
        Mat F_vec = VT.col(VT.cols - 1);
        Mat F = Mat_<double>(3, 3);
        for (int i = 0; i < 9; i++)
                F.at<double>(i) = F_vec.at<double>(i);

        //perform SVD for F
        SVD::compute(F.clone(), W, U, VT, SVD::FULL_UV);
        if(debug)
        {
                clog << "W is " << W.size() << " of type " << W.type() << endl;
                clog << "U is " << U.size() << " of type " << U.type() << endl;
                clog << "VT is " << VT.size() << " of type " << VT.type() << endl;
        }

        //Setting The Smallest Eigen Value to Zero
        W.at<double>(W.rows - 1, 0) = 0;

        //Building Sigma Matrix
        sigmaMatrix = Mat::zeros(U.cols, VT.rows, CV_64F);
        for (size_t i = 0; i < W.rows; i++)
                sigmaMatrix.at<double>(i, i) = W.at<double>(i, 0);
        
        //Building New F matrix
        Mat NewF = U * sigmaMatrix * VT;
        if(norm)
                NewF = y_prime_norm.t() * NewF * y_norm;
        
        return NewF;
}

Mat EightPointAlgorithm::F_Matrix_Normalized_Eight_Point(vector<Point2d> &points_left, vector<Point2d> &points_right)
{
        return F_Matrix_Eight_Point(points_left, points_right, true);
}

void EightPointAlgorithm::Plot_Epipolar_lines(vector<Point2d>&points_left , vector<Point2d>&points_right , Mat&image_left, Mat&image_right)
{
        EightPointAlgorithm epa = EightPointAlgorithm();
        //Mat fMatrix = epa.F_Matrix_Eight_Point(points_left, points_right);
        Mat fMatrix = epa.F_Matrix_Normalized_Eight_Point(points_left, points_right);
        //Mat fMatrix = findFundamentalMat(points_left, points_right, FM_8POINT);
        
        vector<Vec3d> lines_left, lines_right;
        computeCorrespondEpilines(points_left, 1, fMatrix, lines_right);
        computeCorrespondEpilines(points_right, 2, fMatrix, lines_left);

        Mat image_epipolar_right, image_epipolar_left;
        image_epipolar_right = Get_DrawLines(points_left, image_right, lines_right, fMatrix);
        image_epipolar_left = Get_DrawLines(points_right, image_left, lines_left, fMatrix);

        cv::imwrite("right.jpg",image_epipolar_right);
        cv::imwrite("left.jpg",image_epipolar_left);
}


Mat EightPointAlgorithm::Get_Homogen(vector<Point2d> &points)
{
        Mat matrix = Mat_<double>(3, 8);
        for (int i = 0; i < points.size(); i++)
        {
                matrix.at<double>(0, i) = points.at(i).x;
                matrix.at<double>(1, i) = points.at(i).y;
                matrix.at<double>(2, i) = 1;
        }
        return matrix;
}

Mat EightPointAlgorithm::Get_Y(Mat &pts1, Mat &pts2)
{
        Mat matrix = Mat_<double>(9, 8);
        Mat y, y_prime, y_tilde;
        for (int i = 0; i < 8; i++)
        {
                pts1.col(i).copyTo(y);
                pts2.col(i).copyTo(y_prime);
                y_tilde = y_prime * y.t(); //for each points pair, y_tilde
                y_tilde = y_tilde.reshape(1, 9);
                y_tilde.copyTo(matrix.col(i));
        }
        //return matrix.t(); //results in full 9x9 VT and 8x8 U
        return matrix; //results in full 8x8 VT and 9x9 U
}

Mat EightPointAlgorithm::Get_NormMat2d(Mat&u)
{       
        // Inspired by https://github.com/DzReal/Normalized-Eight-Point-Algorithm
        Mat centroid, tmp, mean_dist;
        Mat nMatrix = Mat_<double>(3,3);
        
        //Get centroid
        reduce(u, centroid, 1, CV_REDUCE_AVG); //Average rows
        if(debug)
                clog << "centroid is: " << endl << centroid.size() << centroid.type() << endl << centroid << endl;        

        //Compute the distance to the centroid
        tmp = (u - repeat(centroid, 1, u.cols));
        pow(tmp,2,tmp);
        reduce(tmp, tmp, 0, CV_REDUCE_SUM); //Sum columns
        sqrt(tmp, tmp);
        reduce(tmp, mean_dist, 1, CV_REDUCE_AVG); //Average lines

        //Craft normalization matrix
        double data[9] = { std::sqrt(2) / mean_dist.at<double>(0,0), 0,  -std::sqrt(2) / mean_dist.at<double>(0,0) * centroid.at<double>(0,0),
                                0, std::sqrt(2) / mean_dist.at<double>(0,0), -std::sqrt(2) / mean_dist.at<double>(0,0) * centroid.at<double>(1,0),
                                0, 0, 1};

        for (int i = 0; i < 9; i++)
                nMatrix.at<double>(i) = data[i];
        if(debug)
                clog << "Normalization Matrix is: " << endl << nMatrix.size() << nMatrix.type() << endl << nMatrix << endl; 

        return nMatrix;
}

Mat EightPointAlgorithm::Get_DrawLines(vector<Point2d>& points, Mat& image, vector<cv::Vec3d>&lines, Mat&fMatrix)
{
        Mat camMatrix=Mat_<double>(3,1);
 
        for(std::size_t i=0;i<lines.size();i=i+1)
        {
                Vec3d l=lines.at(i);
                double a=l.val[0];
                double b=l.val[1];
                double c=l.val[2];
                
                camMatrix.at<double>(0,0)=points[i].x;
                camMatrix.at<double>(1,0)=points[i].y;
                camMatrix.at<double>(2,0)=1;
                Mat lineMatrix=fMatrix*camMatrix;
                
                if(debug)
                {
                        clog << "a,b,c Using OpenCV (ax+by+c=0): " << a <<", "<<b <<", "<<c << endl;
                        clog << "calculating a,b,c (ax+by+c=0): " << lineMatrix.at<double>(0,0) <<", "<< lineMatrix.at<double>(0,1) <<", "<< lineMatrix.at<double>(0,2) << endl;
                        clog<<"error: "<< a*points.at(i).x+ b*points.at(i).y +c<<endl;
                }

                if(false)
                {
                        a = lineMatrix.at<double>(0,0);
                        b = lineMatrix.at<double>(0,1);
                        c = lineMatrix.at<double>(0,2);
                }
        
                /////////////////////////////////drawing the line on the image/////////////////////////////////
                /*ax+by+c=0*/
                double x0,y0,x1,y1;
                x0=0;
                y0=(-c-a*x0)/b;
                x1=image.cols;
                y1=(-c-a*x1)/b;
        
                line(image, cvPoint(x0,y0), cvPoint(x1,y1), cvScalar(0,255,0), 1);
        }
        
        return image;
}