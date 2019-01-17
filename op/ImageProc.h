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
	enum { _max_dicts = 10 };
	ImageProc();
	~ImageProc();
	//ͼ�ζ�λ
	long FindPic(long x1, long y1, long x2, long y2, const std::wstring& files, double sim, long& x, long &y);
	//
	long SetDict(int idx,const wstring& file);
	long UseDict(int idx);
	long OCR(const wstring& color, double sim, std::wstring& out_str);
	long FindColor(const wstring& color, long&x, long&y);
	std::wstring GetColor(long x, long y);
private:
	dict_t _dicts[_max_dicts];
	dict_t* _curr_dict;
private:
	void str2colordfs(const wstring& color_str, std::vector<color_df_t>& colors);
};

