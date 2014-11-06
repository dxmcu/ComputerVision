#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"

using namespace std;
using namespace cv;

int main()
{
	Mat image1 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/parliament1.bmp", 0);
	Mat image2 = imread("E:/��������/���/openCV/opencv-2-cookbook-src-master/images/parliament2.bmp", 0);
    if (!image1.data || !image2.data)
    {
        cout << " --(!) Error reading images " << endl; return -1;
    }

    //-- Step 1: �� SURF ����������ؼ���
    int minHessian = 400;
    SurfFeatureDetector detector(minHessian);
    vector<KeyPoint> keypoints1, keypoints2;
    detector.detect(image1, keypoints1);
    detector.detect(image2, keypoints2);

    //-- Step 2: ��ȡ����������
    SurfDescriptorExtractor extractor;
    Mat descriptors1, descriptors2;
    extractor.compute(image1, keypoints1, descriptors1);
    extractor.compute(image2, keypoints2, descriptors2);

    //-- Step 3: �� FLANN ��ƥ������������
    FlannBasedMatcher matcher;
    vector< DMatch > matches;
    matcher.match(descriptors1, descriptors2, matches);

	//-- ���ټ���������֮���������С����
    double max_dist = 0; double min_dist = 100;
    for (int i = 0; i < descriptors1.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }
    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

	//-- ��������ƥ�䣬�������С��2����С�����ƥ�䣬���ߵ���С����ǳ�
	//-- С��ʱ��ʹ��һ��С������ֵ��0.02��
	//-- PS.- Ҳ����ʹ�� radiusMatch .
    vector< DMatch > good_matches;
    for (int i = 0; i < descriptors1.rows; i++)
    {
        if (matches[i].distance <= max(2 * min_dist, 0.02))
        {
            good_matches.push_back(matches[i]);
        }
    }

    //-- ����������ƥ��
    Mat imagematches;
    drawMatches(image1, keypoints1, image2, keypoints2,
                good_matches,
                imagematches,
                Scalar::all(-1),
                Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    //-- ��ʾ��⵽��ƥ��
    imshow("Good Matches", imagematches);
    for (int i = 0; i < (int)good_matches.size(); i++)
    {
        printf("-- Good Match [%d] Keypoint 1: %d -- Keypoint 2: %d \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
    }
    waitKey(0);

    return 0;
}

