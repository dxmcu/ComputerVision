#if !defined MATCHER
#define MATCHER

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

class RobustMatcher
{

private:

	// ָ�������������ָ��
	cv::Ptr<cv::FeatureDetector> detector;
	// ָ����������ȡ����ָ��
	cv::Ptr<cv::DescriptorExtractor> extractor;
	float ratio; // ��һ���Լ��ڶ��������֮���������
	bool refineF; // �Ƿ�ı�F����
	double distance; // �����ߵ���С����
	double confidence; // ���ŵȼ������ʣ�

public:

	RobustMatcher() : ratio(0.65f), refineF(true), confidence(0.99), distance(3.0)
	{
		// SURF ΪĬ������
		detector = new cv::SurfFeatureDetector();
		extractor = new cv::SurfDescriptorExtractor();
	}

	// �������������
	void setFeatureDetector(cv::Ptr<cv::FeatureDetector> &detect)
	{
		detector = detect;
	}

	// ������������ȡ��
	void setDescriptorExtractor(cv::Ptr<cv::DescriptorExtractor> &desc)
	{
		extractor = desc;
	}

	// ���� RANSAC �е����ߵ���С����
	void setMinDistanceToEpipolar(double d)
	{
		distance = d;
	}

	// ���� RANSAC �е����ŵȼ�
	void setConfidenceLevel(double c)
	{
		confidence = c;
	}

	// ���������֮��ı���
	void setRatio(float r)
	{
		ratio = r;
	}

	// ����Ƿ� F ������Ҫ���¼���
	void refineFundamental(bool flag)
	{
		refineF = flag;
	}

	// �Ƴ� NN ���ʴ�����ֵ��ƥ��
	// �����Ƴ������Ŀ
	// (��Ӧ�����0�����ߴ罫Ϊ0)
	int ratioTest(std::vector<std::vector<cv::DMatch>> &matches)
	{
		int removed = 0;

		// ��������ƥ��
		for (std::vector<std::vector<cv::DMatch>>::iterator matchIterator = matches.begin();
			matchIterator != matches.end(); ++matchIterator)
		{
			// ���ʶ�����������
			if (matchIterator->size() > 1)
			{
				// ���������
				if ((*matchIterator)[0].distance / (*matchIterator)[1].distance > ratio)
				{
					matchIterator->clear(); // �Ƴ�ƥ��
					removed++;
				}

			}
			else     // ���������������
			{
				matchIterator->clear(); // �Ƴ�ƥ��
				removed++;
			}
		}
		return removed;
	}

