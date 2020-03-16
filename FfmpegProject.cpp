#include <pch.h>
#define _CRT_SECURE_NO_WARNINGS 

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <video.h>
#include <imgproc.hpp>
#include <highgui.hpp>
#include <time.h>
#include <getQR.h>

using namespace cv;
using namespace std;
void Generate_bmp(char *str);
int num=0;

//将单个字符转为二进制
void tran(char ch, int *n)
{
	int x = ch,i;
	for (int j = 0; j < 8; j++)
	{
		i = x % 2; //所求二进制数组为倒序
		x = x / 2;
		n[j] = i;
	}
}

//将二进制转为字符
char detran(int *n)
{
	char ch;
	int m = 1, num = 0;
	//第八位恒为0，我将它处理成了随机数看着美观一点
	for (int j = 0; j < 7; j++)
	{
		num += n[j] * m;
		m *= 2;
	}
	ch = (char)num;
	return ch;
}
//与下一个函数共同作用将二进制转化为二维码
void encode(char *name)
{
	ifstream fin(name);
	char ch;
	char txt[100000];
	int i = 0;
	while (fin.get(ch))//读取文本中的内容
	{
		txt[i]=ch;//写入内容到文件
		i++;
	}
	txt[i] = '\0';
	fin.close();
	Generate_bmp(txt);
}

