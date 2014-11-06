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

	// ѡ������ƥ���� 
	vector<DMatch> selMatches;
	/*
	keypoints1.push_back(KeyPoint(342.,615.,2));
	keypoints2.push_back(KeyPoint(410.,600.,2));
	selMatches.push_back(DMatch(keypoints1.size()-1,keypoints2.size()-1,0)); // street light bulb
	selMatches.push_back(matches[6]);  // right tower
	selMatches.push_back(matches[60]);  // left bottom window
	selMatches.push_back(matches[139]);
	selMatches.push_back(matches[141]);  // middle window
	selMatches.push_back(matches[213]);
	selMatches.push_back(matches[273]);

	int kk=0;
	while (kk<matches.size()) 
	{
	cout<<kk<<endl;
	selMatches.push_back(matches[kk++]);
	selMatches.pop_back();
	waitKey();
	}
	*/

	cout << matches.size() << endl;

	/* between church01 and church03 */
	selMatches.push_back(matches[14]);
	selMatches.push_back(matches[16]);
	selMatches.push_back(matches[141]);
	selMatches.push_back(matches[146]);
	selMatches.push_back(matches[235]);
	selMatches.push_back(matches[238]);
	selMatches.push_back(matches[273]); //vector subscript out of range 274

	// ����ѡ���ƥ��
	Mat imageMatches;
	drawMatches(image1, keypoints1,  // ��һ��ͼ����ؼ���
		image2, keypoints2,  // �ڶ���ͼ����ؼ���
		//selMatches,			// the matches
		matches,			// ƥ����
		imageMatches,		// ���ɵ�ͼ��
		Scalar(255, 255, 255)); // ֱ����ɫ

	namedWindow("Matches");
	imshow("Matches", imageMatches);

	// ��һ�� keypoints ����ת��Ϊ���� Point2f ����
	vector<int> pointIndexes1;
	vector<int> pointIndexes2;
	for (vector<DMatch>::const_iterator it = selMatches.begin(); it != selMatches.end(); ++it) 
	{
		// �õ�ѡ���ƥ��������
		pointIndexes1.push_back(it->queryIdx);
		pointIndexes2.push_back(it->trainIdx);
	}

	// ת�� keypoints ����Ϊ Point2f
	vector<Point2f> selPoints1, selPoints2;
	KeyPoint::convert(keypoints1, selPoints1, pointIndexes1);
	KeyPoint::convert(keypoints2, selPoints2, pointIndexes2);

	// check by drawing the points 
	vector<Point2f>::const_iterator it = selPoints1.begin();
	while (it != selPoints1.end()) 
	{
		// ��ÿһ���ǵ�λ�û�һ��Ȧ
		circle(image1, *it, 3, Scalar(255, 255, 255), 2);
		++it;
	}

	it = selPoints2.begin();
	while (it != selPoints2.end()) 
	{
		// ��ÿһ���ǵ�λ�û�һ��Ȧ
		circle(image2, *it, 3, Scalar(255, 255, 255), 2);
		++it;
	}

	// ��7��ƥ���м���F����
	Mat fundemental = findFundamentalMat(
		Mat(selPoints1), // ͼ1�еĵ�
		Mat(selPoints2), // ͼ2�еĵ�
		CV_FM_7POINT);   // ʹ��7����ķ���

	cout << "F-Matrix size= " << fundemental.rows << "," << fundemental.cols << endl;

	// ����ͼ�л��ƶ�Ӧ�ļ���
	vector<Vec3f> lines1;
	computeCorrespondEpilines(
		Mat(selPoints1), // ͼ���
		1,               // ͼ1��Ҳ������2��
		fundemental,     // F����
		lines1);         // һ�鼫��

	// �������м���
	for (vector<Vec3f>::const_iterator it = lines1.begin(); it != lines1.end(); ++it) 
	{
		// ���Ƶ�һ�������һ��֮��ļ���
		line(image2, Point(0, -(*it)[2] / (*it)[1]),
			Point(image2.cols, -((*it)[2] + (*it)[0] * image2.cols) / (*it)[1]),
			Scalar(255, 255, 255));
	}

	// ����ͼ�л��ƶ�Ӧ�ļ���
	vector<Vec3f> lines2;
	computeCorrespondEpilines(Mat(selPoints2), 2, fundemental, lines2);

	for (vector<Vec3f>::const_iterator it = lines2.begin(); it != lines2.end(); ++it) 
	{
		// ���Ƶ�һ�������һ��֮��ļ���
		line(image1, Point(0, -(*it)[2] / (*it)[1]),
			Point(image1.cols, -((*it)[2] + (*it)[0] * image1.cols) / (*it)[1]),
			Scalar(255, 255, 255));
	}

	// ��ʾͼ���Լ�ͼ���еĵ�ͼ���
	namedWindow("Right Image Epilines");
	imshow("Right Image Epilines", image1);
	namedWindow("Left Image Epilines");
	imshow("Left Image Epilines", image2);

	waitKey();
	return 0;
}