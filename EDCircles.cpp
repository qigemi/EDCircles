// EDCircls.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>

#define line_squares 1
#define pi 3.141592653
#define mag_threshold 50
#define arc_squares 2
int shortestline = 50;

using namespace cv;
using namespace std;

class link_pix{
public:
	bool ifend = 0;
	int x;
	int y;
	link_pix* next;
	link_pix* prev;
};

class link_line{
public:
	Vec4f line;
	vector<link_pix*>* pix_chain;
};

class link_seg{
public:
	vector<link_pix*>* pix_chain;
	vector<link_line*>* line_chain;
	link_pix* addr;
	link_seg* next;
};

class link_arc{
public:
	Point o;
	float r;
	vector<link_pix*>* pix_chain;
	vector<link_line*>* line_chain;
};

class link_circle{
public:
	vector<link_arc>* arc_chain;
	vector<link_pix*>* pix_chain;
	Point o;
	float r;
};

class link_ellips{
public:
	vector<link_arc>* arc_chain;
	vector<link_pix*>* pix_chain;
	float A[6];
	Point o;
	float a;
	float b;
	float angle;
};


void goleft1(int &i, int &j, int &k, Mat mag, Mat &edge, Mat dir, link_pix* &pix_now)
{
	if (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		int x = i;
		int y = j + 1;
		if (mag.at<short>(i - 1, j + 1) > mag.at<short>(i, j + 1) &&
			mag.at<short>(i - 1, j + 1) > mag.at<short>(i + 1, j + 1))
			x = i - 1;
		else
		{
			if (mag.at<short>(i + 1, j + 1) > mag.at<short>(i, j + 1) &&
				mag.at<short>(i + 1, j + 1) > mag.at<short>(i - 1, j + 1))
				x = i + 1;
		}
		if (mag.at<short>(x, y) > 0 && edge.at<uchar>(x, y) != 255)
			edge.at<uchar>(x, y) = 128;
	}
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		edge.at<uchar>(i, j) = 255;
		pix_now->x = i;
		pix_now->y = j;
		pix_now->next = (link_pix*)malloc(sizeof(link_pix));
		k++;                                                    //每加入一个像素自加一
		(pix_now->next)->prev = pix_now;
		pix_now = pix_now->next;
		if (mag.at<short>(i - 1, j - 1) > mag.at<short>(i, j - 1) &&
			mag.at<short>(i - 1, j - 1) > mag.at<short>(i + 1, j - 1))
			i = i - 1;
		else
		{
			if (mag.at<short>(i + 1, j - 1) > mag.at<short>(i, j - 1) &&
				mag.at<short>(i + 1, j - 1) > mag.at<short>(i - 1, j - 1))
				i = i + 1;
		}
		j = j - 1;
	}
}

void goleft2(int &i, int &j, int &k, Mat mag, Mat &edge, Mat dir, link_pix* &pix_now)
{
	if (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		int x = i;
		int y = j + 1;
		if (mag.at<short>(i - 1, j + 1) > mag.at<short>(i, j + 1) &&
			mag.at<short>(i - 1, j + 1) > mag.at<short>(i + 1, j + 1))
			x = i - 1;
		else
		{
			if (mag.at<short>(i + 1, j + 1) > mag.at<short>(i, j + 1) &&
				mag.at<short>(i + 1, j + 1) > mag.at<short>(i - 1, j + 1))
				x = i + 1;
		}
		if (mag.at<short>(x, y) > 0 && edge.at<uchar>(x, y) != 255)
			edge.at<uchar>(x, y) = 128;
	}
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		edge.at<uchar>(i, j) = 255;
		pix_now->x = i;
		pix_now->y = j;
		pix_now->prev = (link_pix*)malloc(sizeof(link_pix));
		k++;
		(pix_now->prev)->next = pix_now;
		pix_now = pix_now->prev;
		if (mag.at<short>(i - 1, j - 1) > mag.at<short>(i, j - 1) &&
			mag.at<short>(i - 1, j - 1) > mag.at<short>(i + 1, j - 1))
			i = i - 1;
		else
		{
			if (mag.at<short>(i + 1, j - 1) > mag.at<short>(i, j - 1) &&
				mag.at<short>(i + 1, j - 1) > mag.at<short>(i - 1, j - 1))
				i = i + 1;
		}
		j = j - 1;
	}
}

void goright1(int &i, int &j, int &k, Mat mag, Mat &edge, Mat dir, link_pix* &pix_now)
{
	if (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		int x = i;
		int y = j - 1;
		if (mag.at<short>(i - 1, j - 1) > mag.at<short>(i, j - 1) &&
			mag.at<short>(i - 1, j - 1) > mag.at<short>(i + 1, j - 1))
			x = i - 1;
		else
		{
			if (mag.at<short>(i + 1, j - 1) > mag.at<short>(i, j - 1) &&
				mag.at<short>(i + 1, j - 1) > mag.at<short>(i - 1, j - 1))
				x = i + 1;
		}
		if (mag.at<short>(x, y) > 0 && edge.at<uchar>(x, y) != 255)
			edge.at<uchar>(x, y) = 128;
	}
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		edge.at<uchar>(i, j) = 255;
		pix_now->x = i;
		pix_now->y = j;
		pix_now->next = (link_pix*)malloc(sizeof(link_pix));
		k++;
		(pix_now->next)->prev = pix_now;
		pix_now = pix_now->next;
		if (mag.at<short>(i - 1, j + 1) > mag.at<short>(i, j + 1) &&
			mag.at<short>(i - 1, j + 1) > mag.at<short>(i + 1, j + 1))
			i = i - 1;
		else
		{
			if (mag.at<short>(i + 1, j + 1) > mag.at<short>(i, j + 1) &&
				mag.at<short>(i + 1, j + 1) > mag.at<short>(i - 1, j + 1))
				i = i + 1;
		}
		j = j + 1;
	}
}

