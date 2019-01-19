#include "ximage.h"

#include <qdebug.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using cv::Mat;
//���ֶ�ֵ��
void thresholdIntegral(const Mat& inputMat, Mat& outputMat)
{

	int nRows = inputMat.rows;
	int nCols = inputMat.cols;

	// create the integral image
	Mat sumMat;
	integral(inputMat, sumMat);

	int S = MAX(nRows, nCols) / 8;
	double T = 0.15;

	// perform thresholding
	int s2 = S / 2;
	int x1, y1, x2, y2, count, sum;

	int* p_y1, *p_y2;
	const uchar* p_inputMat;
	uchar*p_outputMat;

	for (int i = 0; i < nRows; ++i)
	{
		y1 = i - s2;
		y2 = i + s2;

		if (y1 < 0)
		{
			y1 = 0;
		}
		if (y2 >= nRows)
		{
			y2 = nRows - 1;
		}

		p_y1 = sumMat.ptr<int>(y1);
		p_y2 = sumMat.ptr<int>(y2);
		p_inputMat = inputMat.ptr<uchar>(i);
		p_outputMat = outputMat.ptr<uchar>(i);

		for (int j = 0; j < nCols; ++j)
		{
			// set the SxS region
			x1 = j - s2;
			x2 = j + s2;

			if (x1 < 0)
			{
				x1 = 0;
			}
			if (x2 >= nCols)
			{
				x2 = nCols - 1;
			}

			count = (x2 - x1)* (y2 - y1);

			// I(x,y)=s(x2,y2)-s(x1,y2)-s(x2,y1)+s(x1,x1)
			sum = p_y2[x2] - p_y1[x2] - p_y2[x1] + p_y1[x1];

			if ((int)(p_inputMat[j] * count) < (int)(sum* (1.0 - T)))
			{
				p_outputMat[j] = 0;
			}
			else
			{
				p_outputMat[j] = 255;
			}
		}
	}
}
//��ֱ����ͶӰ
void binshadowx(const Mat& binary, const rect_t& rc, std::vector<rect_t>& out_put)
{
	//qDebug("in x rc:%d,%d,%d,%d", rc.x1, rc.y1, rc.x2, rc.y2);
	out_put.clear();
	//ys.clear();
	//Mat paintx(binary.size(), CV_8UC1, cv::Scalar(255)); //����һ��ȫ��ͼƬ��������ʾ

	//int* blackcout = new int[binary.cols];
	std::vector<int> ys;
	ys.resize(binary.cols);
	memset(&ys[0], 0, binary.cols * 4);

	for (int i = rc.y1; i < rc.y2; i++)
	{
		for (int j = rc.x1; j < rc.x2; j++)
		{
			if (binary.at<uchar>(i, j) == 0)
			{
				ys[j]++; //��ֱͶӰ������x�����ͶӰ
			}
		}
	}
	
	int startindex = 0;
	int endindex = 0;
	bool inblock = false; //�Ƿ�������ַ�λ��

	for (int j = rc.x1; j <rc.x2; j++)
	{

		if (!inblock&&ys[j] != 0) //�������ַ�����
		{
			inblock = true;
			startindex = j;
			//std::cout << "startindex:" << startindex << std::endl;
		}
		if (inblock&&ys[j] == 0) //����հ���
		{
			endindex = j;
			inblock = false;
			//Mat roi = binary.colRange(startindex, endindex + 1);
			rect_t roi;
			roi.x1 = startindex; roi.y1 = rc.y1;
			roi.x2 = endindex; roi.y2 = rc.y2;
			//qDebug("out xrc:%d,%d,%d,%d", roi.x1, roi.y1, roi.x2, roi.y2);
			out_put.push_back(roi);
		}
	}

}
//ˮƽ����ͶӰ���зָ�
void binshadowy(const Mat& binary, const rect_t& rc, std::vector<rect_t>&out_put)
{
	//qDebug("in y rc:%d,%d,%d,%d", rc.x1, rc.y1, rc.x2, rc.y2);
	out_put.clear();
	//�Ƿ�Ϊ��ɫ���ߺ�ɫ���ݶ�ֵͼ��Ĵ������
	//Mat painty(binary.size(), CV_8UC1, cv::Scalar(255)); //��ʼ��Ϊȫ��
	//ˮƽͶӰ
	//int* pointcount = new int[binary.rows]; //�ڶ�ֵͼƬ�м�¼����������ĸ���
	std::vector<int> xs;
	xs.resize(binary.rows);
	memset(&xs[0], 0, binary.rows * 4);//ע��������Ҫ���г�ʼ��

	for (int i = rc.y1; i < rc.y2; i++)
	{
		for (int j = rc.x1; j < rc.x2; j++)
		{
			if (binary.at<uchar>(i, j) == 0)
			{
				xs[i]++; //��¼ÿ���к�ɫ��ĸ��� //ˮƽͶӰ������y���ϵ�ͶӰ
			}
		}
	}

	
	//std::vector<Mat> result;
	int startindex = 0;
	int endindex = 0;
	bool inblock = false; //�Ƿ�������ַ�λ��

	for (int i = rc.y1; i < rc.y2; i++)
	{

		if (!inblock&&xs[i] != 0) //�������ַ�����
		{
			inblock = true;
			startindex = i;
			//std::cout << "startindex:" << startindex << std::endl;
		}
		if (inblock&&xs[i] == 0) //����հ���
		{
			endindex = i;
			inblock = false;
			//Mat roi = binary.rowRange(startindex, endindex + 1); //�Ӷ���¼�ӿ�ʼ�������е�λ�ã����ɽ������з�
			rect_t roi;
			roi.x1 = rc.x1; roi.y1 = startindex;
			roi.x2 = rc.x2; roi.y2 = endindex ;
			//qDebug("out yrc:%d,%d,%d,%d", roi.x1, roi.y1, roi.x2, roi.y2);
			out_put.push_back(roi);
		}
	}

	/*for (int i = 0; i < result.size(); i++)
	{
		Mat tmp = result[i];
		imshow("test" + std::to_string(i), tmp);
	}*/

}
