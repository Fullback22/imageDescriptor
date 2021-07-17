// testImageDescriptor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "opencv2\opencv.hpp"

#include <stdio.h>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

int main()
{
    int threshold_{ 20 };
    cv::Mat img1 = cv::imread("1234.png");    //train
    cv::Mat img2 = cv::imread("1_.png");     //query



    //cv::GaussianBlur(img1, img1, cv::Size(3,3), 2);
    //cv::medianBlur(img1, img1, 3);
    //cv::Mat coreMorfologis(cv::Size(5, 5), CV_8U);
    ////cv::morphologyEx(img1, img1, cv::MORPH_ERODE, coreMorfologis);
    //cv::Canny(img1, img1, threshold_, threshold_ * 2, 3, false);
    //std::vector<std::vector<cv::Point>> contours;
    //std::vector<cv::Vec4i> hiararchy;
    //cv::findContours(img1, contours, hiararchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    //img1 = cv::Mat(img1.size(), CV_8UC1, cv::Scalar(0));
    //for (size_t i{ 0 }; i < contours.size(); ++i)
    //{
    //    cv::drawContours(img1, contours, int(i), cv::Scalar(255), 1);
    //}

    ////cv::blur(img2, img2, cv::Size(4, 4));
    //cv::GaussianBlur(img2, img2, cv::Size(3,3), 2);
    //cv::medianBlur(img2, img2, 3);
    ////cv::Mat coreMorfologis(cv::Size(5, 5), CV_8U);
    ////cv::morphologyEx(img2, img2, cv::MORPH_ERODE, coreMorfologis);
    //cv::Canny(img2, img2, threshold_, threshold_ * 2, 3, false);
    //contours.clear();
    //hiararchy.clear();
    //cv::findContours(img2, contours, hiararchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    //img2 = cv::Mat(img2.size(), CV_8UC1, cv::Scalar(0));
    //for (size_t i{ 0 }; i < contours.size(); ++i)
    //{
    //    cv::drawContours(img2, contours, int(i), cv::Scalar(255), 1);
    //}

    cv::SURF orb;
    std::vector<cv::KeyPoint> keyPoint_1, keyPoint_2;
    orb.detect(img1, keyPoint_1);
    orb.detect(img2, keyPoint_2);
    cv::Mat descript1, descript2;
    orb.compute(img1, keyPoint_1,descript1);
    orb.compute(img2, keyPoint_2,descript2);
    //std::vector<cv::DMatch> mathes;
    std::vector<std::vector<cv::DMatch>> mathes;
    cv::FlannBasedMatcher flan;//(new cv::flann::KDTreeIndexParams(5),new cv::flann::SearchParams(50));
    cv::BFMatcher bf(cv::NORM_HAMMING, true);
    //bf.knnMatch(descript2, descript1, mathes, 2);
    flan.knnMatch(descript2, descript1, mathes,2);
    
    std::vector<cv::DMatch> mathesOut;
    /*for (size_t i{ 0 }; i < mathes.size(); ++i)
    {
        for (size_t j{ 0 }; j < mathes.size()-1-i; ++j)
        {
            if (mathes[j].distance > mathes[j+1].distance)
            {
                cv::DMatch bufer;
                bufer = mathes[j];
                mathes[j] = mathes[j + 1];
                mathes[j + 1] = bufer;
            }
        }
    }*/
    for (size_t i{ 0 }; i < mathes.size(); ++i)
    {
        if (mathes[i][0].distance < 0.75 * mathes[i][1].distance)
            mathesOut.push_back(mathes[i][0]);
    }

    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;

    for (size_t i{0}; i < mathesOut.size(); ++i)
    {
        obj.push_back(keyPoint_1[mathesOut[i].trainIdx].pt);
        scene.push_back(keyPoint_2[mathesOut[i].queryIdx].pt);
    }

    cv::Mat H = findHomography(obj, scene, CV_RANSAC, 5.0);

    std::vector<cv::Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0, 0); 
    obj_corners[1] = cvPoint(img2.cols, 0);
    obj_corners[2] = cvPoint(img2.cols, img2.rows); 
    obj_corners[3] = cvPoint(0, img2.rows);
    std::vector<cv::Point2f> scene_corners(4);

    //////-- ќтобразить углы целевого объекта, использу€ найденное преобразование, на сцену
    perspectiveTransform(obj_corners, scene_corners, H);

    //////-- —оеденить отображенные углы
    //////cv::polylines(img1, scene_corners, true, cv::Scalar(0, 0, 0));
    cv::line(img1, scene_corners[0] , scene_corners[1] , cv::Scalar(0, 255, 0), 4);
    cv::line(img1, scene_corners[1] , scene_corners[2] , cv::Scalar(0, 255, 0), 4);
    cv::line(img1, scene_corners[2] , scene_corners[3] , cv::Scalar(0, 255, 0), 4);
    cv::line(img1, scene_corners[3] , scene_corners[0] , cv::Scalar(0, 255, 0), 4);
    //mathes.push_back(bufer);
    //cv::findHomography()
    //cv::BFMatcher bf(cv::NORM_HAMMING, true);
    
    //bf.match(descript1, descript2, mathes);

    cv::Mat img3;
    cv::drawMatches(img2, keyPoint_2, img1, keyPoint_1, mathesOut, img3);
    cv::imshow("sada", img3);
    cv::waitKey();
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
