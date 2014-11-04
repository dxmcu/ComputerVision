#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

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

	// �ؼ�������
	vector<KeyPoint> keypoints;

	MserFeatureDetector mser;
	mser.detect(image, keypoints);

	Mat featureImage;
	// ���������㣬���ϳ߶Ⱥͷ�����Ϣ
	drawKeypoints(image,        // ԭʼͼ��
		keypoints,                  // �ؼ�������
		featureImage,               // ����ͼ��
		Scalar(255, 255, 255),  // ��������ɫ
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS); // ���Ʊ�־

	// ��ʾ�ǵ�
	namedWindow("MSER Features");
	imshow("MSER Features", featureImage);

	waitKey();
	return 0;
}
