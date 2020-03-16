#define _CRT_SECURE_NO_WARNINGS 
#ifndef _IMG_TO_VIDEO_H_
#define _IMG_TO_VIDEO_H_
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <stdio.h>
#include <string.h>

#ifndef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
}
#endif // __cplusplus

using namespace cv;

#define BUF_SIZE (32 << 1)

int ImageOverlay(char*src,
	const char*format, const char*dst,
	int width, int height,
	size_t start_name, size_t end_name,
	int fps, int isColor)
{
	char img_name[BUF_SIZE];
	char psrc[BUF_SIZE];
	strcpy_s(psrc, src);
	Mat r;

	VideoWriter writer(dst, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps,
		Size(width, height), isColor);
	if (!writer.isOpened()) { printf("Error at line %d :function VideoWriter\n", __LINE__); return -1; }
	for (size_t i = start_name; i <= end_name; i++)
	{
#ifdef _WIN64
		if (sprintf_s(img_name, "/%I64d.", i) < 0)
#else
		if (sprintf_s(img_name, "/%lld.", i) < 0)
#endif
		{
			printf("Error at line %d :function sprintf()\n", __LINE__);
			return -3;
		}

		strcat(img_name, format);
		strcat(src, img_name);
		printf("%s\n", src);
		r = imread(src);
		strcpy(src, psrc);
		if (r.empty()) { printf("Error at line %d :function imread()\n", __LINE__); return -4; }
		waitKey(5);
		writer.write(r);
	}
	printf("图片加载完成，生成视频成功！\n");
	return 0;
}
#endif // _IMG_TO_VIDEO_H_