#pragma once
#ifndef __IMAGELOC_H_
#define __IMAGELOC_H_
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include "Common.h"
#include <string>
#include "Dict.h"
#include "Color/color.h"

inline int HEX2INT(wchar_t c) {
	if (L'0' <= c && c <= L'9')
		return c - L'0';
	if (L'A' <= c && c <= L'Z')
		return c - L'A' + 10;
	if (L'a' <= c && c <= L'z')
		return c - L'a' + 10;
	return 0;
}


#define SET_BIT(x, idx) x |= 1u << (idx)

#define GET_BIT(x, idx) (x >> (idx)) & 1u
using images_t = std::vector<std::wstring>;



/*
��������ʵ��һЩͼ���ܣ���ͼ��λ����ocr��
*/
class ImageBase
{
public:
	template<typename T>
	static int get_bit_count(T x);
	const static int _max_return_obj_ct = 1800;

	ImageBase();

	~ImageBase();

	//brief:����ͼ�񣬽���ͼ�ξ���,��ͼ�����ǰ����
	//image_data:	4�ӽڶ��������ָ��
	//widht:		ͼ����
	//hegith:		h
	//x1,y1,x2,y2 ��������
	//type:			��������,type=0��ʾ�������룬Ϊ-1ʱ��ʾ��������
	

	long input_image(byte* image_data, int width, int height,long x1,long y1,long x2,long y2, int type = 0);

	/*
	if(abs(cr-src)<=df) pixel=1;
	else pixel=0;
	*/
	void bgr2binary(vector<color_df_t>& colors);
	//brief:ͼ��λ
	//images:ͼ���ļ���������Ϊ���
	//sim:����5-599.
	//x,y:Ŀ������
	long imageloc(images_t& images, double sim, long&x, long&y);

	long is_valid(long x, long y) {
		return x >= 0 && y >= 0 && x < _src.cols && y < _src.rows;
	}

	long GetPixel(long x, long y, color_t&cr);

	long CmpColor(long x, long y, std::vector<color_df_t>&colors, double sim);

	long FindColor(std::vector<color_df_t>&colors, long&x, long&y);

	long FindColorEx(std::vector<color_df_t>&colors, std::wstring& retstr);

	long FindMultiColor(std::vector<color_df_t>&first_color, std::vector<pt_cr_df_t>& offset_color, double sim, long dir, long&x, long&y);

	long FindMultiColorEx(std::vector<color_df_t>&first_color, std::vector<pt_cr_df_t>& offset_color, double sim, long dir, std::wstring& retstr);


	long Ocr(Dict& dict, double sim, std::wstring& ret_str);

	long OcrEx(Dict& dict, double sim, std::wstring& out_str);

	long FindStr(Dict& dict, const vector<wstring>& vstr,  double sim, long& retx, long& rety);

	long FindStrEx(Dict& dict, const vector<wstring>& vstr, double sim, std::wstring& out_str);

	
	
public:
	cv::Mat _src;
	cv::Mat _target;
	cv::Mat _binary;
	cv::Mat _result;
	
private:
	
	
};

#endif