void Generate_bmp(char *str)
{
	cout << "文件中字符数：" << strlen(str) << endl;
	char path[] = "E:/输出图片/101.bmp";  //path[12]/[13]/[14]为数字
	int bit[8],flag=1;
	Mat image2,M2;
	srand(time(NULL));
	Mat M(32, 32, CV_8UC1);   
	M = Scalar(255);
	rectangle(M, Point(0, 0), Point(6, 6), Scalar(0, 0, 0));
	rectangle(M, Point(31, 0), Point(25, 6), Scalar(0, 0, 0));
	rectangle(M, Point(0, 31), Point(6, 25), Scalar(0, 0, 0));
	rectangle(M, Point(2, 2), Point(4, 4), Scalar(0, 0, 0));
	rectangle(M, Point(29, 2), Point(27, 4), Scalar(0, 0, 0));
	rectangle(M, Point(2, 27), Point(4, 29), Scalar(0, 0, 0));
	rectangle(M, Point(3, 3), Point(3, 3), Scalar(0, 0, 0));
	rectangle(M, Point(28, 3), Point(28, 3), Scalar(0, 0, 0));
	rectangle(M, Point(3, 28), Point(3, 28), Scalar(0, 0, 0));
	cv::Rect SrcImgROI1 = cv::Rect(8, 0, 16, 8);//
	cv::Mat subImg1 = M(SrcImgROI1);
	cv::Rect SrcImgROI2 = cv::Rect(0, 8, 32, 16);//
	cv::Mat subImg2 = M(SrcImgROI2);
	cv::Rect SrcImgROI3 = cv::Rect(8, 24, 24, 8);//
	cv::Mat subImg3 = M(SrcImgROI3);
	MatIterator_<uchar>M1begin, M1end;
	M1begin = subImg1.begin<uchar>(); M1end = subImg1.end<uchar>();
	MatIterator_<uchar>M2begin, M2end;
	M2begin = subImg2.begin<uchar>(); M2end = subImg2.end<uchar>();
	MatIterator_<uchar>M3begin, M3end;
	M3begin = subImg3.begin<uchar>(); M3end = subImg3.end<uchar>();
	int i = 0, j = 0;
	MatIterator_<uchar>Mbegin, Mend;
	Mbegin = M.begin<uchar>(); Mend = M.end<uchar>();
	for (int i = 0; i < strlen(str); i++)
	{
		if (M3begin == M3end)
		{
			tran(str[i], bit);
			bit[7] = rand() % 2;
			//bit[5] = rand() % 2;
			for (int j = 0; j < 8; j++)
			{
					if (bit[j] == 0)*M3begin = 0;
					else *M3begin = 255;
					++M3begin;
			}
			resize(M, image2, Size(M.cols * 20, M.rows * 20), (0, 0), (0, 0), 3); //放大
			imshow("Test", image2);   //窗口中显示图像
			waitKey(100);
			imwrite(path, image2);
			M3begin = subImg3.begin<uchar>();
			if (path[14] == '9')
			{
				if (path[13] == '9')
				{
					path[12]++;
					path[13] = '0';
					path[14] = '0';
				}
				else {
					path[13]++;
					path[14] = '0';
				}
			}
			else path[14]++;
			cout << "成功输出第" << ++num << "张bmp图" << endl;
			flag = 1;
		}
		else if (M2begin == M2end)
		{
			tran(str[i], bit);
			bit[7] = rand() % 2;
			//bit[5] = rand() % 2;
			for (int j = 0; j < 8; j++)
			{
				if (bit[j] == 0)*M2begin = 0;
				else *M2begin = 255;
				++M2begin;
			}
			M2begin = subImg2.begin<uchar>();
			flag = 3;
		}
		else if (M1begin == M1end)
		{
			tran(str[i], bit);
			bit[7] = rand() % 2;
			//bit[5] = rand() % 2;
			for (int j = 0; j < 8; j++)
			{
				if (bit[j] == 0)*M1begin = 0;
				else *M1begin = 255;
				++M1begin;
			}
			M1begin = subImg1.begin<uchar>();
			flag = 2;
		}
		else
		{
			tran(str[i], bit);
			bit[7] = rand() % 2;
			//bit[5] = rand() % 2;
			for (int j = 0; j < 8; j++)
			{
				if (flag == 1)
				{
					if (bit[j] == 0)*M1begin = 0;
					else *M1begin = 255;
					++M1begin;
				}
				else if (flag == 2)
				{
					if (bit[j] == 0)*M2begin = 0;
					else *M2begin = 255;
					++M2begin;
				}
				else {
					if (bit[j] == 0)*M3begin = 0;
					else *M3begin = 255;
					++M3begin;
				}
			}
		}
		if (i == (strlen(str) - 1) && M3begin != M3end)
		{
			if(flag==3)
		    for (; M3begin != M3end; ++M3begin)
			
				*M3begin = 123;
			
			else if (flag == 2)
			{
				for (; M3begin != M3end; ++M3begin)
				
					*M3begin = 123;
				
				for(; M2begin != M2end; ++M2begin)
					*M2begin = 123;
			}
			else
			{
				for (; M3begin != M3end; ++M3begin)

					*M3begin = 123;

				for (; M2begin != M2end; ++M2begin)
					*M2begin = 123;
				for (; M1begin != M1end; ++M1begin)
					*M1begin = 123;

			}
			resize(M, image2, Size(M.cols * 20, M.rows * 20), (0, 0), (0, 0), 3); //放大
			imshow("Test", image2);
			imwrite(path, image2);
			waitKey(100);
			if (path[14] == '9')
			{
				if (path[13] == '9')
				{
					path[12]++;
					path[13] = path[14] = '0';
				}
				path[13]++;
				path[14] = '0';
			}
			path[14]++;
			cout << "已输出最后一张bmp图" << endl;
			num++;
			imwrite(path, image2);
		}
	}
}
//将视频转为图片并做相应处理
void videotojpg()
{
	VideoCapture cap("E:/输出视频/VIDforDecode2.mp4");

	// 获取视频总帧数
	
	long totalFrameNumber = cap.get(CAP_PROP_FRAME_COUNT);
	int num=0;
	cout << "total frames: " << totalFrameNumber << endl;
	char path[] = "E:/输出图图/101.bmp";
	Mat frame,f;
	bool flags = true;
	long currentFrame = 2;

	while (flags) {
		// 读取视频每一帧
		cap.read(frame);
		cout << "正在处理第" << currentFrame << "帧" << endl;

		// 设置每6帧获取一次帧,手机帧率30fps,原视频帧率5fps
		if (currentFrame % 6 == 0) {
			cout << "获取此帧" << endl;
			printf("\n");
			// 将帧转成图片输出
			resize(frame,f, Size(frame.cols / 5, frame.rows / 5), (0, 0), (0, 0), 3); //缩小
			//cv::Rect SrcImgROI = cv::Rect(180, 10, 420, 420);
			//f = f(SrcImgROI);
			imwrite(path, f);
			num++;
			//改图片名称
			if (path[14] == '9')
			{
				if (path[13] == '9')
				{
					path[12]++;
					path[13] = '0';
					path[14] = '0';
				}
				else {
					path[13]++;
					path[14] = '0';
				}
			}
			else path[14]++;



		}
		printf("\n");
		// 结束条件
		if (currentFrame >= totalFrameNumber-8) {//减8是减去无效帧
			flags = false;
		}
		currentFrame++;
	}
	path[12] = '1'; path[13] = '0'; path[14] = '1';
	char outpath[] = "E:/输出图图/result/101.jpg";
	cout << "视频帧提取完成，接下来对每张图片进行处理，共有帧数："<<num << endl;
	Mat temp;
	for (int i = 0; i < num; i++)
	{
		cout << "处理第" << (i + 1) << "帧" << endl;
		temp = imread(path);
		LocateQRcode(temp, outpath);
		if (path[14] == '9')
		{
			if (path[13] == '9')
			{
				path[12]++;
				path[13] = '0';
				path[14] = '0';
			}
			else {
				path[13]++;
				path[14] = '0';
			}
		}
		else path[14]++;
		if (outpath[21] == '9')
		{
			if (outpath[20] == '9')
			{
				outpath[19]++;
				outpath[20] = '0';
				outpath[21] = '0';
			}
			else {
				outpath[20]++;
				outpath[21] = '0';
			}
		}
		else outpath[21]++;
		cout << "处理第" << (i + 1) << "帧成功！" << endl;
	}

}
//对图片进行解码
void decode()
{
	char path[] = "E:/输出图图/result/101.jpg";
	Mat M;   //读取灰度图
	char ch[2],cc;
	int chc=0;
	ofstream file("decode.txt", ios::app);//创建文本文件
	cout << "解码中……" << endl;
	for (int i = 0; i < 43; i++)
	{
		M = imread(path, 0);
		resize(M,M,Size(2560, 2560));
		int n[8], count = 0;
		int nnnum=0;
		cv::Rect SrcImgROI1 = cv::Rect(640, 0, 1280, 640);//
		cv::Mat subImg1 = M(SrcImgROI1);
		cv::Rect SrcImgROI2 = cv::Rect(0, 640, 2560, 1280);//
		cv::Mat subImg2 = M(SrcImgROI2);
		cv::Rect SrcImgROI3 = cv::Rect(640, 1920, 1920, 640);//
		cv::Mat subImg3 = M(SrcImgROI3);
		MatIterator_<uchar>M1begin, M1end;
		M1begin = subImg1.begin<uchar>(); M1end = subImg1.end<uchar>();
		MatIterator_<uchar>M2begin, M2end;
		M2begin = subImg2.begin<uchar>(); M2end = subImg2.end<uchar>();
		MatIterator_<uchar>M3begin, M3end;
		M3begin = subImg3.begin<uchar>(); M3end = subImg3.end<uchar>();
		int flag1 = 1, flag2 = 1, flag3 = 1;
		for (M1begin += (39 * 1280 + 39); flag1<=16*8; M1begin += 80)
		{ 
			if(i==42)if (*M1begin >50&& *M1begin<175)break;
			if (*M1begin > 123)n[count] = 1;
			else n[count] = 0;
			count++;
			if (count == 8)
			{
				nnnum++;
				cc = detran(n);
				file << cc;
				count = 0;
			}
			if (flag1 % 16 == 0) { M1begin += 1280 * 79;}
			flag1++;
		}
		for (M2begin += (39 * 2560 + 39); flag2 <= 32*16; M2begin += 80)
		{
			if (i == 42)if (*M1begin > 50 && *M1begin < 175)break;
			if (*M2begin > 123)n[count] = 1;
			else n[count] = 0;
			count++;
			if (count == 8)
			{
				nnnum++;
				cc = detran(n);
				file << cc;
				count = 0;
			}
			if (flag2 % 32 == 0) { M2begin += 2560 * 79;}
			flag2++;
		}
		for (M3begin += (39 * 1920 + 39); flag3 <= 24 * 8; M3begin += 80)
		{
			if (i == 42)if (*M1begin > 50 && *M1begin < 175)break;
			if (*M3begin > 123)n[count] = 1;
			else n[count] = 0;
			count++;
			if (count == 8)
			{
				nnnum++;
				cc = detran(n);
				file << cc;
				count = 0;
			}
			if (flag3 % 24 == 0) { M3begin += 1920 * 79;  }

			 flag3++;
		}
		if (path[21] == '9')
		{
			if (path[20] == '9')
			{
				path[19]++;
				path[20] = '0';
				path[21] = '0';
			}
			else {
				path[20]++;
				path[21] = '0';
			}
		}
		else path[21]++;
		cout <<"解码完成第"<<i+1<<"张图片,共有" <<nnnum <<"个字符"<< endl;
	}
	cout << "解码成功！" << endl;
}

int main()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED |
		FOREGROUND_GREEN | FOREGROUND_BLUE);//设置白色
	char name[] = "test.txt";
	//encode(name);       //编码入口
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN |
		FOREGROUND_BLUE);//设置青色
	char src[] = "E:/输出图片";
	char format[] = "bmp";
    //ImageOverlay(src, format, "E:/输出视频/code.avi", 32*20, 32*20, 101, 100+num+1, 5, 1);       //图片转视频入口
	//videotojpg();     //视频转图片入口
	decode();         //解码入口

	system("Pause");
	return 0;
}

