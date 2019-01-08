#pragma once
#ifndef __IMAGELOC_H_
#define __IMAGELOC_H_
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using images_t = std::vector<std::wstring>;
class ImageLoc
{
public:
	
	ImageLoc();
	~ImageLoc();
	//
	
	//brief:����ͼ�񣬽���ͼ�ξ���,��ͼ�����ǰ����
	//image_data:	4�ӽڶ��������ָ��
	//widht:		ͼ����
	//height:		ͼ��߶�
	//type:			��������,type=0��ʾ�������룬Ϊ-1ʱ��ʾ��������
	long input_image(byte* image_data, int width, int height,int type=0);
	//brief:ͼ��λ
	//images:ͼ���ļ���������Ϊ���
	//sim:����
	//x,y:Ŀ������
	long imageloc(images_t& images, double sim, long&x, long&y);
private:
	cv::Mat _src;
	cv::Mat _src_gray;
	cv::Mat _target;
	cv::Mat _target_gray;
	cv::Mat _result;
};

#endif

