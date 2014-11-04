#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

// SurfDescriptorExtractor��������Ҫ����opencv_nonfree249d.lib�ļ�
#include<opencv2/nonfree/features2d.hpp>
#include<opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace cv;

int main()
{
	// ��ȡͼ��
	Mat image = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church03.jpg", 0);
	if (!image.data)
	{
		return 0;
	}

	// ��ʾͼ��
	namedWindow("Original Image");
	imshow("Original Image", image);

	// �ؼ�������
	vector<KeyPoint> keypoints;

	// ����surf����������
	SurfFeatureDetector surf(2500);
	// ���surf����
	surf.detect(image, keypoints);

	Mat featureImage;
	drawKeypoints(image, keypoints, featureImage, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	// ��ʾ�ǵ�
	namedWindow("SURF Features");
	imshow("SURF Features", featureImage);


	waitKey();
	return 0;
}
