#include <opencv2/opencv.hpp>

struct ColorThresholdData
{
    int h;
    int s;
    int v;
    int h_high;
    int s_high;
    int v_high;
};

enum ColorThresholdType
{
    H,
    S,
    V,
    H_HIGH,
    S_HIGH,
    V_HIGH
};

using namespace cv;
using namespace std;

namespace
{
// windows and trackbars name
const std::string windowName = "Hough Circle AOI";
const std::string cannyThresholdTrackbarName = "Canny threshold";
const std::string accumulatorThresholdTrackbarName = "Accumulator Threshold";
const std::string usage = "Usage : tutorial_HoughCircle_Demo <path_to_input_image>\n";

// initial and max values of the parameters of interests.
const int cannyThresholdInitialValue = 100;
const int accumulatorThresholdInitialValue = 50;
const int maxAccumulatorThreshold = 200;
const int maxCannyThreshold = 255;

void HoughDetection(const Mat &src_gray, const Mat &src_display, int cannyThreshold, int accumulatorThreshold)
{
    // will hold the results of the detection
    std::vector<Vec3f> circles;
    // runs the actual detection
    try {
        HoughCircles(src_gray, circles, HOUGH_GRADIENT, 1, abs(src_gray.rows / 8), cannyThreshold, accumulatorThreshold, 0, 0);
    } catch (Exception e){
        return;
    }
    

    // clone the colour, input image for displaying purposes
    Mat display = src_display.clone();
    for (size_t i = 0; i < circles.size(); i++)
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        circle(display, center, 3, Scalar(0, 255, 0), -1, 8, 0);
        // circle outline
        circle(display, center, radius, Scalar(0, 0, 255), 3, 8, 0);
    }

    // shows the results
    imshow(windowName, display);
}
} // namespace

void hsvCallBack(int value, void *colorThreshold);

struct ColorThresholdData colorData = {
    20,
    37,
    100,
    45,
    255,
    255};
int main()
{
    //declare and initialize both parameters that are subjects to change
    int cannyThreshold = cannyThresholdInitialValue;
    int accumulatorThreshold = accumulatorThresholdInitialValue;

    namedWindow(windowName, WINDOW_NORMAL);
    createTrackbar(cannyThresholdTrackbarName, windowName, &cannyThreshold, maxCannyThreshold);
    createTrackbar(accumulatorThresholdTrackbarName, windowName, &accumulatorThreshold, maxAccumulatorThreshold);

    namedWindow("frame", 1);
    namedWindow("AOI", WINDOW_NORMAL);
    //namedWindow("mask", 1);
    namedWindow("result", WINDOW_AUTOSIZE);
    //namedWindow("thresh", 1);

    createTrackbar("LOWER H", "result", &colorData.h, 255, hsvCallBack, new ColorThresholdType(H));
    createTrackbar("LOWER S", "result", &colorData.s, 255, hsvCallBack, new ColorThresholdType(S));
    createTrackbar("LOWER V", "result", &colorData.v, 255, hsvCallBack, new ColorThresholdType(V));

    createTrackbar("UPPER H", "result", &colorData.h_high, 255, hsvCallBack, new ColorThresholdType(H_HIGH));
    createTrackbar("UPPER S", "result", &colorData.s_high, 255, hsvCallBack, new ColorThresholdType(S_HIGH));
    createTrackbar("UPPER V", "result", &colorData.v_high, 255, hsvCallBack, new ColorThresholdType(V_HIGH));

    VideoCapture cap(1); // open the default camera
    if (!cap.isOpened()) // check if we succeeded
        return -1;

    Mat frame;
    for (;;)
    {
        Mat frame;
        Mat hsv;
        Mat mask;
        Mat result;
        Mat thresh;

        int largest_area = 0;
        int largest_contour_index = 0;
        Rect bounding_rect;

        cap >> frame; // get a new frame from camera
        //cvtColor(frame, edges, COLOR_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);

        cvtColor(frame, hsv, COLOR_BGR2HSV);

        inRange(hsv, Scalar(colorData.h, colorData.s, colorData.v), Scalar(colorData.h_high, colorData.s_high, colorData.v_high), mask);

        bitwise_and(frame, frame, result, mask = mask);
        //findContours(result, );

        vector<vector<Point>> contours; // Vector for storing contour
        vector<Vec4i> hierarchy;

        threshold(mask, thresh, 40, 255, 0);

        findContours(thresh, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE); // Find the contours in the image

        for (size_t i = 0; i < contours.size(); i++) // iterate through each contour.
        {
            double a = contourArea(contours[i], false); //  Find the area of contour
            if (a > largest_area)
            {
                largest_area = a;
                largest_contour_index = i;                 //Store the index of largest contour
                bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
            }
        }

        Scalar color(255, 255, 255);
        //drawContours(frame, contours, largest_contour_index, color, FILLED, 8, hierarchy); // Draw the largest contour using previously stored index.
        rectangle(frame, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);

        Mat myAOI, src_gray;

        myAOI = Mat(frame, bounding_rect);

        if (myAOI.data != NULL)
        {
            // Convert it to gray
            cvtColor(myAOI, src_gray, COLOR_BGR2GRAY);

            // Reduce the noise so we avoid false circle detection
            GaussianBlur(src_gray, src_gray, Size(3, 3), 2, 2);

            imshow("AOI", src_gray);

            // those parameters cannot be =0
            // so we must check here
            cannyThreshold = std::max(cannyThreshold, 1);
            accumulatorThreshold = std::max(accumulatorThreshold, 1);

            //runs the detection, and update the display
            HoughDetection(src_gray, myAOI, cannyThreshold, accumulatorThreshold);
        }

        imshow("frame", frame);

        //imshow("mask", mask);
        imshow("result", result);
        //imshow("thresh", thresh);
        if (waitKey(30) >= 0)
            break;
    }
}

void hsvCallBack(int value, void *colorThreshold)
{
    ColorThresholdType *ct = reinterpret_cast<ColorThresholdType *>(colorThreshold);
    switch (*ct)
    {
    case H:
        colorData.h = value;
        break;
    case S:
        colorData.s = value;
        break;
    case V:
        colorData.v = value;
        break;
    case H_HIGH:
        colorData.h_high = value;
        break;
    case S_HIGH:
        colorData.s_high = value;
        break;
    case V_HIGH:
        colorData.v_high = value;
        break;
    }
}