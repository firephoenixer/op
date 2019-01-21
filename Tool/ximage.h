#pragma once
#ifndef __XIMAGE_H_
#define __XIMAGE_H_
#include <opencv2/core.hpp>
#include <fstream>
#include "Dict.h"


//���ֶ�ֵ��
void thresholdIntegral(const cv::Mat& inputMat, cv::Mat& outputMat);
//��ֱ����ͶӰ(x)��
void binshadowx(const cv::Mat& binary,const rect_t& rc, std::vector<rect_t>& out_put);
//ˮƽ����ͶӰ(y)��
void binshadowy(const cv::Mat& binary, const rect_t& rc, std::vector<rect_t>&out_put);
//ͼ��ü�
void bin_image_cut(const cv::Mat& binary, const rect_t&inrc, rect_t& outrc);
//ocr in rc only a line
void bin_ocr(const cv::Mat& binary, const rect_t&rc,const Dict& dict, std::wstring& outstr);
void bin_ocr(const cv::Mat& binary, const Dict& dict, std::wstring& outstr);


#endif

