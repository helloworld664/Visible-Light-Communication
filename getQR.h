#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <opencv2\imgproc\types_c.h>

using namespace cv;
using namespace std;

//找到所提取轮廓的中心点

//在提取的中心小正方形的边界上每隔周长个像素提取一个点的坐标，求所提取四个点的平均坐标（即为小正方形的大致中心）
Point Center_cal(vector<vector<Point> > contours, int i)
{
	int centerx = 0, centery = 0, n = contours[i].size();
	centerx = (contours[i][n / 4].x + contours[i][n * 2 / 4].x + contours[i][3 * n / 4].x + contours[i][n - 1].x) / 4;
	centery = (contours[i][n / 4].y + contours[i][n * 2 / 4].y + contours[i][3 * n / 4].y + contours[i][n - 1].y) / 4;
	Point point1 = Point(centerx, centery);
	return point1;
}

void LocateQRcode(Mat image,char *path)
{
	Mat src = image, src_gray;
	Mat src_all = src.clone(), threshold_output,src_end;
	Point QRPoints[4];
	vector<vector<Point> > contours, contours2;
	vector<Vec4i> hierarchy;
	//预处理
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3)); //模糊，去除毛刺
	threshold(src_gray, threshold_output, 100, 255, THRESH_OTSU);
	//寻找轮廓 
	//第一个参数是输入图像 2值化的
	//第二个参数是内存存储器，FindContours找到的轮廓放到内存里面。
	//第三个参数是层级，**[Next, Previous, First_Child, Parent]** 的vector
	//第四个参数是类型，采用树结构
	//第五个参数是节点拟合模式，这里是全部寻找
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

	//轮廓筛选
	int c = 0, ic = 0, area = 0;
	int parentIdx = -1;
	for (int i = 0; i < contours.size(); i++)
	{
		//hierarchy[i][2] != -1 表示不是最外面的轮廓
		if (hierarchy[i][2] != -1 && ic == 0)
		{
			parentIdx = i;
			ic++;
		}
		else if (hierarchy[i][2] != -1)
		{
			ic++;
		}
		//最外面的清0
		else if (hierarchy[i][2] == -1)
		{
			ic = 0;
			parentIdx = -1;
		}
		//找到定位点信息
		if (ic >= 2)
		{
			contours2.push_back(contours[parentIdx]);
			ic = 0;
			parentIdx = -1;
		}
	}
	if (contours2.size() < 3)
	{
		for (int i = 0; i < 4; i++)
		{
			QRPoints[i].x = 0;
			QRPoints[i].y = 0;
		}
	}
	else
	{
		//连接定位点
		Point point[3];
		for (int i = 0; i < contours2.size(); i++)
		{
			point[i] = Center_cal(contours2, i);
		}

		//对点的顺序进行修改不然会报错
		int pt[6];
		pt[0] = point[0].x;
		pt[1] = point[0].y;
		pt[2] = point[1].x;
		pt[3] = point[1].y;
		pt[4] = point[2].x;
		pt[5] = point[2].y;
		point[1].x = max(max(pt[0], pt[2]), pt[4]);
		point[0].y = max(max(pt[1], pt[3]), pt[5]);

		point[0].x = min(min(pt[0], pt[2]), pt[4]);
		point[2].x = min(min(pt[0], pt[2]), pt[4]);
		point[1].y = min(min(pt[1], pt[3]), pt[5]);
		point[2].y = min(min(pt[1], pt[3]), pt[5]);

		//输出各定位点中心坐标
		cout << "三个定位点坐标：" << endl;
		cout << point[0].x << " " << point[0].y << endl;
		cout << point[1].x << " " << point[1].y << endl;
		cout << point[2].x << " " << point[2].y << endl;
		//求出二维码区域
		float each_xs = (float)max((point[1].x - point[0].x), (point[0].y - point[1].y)) / 25;
		if (each_xs > 12 || each_xs < 10) { cout << "此帧处理失败！失败原因：定位点定位错误！" << endl; return; }
		cout <<"计算得每个小方块像素："<< each_xs<<endl;
		float xx, yy,chang,kuan;
		xx = point[2].x - 3.5*each_xs;
		if (xx < 0)xx = 0;
		yy = point[2].y - 3.5*each_xs;
	    if (yy < 0)yy = 0;
		chang = each_xs * 33;
		if ((xx + chang) > src_all.rows)chang = (float)src_all.rows - xx;
		kuan = each_xs * 33;
		if ((yy + kuan) > src_all.cols)kuan = (float)src_all.cols - yy;
		//截取二维码部分
		cv::Rect SrcImgROI = cv::Rect((int)xx, (int)yy, (int)each_xs*32.85, (int)each_xs*32.85);
		src_all = src_all(SrcImgROI);
		waitKey(0);
		//再对图像进行处理
		resize(src_all, src_all, Size(640, 640));
		Mat contrastImage = Mat::zeros(src_all.size(), src_all.type());    //亮度与对比度调节
		double alpha = 1.8;  //对比度因子
		int beta = -30;   //亮度因子
		for (int y = 0; y < src_all.rows; y++) {
			for (int x = 0; x < src_all.cols; x++) {
				for (int c = 0; c < 3; c++) {
					contrastImage.at<Vec3b>(y, x)[c] =
						saturate_cast<uchar>(alpha*(src_all.at<Vec3b>(y, x)[c]) + beta);
				}
			}
		}
		Mat filterImage;
		bilateralFilter(contrastImage, filterImage, 13, 26, 6);  //双边滤波
		imwrite(path, filterImage);  //保存滤波后图片
	}
}