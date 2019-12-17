#pragma once
#ifndef __IMAGELOC_H_
#define __IMAGELOC_H_

#include <vector>
#include "optype.h"
#include <string>
#include "include/Dict.h"
#include "include/color.h"
#include "ocr.h"
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

using img_names = std::vector<std::wstring>;
//����Ƿ�Ϊ͸��ͼ
int check_transparent(Image* img);
//��ȡƥ���
void get_match_points(const Image& img, vector<int>&points);
//generate next index for kmp
void gen_next(const Image& img, vector<int>& next);

/*
��������ʵ��һЩͼ���ܣ���ͼ��λ����ocr��
*/
class ImageBase
{
public:
	
	const static int _max_return_obj_ct = 1800;

	ImageBase();

	~ImageBase();

	//brief:����ͼ�񣬽���ͼ�ξ���,��ͼ�����ǰ����
	//image_data:	4�ӽڶ��������ָ��
	//widht:		ͼ����
	//hegith:		h
	//x1,y1,x2,y2 ��������
	//type:			��������,type=0��ʾ�������룬Ϊ-1ʱ��ʾ��������
	long input_image(byte* psrc, int cols, int height,long x1,long y1,long x2,long y2, int type = 0);

	void set_offset(int dx, int dy);


	
	template<bool nodfcolor>
	long simple_match(long x, long y, Image* timg,color_t dfcolor,int max_error);
	template<bool nodfcolor>
	long trans_match(long x, long y, Image* timg, color_t dfcolor,vector<uint>points, int max_error);

	long real_match(long x, long y, ImageBin* timg, double sim);
	
	long is_valid(long x, long y) {
		return x >= 0 && y >= 0 && x < _src.width && y < _src.height;
	}

	long GetPixel(long x, long y, color_t&cr);

	long CmpColor(long x, long y, std::vector<color_df_t>&colors, double sim);

	long FindColor(std::vector<color_df_t>&colors, long&x, long&y);

	long FindColorEx(std::vector<color_df_t>&colors, std::wstring& retstr);

	long FindMultiColor(std::vector<color_df_t>&first_color, std::vector<pt_cr_df_t>& offset_color, double sim, long dir, long&x, long&y);

	long FindMultiColorEx(std::vector<color_df_t>&first_color, std::vector<pt_cr_df_t>& offset_color, double sim, long dir, std::wstring& retstr);

	long FindPic(std::vector<Image*>&pics,color_t dfcolor,double sim, long&x, long&y);

	long FindPicEx(std::vector<Image*>&pics, color_t dfcolor,double sim, wstring& retstr);

	long Ocr(Dict& dict, double sim, std::wstring& ret_str);

	long OcrEx(Dict& dict, double sim, std::wstring& out_str);

	long FindStr(Dict& dict, const vector<wstring>& vstr,  double sim, long& retx, long& rety);

	long FindStrEx(Dict& dict, const vector<wstring>& vstr, double sim, std::wstring& out_str);

public:
	Image _src;
	ImageBin _gray;
	ImageBin _record;
	ImageBin _binary;
private:
	//��ʼ��
	int _x1, _y1;
	//ƫ��
	int _dx, _dy;
	
};

#endif