void goright2(int &i, int &j, int &k, Mat mag, Mat &edge, Mat dir, link_pix* &pix_now)
{
	if (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		int x = i;
		int y = j - 1;
		if (mag.at<short>(i - 1, j - 1) > mag.at<short>(i, j - 1) &&
			mag.at<short>(i - 1, j - 1) > mag.at<short>(i + 1, j - 1))
			x = i - 1;
		else
		{
			if (mag.at<short>(i + 1, j - 1) > mag.at<short>(i, j - 1) &&
				mag.at<short>(i + 1, j - 1) > mag.at<short>(i - 1, j - 1))
				x = i + 1;
		}
		if (mag.at<short>(x, y) > 0 && edge.at<uchar>(x, y) != 255)
			edge.at<uchar>(x, y) = 128;
	}
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		edge.at<uchar>(i, j) = 255;
		pix_now->x = i;
		pix_now->y = j;
		pix_now->prev = (link_pix*)malloc(sizeof(link_pix));
		k++;
		(pix_now->prev)->next = pix_now;
		pix_now = pix_now->prev;
		if (mag.at<short>(i - 1, j + 1) > mag.at<short>(i, j + 1) &&
			mag.at<short>(i - 1, j + 1) > mag.at<short>(i + 1, j + 1))
			i--;
		else
		{
			if (mag.at<short>(i + 1, j + 1) > mag.at<short>(i, j + 1) &&
				mag.at<short>(i + 1, j + 1) > mag.at<short>(i - 1, j + 1))
				i++;
		}
		j++;
	}
}

void godown1(int &i, int &j, int &k, Mat mag, Mat &edge, Mat dir, link_pix* &pix_now)
{
	if (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		int x = i - 1;
		int y = j;
		if (mag.at<short>(i - 1, j - 1) > mag.at<short>(i - 1, j) &&
			mag.at<short>(i - 1, j - 1) > mag.at<short>(i - 1, j + 1))
			y = j - 1;
		else
		{
			if (mag.at<short>(i - 1, j + 1) > mag.at<short>(i - 1, j) &&
				mag.at<short>(i - 1, j + 1) > mag.at<short>(i - 1, j - 1))
				y = j + 1;
		}
		if (mag.at<short>(x, y) > 0 && edge.at<uchar>(x, y) != 255)
			edge.at<uchar>(x, y) = 128;
	}
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 255)
	{
		edge.at<uchar>(i, j) = 255;
		pix_now->x = i;
		pix_now->y = j;
		pix_now->next = (link_pix*)malloc(sizeof(link_pix));
		k++;
		(pix_now->next)->prev = pix_now;
		pix_now = pix_now->next;
		if (mag.at<short>(i + 1, j - 1) > mag.at<short>(i + 1, j) &&
			mag.at<short>(i + 1, j - 1) > mag.at<short>(i + 1, j + 1))
			j--;
		else
		{
			if (mag.at<short>(i + 1, j + 1) > mag.at<short>(i + 1, j) &&
				mag.at<short>(i + 1, j + 1) > mag.at<short>(i + 1, j - 1))
				j++;
		}
		i++;
	}
}

void godown2(int &i, int &j, int &k, Mat mag, Mat &edge, Mat dir, link_pix* &pix_now)
{
	if (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		int x = i - 1;
		int y = j;
		if (mag.at<short>(i - 1, j - 1) > mag.at<short>(i - 1, j) &&
			mag.at<short>(i - 1, j - 1) > mag.at<short>(i - 1, j + 1))
			y = j - 1;
		else
		{
			if (mag.at<short>(i - 1, j + 1) > mag.at<short>(i - 1, j) &&
				mag.at<short>(i - 1, j + 1) > mag.at<short>(i - 1, j - 1))
				y = j + 1;
		}
		if (mag.at<short>(x, y) > 0 && edge.at<uchar>(x, y) != 255)
			edge.at<uchar>(x, y) = 128;
	}
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 255)
	{
		edge.at<uchar>(i, j) = 255;
		pix_now->x = i;
		pix_now->y = j;
		pix_now->prev = (link_pix*)malloc(sizeof(link_pix));
		k++;
		(pix_now->prev)->next = pix_now;
		pix_now = pix_now->prev;
		if (mag.at<short>(i + 1, j - 1) > mag.at<short>(i + 1, j) &&
			mag.at<short>(i + 1, j - 1) > mag.at<short>(i + 1, j + 1))
			j--;
		else
		{
			if (mag.at<short>(i + 1, j + 1) > mag.at<short>(i + 1, j) &&
				mag.at<short>(i + 1, j + 1) > mag.at<short>(i + 1, j - 1))
				j++;
		}
		i++;
	}
}

void goup1(int &i, int &j, int &k, Mat mag, Mat &edge, Mat dir, link_pix* &pix_now)
{
	if (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		int x = i + 1;
		int y = j;
		if (mag.at<short>(i + 1, j - 1) > mag.at<short>(i + 1, j) &&
			mag.at<short>(i + 1, j - 1) > mag.at<short>(i + 1, j + 1))
			y = j - 1;
		else
		{
			if (mag.at<short>(i + 1, j + 1) > mag.at<short>(i + 1, j) &&
				mag.at<short>(i + 1, j + 1) > mag.at<short>(i + 1, j - 1))
				y = j + 1;
		}
		if (mag.at<short>(x, y) > 0 && edge.at<uchar>(x, y) != 255)
			edge.at<uchar>(x, y) = 128;
	}
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 255)
	{
		edge.at<uchar>(i, j) = 255;
		pix_now->x = i;
		pix_now->y = j;
		pix_now->next = (link_pix*)malloc(sizeof(link_pix));
		k++;
		(pix_now->next)->prev = pix_now;
		pix_now = pix_now->next;
		if (mag.at<short>(i - 1, j - 1) > mag.at<short>(i - 1, j) &&
			mag.at<short>(i - 1, j - 1) > mag.at<short>(i - 1, j + 1))
			j--;
		else
		{
			if (mag.at<short>(i - 1, j + 1) > mag.at<short>(i - 1, j) &&
				mag.at<short>(i - 1, j + 1) > mag.at<short>(i - 1, j - 1))
				j++;
		}
		i--;
	}
}

void goup2(int &i, int &j, int &k, Mat mag, Mat &edge, Mat dir, link_pix* &pix_now)
{
	if (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 0)
	{
		int x = i + 1;
		int y = j;
		if (mag.at<short>(i + 1, j - 1) > mag.at<short>(i + 1, j) &&
			mag.at<short>(i + 1, j - 1) > mag.at<short>(i + 1, j + 1))
			y = j - 1;
		else
		{
			if (mag.at<short>(i + 1, j + 1) > mag.at<short>(i + 1, j) &&
				mag.at<short>(i + 1, j + 1) > mag.at<short>(i + 1, j - 1))
				y = j + 1;
		}
		if (mag.at<short>(x, y) > 0 && edge.at<uchar>(x, y) != 255)
			edge.at<uchar>(x, y) = 128;
	}
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255 && dir.at<uchar>(i, j) == 255)
	{
		edge.at<uchar>(i, j) = 255;
		pix_now->x = i;
		pix_now->y = j;
		pix_now->prev = (link_pix*)malloc(sizeof(link_pix));
		k++;
		(pix_now->prev)->next = pix_now;
		pix_now = pix_now->prev;
		if (mag.at<short>(i - 1, j - 1) > mag.at<short>(i - 1, j) &&
			mag.at<short>(i - 1, j - 1) > mag.at<short>(i - 1, j + 1))
			j--;
		else
		{
			if (mag.at<short>(i - 1, j + 1) > mag.at<short>(i - 1, j) &&
				mag.at<short>(i - 1, j + 1) > mag.at<short>(i - 1, j - 1))
				j++;
		}
		i--;
	}
}

