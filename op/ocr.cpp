#include "stdafx.h"

#include <list>
#include "include/color.h"
#include "ocr.h"

constexpr int MIN_CUT_W = 5;
constexpr int MIN_CUT_H = 2;

int get_bk_color(inputbin bin) {
	int y[256] = { 0 };
	auto ptr = bin.pixels.data();
	int n = bin.pixels.size();
	for (int i = 0; i < n; ++i)
		y[ptr[i]]++;
	//scan max
	int m = 0;
	for (int i = 0; i < 256; ++i) {
		if (y[i] > y[m])m = i;
	}
	return m;
}

void bgr2binary(inputimg src, outputbin bin, vector<color_df_t>& colors) {
	if (src.empty())
		return;
	int ncols = src.width, nrows = src.height;
	bin.create(ncols, nrows);
	for (int i = 0; i < nrows; ++i) {
		auto psrc = src.ptr<color_t>(i);
		auto pbin = bin.ptr(i);
		for (int j = 0; j < ncols; ++j) {
			*pbin = WORD_BKCOLOR;
			for (auto&it : colors) {//��ÿ����ɫ����
				if (IN_RANGE(*psrc, it.color, it.df)) {
					*pbin = WORD_COLOR;
					break;
				}
			}
			++pbin; ++psrc;
		}
	}
	//test
	//cv::imwrite("src.png", _src);
	//cv::imwrite("binary.png", _binary);
}

//��ֵ��
void auto2binary(inputimg src, outputbin bin)
{
	//תΪ�Ҷ�ͼ
	bin.fromImage4(src);
	//������ֵͼ
	//_binary.create(_record.size(), CV_8UC1);
	//��ȡ������ɫ
	int bkcolor = get_bk_color(bin);
	int n = bin.width*bin.height;

	auto pdst = bin.data();
	for (int i = 0; i < n; ++i) {
		pdst[i] = (std::abs(pdst[i] - bkcolor) < 20 ? WORD_BKCOLOR : WORD_COLOR);
	}
}


//��ֱ����ͶӰ��x��
void binshadowx(const ImageBin& binary, const rect_t& rc, std::vector<rect_t>& out_put)
{
	//qDebug("in x rc:%d,%d,%d,%d", rc.x1, rc.y1, rc.x2, rc.y2);
	out_put.clear();
	//ys.clear();
	//Mat paintx(binary.size(), CV_8UC1, cv::Scalar(255)); //����һ��ȫ��ͼƬ��������ʾ

	//int* blackcout = new int[binary.cols];
	std::vector<int> vx;
	vx.resize(binary.width);
	memset(&vx[0], 0, binary.width * 4);
	for (int j = rc.x1; j < rc.x2; j++)
	{
		for (int i = rc.y1; i < rc.y2; i++)
		{
			if (binary.at(i, j) == 0)
			{
				vx[j]++; //��ֱͶӰ������x�����ͶӰ
			}
		}
	}

	int startindex = 0;
	int endindex = 0;
	bool inblock = false; //�Ƿ�������ַ�λ��
	rect_t roi;
	for (int j = rc.x1; j < rc.x2; j++)
	{

		if (!inblock&&vx[j] != 0) //�������ַ�����
		{
			inblock = true;
			startindex = j;
			//std::cout << "startindex:" << startindex << std::endl;
		}
		//if (inblock&&vx[j] == 0) //����հ���
		else if(inblock&&vx[j]==0&&j-startindex>=MIN_CUT_W)//����հ������ҿ�Ȳ�С��1
		{
			endindex = j;
			inblock = false;
			//Mat roi = binary.colRange(startindex, endindex + 1);
		
			roi.x1 = startindex; roi.y1 = rc.y1;
			roi.x2 = endindex; roi.y2 = rc.y2;
			//qDebug("out xrc:%d,%d,%d,%d", roi.x1, roi.y1, roi.x2, roi.y2);
			out_put.push_back(roi);
		}
	}
	//special case
	if (inblock) {
		roi.x1 = startindex; roi.y1 = rc.y1;
		roi.x2 = rc.x2; roi.y2 = rc.y2;
		out_put.push_back(roi);
	}

}
//ͶӰ��y��
void binshadowy(const ImageBin& binary, const rect_t& rc, std::vector<rect_t>&out_put)
{
	//qDebug("in y rc:%d,%d,%d,%d", rc.x1, rc.y1, rc.x2, rc.y2);
	out_put.clear();
	//�Ƿ�Ϊ��ɫ���ߺ�ɫ���ݶ�ֵͼ��Ĵ������
	//Mat painty(binary.size(), CV_8UC1, cv::Scalar(255)); //��ʼ��Ϊȫ��
	//ˮƽͶӰ
	//int* pointcount = new int[binary.rows]; //�ڶ�ֵͼƬ�м�¼����������ĸ���
	std::vector<int> vy;
	vy.resize(binary.height);
	memset(&vy[0], 0, binary.height * 4);//ע��������Ҫ���г�ʼ��

	for (int i = rc.y1; i < rc.y2; i++)
	{
		for (int j = rc.x1; j < rc.x2; j++)
		{
			if (binary.at(i, j) == WORD_COLOR)
			{
				vy[i]++; //��¼ÿ���к�ɫ��ĸ��� //ˮƽͶӰ������y���ϵ�ͶӰ
			}
		}
	}


	//std::vector<Mat> result;
	int startindex = 0;
	int endindex = 0;
	bool inblock = false; //�Ƿ�������ַ�λ��
	rect_t roi;
	for (int i = rc.y1; i < rc.y2; i++)
	{

		if (!inblock&&vy[i] != 0) //�������ַ�����
		{
			inblock = true;
			startindex = i;
			//std::cout << "startindex:" << startindex << std::endl;
		}
		//if (inblock&&vy[i] == 0) //����հ���
		if(inblock&&vy[i]==0&&i-startindex>=MIN_CUT_H)//����հ���,�Ҹ߶Ȳ�С��1
		{
			endindex = i;
			inblock = false;
		
			roi.x1 = rc.x1; roi.y1 = startindex;
			roi.x2 = rc.x2; roi.y2 = endindex;
			out_put.push_back(roi);
		}
	}

	if (inblock) {
		roi.x1 = rc.x1; roi.y1 = startindex;
		roi.x2 = rc.x2; roi.y2 = rc.y2;
		out_put.push_back(roi);
	}

}

