#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

// SurfDescriptorExtractor��������Ҫ����opencv_nonfree249d.lib�ļ�
#include<opencv2/nonfree/features2d.hpp>
#include<opencv2/nonfree/nonfree.hpp>

// BruteForceMatcher��������Ҫ����opencv_legacy249d.lib�ļ�
#include<opencv2/legacy/legacy.hpp>

#include "matcher.h"

using namespace std;
using namespace cv;

int main()
{
    // ����ͼ��
    Mat image1 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/parliament1.bmp", 0);
    Mat image2 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/parliament2.bmp", 0);
    if (!image1.data || !image2.data)
        return 0;

    // ��ʾͼ��
    namedWindow("Image 1");
    imshow("Image 1", image1);
    namedWindow("Image 2");
    imshow("Image 2", image2);

    // ׼��ƥ����
    RobustMatcher rmatcher;
    rmatcher.setConfidenceLevel(0.98);
    rmatcher.setMinDistanceToEpipolar(1.0);
    rmatcher.setRatio(0.65f);
    Ptr<FeatureDetector> pfd = new SurfFeatureDetector(10);
    rmatcher.setFeatureDetector(pfd);

    // ƥ������ͼ��
    vector<DMatch> matches;
    vector<KeyPoint> keypoints1, keypoints2;
    Mat fundemental = rmatcher.match(image1, image2, matches, keypoints1, keypoints2);

    // ����ƥ����
    Mat imageMatches;
    drawMatches(image1, keypoints1,  // ��һ��ͼ����ؼ���
                image2, keypoints2,  // �ڶ���ͼ����ؼ���
                matches,            // ƥ����
                imageMatches,       // ���ɵ�ͼ��
                Scalar(255, 255, 255)); // ֱ����ɫ
    namedWindow("Matches");
    imshow("Matches", imageMatches);

    // �� keypoints ת��Ϊ Point2f
    vector<Point2f> points1, points2;
    for (vector<DMatch>::const_iterator it = matches.begin(); it != matches.end(); ++it)
    {
        // /������ keypoints ��λ��
        float x = keypoints1[it->queryIdx].pt.x;
        float y = keypoints1[it->queryIdx].pt.y;
        points1.push_back(Point2f(x, y));
        // ����Ҳ� keypoints ��λ��
        x = keypoints2[it->trainIdx].pt.x;
        y = keypoints2[it->trainIdx].pt.y;
        points2.push_back(Point2f(x, y));
    }

    cout << points1.size() << " " << points2.size() << endl;

    // �ҵ�����ͼ��֮��ĵ�Ӧ����
    vector<uchar> inliers(points1.size(), 0);
    Mat homography = findHomography(
                         Mat(points1), Mat(points2), // ��Ӧ�ĵ㼯
                         inliers,    // �������ȷֵ
                         CV_RANSAC,  // RANSAC ��
                         1.);        // ����ͶӰ���������

    // ���� inlier ��
    vector<Point2f>::const_iterator itPts = points1.begin();
    vector<uchar>::const_iterator itIn = inliers.begin();
    while (itPts != points1.end())
    {
        // ��ÿ�� inlier λ�û�Բ
        if (*itIn)
            circle(image1, *itPts, 3, Scalar(255, 255, 255), 2);

        ++itPts;
        ++itIn;
    }

    itPts = points2.begin();
    itIn = inliers.begin();
    while (itPts != points2.end())
    {
        // ��ÿ�� inlier λ�û�Բ
        if (*itIn)
            circle(image2, *itPts, 3, Scalar(255, 255, 255), 2);

        ++itPts;
        ++itIn;
    }

    // ��ʾ����ͼ��
    namedWindow("Image 1 Homography Points");
    imshow("Image 1 Homography Points", image1);
    namedWindow("Image 2 Homography Points");
    imshow("Image 2 Homography Points", image2);

    // ����ͼ1 �� ͼ2
    Mat result;
    warpPerspective(image1, // ����ͼ��
                    result,         // ���ͼ��
                    homography,     // homography
                    Size(2 * image1.cols, image1.rows)); // ���ͼ���С

    // ����ͼ1������ͼ���ǰ�벿��
    Mat half(result, Rect(0, 0, image2.cols, image2.rows));
    image2.copyTo(half);// ����ͼ2��ͼ1��ROI����

    // ��ʾ������ͼ��
    namedWindow("After warping");
    imshow("After warping", result);

    waitKey();
    return 0;
}