int rout(int i, int j, Mat mag, Mat &edge, Mat dir, link_pix* &pix_now, const link_seg* seg_now)
{
	int k = 0;
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255)
	{
		goleft1(i, j, k, mag, edge, dir, pix_now);                //先向左走
		if (k>1 && i == pix_now->prev->prev->x && j == pix_now->prev->prev->y)
		{
			pix_now = pix_now->prev;
			i = pix_now->x;
			j = pix_now->y;
			pix_now->next = NULL;
			edge.at<uchar>(i, j) = 128;
			goright1(i, j, k, mag, edge, dir, pix_now);
		}
		godown1(i, j, k, mag, edge, dir, pix_now);
		if (k>1 && i == pix_now->prev->prev->x && j == pix_now->prev->prev->y)
		{
			pix_now = pix_now->prev;
			i = pix_now->x;
			j = pix_now->y;
			pix_now->next = NULL;
			edge.at<uchar>(i, j) = 128;
			goup1(i, j, k, mag, edge, dir, pix_now);
		}
		goright1(i, j, k, mag, edge, dir, pix_now);
		if (k>1 && i == pix_now->prev->prev->x && j == pix_now->prev->prev->y)
		{
			pix_now = pix_now->prev;
			i = pix_now->x;
			j = pix_now->y;
			pix_now->next = NULL;
			edge.at<uchar>(i, j) = 128;
			goleft1(i, j, k, mag, edge, dir, pix_now);
		}
		goup1(i, j, k, mag, edge, dir, pix_now);
		if (k>1 && i == pix_now->prev->prev->x && j == pix_now->prev->prev->y)
		{
			pix_now = pix_now->prev;
			i = pix_now->x;
			j = pix_now->y;
			pix_now->next = NULL;
			edge.at<uchar>(i, j) = 128;
			godown1(i, j, k, mag, edge, dir, pix_now);
		}
	}
	pix_now = pix_now->prev;
	pix_now->ifend = 0;                                     //走到头，即边缘的最后一个点
	link_pix* end = pix_now;
	pix_now = seg_now->addr;                                //回到开始的点
	i = pix_now->x;
	j = pix_now->y;
	edge.at<uchar>(i, j) = 128;
	while (mag.at<short>(i, j) > 0 && edge.at<uchar>(i, j) != 255)
	{
		goleft2(i, j, k, mag, edge, dir, pix_now);                //先向左走
		if (k>1 && i == pix_now->next->next->x && j == pix_now->next->next->y)
		{
			pix_now = pix_now->next;
			i = pix_now->x;
			j = pix_now->y;
			pix_now->prev = NULL;
			edge.at<uchar>(i, j) = 128;
			goright2(i, j, k, mag, edge, dir, pix_now);
		}
		godown2(i, j, k, mag, edge, dir, pix_now);
		if (k>1 && i == pix_now->next->next->x && j == pix_now->next->next->y)
		{
			pix_now = pix_now->next;
			i = pix_now->x;
			j = pix_now->y;
			pix_now->prev = NULL;
			edge.at<uchar>(i, j) = 128;
			goup2(i, j, k, mag, edge, dir, pix_now);
		}
		goright2(i, j, k, mag, edge, dir, pix_now);
		if (k>1 && i == pix_now->next->next->x && j == pix_now->next->next->y)
		{
			pix_now = pix_now->next;
			i = pix_now->x;
			j = pix_now->y;
			pix_now->prev = NULL;
			edge.at<uchar>(i, j) = 128;
			goleft2(i, j, k, mag, edge, dir, pix_now);
		}
		goup2(i, j, k, mag, edge, dir, pix_now);
		if (k>1 && i == pix_now->next->next->x && j == pix_now->next->next->y)
		{
			pix_now = pix_now->next;
			i = pix_now->x;
			j = pix_now->y;
			pix_now->prev = NULL;
			edge.at<uchar>(i, j) = 128;
			godown2(i, j, k, mag, edge, dir, pix_now);
		}
	}
	pix_now = pix_now->next;
	pix_now->ifend = 0;
	pix_now->prev = end;
	end->next = pix_now;

	return(k);
}

float getdistance(Point p1, Point p2)
{
	return(sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)));
}

void leastsquareslinefit(vector<link_pix*>::iterator pix_chain_iter, int n, Vec4f &line, float &r)
{
	r = 0;
	vector<float> x, y;
	float X = 0;
	float Y = 0;
	for (int i = 0; i < n; i++)
	{
		x.push_back(pix_chain_iter[i]->x);
		y.push_back(pix_chain_iter[i]->y);
		X = X + x[i] / n;
		Y = Y + y[i] / n;
	}

	//计算a，b，r

	float sum1 = 0, sum2 = 0;
	for (int i = 0; i < n; i++)
	{
		sum1 = sum1 + (X - x[i]) * (Y - y[i]);
		sum2 = sum2 + (X - x[i]) * (X - x[i]);
	}

	if (sum2 == 0)                                      //如果直线垂直，即斜率不存在
	{
		line[0] = line[1] = X;
		line[2] = 0;
		r = 0;
		if (y[0] < y[n - 1])
			line[3] = 1;
		else
			line[3] = -1;
		for (int i = 0; i < n; i++)
			r = r + (x[i] - X) * (x[i] - X);
	}
	else                                                   //如果斜率存在
	{
		line[0] = sum1 / sum2;
		line[1] = Y - line[0] * X;
		if (x[0] < x[n - 1])
		{
			line[2] = 1 / sqrt(1 + line[0] * line[0]);
			line[3] = line[0] / sqrt(1 + line[0] * line[0]);
		}
		else
		{
			line[2] = -1 / sqrt(1 + line[0] * line[0]);
			line[3] = -line[0] / sqrt(1 + line[0] * line[0]);
		}
		for (int i = 0; i < n; i++)
			r = r + (y[i] - line[0] * x[i] - line[1]) * (y[i] - line[0] * x[i] - line[1]);
	}
}