void bin_image_cut(const ImageBin& binary, int min_word_h, const rect_t&inrc, rect_t& outrc) {
	//ˮƽ�ü�����С�߶�
	std::vector<int>v;
	outrc = inrc;
	int i, j;
	v.resize(binary.height);
	for (auto&it : v)it = 0;
	for (i = inrc.y1; i < inrc.y2; ++i) {
		for (j = inrc.x1; j < inrc.x2; ++j)
			v[i] += (binary.at(i, j) == WORD_COLOR ? 1 : 0);
	}
	i = inrc.y1;
	while (v[i] == 0)i++;
	outrc.y1 = i;
	i = inrc.y2 - 1;
	while (v[i] == 0)i--;
	if (i + 1 - outrc.y1 > min_word_h)
		outrc.y2 = i + 1;
	//��ֱ�ü�.��С���
	v.resize(binary.width);
	for (auto&it : v)it = 0;
	
	for (i = inrc.y1; i < inrc.y2; ++i) {
		for (j = inrc.x1; j < inrc.x2; ++j)
			v[j] += binary.at(i, j) == 0 ? 1 : 0;
	}
	i = inrc.x1;
	while (v[i] == 0)i++;
	outrc.x1 = i;
	i = inrc.x2-1;
	while (v[i] == 0)i--;
	outrc.x2 = i+1;
	
}

void get_rois(const ImageBin& bin, int min_word_h,std::vector<rect_t>& vroi) {
	vroi.clear();
	std::vector<rect_t> vrcx, vrcy;
	rect_t rc;
	rc.x1 = rc.y1 = 0;
	rc.x2 = bin.width; rc.y2 = bin.height;
	binshadowy(bin, rc, vrcy);
	for (int i = 0; i < vrcy.size();++i) {
		binshadowx(bin, vrcy[i], vrcx);
		for (int j = 0; j < vrcx.size();j++) {
			if (vrcx[j].width() >= min_word_h)
				bin_image_cut(bin, min_word_h, vrcx[j], vrcx[j]);
				vroi.push_back(vrcx[j]);
		}
	}
	
}

inline int full_match(const ImageBin& binary, rect_t& rc, const word_t::cline_t* lines) {
	//ƥ��
	unsigned __int32 val;
	for (int x = rc.x1; x < rc.x2; ++x) {
		val = 0;
		for (int y = rc.y1, id = 31; y < rc.y2; ++y, --id) {
			if (binary.at(y, x) == 0)
				SET_BIT(val, id);
		}
		if (lines[x - rc.x1] != val)
			return 0;
		//t = QString::asprintf("%08X", x);
		//tp += t;

	}
	return 1;
}

inline int part_match(const ImageBin& binary, rect_t& rc, int max_error, const word_t::cline_t* lines) {
	//ƥ��
	unsigned __int32 sval, wval;
	int error_ct = 0;
	for (int x = rc.x1; x < rc.x2; ++x) {
		wval = lines[x - rc.x1];
		for (int y = rc.y1, id = 31; y < rc.y2; ++y, --id) {
			sval = binary.at(y, x);
			if ((sval & 1) == GET_BIT(wval, id))
				++error_ct;
			if (error_ct > max_error)
				return 0;
		}
	}
	return 1;
}

inline void fill_rect(ImageBin& record, const rect_t& rc) {

	int w = rc.width();
	for (int y = rc.y1; y < rc.y2; ++y) {
		uchar* p = record.ptr(y) + rc.x1;
		memset(p, 1, sizeof(uchar)*w);
	}

}

