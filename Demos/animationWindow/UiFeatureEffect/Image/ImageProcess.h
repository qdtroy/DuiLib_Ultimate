#pragma once

#include "..\Graph\Graph_WarpMatrix.h"
using namespace Graph;

class CWarp
{
public:

	// 将图片扭曲到凸四边形rcdes中
	// input pData 图片数据
	// input rcsrc 图片原来rect
	// input output warprect 图片扭曲后的rect
	// output imgdes  扭曲后的图片数据
	// input bFill   是否用color填充其它区域
	// 外部分配内存的方式	
	//remark  rcsrc 是pData的下标范围 rcBound是图片的大小裁剪范围（如rcSrc 0,0,400,300-->rcBound 0,0,401,301)
	static BOOL WarpImage(BYTE*pData,RECT &rcSrc,WarpRect &rcDes,RECT &rcBound,BYTE* imgDes,Color color = Color::Transparent,BOOL bFill = TRUE);

	// 将图片rcSrc大小的内容扭曲到凸四边形rcdes中
	// input pData 图片数据
	// input srcDataWidth pdata的宽度
	// input srcDataHeight pdata的高度
	// input rcsrc 图片原来rect
	// input output warprect 图片扭曲后的rect
	// output imgdes  扭曲后的图片数据
	// input bFill   是否用color填充其它区域
	// 外部分配内存的方式	
	//remark  rcsrc 是pData的下标范围 rcBound是图片的大小裁剪范围（如rcSrc 0,0,400,300-->rcBound 0,0,401,301)
	static BOOL WarpImage(BYTE*pData,int srcDataWidth,int srcDataHeight, RECT &rcSrc,WarpRect &rcDes,RECT &rcBound,BYTE* imgDes,Color color = Color::Transparent,BOOL bFill = TRUE);

	// 将图片rcSrc大小的内容扭曲到凸四边形rcdes中
	static BOOL WarpImage(BYTE *pData, int width, int height, BYTE* &imgDes, real param1);

private:
	// Calculate coordinates in input image from points in output image
	// x and y are normalised coordinates, -1 ..1
	// xnew and ynew are also normalised coordinates
	// x,y warpimage coordination
	// xnew, ynew srcimage coordination
	// param1 some warp fun param
	static inline void Evaluate(real x, real y, real &xnew, real &ynew, real param1);
};

class CMemery
{
public:
	// 填充数据
	static void SetPixels(BYTE *Data,int width, int height, DWORD iValue);
	// 填充局部数据
	static void SetPixels(BYTE *Data,int stride, int left, int top, int right, int bottom, DWORD iValue);
	// 拷贝数据
	static void CopyPixels(BYTE*DataDes,int strideDes,int leftDes,int topDes,int rightDes,int bottomDes,BYTE *DataSrc,int strideSrc,int leftSrc,int topSrc);
};

class CBlinds
{
public:
	// 将图片处理成百叶窗特效
	// input Data
	// input width height 图像的长和宽
	// input iBlindHeight百叶效果的高度，iBlindNum百叶效果的个数
	// offset 每个百叶效果要处理的行距离自己首行的距离（像素）
	// color  百叶的颜色
	// output Data
	static BOOL BlindsH(BYTE *Data,int width, int height,int iBlindHeight,int iBlindNum,int offsetDistance,DWORD color = 0);
	// 将图片处理成百叶窗特效
	// input Data
	// input width height 图像的长和宽
	// input iBlindWidth百叶效果的kuan度，iBlindNum百叶效果的个数
	// offset 每个百叶效果要处理的列距离自己首列的距离（像素）
	// color  百叶的颜色
	// output Data
	static BOOL BlindsV(BYTE *Data,int width, int height,int iBlindWidth,int iBlindNum,int offsetDistance,DWORD color = 0);
	// 将图片以百叶方式拷贝特效
	// input DataDes
	// input DataSrc
	// input width height 图像的长和宽
	// input iBlindHeight百叶效果的高度，iBlindNum百叶效果的个数
	// offset 每个百叶效果要处理的行距离自己首行的距离（像素）
	// output DataDes
	static BOOL BlindsH(BYTE *DataDes,BYTE *DataSrc,int width, int height,int iBlindHeight,int iBlindNum,int offsetDistance);
	// 将图片以百叶方式拷贝特效
	// input DataDes
	// input DataSrc
	// input width height 图像的长和宽
	// input iBlindWidth百叶效果的kuan度，iBlindNum百叶效果的个数
	// offset 每个百叶效果要处理的列距离自己首列的距离（像素）
	// output DataDes
	static BOOL BlindsV(BYTE *DataDes,BYTE *DataSrc,int width, int height,int iBlindWidth,int iBlindNum,int offsetDistance);
};

