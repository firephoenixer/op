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
	//special case
	if (inblock) {
		out_put.push_back(rc);
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

	if (inblock) {
		out_put.push_back(rc);
	}

}

void bin_image_cut(const cv::Mat& binary, const rect_t&inrc, rect_t& outrc) {
	//ˮƽ�ü�
	std::vector<int>v;
	
	int i, j;
	v.resize(binary.rows);
	for (auto&it : v)it = 0;
	for (i = inrc.y1; i < inrc.y2; ++i) {
		for (j = inrc.x1; j < inrc.x2; ++j)
			v[i] += (binary.at<uchar>(i, j) == 0 ? 1 : 0);
	}
	for(i = inrc.y1+1; i < inrc.y2; ++i)
		if (v[i-1] == 0 && v[i] != 0) {
			outrc.y1 = i;
			break;
		}
	for (i = inrc.y2 - 2; i >= inrc.y1; --i)
		if (v[i + 1] == 0 && v[i] != 0) {
			outrc.y2 = i;
			break;
		}
	//��ֱ�ü�
	v.resize(binary.cols);
	for (auto&it : v)it = 0;
	for (i = inrc.y1; i < inrc.y2; ++i) {
		for (j = inrc.x1; j < inrc.x2; ++j)
			v[j] += binary.at<uchar>(i, j) == 0 ? 1 : 0;
	}
	for (j = inrc.x1 + 1; i < inrc.x2; ++i)
		if (v[j - 1] == 0 && v[j] != 0) {
			outrc.x1 = j;
			break;
		}
	for (j = inrc.x2 - 2; j >= inrc.x1; --j)
		if (v[j + 1] == 0 && v[j] != 0) {
			outrc.x2 = j;
			break;
		}
}

void bin_ocr(const cv::Mat& binary, const rect_t&rc, const Dict& dict, std::wstring& outstr) {
	int i, j;
	outstr.clear();
	//������
	for (i = rc.y1; i < rc.y2; ++i) {
		//������
		for (j = rc.x1; j < rc.x2; ++j) {
			//�����ֿ�
			for (auto&it : dict.words) {
				//�߽���
				if (i + it.info.height > rc.y2 || j + it.info.width > rc.x2)
					continue;
				//ƥ��
				int x;
				for (x = j; x < j+it.info.width; ++x) {
					unsigned __int32 val=0;
					for (int y = i, id = 0; y<i+it.info.height; ++y, ++id) {
						if (binary.at<uchar>(y, x) == 0)
							SET_BIT(val, 31 - id);
					}
					if (it.clines[x - j] != val)
						break;
					//t = QString::asprintf("%08X", x);
					//tp += t;

				}
				if (x == j + it.info.width) {
					outstr.append(it.info._char);
					j = x;
				}
				else {
					//del
				}
					
			}
		}
	}
}

void bin_ocr(const cv::Mat& binary, const Dict& dict, std::wstring& outstr) {
	std::vector<rect_t> out_y, out_x;
	rect_t rc;
	rc.x1 = rc.y1 = 0;
	rc.x2 = binary.cols; rc.y2 = binary.rows;
	//qDebug("rc:%d,%d,%d,%d", rc.x1, rc.y1, rc.x2, rc.y2);
	std::wstring s;
	//step1. ˮƽ�ָ�
	binshadowy(binary, rc, out_y);
	for (auto&ity : out_y) {
		//step 2. ��ֱ�ָ�
		binshadowx(binary, ity, out_x);
		for (auto&itx : out_x) {
			//pic_name = std::to_string((i << 16) | j);
			//cv::imshow(pic_name, out_x[j]);
			//�ü�
			bin_image_cut(binary, itx, itx);
			bin_ocr(binary, itx, dict, s);
			outstr.append(s);
			//_chars.push_back(out_x[j]);
		}
	}
}