void _bin_ocr(const ImageBin& binary, ImageBin& record, const rect_t&rc, const Dict& dict, std::map<point_t, std::wstring>&ps) {
	int i, j, y;
	//outstr.clear();
	//������һ����
	//qDebug("in y rc:%d,%d,%d,%d", rc.x1, rc.y1, rc.x2, rc.y2);
	if (rc.width() <= 0 || rc.height() <= 0)
		return;
	//������
	for (i = rc.y1; i < rc.y2; ++i) {
		//������
		for (j = rc.x1; j < rc.x2; ++j) {
			if (record.at(i, j))
				continue;
			point_t pt;
			pt.x = j; pt.y = i;
			//�����ֿ�
			//assert(i != 4 || j != 3);
			for (auto&it : dict.words) {

				rect_t crc;
				crc.x1 = j; crc.y1 = i;
				crc.x2 = j + it.info.width; crc.y2 = i + it.info.height;
				//�߽���
				if (crc.y2 > rc.y2 || crc.x2 > rc.x2)
					continue;
				//match

				int matched = full_match(binary, crc, it.clines);
				if (matched) {
					if (crc.x2 < rc.x2)//����ʣ�ಿ�֣�����ұ��Ƿ�հ�
					{
						/*for (y = crc.y1; y < crc.y2; ++y)
							if (binary.at(y, crc.x2) == 0)
								break;*/
						if (/*y == crc.y2*/1) {
							//outstr.append(it.info._char);
							ps[pt] = it.info._char;
							//������һ���������� �ֱ�Ϊ�ұߺ��·�
							//�ұ����Ȳ��ң��·����
							//��
							fill_rect(record, crc);
							//goto __next_y;
							break;//words
						}

					}
					else {
						ps[pt] = it.info._char;
						//

						fill_rect(record, crc);
						//������һ���������� ֻ���Ϸ�
						//i = i - it.info.height;
						break;
					}

				}
			}//end for words
		}//end for j
	}//end for i


}

void _bin_ocr(const ImageBin& binary, ImageBin& record, const rect_t&rc, const Dict& dict, int *max_error, std::map<point_t, std::wstring>& ps) {
	int i, j, y;
	if (rc.width() <= 0 || rc.height() <= 0)
		return;
	//������
	for (i = rc.y1; i < rc.y2; ++i) {
		//������
		for (j = rc.x1; j < rc.x2; ++j) {
			if (record.at(i, j))
				continue;
			point_t pt;
			pt.x = j; pt.y = i;
			//�����ֿ�
			//assert(i != 4 || j != 3);
			int k = 0;
			for (auto&it : dict.words) {

				rect_t crc;
				crc.x1 = j; crc.y1 = i;
				crc.x2 = j + it.info.width; crc.y2 = i + it.info.height;
				//�߽���
				if (crc.y2 > rc.y2 || crc.x2 > rc.x2) {
					++k;
					continue;
				}
				//match
				int matched = part_match(binary, crc, max_error[k], it.clines);
				++k;
				if (matched) {
					if (crc.x2 < rc.x2)//����ʣ�ಿ�֣�����ұ��Ƿ�հ�
					{
						for (y = crc.y1; y < crc.y2; ++y)
							if (binary.at(y, crc.x2) == 0)
								break;
						if (/*y == crc.y2*/1) {
							//outstr.append(it.info._char);
							ps[pt] = it.info._char;
							//������һ���������� �ֱ�Ϊ�ұߺ��·�
							//�ұ����Ȳ��ң��·����
							//��
							fill_rect(record, crc);
							//goto __next_y;
							break;//words
						}

					}
					else {
						ps[pt] = it.info._char;
						//

						fill_rect(record, crc);
						//������һ���������� ֻ���Ϸ�
						//i = i - it.info.height;
						break;
					}

				}
			}//end for words
		}//end for j
	}//end for i


}


void bin_ocr(const ImageBin& binary, ImageBin& record, const Dict& dict, double sim, std::map<point_t, std::wstring>&ps) {
	ps.clear();
	if (dict.words.empty())return;
	if (binary.empty())
		return;
	record.create(binary.width, binary.height);
	memset(record.data(), 0, sizeof(uchar)*record.width*record.height);

	/*
	�������
	*/
	sim = 0.5 + sim / 2;
	sim = 1.0 - sim;
	int word_h_min = 32;
	std::vector<int> vmax_error;
	vmax_error.resize(dict.words.size());
	for (int i = 0; i < vmax_error.size(); ++i) {
		vmax_error[i] = dict.words[i].info.bit_count*sim;
		if (word_h_min > dict.words[i].info.height)
			word_h_min = dict.words[i].info.height;
	}
	vector<rect_t> vroi;
	get_rois(binary,word_h_min, vroi);
	for (int i = 0; i < vroi.size(); ++i) {
		if (sim > 1.0 - 1e-5) {
			_bin_ocr(binary, record, vroi[i], dict, ps);
		}
		else {
			_bin_ocr(binary, record, vroi[i], dict, vmax_error.data(), ps);
		}
	}
		

	
}