	// �� symmetrical �����в���Գ�ƥ��
	void symmetryTest(const std::vector<std::vector<cv::DMatch>> &matches1,
		const std::vector<std::vector<cv::DMatch>> &matches2,
		std::vector<cv::DMatch> &symMatches)
	{
		// ����ͼ1 -> ͼ2������ƥ��
		for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator1 = matches1.begin();
			matchIterator1 != matches1.end(); ++matchIterator1)
		{
			if (matchIterator1->size() < 2) // ���Ա�ɾ����ƥ��
				continue;
			// ����ͼ2 -> ͼ1������ƥ��
			for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator2 = matches2.begin();
				matchIterator2 != matches2.end(); ++matchIterator2)
			{
				if (matchIterator2->size() < 2) // ���Ա�ɾ����ƥ��
					continue;
				// �Գ��Բ���
				if ((*matchIterator1)[0].queryIdx == (*matchIterator2)[0].trainIdx &&
					(*matchIterator2)[0].queryIdx == (*matchIterator1)[0].trainIdx)
				{
					// ���ӶԳƵ�ƥ��
					symMatches.push_back(cv::DMatch((*matchIterator1)[0].queryIdx,
						(*matchIterator1)[0].trainIdx,
						(*matchIterator1)[0].distance));
					break; // ͼ1 -> ͼ2����һ��ƥ��
				}
			}
		}
	}

	// ���� RANSAC ʶ������ƥ��
	// ���ػ�������
	cv::Mat ransacTest(const std::vector<cv::DMatch> &matches,
		const std::vector<cv::KeyPoint> &keypoints1,
		const std::vector<cv::KeyPoint> &keypoints2,
		std::vector<cv::DMatch> &outMatches)
	{
		// �� keypoints ת��Ϊ Point2f
		std::vector<cv::Point2f> points1, points2;
		for (std::vector<cv::DMatch>::const_iterator it = matches.begin();
			it != matches.end(); ++it)
		{
			// �õ��������ֵ������
			float x = keypoints1[it->queryIdx].pt.x;
			float y = keypoints1[it->queryIdx].pt.y;
			points1.push_back(cv::Point2f(x, y));
			// �õ��ұ�����ֵ������
			x = keypoints2[it->trainIdx].pt.x;
			y = keypoints2[it->trainIdx].pt.y;
			points2.push_back(cv::Point2f(x, y));
		}

		// ���� RANSAC ����F����
		std::vector<uchar> inliers(points1.size(), 0);
		cv::Mat fundemental = cv::findFundamentalMat(
			cv::Mat(points1), cv::Mat(points2), // ƥ���
			inliers,      // ƥ��״̬ (inlier ou outlier)
			CV_FM_RANSAC, // RANSAC ����
			distance,     // �����ߵľ���
			confidence);  // ���Ÿ���

		// ��ȡͨ����ƥ��
		std::vector<uchar>::const_iterator itIn = inliers.begin();
		std::vector<cv::DMatch>::const_iterator itM = matches.begin();
		// ��������ƥ��
		for (; itIn != inliers.end(); ++itIn, ++itM)
		{
			if (*itIn)   // Ϊ����ƥ��
			{
				outMatches.push_back(*itM);
			}
		}

		std::cout << "Number of matched points (after cleaning): " << outMatches.size() << std::endl;

		if (refineF)
		{
			// F����ʹ�����н��ܵ�ƥ�����¼���

			// ��  KeyPoint ����ת��Ϊ Point2f ����
			points1.clear();
			points2.clear();

			for (std::vector<cv::DMatch>::const_iterator it = outMatches.begin();
				it != outMatches.end(); ++it)
			{
				// �õ���������������
				float x = keypoints1[it->queryIdx].pt.x;
				float y = keypoints1[it->queryIdx].pt.y;
				points1.push_back(cv::Point2f(x, y));
				// �õ��ұ������������
				x = keypoints2[it->trainIdx].pt.x;
				y = keypoints2[it->trainIdx].pt.y;
				points2.push_back(cv::Point2f(x, y));
			}

			// �����н��ܵ�ƥ���м���8��F
			fundemental = cv::findFundamentalMat(
				cv::Mat(points1), cv::Mat(points2), // ƥ���
				CV_FM_8POINT); // 8��ƥ�䷨
		}
		return fundemental;
	}

	// ʹ�öԳ��Բ����Լ� PANSAC ƥ��������
	// ���ػ�������
	cv::Mat match(cv::Mat &image1, cv::Mat &image2, // ����ͼ��
		std::vector<cv::DMatch> &matches, // ���ƥ�估������
		std::vector<cv::KeyPoint> &keypoints1, std::vector<cv::KeyPoint> &keypoints2)
	{
		// 1a. ���SURF����
		detector->detect(image1, keypoints1);
		detector->detect(image2, keypoints2);

		std::cout << "Number of SURF points (1): " << keypoints1.size() << std::endl;
		std::cout << "Number of SURF points (2): " << keypoints2.size() << std::endl;

		// 1b. ��ȡSURF����������
		cv::Mat descriptors1, descriptors2;
		extractor->compute(image1, keypoints1, descriptors1);
		extractor->compute(image2, keypoints2, descriptors2);

		std::cout << "descriptor matrix size: " << descriptors1.rows << " by " << descriptors1.cols << std::endl;

		// 2. ƥ������ͼ���������

		// ����ƥ����
		cv::BruteForceMatcher<cv::L2<float>> matcher;

		// ��ͼ1 �� ͼ2��k������ڣ�k=2��
		std::vector<std::vector<cv::DMatch>> matches1;
		matcher.knnMatch(descriptors1, descriptors2,
			matches1, // ƥ������������ÿ��������ֵ��
			2);       // �������������

		// ��ͼ2 �� ͼ1��k������ڣ�k=2��
		std::vector<std::vector<cv::DMatch>> matches2;
		matcher.knnMatch(descriptors2, descriptors1,
			matches2, // ƥ������������ÿ��������ֵ��
			2);       // �������������

		std::cout << "Number of matched points 1->2: " << matches1.size() << std::endl;
		std::cout << "Number of matched points 2->1: " << matches2.size() << std::endl;

		// 3. �Ƴ�NN���ʴ�����ֵ��ƥ��

		// ����ͼ1 -> ͼ2��ƥ��
		int removed = ratioTest(matches1);
		std::cout << "Number of matched points 1->2 (ratio test) : " << matches1.size() - removed << std::endl;
		// ����ͼ2 -> ͼ1��ƥ��
		removed = ratioTest(matches2);
		std::cout << "Number of matched points 1->2 (ratio test) : " << matches2.size() - removed << std::endl;

		// 4. �Ƴ��ǶԳƵ�ƥ��
		std::vector<cv::DMatch> symMatches;
		symmetryTest(matches1, matches2, symMatches);

		std::cout << "Number of matched points (symmetry test): " << symMatches.size() << std::endl;

		// 5. ʹ��RANSAC����������֤
		cv::Mat fundemental = ransacTest(symMatches, keypoints1, keypoints2, matches);

		// �����ҵ��Ļ�������
		return fundemental;
	}
};

#endif
