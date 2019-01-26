#pragma once
#ifndef __COLOR_H_
#define __COLOR_H_
#include <string>
#include <opencv2/core.hpp>
#include <boost/format.hpp>
#include "../Common.h"
//��ɫ�ṹ
struct color_t {
	uchar b, g, r, alpha;
	color_t() :b(0), g(0), r(0), alpha(0) {}
	//absolute val
	color_t operator-(const color_t& rhs) {
		color_t c;
		c.b = b - rhs.b;
		c.g = g - rhs.g;
		c.r = r - rhs.r;
		return c;
	}
	bool operator<=(const color_t& rhs) {
		return b <= rhs.b&&g <= rhs.g&&r <= rhs.r;
	}
	color_t& str2color(std::wstring&s) {
		int r, g, b;
		std::transform(s.begin(), s.end(), s.begin(), ::towupper);
		swscanf(s.c_str(), L"%02X%02X%02X", &r, &g, &b);
		this->b = b; this->r = r; this->g = g;
		return *this;
	}
	std::wstring tostr() {
		char buff[10];
		sprintf(buff, "%02X%02X%02X", r, g, b);
		return _sto_wstring(buff);
	}
};

//��ɫ-ƫɫ�ṹ
struct color_df_t {
	//��ɫ
	color_t color;
	//ƫɫ
	color_t df;
};

#endif
