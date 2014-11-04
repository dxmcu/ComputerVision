#if !defined HARRISD
#define HARRISD

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

class HarrisDetector
{
private:

	// ��ʾ�ǵ�ǿ�ȵ�32λ����ͼ��
	cv::Mat cornerStrength;
	// ��ʾ��ֵ����ǵ�ǿ�ȵ�32λ����ͼ��
	cv::Mat cornerTh;
	// �ֲ�����ֵͼ���ڲ���
	cv::Mat localMax;
	// ����ƽ�����������صĳߴ�
	int neighbourhood;
	// �ݶȼ���Ŀ׾���С
	int aperture;
	// Harris ����
	double k;
	// ��ֵ��������ǿ��
	double maxStrength;
	// ����õ�����ֵ���ڲ���
	double threshold;
	// �Ǽ���ֵ���Ƶ��������صĳߴ�
	int nonMaxSize;
	// �Ǽ���ֵ���Ƶĺ�
	cv::Mat kernel;

public:

	HarrisDetector() : neighbourhood(3), aperture(3), k(0.1), maxStrength(0.0), threshold(0.01), nonMaxSize(3)
	{
		setLocalMaxWindowSize(nonMaxSize);
	}

	// �����Ǽ���ֵ���Ƶĺ�
	void setLocalMaxWindowSize(int size)
	{
		nonMaxSize = size;
		kernel.create(nonMaxSize, nonMaxSize, CV_8U);
	}

	// ���� Harris �ǵ� 
	void detect(const cv::Mat& image)
	{
		// Harris ����
		cv::cornerHarris(image, cornerStrength,
			neighbourhood,// �����С
			aperture,     // �˲����Ŀ׾���С
			k);           // Harris ����

		// �ڲ���ֵ����
		double minStrength; // δʹ��
		cv::minMaxLoc(cornerStrength, &minStrength, &maxStrength);

		// �ֲ�����ֵ���
		cv::Mat dilated;  // ��ʱͼ��
		cv::dilate(cornerStrength, dilated, cv::Mat());
		cv::compare(cornerStrength, dilated, localMax, cv::CMP_EQ);
	}

	// �� Harris ֵ��ȡ�ǵ�ͼ
	cv::Mat getCornerMap(double qualityLevel)
	{
		cv::Mat cornerMap;

		// �Խǵ�ͼ�������ֵ��
		threshold = qualityLevel*maxStrength;
		cv::threshold(cornerStrength, cornerTh, threshold, 255, cv::THRESH_BINARY);

		// ת��Ϊ8λͼ��
		cornerTh.convertTo(cornerMap, CV_8U);

		// �Ǽ���ֵ����
		cv::bitwise_and(cornerMap, localMax, cornerMap);

		return cornerMap;
	}

	// �Ӽ���õ��� Harris ֵ�õ��ǵ�
	void getCorners(std::vector<cv::Point> &points, double qualityLevel)
	{
		// �õ��ǵ�ӳ��
		cv::Mat cornerMap = getCornerMap(qualityLevel);
		// �õ��ǵ�
		getCorners(points, cornerMap);
	}

	// �ɼ���õ��Ľǵ�ӳ���ȡ������
	void getCorners(std::vector<cv::Point> &points, const cv::Mat& cornerMap)
	{
		// �������صõ���������
		for (int y = 0; y < cornerMap.rows; y++) 
		{
			const uchar* rowPtr = cornerMap.ptr<uchar>(y);

			for (int x = 0; x < cornerMap.cols; x++) 
			{
				// �����������
				if (rowPtr[x]) 
				{
					points.push_back(cv::Point(x, y));
				}
			}
		}
	}

	// ���������λ�û���Բ
	void drawOnImage(cv::Mat &image, const std::vector<cv::Point> &points, cv::Scalar color = cv::Scalar(255, 255, 255), int radius = 3, int thickness = 2)
	{
		std::vector<cv::Point>::const_iterator it = points.begin();

		// ��������������
		while (it != points.end()) 
		{
			// ����Բ
			cv::circle(image, *it, radius, color, thickness);
			++it;
		}
	}
};

#endif