float ComputePointDistance2Line(link_line* line_now, link_pix* pix_now)
{
	int x = pix_now->x;
	int y = pix_now->y;
	float a = line_now->line[0];
	float b = line_now->line[1];
	if (line_now->line[2] != 0)
		return(abs(a*x - y + b) / sqrt(a*a + 1));
	else
		return(abs(x - line_now->line[0]));
}

void draw_line(vector<link_pix*>::iterator pix_chain_iter, int length, vector<link_line*> &line_chain)
{
	link_line* line_now = (link_line*)malloc(sizeof(link_line*));
	float r = 2;
	while (shortestline <= length)
	{
		r = 0;
		leastsquareslinefit(pix_chain_iter, shortestline, line_now->line, r);
		if (r < line_squares)break;
		pix_chain_iter++;
		length--;
	}
	if (r >= line_squares || shortestline > length)return;
	int linelen = shortestline;
	while (linelen < length)
	{
		float d = ComputePointDistance2Line(line_now, pix_chain_iter[linelen]);
		if (d > 1)break;
//		leastsquareslinefit(pix_chain_iter, linelen + 1, line_now->line, r);
//		if (r > 1)break;
		linelen++;
	}
	leastsquareslinefit(pix_chain_iter, linelen, line_now->line, r);
	line_now->pix_chain = new vector<link_pix*>;
	for (int i = 0; i < linelen; i++)
	{
		(*line_now->pix_chain).push_back(pix_chain_iter[i]);
	}
	line_chain.push_back(line_now);
	draw_line(pix_chain_iter + linelen, length - linelen, line_chain);
}

bool checkifarc(vector<link_line*>::iterator line_chain_iter)
{
	float a[2] = { line_chain_iter[0]->line[2], line_chain_iter[0]->line[3] };
	float b[2] = { line_chain_iter[1]->line[2], line_chain_iter[1]->line[3] };
	float c[2] = { line_chain_iter[2]->line[2], line_chain_iter[2]->line[3] };
	float inner_a_b = a[0] * b[0] + a[1] * b[1];
	float inner_b_c = b[0] * c[0] + b[1] * c[1];
	float outer_a_b = a[0] * b[1] - a[1] * b[0];
	float outer_b_c = b[0] * c[1] - b[1] * c[0];
	return((0.5 < inner_a_b) && (0.5 < inner_b_c) && (outer_a_b * outer_b_c > 0)
		&& (inner_a_b < 1) && (inner_b_c < 1));
}

void draw_arc(vector<link_line*>::iterator line_chain_iter, int length, vector<link_arc> &arc_list)
{
	bool ifarc = 0;
	while (3 <= length)
	{
		ifarc = checkifarc(line_chain_iter);
		if (ifarc == 1)break;
		line_chain_iter++;
		length--;
	}
	if (ifarc == 0)return;

	link_arc *arc = new link_arc;
	arc->pix_chain = new vector<link_pix*>;
	arc->line_chain = new vector<link_line*>;
	for (int i = 0; i < 3; i++)
	{
		vector<link_pix*>::iterator pix_end = (*line_chain_iter[i]->pix_chain).end();
		for (vector<link_pix*>::iterator pix_iter = (*line_chain_iter[i]->pix_chain).begin();
			pix_iter != pix_end; pix_iter++)
			(*arc->pix_chain).push_back(*pix_iter);
		(*arc->line_chain).push_back(line_chain_iter[i]);
	}

	int arclen = 3;
	while (arclen < length && checkifarc(line_chain_iter + arclen - 2))
	{
		arclen++;
		vector<link_pix*>::iterator pix_end = (*line_chain_iter[arclen - 1]->pix_chain).end();
		for (vector<link_pix*>::iterator pix_iter = (*line_chain_iter[arclen - 1]->pix_chain).begin();
			pix_iter != pix_end; pix_iter++)
			(*arc->pix_chain).push_back(*pix_iter);
		(*arc->line_chain).push_back(line_chain_iter[arclen - 1]);
	}
	arc_list.push_back(*arc);
	draw_arc(line_chain_iter + arclen, length - arclen, arc_list);
}

/*
void compute_angle(double &angle, double k, double a)
{
angle = angle + a;
double cha = sin(angle) - angle*k;
if (cha < 0.1)return;
if (cha>0)
a = a*pi / 2;
else
a = -a*pi / 2;
compute_angle(angle, k, a);
}
*/

void compute_circle_centre(vector<link_pix*>::iterator pix_iter, int n, float &r, float &R, Point &o)
{
	vector<int> x, y;
	float X = 0.0;
	float Y = 0.0;

	for (int i = 0; i < n; i++)
	{
		x.push_back(pix_iter[i]->x);
		y.push_back(pix_iter[i]->y);
		X = X + (float)pix_iter[i]->x;
		Y = Y + (float)pix_iter[i]->y;
	}

	X = X / n;
	Y = Y / n;
	vector<float> u, v;
	for (int i = 0; i<n; i++)
	{
		u.push_back(x[i] - X);
		v.push_back(y[i] - Y);
	}
	float Suu = 0, Svv = 0, Suv = 0, Suvv = 0, Svuu = 0, Suuu = 0, Svvv = 0;
	for (int i = 0; i<n; i++)
	{
		Suu = Suu + u[i] * u[i];
		Svv = Svv + v[i] * v[i];
		Suv = Suv + u[i] * v[i];
		Suvv = Suvv + u[i] * v[i] * v[i];
		Svuu = Svuu + v[i] * u[i] * u[i];
		Suuu = Suuu + u[i] * u[i] * u[i];
		Svvv = Svvv + v[i] * v[i] * v[i];
	}
	float uc = 0.5*(Suuu*Svv + Suvv*Svv - Svvv*Suv - Svuu*Suv) / (Suu*Svv - Suv*Suv);
	float vc = (0.5*(Suuu + Suvv) - uc*Suu) / Suv;
	if (Suv == 0 || (Suu*Svv - Suv*Suv) == 0)
	{
		r = 10000;
		return;
	}
	o.y = X + uc;
	o.x = Y + vc;
	R = sqrt(uc*uc + vc*vc + (Suu + Svv) / n);
	r = 0;
	for (int i = 0; i<n; i++)
		r = r + (u[i] - uc)*(u[i] - uc) + (v[i] - vc)*(v[i] - vc) + R*R - 2 * R*sqrt((u[i] - uc)*(u[i] - uc) + (v[i] - vc)*(v[i] - vc));
	r = sqrt(r / n);
}

