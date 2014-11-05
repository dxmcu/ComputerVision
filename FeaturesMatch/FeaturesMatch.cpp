#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

// SurfDescriptorExtractor��������Ҫ����opencv_nonfree249d.lib�ļ�
#include<opencv2/nonfree/features2d.hpp>
#include<opencv2/nonfree/nonfree.hpp>

// BruteForceMatcher��������Ҫ����opencv_legacy249d.lib�ļ�
#include<opencv2/legacy/legacy.hpp>

using namespace std;
using namespace cv;
int main()
{
    // ����ͼ��
    Mat image1 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church01.jpg", 0);
    Mat image2 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/church02.jpg", 0);
    if (!image1.data || !image2.data)
        return 0;

    // ��ʾͼ��
    namedWindow("Right Image");
    imshow("Right Image", image1);
    namedWindow("Left Image");
    imshow("Left Image", image2);

    // �ؼ�������
    vector<KeyPoint> keypoints1;
    vector<KeyPoint> keypoints2;

    // ����surf����������
    SurfFeatureDetector surf(3000);

    // ���surf����
    surf.detect(image1, keypoints1);
    surf.detect(image2, keypoints2);

    cout << "Number of SURF points (1): " << keypoints1.size() << endl;
    cout << "Number of SURF points (2): " << keypoints2.size() << endl;

    // ���ƹؼ���
    Mat imageKP;
    drawKeypoints(image1, keypoints1, imageKP, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	// ��ʾ�ؼ���ͼ��
    namedWindow("Right SURF Features");
    imshow("Right SURF Features", imageKP);

    drawKeypoints(image2, keypoints2, imageKP, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    namedWindow("Left SURF Features");
    imshow("Left SURF Features", imageKP);

    // ����surf��������ȡ��
    SurfDescriptorExtractor surfDesc;

    // ��ȡsurf������
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

	// ��n��Ԫ�ط��ڵ�n��λ�ã�֮ǰ����С�ڵ�n��Ԫ�أ�֮�󶼴���
    nth_element(matches.begin(),    // ��ʼλ��
                     matches.begin() + 24, // ����Ԫ�ص�λ��
                     matches.end());     // ��ֹλ��


    // �Ƴ���25λ֮�����е�Ԫ��
    matches.erase(matches.begin() + 25, matches.end());

    Mat imageMatches;
    drawMatches(image1, keypoints1,  // ��һ��ͼ����������
                    image2, keypoints2,  // �ڶ���ͼ����������
                    matches,            // ƥ����
                    imageMatches,       // ���ɵ�ͼ��
                    Scalar(255, 255, 255)); // ֱ�ߵ���ɫ

	// ��ʾƥ��ͼ��
    namedWindow("Matches");
    imshow("Matches", imageMatches);

    waitKey();
    return 0;
}