class CAlpha
{
public:
	// 设置图像的alpha
	// alphachange 0~1
	static void SetAlpha(BYTE *Data,int width, int height,real alphaChange);

	//MMX设置图片的alpha值为指定的值
	static void SetAlphaMMX(BYTE *Data,int width, int height,BYTE alphaNew);

	//设置图像局部的alpha
	static void SetAlpha(BYTE *Data,int stride, int left ,int top, int right, int bottom, real alphaChange);
};

class CWater
{
public:
	CWater(){m_iParticleAmplitudeField1 = m_iParticleAmplitudeField2 = NULL;};
	~CWater(){};

	// 创建水波粒子系统,必须与Destory成对使用
	BOOL Create(int iWidth,int iHeight);

	// 删除粒子系统,必须与Create成对使用
	BOOL Destory();

	//  设置衰减阻尼系数，默认在创建粒子的时候初始化为6
	BOOL SetDensity(int idensity);

	// 复位粒子振幅
	BOOL ResetAmplitude();

	// 生成一个震荡波
	// x ,y						位置
	// radius					半径
	// amplitude				振幅
	BOOL AmplitudeBlob(int x, int y, int radius, int amplitude);

	// 渲染粒子系统，生成水波图片
	BOOL WaterImage(BYTE* pSrcImage,BYTE* pTargetImage);

private:
	// 计算npage页的振幅
	BOOL CalcWaterAmplitude(int npage, int density);

	// 根据npage页的振幅生成图片
	BOOL DrawWaterWithLight(int page,BYTE* pSrcImage,BYTE* pTargetImage);
private:
	int			m_iWidth;						// width
	int			m_iHeight;						// height
	int			m_iHpage;						// The current Amplitude field
	int			m_density;						// The water density
	int*		m_iParticleAmplitudeField1;		// the Amplitude fields
	int*		m_iParticleAmplitudeField2;		// the Amplitude fields
};

class CScale
{
private:
	CScale(){m_paramInited = FALSE;}
public:

	static BOOL ImageScale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, int interpolationMode = 0);

private:
	BOOL InitParam(real sharpenCoefficient);
	real SinXDivX(real x, real sharpenCoefficient);
	void ThreeOrder(const BYTE *DataSrc, const int srcStride, const real fx, const real fy, BYTE *resultColor);
	void ThreeOrderBorder(const BYTE *DataSrc, const int srcStride, int srcLeft, int srcTop, const int srcWidth, const int srcHeight, const real fx, const real fy, BYTE *resultColor);
	inline void GetBorderColor(const BYTE *DataSrc, const int srcStride, int row, int col, real* borderColor);
	// 高质量插值算法（三次卷积插值）
	BOOL Scale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, real sharpenCoefficient);
	// 低质量插值算法（临近点插值）
	BOOL Scale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcleft, int srctop, int srcWidth, int srcHeight);
	// 中质量插值算法（双线性插值）
	BOOL BilinearScale(BYTE *DataDes, int desWidth, int desHeight, BYTE *DataSrc, int srcStride, int srcleft, int srctop, int srcWidth, int srcHeight);

private:
	static real m_sinxDivxTable[513];
	static BOOL m_paramInited;
};

class CAlphaBlend
{
private:
	// same size 
	static BOOL AlphaBlend(BYTE *desData, BYTE *srcData, int width, int height, BYTE sourceAlpha = 255);
	// diff size
	static BOOL AlphaBlend(BYTE *desData, int desStride, int desLeft, int desTop, int desWidth, int desHeight, BYTE *srcData, int srcStride, int srcLeft, int srcTop, BYTE sourceAlpha = 255);
public:
	// interface
	// interpolationMode 插值模式，默认为临近点插值
	static BOOL AlphaBlend(BYTE *desData, int desStride, int desLeft, int desTop, int desWidth, int desHeight, BYTE *srcData, int srcStride, int srcLeft, int srcTop, int srcWidth, int srcHeight, BYTE sourceAlpha = 255, int interpolationMode = 0);
};

class CGaussBlur
{
public:
	// weights is calculated for ImageGaussiabBlur function
	static void InitWeights(double Q,int radius,int **weights);
	static void ImageGaussiabBlur(BYTE *Data,int width, int height,int radius, int *weights);
};

class CTraileBlur
{
public:
	//均值模糊
	//input DataSrc
	//input width height 图像的长和宽
	//input length 模糊范围
	//output DataDes
	static BOOL TraileBlur(BYTE *DataDes, BYTE *DataSrc, int width, int height, int lengthBlur);
};