void select_arc(vector<link_arc>::iterator arc_candi_iter, vector<link_line*>::iterator line_iter, vector<link_pix*>::iterator pix_iter, int line_num, vector<link_arc> &arc_list)
{
	if (line_num < 3)return;
	float r = 0;                                              //整个计算，符合则直接导入圆弧列表
	int n = 0;
	if (arc_candi_iter->line_chain->begin() == line_iter)
	{
		for (int i = 0; i < line_num; i++)
			n = n + (*line_iter[i]->pix_chain).size();
		compute_circle_centre(pix_iter, n, r, arc_candi_iter->r, arc_candi_iter->o);
		if (r < arc_squares)
		{
			arc_list.push_back(*arc_candi_iter);
			return;
		}
	}

	link_arc* arc = new link_arc;                                  //否则从前三个开始算
	arc->pix_chain = new vector<link_pix*>;
	arc->line_chain = new vector<link_line*>;
	float R = 0;
	Point o = (0, 0);

	while (3 <= line_num)
	{
		n = 0;
		for (int i = 0; i < 3; i++)
			n = n + (*line_iter[i]->pix_chain).size();
		compute_circle_centre(pix_iter, n, r, R, o);
		if (r < arc_squares)break;
		line_num--;
		line_iter++;
		pix_iter++;
	}
	if (r >= arc_squares)return;
	int arc_length = 3;
	arc->o = o;
	arc->r = R;
	while (arc_length < line_num)
	{
		n = n + (*line_iter[arc_length]->pix_chain).size();
		compute_circle_centre(pix_iter, n, r, R, o);
		if (r < arc_squares)
		{
			arc->o = o;
			arc->r = R;
			arc_length++;
		}
		else
			break;
	}
	for (int i = 0; i < arc_length; i++)
	{
		(*arc->line_chain).push_back(line_iter[i]);
		vector<link_pix*>::iterator pix_end = (*line_iter[i]->pix_chain).end();
		for (vector<link_pix*>::iterator pix_iter = (*line_iter[i]->pix_chain).begin(); pix_iter != pix_end; pix_iter++)
			(*arc->pix_chain).push_back(*pix_iter);
	}
	arc_list.push_back(*arc);
	select_arc(arc_candi_iter, line_iter + arc_length, pix_iter + arc_length, line_num - arc_length, arc_list);
}

void paixu(vector<link_arc> &arc_list)
{
	if (arc_list.size() <= 1)return;
	vector<link_arc> left, right;
	vector<link_arc>::size_type length = arc_list[0].pix_chain->size();
	vector<link_arc>::iterator end = arc_list.end();
	for (vector<link_arc>::iterator iter = arc_list.begin() + 1; iter != end; iter++)
	{
		if (iter->pix_chain->size() > length)
			left.push_back(*iter);
		else
			right.push_back(*iter);
	}
	paixu(left);
	paixu(right);
	left.push_back(arc_list[0]);
	left.insert(left.end(), right.begin(), right.end());
	arc_list = left;
}

vector<link_arc>::iterator find_the_nearest(Point p1, Point p2, vector<link_arc>::iterator arc_iter, vector<link_arc>::iterator arc_end)
{
	vector<link_arc>::iterator iter;
	Point p3, p4;
	float distance = 10000.0;
	float d = 0.0;
	for (arc_iter; arc_iter != arc_end; arc_iter++)
	{
		p3.x = (*arc_iter[0].pix_chain->begin())->x;
		p3.y = (*arc_iter[0].pix_chain->begin())->y;
		p4.x = (*(arc_iter[0].pix_chain->end() - 1))->x;
		p4.y = (*(arc_iter[0].pix_chain->end() - 1))->y;
		if (d = getdistance(p1,p3) < distance)
		{
			distance = d;
			iter = arc_iter;
		}
		if (d = getdistance(p1, p4) < distance)
		{
			distance = d;
			iter = arc_iter;
		}
		if (d = getdistance(p2, p3) < distance)
		{
			distance = d;
			iter = arc_iter;
		}
		if (d = getdistance(p2, p4) < distance)
		{
			distance = d;
			iter = arc_iter;
		}
	}
	return(iter);
}

void updata_the_end(Point &p1, Point &p2, vector<link_arc>::iterator arc_iter)
{
	Point p3, p4;
	p3.x = (*arc_iter[0].pix_chain->begin())->x;
	p3.y = (*arc_iter[0].pix_chain->begin())->y;
	p4.x = (*(arc_iter[0].pix_chain->end() - 1))->x;
	p4.y = (*(arc_iter[0].pix_chain->end() - 1))->y;
	float d = getdistance(p1, p3);
	int i = 1;
	if (getdistance(p1, p4) < d)
	{
		d = getdistance(p1, p4);
		i = 2;
	}
	if (getdistance(p2, p3) < d)
	{
		d = getdistance(p2, p3);
		i = 3;
	}
	if (getdistance(p2, p4) < d)
	{
		d = getdistance(p2, p4);
		i = 4;
	}
	switch (i)
	{
	case 1:p1 = p4;
	case 2:p1 = p3;
	case 3:p2 = p4;
	case 4:p2 = p3;
	default:
		break;
	} 
}

