#pragma once
#ifndef __XIMAGE_H_
#define __XIMAGE_H_
#include <opencv2/core.hpp>
//���ֶ�ֵ��
void thresholdIntegral(const cv::Mat& inputMat, cv::Mat& outputMat);
//��ֱ����ͶӰ
void picshadowx(const cv::Mat& binary, std::vector<cv::Mat>& out_put,std::vector<int>&ys);
//ˮƽ����ͶӰ���зָ�
void picshadowy(const cv::Mat& binary, std::vector<cv::Mat>&out_put,std::vector<int>&xs);

#endif

