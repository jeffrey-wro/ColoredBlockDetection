/**
 * OpenCV SimpleBlobDetector Example
 *
 * Copyright 2015 by Satya Mallick <spmallick@gmail.com>
 *
 */

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{

    // Setup SimpleBlobDetector parameters.
    SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = 10;
    params.maxThreshold = 200;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 1500;

    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.1;

    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.01;

    // Storage for blobs
    vector<KeyPoint> keypoints;

    VideoCapture cap(1); // open the default camera
    if (!cap.isOpened()) // check if we succeeded
        return -1;

    Mat edges;
    for (;;)
    {

        Mat frame;
        cap >> frame;
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);

#if CV_MAJOR_VERSION < 3 // If you are using OpenCV 2

        // Set up detector with params
        SimpleBlobDetector detector(params);

        // Detect blobs
        detector.detect(im, keypoints);
#else

        // Set up detector with params
        Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

        // Detect blobs
        detector->detect(edges, keypoints);
#endif

        // Draw detected blobs as red circles.
        // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
        // the size of the circle corresponds to the size of blob

        Mat im_with_keypoints;
        drawKeypoints(edges, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

        // Show blobs
        imshow("keypoints", im_with_keypoints);

        if (waitKey(30) >= 0)
            break;
    }
}