void ExtendArcToCircle(vector<link_arc> arc_list, vector<link_circle>& circle_list)
{
	while (arc_list.size() != 0)
	{
		//创建一个新圆
		link_circle circle;
		circle.arc_chain = new vector<link_arc>;
		circle.pix_chain = new vector<link_pix*>;
		vector<link_arc>::iterator arc_iter = arc_list.begin();
		vector<link_arc>::iterator arc_end = arc_list.end();
		vector<link_arc> arc_copy_list;
		vector<link_pix*> pix_chain;
		Point o;
		float r = 0;
		float R = 0;
		Point p1, p2;
		p1.x = (*arc_list[0].pix_chain->begin())->x;
		p1.y = (*arc_list[0].pix_chain->begin())->y;
		p2.x = (*(arc_list[0].pix_chain->end() - 1))->x;
		p2.y = (*(arc_list[0].pix_chain->end() - 1))->y;
		//先把第一个圆弧放进去
		pix_chain.insert(pix_chain.end(), (*arc_iter->pix_chain).begin(), (*arc_iter->pix_chain).end());
		compute_circle_centre(pix_chain.begin(), pix_chain.size(), r, R, o);
		circle.arc_chain->push_back(*arc_iter);
		(*circle.pix_chain) = pix_chain;
		circle.o = o;
		circle.r = R;
		arc_list.erase(arc_iter);
		arc_iter = arc_list.begin();
		arc_end = arc_list.end();
		//从剩下的圆弧中依次找到最近的连接
		while (arc_list.size()!=0)
		{
			arc_iter = find_the_nearest(p1, p2, arc_iter, arc_end);
			pix_chain.insert(pix_chain.end(), (*arc_iter->pix_chain).begin(), (*arc_iter->pix_chain).end());
			compute_circle_centre(pix_chain.begin(), pix_chain.size(), r, R, o);
			if (r < 1.5)
			{
				circle.arc_chain->push_back(*arc_iter);
				(*circle.pix_chain) = pix_chain;
				circle.o = o;
				circle.r = R;
				updata_the_end(p1, p2, arc_iter);
				arc_list.erase(arc_iter);
				arc_iter = arc_list.begin();
				arc_end = arc_list.end();
			}
			else
			{
				pix_chain = (*circle.pix_chain);
				arc_copy_list.push_back(*arc_iter);
				arc_list.erase(arc_iter);
				arc_iter = arc_list.begin();
				arc_end = arc_list.end();
			}
		}	
		circle_list.push_back(circle);
		arc_list = arc_copy_list;
		arc_copy_list.clear();
	}
}

void compute_ellips(vector<link_pix*> pix_chain, float* a, float &r)
{
	float c[36] = {0, 0, 2, 0, 0, 0, 0, -1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Mat C(6, 6, CV_32FC1, c);
	int pix_num = pix_chain.size();
	Mat D(pix_num, 6, CV_32FC1);
	vector<vector<float> > X;
	
	for (int i = 0; i < pix_num;i++)
	{
		float x = (*pix_chain[i]).x/10;
		float y = (*pix_chain[i]).y/10;
		float* data = D.ptr<float>(i);
		data[0] = x*x;
		data[1] = x*y;
		data[2] = y*y;
		data[3] = x;
		data[4] = y;
		data[5] = 1;
		vector<float> X_x;
		X_x.push_back(x*x);
		X_x.push_back(x*y);
		X_x.push_back(y*y);
		X_x.push_back(x);
		X_x.push_back(y);
		X_x.push_back(1);
		X.push_back(X_x);
	}
//	cout << D << endl;
	Mat DT(6, pix_num, CV_32FC1);
	Mat S(6, 6, CV_32FC1);
	transpose(D, DT);
	S = DT*D;
	Mat A(6, 6, CV_32FC1);
	Mat S_invert;
	invert(S, S_invert);
	A = S_invert*C;

	Mat vec(6, 6, CV_32FC1);
	Mat val(6, 1, CV_32FC1);
	eigen(A, val, vec);
	cout << S << endl << S_invert << endl << " " << A << endl << val << endl << vec << endl;
	system("pause");
	for (int i = 0; i < 6; i++)
	{
		if (val.at<float>(i,0)>0)
		{
			for (int j = 0; j < 6; j++)
			{
				a[j] = vec.at<float>(i, j);
			}
			break;
		}
	}

	float u = sqrt(1 / (4 * a[0] * a[2] - a[1] * a[1]));
	for (int i = 0; i < 6; i++)
	{
		a[i] = a[i] * u;
	}

	r = 0;
	for (int i = 0; i < pix_num; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			r = r + a[j] * X[i][j];
		}
	}
}

void ExtendArcToEllips(vector<link_arc> arc_list, vector<link_ellips>& ellips_list)
{
	while (arc_list.size() != 0)
	{
		//创建一个新圆
		link_ellips ellips;
		ellips.arc_chain = new vector<link_arc>;
		ellips.pix_chain = new vector<link_pix*>;
		vector<link_arc>::iterator arc_iter = arc_list.begin();
		vector<link_arc>::iterator arc_end = arc_list.end();
		vector<link_arc> arc_copy_list;
		vector<link_pix*> pix_chain;
		float r = 0;
		float A[6] = { 0 };

		//记录圆弧的两个端点
		Point p1, p2;
		p1.x = (*arc_list[0].pix_chain->begin())->x;
		p1.y = (*arc_list[0].pix_chain->begin())->y;
		p2.x = (*(arc_list[0].pix_chain->end() - 1))->x;
		p2.y = (*(arc_list[0].pix_chain->end() - 1))->y;
		//先把第一个圆弧放进去
		pix_chain.insert(pix_chain.end(), (*arc_iter->pix_chain).begin(), (*arc_iter->pix_chain).end());
		compute_ellips(pix_chain, ellips.A, r);
		ellips.arc_chain->push_back(*arc_iter);
		(*ellips.pix_chain) = pix_chain;
		//去掉第一个圆弧
		arc_list.erase(arc_iter);
		arc_iter = arc_list.begin();
		arc_end = arc_list.end();
		//从剩下的圆弧中依次找到最近的连接
		while (arc_list.size() != 0)
		{
			arc_iter = find_the_nearest(p1, p2, arc_iter, arc_end);
			pix_chain.insert(pix_chain.end(), (*arc_iter->pix_chain).begin(), (*arc_iter->pix_chain).end());
			compute_ellips(pix_chain, A, r);
			if (r < 1.5)
			{
				ellips.arc_chain->push_back(*arc_iter);
				(*ellips.pix_chain) = pix_chain;
				for (int i = 0; i < 6; i++)
				{
					ellips.A[i] = A[i];
				}
				updata_the_end(p1, p2, arc_iter);
				arc_list.erase(arc_iter);
				arc_iter = arc_list.begin();
				arc_end = arc_list.end();
			}
			else
			{
				pix_chain = (*ellips.pix_chain);
				arc_copy_list.push_back(*arc_iter);
				arc_list.erase(arc_iter);
				arc_iter = arc_list.begin();
				arc_end = arc_list.end();
			}
		}
		ellips_list.push_back(ellips);
		arc_list = arc_copy_list;
		arc_copy_list.clear();
	}
}

double jiecheng(int n)
{
	if (n == 1)
		return(1);
	else
		return(n*jiecheng(n - 1));
}

double collect(int n, int i)
{
	double result = 1;
	if (i > n / 2)
		i = n - i;
	if (i == 0 || i == n)
		return(1);
	for (int j = i; j > 0; j--)
		result = result*n--;
	result = result / jiecheng(i);
	return(result);
}

