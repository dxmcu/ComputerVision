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

using namespace std;
using namespace cv;

int main()
{
	// ��ȡ����ͼ��
	Mat image1 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church01.jpg", 0);
	Mat image2 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church03.jpg", 0);
	if (!image1.data || !image2.data)
		return 0;

	// ��ʾ����ͼ��
	namedWindow("Right Image");
	imshow("Right Image", image1);
	namedWindow("Left Image");
	imshow("Left Image", image2);

	// �ؼ�������
	vector<KeyPoint> keypoints1;
	vector<KeyPoint> keypoints2;

	// ���� surf ���������
	SurfFeatureDetector surf(2500);

	// ��� surf ����
	surf.detect(image1, keypoints1);
	surf.detect(image2, keypoints2);

	cout << "Number of SURF points (1): " << keypoints1.size() << endl;
	cout << "Number of SURF points (2): " << keypoints2.size() << endl;

	// ����keypoints
	Mat imageKP;
	drawKeypoints(image1, keypoints1, imageKP, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	namedWindow("Right SURF Features");
	imshow("Right SURF Features", imageKP);

	drawKeypoints(image2, keypoints2, imageKP, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	namedWindow("Left SURF Features");
	imshow("Left SURF Features", imageKP);

	// ���� surf ������������ȡ��
	SurfDescriptorExtractor surfDesc;

	// ��ȡ surf ����������
	Mat descriptors1, descriptors2;
	surfDesc.compute(image1, keypoints1, descriptors1);
	surfDesc.compute(image2, keypoints2, descriptors2);

	cout << "descriptor matrix size: " << descriptors1.rows << " by " << descriptors1.cols << endl;

	// ����ƥ����
	BruteForceMatcher<L2<float>> matcher;

	// ƥ������ͼ���������
	vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);

	cout << "Number of matched points: " << matches.size() << endl;

	
	nth_element(matches.begin(),    // ��ʼλ��
	matches.begin() + matches.size()/4, // �м�λ��
	matches.end());     // ����λ��
	// ɾ���м�λ��֮�������ֵ
	matches.erase(matches.begin() + matches.size()/4, matches.end());
	

	// ����ѡ���ƥ��
	Mat imageMatches;
	drawMatches(image1, keypoints1,  // ��һ��ͼ����ؼ���
		image2, keypoints2,  // �ڶ���ͼ����ؼ���
		//selMatches,           // the matches
		matches,            // ƥ����
		imageMatches,       // ���ɵ�ͼ��
		Scalar(255, 255, 255)); // ֱ����ɫ

	namedWindow("Matches");
	imshow("Matches", imageMatches);

	// ��һ�� keypoints ����ת��Ϊ���� Point2f ����
	vector<Point2f> points1, points2;
	for (vector<DMatch>::const_iterator it = matches.begin(); it != matches.end(); ++it)
	{
		// �õ����ؼ����λ��
		float x = keypoints1[it->queryIdx].pt.x;
		float y = keypoints1[it->queryIdx].pt.y;
		points1.push_back(Point2f(x, y));

		// �õ��Ҳ�ؼ����λ��
		x = keypoints2[it->trainIdx].pt.x;
		y = keypoints2[it->trainIdx].pt.y;
		points2.push_back(Point2f(x, y));
	}
	cout << points1.size() << " " << points2.size() << endl;

	// �� RANSAC ���� F ����
	vector<uchar> inliers(points1.size(), 0);
	Mat fundemental = findFundamentalMat(
		Mat(points1), Mat(points2), // ƥ���
		inliers,      // ƥ��״̬ (inlier ou outlier)
		CV_FM_RANSAC, // RANSAC ��
		1,            // �����ߵľ���
		0.98);        // ���Ÿ���

	// ��ȡͼ��
	image1 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church01.jpg", 0);
	image2 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church03.jpg", 0);

	// ��һ�� keypoints ����ת��Ϊ���� int ����
	vector<int> pointIndexes1;
	vector<int> pointIndexes2;


	// ת�� keypoints ����Ϊ Point2f
	vector<Point2f> selPoints1, selPoints2;
	KeyPoint::convert(keypoints1, selPoints1, pointIndexes1);
	KeyPoint::convert(keypoints2, selPoints2, pointIndexes2);

	// ����������ļ���
	vector<Vec3f> lines1;
	computeCorrespondEpilines(Mat(selPoints1), 1, fundemental, lines1);
	for (vector<Vec3f>::const_iterator it = lines1.begin(); it != lines1.end(); ++it)
	{
		line(image2, Point(0, -(*it)[2] / (*it)[1]),
			Point(image2.cols, -((*it)[2] + (*it)[0] * image2.cols) / (*it)[1]),
			Scalar(255, 255, 255));
	}

	vector<Vec3f> lines2;
	computeCorrespondEpilines(Mat(selPoints2), 2, fundemental, lines2);
	for (vector<Vec3f>::const_iterator it = lines2.begin(); it != lines2.end(); ++it)
	{
		line(image1, Point(0, -(*it)[2] / (*it)[1]),
			Point(image1.cols, -((*it)[2] + (*it)[0] * image1.cols) / (*it)[1]),
			Scalar(255, 255, 255));
	}

	// ���� inlier ��
	vector<Point2f> points1In, points2In;

	vector<Point2f>::const_iterator itPts = points1.begin();
	vector<uchar>::const_iterator itIn = inliers.begin();
	while (itPts != points1.end())
	{
		// ��ÿһ�� inlier ���ϻ���һ��Բ
		if (*itIn)
		{
			circle(image1, *itPts, 3, Scalar(255, 255, 255), 2);
			points1In.push_back(*itPts);
		}
		++itPts;
		++itIn;
	}

	itPts = points2.begin();
	itIn = inliers.begin();
	while (itPts != points2.end())
	{
		// ��ÿһ�� inlier ���ϻ���һ��Բ
		if (*itIn)
		{
			circle(image2, *itPts, 3, Scalar(255, 255, 255), 2);
			points2In.push_back(*itPts);
		}
		++itPts;
		++itIn;
	}

	// ��ʾͼ��͵�
	namedWindow("Right Image Epilines (RANSAC)");
	imshow("Right Image Epilines (RANSAC)", image1);
	namedWindow("Left Image Epilines (RANSAC)");
	imshow("Left Image Epilines (RANSAC)", image2);


	waitKey();
	return 0;
}
