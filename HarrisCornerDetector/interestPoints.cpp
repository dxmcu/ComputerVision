#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "harrisDetector.h"

using namespace std;
using namespace cv;

int main()
{
	// ��ȡͼ��
	Mat image = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church01.jpg", 0);
	if (!image.data)
		return 0;

	// ��ʾͼ��
	namedWindow("Original Image");
	imshow("Original Image", image);

	// ��� Harris �ǵ�
	Mat cornerStrength;
	cornerHarris(image, cornerStrength,
		3,     // �������سߴ�
		3,     // �˲����Ŀ׾���С
		0.01); // Harris ����

	// �ǵ�ǿ�ȵ���ֵ
	Mat harrisCorners;
	double thresh = 0.0001;
	threshold(cornerStrength, harrisCorners, thresh, 255, THRESH_BINARY_INV);

	// ��ʾ�ǵ�ͼ
	namedWindow("Harris Corner Map");
	imshow("Harris Corner Map", harrisCorners);

	// ���� Harris ������
	HarrisDetector harris;
	// ���� Harris ֵ 
	harris.detect(image);
	// ��� Harris �ǵ�
	vector<Point> pts;
	harris.getCorners(pts, 0.01);
	// ���� Harris �ǵ�
	harris.drawOnImage(image, pts);

	// ��ʾ�ǵ�
	namedWindow("Harris Corners");
	imshow("Harris Corners", image);

	// ��ȡͼ��
	image = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church01.jpg", 0);

	// �����ʺ�׷�ٵ���������
	vector<Point2f> corners;
	goodFeaturesToTrack(image, corners,
		500,	// ���ص������������Ŀ
		0.01,	// �����ȼ�
		10);	// ����֮�����С���о���

	// �������нǵ�
	vector<Point2f>::const_iterator it = corners.begin();
	while (it != corners.end()) 
	{
		// ��ÿһ���ǵ�λ�û�һ��Բ
		circle(image, *it, 3, Scalar(255, 255, 255), 2);
		++it;
	}

	// ��ʾ�ǵ�
	namedWindow("Good Features to Track");
	imshow("Good Features to Track", image);

	// ��ȡͼ��
	image = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church01.jpg", 0);

	// �ؼ�������
	vector<KeyPoint> keypoints;
	// �����ʺ�׷�ٵ��������� 
	GoodFeaturesToTrackDetector gftt(
		500,	// ���ص������������Ŀ
		0.01,	// �����ȼ�
		10);	// ����֮�����С���о���
	// ��������ⷨ���йؼ�����
	gftt.detect(image, keypoints);

	drawKeypoints(image,		// ԭʼͼ��
		keypoints,					// �ؼ�������
		image,						// ���ͼ��
		Scalar(255, 255, 255),	// �ؼ�����ɫ
		DrawMatchesFlags::DRAW_OVER_OUTIMG); // ���Ʊ�־

	// ��ʾ�ǵ�
	namedWindow("Good Features to Track Detector");
	imshow("Good Features to Track Detector", image);

	//keypoints.clear();

	waitKey();
	return 0;
}