float helmholtz(vector<link_circle>::iterator circle_list_iter, Mat sobelx, Mat sobely)
{
	int k = 0;
	int n = 0;
	int N = sobelx.rows;
	double alpha = 0;
	Mat black(sobelx.size(), CV_8U, Scalar(0));
	circle(black, circle_list_iter->o, circle_list_iter->r, Scalar(255));
	
	for (int i = 0; i < sobelx.rows; i++)
	{
		for (int j = 0; j < sobelx.cols; j++)
		{
			if (black.at<uchar>(i, j) == 255)
			{
				n++;
				float t1 = 0;
				if (sobelx.at<short>(i, j) != 0)
					t1 = atan(sobely.at<short>(i, j) / sobelx.at<short>(i, j)) / pi * 180;
				else
					t1 = 90;
				float t2 = 0;
				if (j != circle_list_iter->o.x)
					t2 = atan((i - circle_list_iter->o.y) / (j - circle_list_iter->o.x)) / pi * 180;
				else
					t2 = 90;
				if (abs(t1 - t2) < 22.5)
				{
					k++;
				}
			}
		}
	}
	for (int i = k; i <= n; i++)
	{
//		alpha = collect(n, i);
		alpha = alpha +collect(n,i)*pow(0.125, i)*pow(0.875, n - i);
	}
	alpha = alpha*pow(N, 5);
	return(alpha);
}

/****************************主函数*************************************************/

int _tmain(int argc, _TCHAR* argv[])
{
	//读图片
	Mat image = imread("D://yuan.jpg", 1);
	shortestline = log(image.rows);
	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);
	imshow("image", gray);
	waitKey(0);

	/******************用EDPF提取边缘*********************/

	//高斯平滑
	GaussianBlur(gray, gray, Size(5, 5), 1);

	//计算梯度
	Mat sobelx, sobely;
	Sobel(gray, sobelx, CV_16S, 1, 0, 3);
	Sobel(gray, sobely, CV_16S, 0, 1, 3);

	//梯度方向
	Mat dir(image.size(), CV_8U);
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (abs(sobelx.at<short>(i, j)) > abs(sobely.at<short>(i, j)))
				dir.at<uchar>(i, j) = 255;
			else
				dir.at<uchar>(i, j) = 0;
		}
	}

	//梯度大小
	Mat mag = abs(sobelx) + abs(sobely);
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (mag.at<short>(i, j) < mag_threshold)
				mag.at<short>(i, j) = 0;
		}
	}
	rectangle(mag, Point(0, 0), Point(mag.cols, mag.rows), Scalar(0), 2, 8);

	//寻找锚点
	Mat edge(image.size(), CV_8U, Scalar(0));
	for (int i = 1; i < mag.rows - 1; i = i + 1)
	{
		for (int j = 1; j < mag.cols - 1; j = j + 1)
		{
			if (dir.at<uchar>(i, j) == 255)              //纵向边缘
			{
				if (mag.at<short>(i, j) > mag.at<short>(i, j - 1) + 3
					&& mag.at<short>(i, j) > mag.at<short>(i, j + 1) + 3)
					edge.at<uchar>(i, j) = 128;
			}
			else                                        //横向边缘
			{
				if (mag.at<short>(i, j) > mag.at<short>(i - 1, j) + 3
					&& mag.at<short>(i, j) > mag.at<short>(i + 1, j) + 3)
					edge.at<uchar>(i, j) = 128;
			}
		}
	}

	//连接锚点，画边缘

	link_seg* seg_head, *seg_now;
	link_pix* pix_now = NULL;

	//创建边缘集合链表的表头
	seg_head = seg_now = (link_seg*)malloc(sizeof(link_seg));
	seg_now->next = (link_seg*)malloc(sizeof(link_seg));            //创建新的边缘
	seg_now = seg_now->next;                                     //指针指过去
	seg_now->next = NULL;

	//自动描绘边缘
	for (int i = 1; i < mag.rows; i++)
	{
		for (int j = 1; j < mag.cols; j++)
		{
			if (edge.at<uchar>(i, j) == 128)                      //如果找到了锚点则进行下一步
			{
				seg_now->addr = (link_pix*)malloc(sizeof(link_pix));  //创建该边缘中的第一个像素点
				pix_now = seg_now->addr;                           //像素指针指向刚才创建的第一个点

				int k = 0;
				k = rout(i, j, mag, edge, dir, pix_now, seg_now);
				if (k > shortestline)
				{
					seg_now->addr = pix_now;
					seg_now->next = (link_seg*)malloc(sizeof(link_seg));            //创建新的边缘
					seg_now = seg_now->next;                                     //指针指过去
					seg_now->next = NULL;
					seg_now->addr = NULL;
				}
				else
				{
					seg_now->addr = NULL;
				}
			}
		}
	}
	

	//转化为vector

	Mat BGR_edge(mag.size(), CV_8UC3, Scalar(0));
	seg_now = seg_head->next;
	for (; seg_now->addr != NULL; seg_now = seg_now->next)
	{
		vector<link_pix*>* pix_chain = new vector<link_pix*>;
		pix_now = seg_now->addr;
		(*pix_chain).push_back(pix_now);
		BGR_edge.at<Vec3b>(pix_now->x, pix_now->y)[2] = 255;
		do{
			pix_now = pix_now->next;
			(*pix_chain).push_back(pix_now);
			BGR_edge.at<Vec3b>(pix_now->x, pix_now->y)[2] = 255;
		} while (pix_now->ifend != 0);
		seg_now->pix_chain = pix_chain;
	}

	imshow("image_edge", edge);           //显示边缘
	waitKey(0);

	/***************************拆分成直线**************************************/

	seg_now = seg_head->next;
	while (seg_now->addr != NULL)
	{
		(seg_now->line_chain) = new vector<link_line*>;
		draw_line((*seg_now->pix_chain).begin(), (*seg_now->pix_chain).size(), *seg_now->line_chain);
		//		seg_now->line_chain = line_chain;
		seg_now = seg_now->next;
	}

	/***************************连接成圆弧************************************/

	vector<link_arc> arc_list;
	seg_now = seg_head->next;
	while (seg_now->addr != NULL)
	{
		draw_arc((*seg_now->line_chain).begin(), (*seg_now->line_chain).size(), arc_list);
		seg_now = seg_now->next;
	}

	BGR_edge = 0;
	for (vector<link_arc>::iterator arc_iter = arc_list.begin(); arc_iter != arc_list.end(); arc_iter++)
	{
		vector<link_pix*>::iterator pix_end = (*arc_iter->pix_chain).end();
		for (vector<link_pix*>::iterator pix_iter = (*arc_iter->pix_chain).begin()
			; pix_iter != pix_end; pix_iter++)
			BGR_edge.at<Vec3b>((*pix_iter)->x, (*pix_iter)->y)[2] = 255;
	}
	imshow("arc_candi", BGR_edge);                          //显示可能的圆弧
	waitKey(0);

	/****************************判断是否是圆弧****************************************************/

	vector<link_arc> arc_candi_list = arc_list;
	arc_list.clear();

	vector<link_arc>::iterator arc_candi_list_iter = arc_candi_list.begin();
	vector<link_arc>::iterator arc_candi_list_end = arc_candi_list.end();
	for (; arc_candi_list_iter != arc_candi_list_end; arc_candi_list_iter++)
		select_arc(arc_candi_list_iter, (*arc_candi_list_iter->line_chain).begin(), (*arc_candi_list_iter->pix_chain).begin(),
		(*arc_candi_list_iter->line_chain).size(), arc_list);


	BGR_edge = 0;
	for (vector<link_arc>::iterator arc_iter = arc_list.begin(); arc_iter != arc_list.end(); arc_iter++)
	{
		vector<link_pix*>::iterator pix_end = (*arc_iter->pix_chain).end();
		for (vector<link_pix*>::iterator pix_iter = (*arc_iter->pix_chain).begin(); pix_iter != pix_end; pix_iter++)
			BGR_edge.at<Vec3b>((*pix_iter)->x, (*pix_iter)->y)[2] = 255;
	}
	imshow("arc", BGR_edge);                                  //显示确定的圆弧
	waitKey(0);

	/*******************************组成圆或椭圆************************************************/

	paixu(arc_list);
	vector<link_circle> circle_list;
	while (arc_list.size() != 0)
	{
		link_circle circle;                                            //创建一个新圆
		circle.arc_chain = new vector<link_arc>;
		circle.pix_chain = new vector<link_pix*>;
		float radius = arc_list[0].r;                                   //记录第一个圆弧的圆心和半径
		Point center = arc_list[0].o;

		vector<link_arc>::iterator iter = arc_list.begin();
		(*circle.arc_chain).push_back(*iter);                           //将第一个圆弧插入新建的圆的第一个位置
		iter = arc_list.erase(iter);                                          //将已插入的圆弧删除		
		vector<link_arc>::iterator end = arc_list.end();

		//将可能属于一个圆的圆弧放在一起
		while (iter != end)
		{
			if (pow(iter->o.x - center.x, 2) + pow(iter->o.y - center.y, 2) < radius*radius*0.0625 && abs(iter->r - radius) < radius*0.25)
			{
				(*circle.arc_chain).push_back(*iter);
				iter = arc_list.erase(iter);
				end = arc_list.end();
			}
			else
				iter++;
		}
		circle_list.push_back(circle);
	}

	//延长圆弧，逐一连接
	vector<link_circle> circle_candi_list = circle_list;
	circle_list.clear();
	vector<link_circle>::iterator circle_candi_list_iter = circle_candi_list.begin();
	vector<link_circle>::iterator circle_candi_list_end = circle_candi_list.end();
	for (; circle_candi_list_iter != circle_candi_list_end; circle_candi_list_iter++)
	{
		ExtendArcToCircle((*circle_candi_list_iter->arc_chain), circle_list);
	}

	//delete the circle which is too short

	vector<link_circle>::iterator circle_list_iter = circle_list.begin();
	vector<link_circle>::iterator circle_list_end = circle_list.end();

	for (; circle_list_iter != circle_list_end;)
	{
		if (circle_list_iter->pix_chain->size() < pi*0.75*circle_list_iter->r)                  //不够半圈，放回到圆弧列表，下一步找椭圆
		{
			arc_list.insert(arc_list.end(), circle_list_iter->arc_chain->begin(), circle_list_iter->arc_chain->end());
			circle_list_iter = circle_list.erase(circle_list_iter);
			circle_list_end = circle_list.end();
		}
		else
		{
			circle_list_iter++;
		}
	}

	/***************************剩下的连成椭圆******************************************/
