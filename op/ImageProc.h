#pragma once
#include <string>
#include "ImageLoc.h"
using std::wstring;
/*
����Ϊͼ�����������¹���
1.���رȽϣ�����
2.��ɫת��
3.ͼ��λ
4.��OCR
5....
*/
class ImageProc:public ImageExtend
{
public:
	const static int _max_dict = 10;
	
	ImageProc();
	~ImageProc();
	//
	long Capture(const std::wstring& file);

	long CmpColor(long x, long y, const std::wstring& scolor, double sim);

	long FindColor(const wstring& color,double sim,long dir, long&x, long&y);

	long FindColoEx(const wstring& color, double sim, long dir, wstring& retstr);

	long FindMultiColor(const wstring& first_color,const wstring& offset_color, double sim, long dir, long&x, long&y);

	long FindMultiColorEx(const wstring& first_color, const wstring& offset_color, double sim, long dir, wstring& retstr);
	//ͼ�ζ�λ
	long FindPic(long x1, long y1, long x2, long y2, const std::wstring& files,const wstring& delta_colors, double sim,long dir, long& x, long &y);
	//
	long FindPicEx(long x1, long y1, long x2, long y2, const std::wstring& files, const wstring& delta_colors, double sim, long dir, wstring& retstr);

	std::wstring GetColor(long x, long y);

	long SetDict(int idx,const wstring& file);

	long UseDict(int idx);

	long OCR(const wstring& color, double sim, std::wstring& out_str);
	
	
private:
	Dict _dicts[_max_dict];
	Dict* _curr_dict;
private:
	void str2colordfs(const wstring& color_str, std::vector<color_df_t>& colors);
};