/*	paixu(arc_list);
	vector<link_ellips> ellips_candi_list;
	while (arc_list.size() != 0)
	{
		link_ellips ellips;                                            //创建一个新椭圆
		ellips.arc_chain = new vector<link_arc>;
		ellips.pix_chain = new vector<link_pix*>;
		float radius = arc_list[0].r;                                   //记录第一个圆弧的圆心和半径
		Point center = arc_list[0].o;

		vector<link_arc>::iterator iter = arc_list.begin();
		(*ellips.arc_chain).push_back(*iter);                           //将第一个圆弧插入新建的椭圆的第一个位置
		iter = arc_list.erase(iter);                                          //将已插入的圆弧删除		
		vector<link_arc>::iterator end = arc_list.end();

		//将可能属于一个椭圆的圆弧放在一起
		while (iter != end)
		{
			if (pow(iter->o.x - center.x, 2) + pow(iter->o.y - center.y, 2) < radius*radius*0.25 && abs(iter->r - radius) < radius*0.5)
			{
				(*ellips.arc_chain).push_back(*iter);
				iter = arc_list.erase(iter);
				end = arc_list.end();
			}
			else
				iter++;
		}
		ellips_candi_list.push_back(ellips);
	}

	//extend the arc and fit a ellips
	vector<link_ellips> ellips_list;
	vector<link_ellips>::iterator ellips_candi_list_iter = ellips_candi_list.begin();
	vector<link_ellips>::iterator ellips_candi_list_end = ellips_candi_list.end();
	for (; ellips_candi_list_iter != ellips_candi_list_end; ellips_candi_list_iter++)
	{
		ExtendArcToEllips((*ellips_candi_list_iter->arc_chain), ellips_list);
	}
*/

	/**************************骇母霍兹原则检测***************************************/
	float alpha = 0;
	circle_list_iter = circle_list.begin();
	circle_list_end = circle_list.end();
	while (circle_list_iter != circle_list_end)
	{
		alpha = helmholtz(circle_list_iter,sobelx,sobely);
		if (alpha > 1)
		{
			circle_list_iter = circle_list.erase(circle_list_iter);
			circle_list_end = circle_list.end();
		}
		else
		{
			circle_list_iter++;
		}
	}
	

	circle_list_iter = circle_list.begin();
	for (; circle_list_iter != circle_list_end; circle_list_iter++)
	{
		circle(image, circle_list_iter->o, circle_list_iter->r, Scalar(0, 0, 255));
	}
	imshow("image_circle", image);
	waitKey(0);
	return 0;
}