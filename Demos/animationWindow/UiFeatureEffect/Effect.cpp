#include "StdAfx.h"
#include "Effect.h"
#include "Image\ImageProcess.h"
#include "Graph\Graph_Projection.h"
#include "Image\ColorHelper.h"
#include "Motion\Motion.h"
#include <time.h>
#pragma warning ( disable : 4244)


BOOL SaveBitmapToFile(HBITMAP hBitmap , LPCTSTR lpFileName) 
{    	 
	HDC					hDC;
	DWORD				dwWritten, dwPaletteSize = 0;	
	BITMAP				Bitmap;        
	BITMAPFILEHEADER	bmfHdr;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER	lpbi;			
	HANDLE				fh, hDib, hPal, hOldPal = NULL;	

	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = Bitmap.bmWidth;
	bi.biHeight          = Bitmap.bmHeight;
	bi.biPlanes          = Bitmap.bmPlanes;
	bi.biBitCount        = Bitmap.bmBitsPixel;
	bi.biCompression     = BI_RGB;
	bi.biSizeImage       = Bitmap.bmWidthBytes * Bitmap.bmHeight;
	bi.biXPelsPerMeter   = 0;
	bi.biYPelsPerMeter   = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant    = 0;

	if(bi.biBitCount <= 8)
	{
		dwPaletteSize = (1 << bi.biBitCount) *sizeof(RGBQUAD);
	}

	//为位图内容分配内存
	hDib  = GlobalAlloc(GHND,bi.biSizeImage + dwPaletteSize + sizeof(BITMAPINFOHEADER));

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	// 处理调色板   
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC  = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	// 获取该调色板下新的像素值
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	//恢复调色板   
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件    
	fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	// 设置位图文件头
	bmfHdr.bfType		= 0x4D42;	
	bmfHdr.bfSize		= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + bi.biSizeImage;
	bmfHdr.bfReserved1	= 0;
	bmfHdr.bfReserved2	= 0;
	bmfHdr.bfOffBits	= (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	// 写入位图文件头
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	// 写入位图文件其余内容
	WriteFile(fh, (LPSTR)lpbi, bmfHdr.bfSize, &dwWritten, NULL);

	//消除内存分配  
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

BOOL Save(Image *pImage, LPCTSTR lpszFileName,LPCTSTR format )
{
	CLSID encoderClsid;
	ImageCodecInfo* pCodecInfo = NULL;
	UINT nNum = 0,nSize = 0;
	GetImageEncodersSize(&nNum,&nSize);
	if (nSize<0)
	{
		return FALSE;
	}

	pCodecInfo= new ImageCodecInfo[nSize];

	if (pCodecInfo==NULL)
	{
		return FALSE;
	}

	GetImageEncoders(nNum,nSize,pCodecInfo);

	for (UINT i=0; i<nNum;i++)
	{

		if (wcscmp(pCodecInfo[i].MimeType,format)==0)
		{
			encoderClsid= pCodecInfo[i].Clsid;
		}
	}

	delete[] pCodecInfo;

	if(pImage->Save(lpszFileName,&encoderClsid,NULL) != Ok)
	{
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////
void CFlipEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};

	// 构造要扭曲的矩形数据下标
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};
	CdRect rc(rcSrc);

	// 扭曲矩形与投影矩阵
	WarpRect rcWarp;
	CProjection mtxProject;

	int frame = internalParam->frameNow;
	// pi/2时没有投影
	if (frame == 12)
	{
		CMemery::SetPixels(internalParam->param.pBmpData,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy,0);
		return;
	}

	if(!internalParam->param.bShow)
	{
		mtxProject.Reset();
		if(internalParam->param.animationEffect == 2)
		{
			mtxProject.RotateY(-frame/24.0f*M_PI);
		}
		else if(internalParam->param.animationEffect == 3)
		{
			mtxProject.RotateY(frame/24.0f*M_PI);
		}
		else if(internalParam->param.animationEffect == 4)
		{
			mtxProject.RotateX(frame/24.0f*M_PI);
		}
		else/* if(internalParam->param.animationEffect == 5)*/
		{
			mtxProject.RotateX(-frame/24.0f*M_PI);
		}
		
		mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

		CWarp::WarpImage((BYTE*)pImgData,rcSrc,rcWarp,rcClient,(BYTE*)internalParam->param.pBmpData,Color(0,0,0,0),TRUE);	
	}
	else
	{
		mtxProject.Reset();
		if(internalParam->param.animationEffect == 2)
		{
			mtxProject.RotateY(-(frame+23)/24.0f*M_PI);
		}
		else if(internalParam->param.animationEffect == 3)
		{
			mtxProject.RotateY((frame+23)/24.0f*M_PI);
		}
		else if(internalParam->param.animationEffect == 4)
		{
			mtxProject.RotateX((frame+23)/24.0f*M_PI);
		}
		else/* if(internalParam->param.animationEffect == 5)*/
		{
			mtxProject.RotateX(-(frame+23)/24.0f*M_PI);
		}
		
		mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

		CWarp::WarpImage((BYTE*)pImgData,rcSrc,rcWarp,rcClient,(BYTE*)internalParam->param.pBmpData,Color(0,0,0,0),TRUE);
	}

	if(frame == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CFlipEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	this->m_effectFrame = 25;
}

void CFlipEffect::ReleaseEffectParam()
{
	// todo
}

void CMosaicEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	if(!internalParam->param.bShow)
	{	
		BYTE * pSrc = internalParam->param.pBmpData;	
			
		int x = rand() % m_nWidthCount;
		int y = rand() % m_nHeightCount;

		while(m_pMosaicArray[y*m_nWidthCount+x] == 1)
		{
			x = rand()% m_nWidthCount;
			y = rand()% m_nHeightCount;
		}

		m_pMosaicArray[y*m_nWidthCount+x] = 1;

		int wid = m_nMosaicW;
		int hei = m_nMosaicH;

		if ((x+1)*m_nMosaicW > m_nWidth)
		{
			wid = m_nWidth-x*m_nMosaicW;
		}

		if ((y+1)*m_nMosaicH > m_nHeight)
		{
			hei = m_nHeight-y*m_nMosaicH;
		}

		RECT rc = { x*m_nMosaicW,y*m_nMosaicH,x*m_nMosaicW+wid,y*m_nMosaicH+hei };	
		CMemery::SetPixels(pSrc,m_nWidth*4,rc.left,rc.top,rc.right,rc.bottom,0);
	}
	else
	{
		BYTE * pSrc = internalParam->bmpDataCopy;
		BYTE * pDes = internalParam->param.pBmpData;
		if ((int)internalParam->frameNow == 1)
		{
			memset(pDes,0,m_nWidth*m_nHeight*4);
		}

		int x = rand() % m_nWidthCount;
		int y = rand() % m_nHeightCount;

		while(m_pMosaicArray[y*m_nWidthCount+x] == 1)
		{
			x = rand()% m_nWidthCount;
			y = rand()% m_nHeightCount;
		}

		m_pMosaicArray[y*m_nWidthCount+x] = 1;

		int wid = m_nMosaicW;
		int hei = m_nMosaicH;

		if ((x+1)*m_nMosaicW > m_nWidth)
		{
			wid = m_nWidth-x*m_nMosaicW;
		}

		if ((y+1)*m_nMosaicH > m_nHeight)
		{
			hei = m_nHeight-y*m_nMosaicH;
		}

		RECT rc = { x*m_nMosaicW,y*m_nMosaicH,x*m_nMosaicW+wid,y*m_nMosaicH+hei };
		int strideSrc,strideDes;
		strideSrc = strideDes = m_nWidth*4;
		CMemery::CopyPixels(pDes,strideDes,rc.left,rc.top,rc.right,rc.bottom,pSrc,strideSrc,rc.left,rc.top);
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CMosaicEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_nMosaicW = m_nWidth = internalParam->param.bmpSize.cx;
	m_nMosaicH = m_nHeight = internalParam->param.bmpSize.cy;

	if(m_nMosaicW > m_nMosaicH)
	{
		m_nMosaicW = m_nMosaicH = m_nMosaicW/10;
	} 
	else
	{
		m_nMosaicW = m_nMosaicH = m_nMosaicH/10;
	}

	m_nWidthCount  = ((m_nWidth % m_nMosaicW==0) ? m_nWidth / m_nMosaicW : (m_nWidth/m_nMosaicW + 1)); 
	m_nHeightCount = ((m_nHeight % m_nMosaicH==0) ? m_nHeight / m_nMosaicH : (m_nHeight/m_nMosaicH + 1));

	m_pMosaicArray = new int[m_nWidthCount*m_nHeightCount];
	memset(m_pMosaicArray,0,m_nWidthCount*m_nHeightCount*sizeof(int));
	m_effectFrame = m_nWidthCount * m_nHeightCount;

	srand((unsigned)time(NULL));

}

void CMosaicEffect::ReleaseEffectParam()
{
	delete []m_pMosaicArray;
	m_pMosaicArray = NULL;
}

void CSlideEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;

	memset(internalParam->param.pBmpData,0,width*height*4);

	if (!internalParam->param.bShow)
	{	
		if(internalParam->param.animationEffect == 7)
		{
			RECT rcCopy = { (internalParam->frameNow-1)*m_step, 0, width,height }; 
			if(rcCopy.left < rcCopy.right)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0, width-rcCopy.left ,height,internalParam->bmpDataCopy,width*4,rcCopy.left,rcCopy.top);
			}
		}
		else if(internalParam->param.animationEffect == 8)
		{
			RECT rcCopy = { (internalParam->frameNow-1)*m_step, 0, width,height }; 
			if(rcCopy.left < rcCopy.right)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,rcCopy.left,0, width ,height,internalParam->bmpDataCopy,width*4,0,0);
			}
		}
		else if(internalParam->param.animationEffect == 9)
		{
			RECT rcCopy = { 0,(internalParam->frameNow-1)*m_step, width,height }; 
			if(rcCopy.top < rcCopy.bottom)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,rcCopy.top,width,height,internalParam->bmpDataCopy,width*4,0,0);
			}
		}
		else/* if(internalParam->param.animationEffect == 10)*/
		{
			RECT rcCopy = { 0,(internalParam->frameNow-1)*m_step, width,height }; 
			if(rcCopy.top < rcCopy.bottom)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0,width,height-rcCopy.top,internalParam->bmpDataCopy,width*4,rcCopy.left,rcCopy.top);
			}
		}
	}
	else
	{
		if(internalParam->param.animationEffect == 7)
		{
			RECT rcCopy = { (internalParam->frameNow-1)*m_step, 0, width,height }; 
			if(rcCopy.left < rcCopy.right)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,width-rcCopy.left, 0,width ,height,internalParam->bmpDataCopy,width*4,0,0);
			}
			else
			{
				memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
			}
		}
		else if(internalParam->param.animationEffect == 8)
		{
			RECT rcCopy = { (internalParam->frameNow-1)*m_step, 0, width,height }; 
			if(rcCopy.left < rcCopy.right)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0, rcCopy.left ,height,internalParam->bmpDataCopy,width*4,width-rcCopy.left,rcCopy.top);
			}
			else
			{
				memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
			}
		}
		else if(internalParam->param.animationEffect == 9)
		{
			RECT rcCopy = { 0,(internalParam->frameNow-1)*m_step, width,height }; 
			if(rcCopy.top < rcCopy.bottom)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0,width,rcCopy.top,internalParam->bmpDataCopy,width*4,rcCopy.left,height - rcCopy.top);
			}
			else
			{
				memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
			}
		}
		else/* if(internalParam->param.animationEffect == 10)*/
		{
			RECT rcCopy = { 0,(internalParam->frameNow-1)*m_step, width,height }; 
			if(rcCopy.top < rcCopy.bottom)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,height-rcCopy.top,width,height,internalParam->bmpDataCopy,width*4,0,0);
			}
			else
			{
				memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
			}
		}
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CSlideEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	int frame = 10;
	if(internalParam->param.animationEffect == 7 || internalParam->param.animationEffect == 8)
	{
		CalculateFrame(internalParam->param.bmpSize.cx);
	}
	else
	{
		CalculateFrame(internalParam->param.bmpSize.cy);
	}
}

void CSlideEffect::ReleaseEffectParam()
{
}

void CSlideEffect::CalculateFrame(int length)
{
	int frame = 10;

	m_step = length/frame;
	if(length%frame != 0)
	{
		m_effectFrame = frame + 2;
	}
	else
	{
		m_effectFrame = frame + 1;
	}
}

void CBandSlideEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;

	memset(internalParam->param.pBmpData,0,width*height*4);

	if (!internalParam->param.bShow)
	{	
		if(internalParam->param.animationEffect == 11)
		{
			int slidedistance = (internalParam->frameNow-1)*m_step; 
			
			if(slidedistance < width)
			{	
				for(int row = 0; row < m_bandNum; row +=1)
				{			
					int top = m_bandSize*row;
					int rowsCopy = m_bandSize;
					if(row==m_bandNum-1)
					{
						rowsCopy = height - top;
					}
					
					if(row & 1)
					{			
						// 偶数行
						CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,top, width-slidedistance ,top+rowsCopy,internalParam->bmpDataCopy,width*4,slidedistance,top);	
					}
					else
					{	
						// 奇数行
						CMemery::CopyPixels(internalParam->param.pBmpData,width*4,slidedistance,top, width ,top+rowsCopy,internalParam->bmpDataCopy,width*4,0,top);
					}
				}

			}
		}
		else
		{
			int slidedistance = (internalParam->frameNow-1)*m_step; 

			if(slidedistance < height)
			{	
				for(int col = 0; col < m_bandNum; col +=1)
				{			
					int left = m_bandSize*col;
					int colsCopy = m_bandSize;
					if(col==m_bandNum-1)
					{
						colsCopy = width - left;
					}

					if(col & 1)
					{	
						// 偶数行
						CMemery::CopyPixels(internalParam->param.pBmpData,width*4,left,slidedistance,left+colsCopy,height,internalParam->bmpDataCopy,width*4,left,0);
					}
					else
					{	
						// 奇数行
						CMemery::CopyPixels(internalParam->param.pBmpData,width*4,left,0,left+colsCopy,height-slidedistance,internalParam->bmpDataCopy,width*4,left,slidedistance);
					}
				}

			}
		}
	}
	else
	{
		if(internalParam->param.animationEffect == 11)
		{
			int slidedistance = (internalParam->frameNow-1)*m_step; 

			if(slidedistance < width)
			{	
				for(int row = 0; row < m_bandNum; row +=1)
				{			
					int top = m_bandSize*row;
					int rowsCopy = m_bandSize;
					if(row==m_bandNum-1)
					{
						rowsCopy = height - top;
					}

					if(row & 1)
					{	
						// 偶数行
						CMemery::CopyPixels(internalParam->param.pBmpData,width*4,width-slidedistance, top,width ,top+rowsCopy,internalParam->bmpDataCopy,width*4,0,top);
					}
					else
					{	
						// 奇数行
						CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,top, slidedistance ,top+rowsCopy,internalParam->bmpDataCopy,width*4,width-slidedistance,top);	
					}
				}

			}
			else
			{
				memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
			}
		}
		else
		{
			int slidedistance = (internalParam->frameNow-1)*m_step; 

			if(slidedistance < height)
			{	
				for(int col = 0; col < m_bandNum; col +=1)
				{			
					int left = m_bandSize*col;
					int colsCopy = m_bandSize;
					if(col==m_bandNum-1)
					{
						colsCopy = width - left;
					}

					if(col & 1)
					{	
						// 偶数行
						CMemery::CopyPixels(internalParam->param.pBmpData,width*4,left,0,left+colsCopy,slidedistance,internalParam->bmpDataCopy,width*4,left,height - slidedistance);
					}
					else
					{	
						// 奇数行
						CMemery::CopyPixels(internalParam->param.pBmpData,width*4,left,height-slidedistance,left+colsCopy,height,internalParam->bmpDataCopy,width*4,left,0);	
					}
				}

			}
			else
			{
				memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
			}
		}
	}
	
	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CBandSlideEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	int frame = 10;
	if(internalParam->param.animationEffect == 11)
	{
		CalculateFrame(internalParam->param.bmpSize.cx);
		
		m_bandSize = internalParam->param.bmpSize.cy/10;
		m_bandNum = ((internalParam->param.bmpSize.cy % m_bandSize == 0) ? internalParam->param.bmpSize.cy / m_bandSize : (internalParam->param.bmpSize.cy / m_bandSize + 1));
	}
	else
	{
		CalculateFrame(internalParam->param.bmpSize.cy);

		m_bandSize = internalParam->param.bmpSize.cx/10;
		m_bandNum = ((internalParam->param.bmpSize.cx % m_bandSize == 0) ? internalParam->param.bmpSize.cx / m_bandSize : (internalParam->param.bmpSize.cx / m_bandSize + 1));
	}
}

void CBandSlideEffect::ReleaseEffectParam()
{
}

void CBlindsEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long width = internalParam->param.bmpSize.cx;
	long height = internalParam->param.bmpSize.cy;

	if(!internalParam->param.bShow)
	{
		if(internalParam->param.animationEffect == 13)
		{
			CBlinds::BlindsH(internalParam->param.pBmpData,width,height,m_bandSize,m_bandNum,internalParam->frameNow-1);
		}
		else
		{
			CBlinds::BlindsV(internalParam->param.pBmpData,width,height,m_bandSize,m_bandNum,internalParam->frameNow-1);		
		}
	}
	else
	{
		if(internalParam->frameNow == 1)
		{
			memset(internalParam->param.pBmpData,0,width*height*4);
		}

		if(internalParam->param.animationEffect == 13)
		{
			CBlinds::BlindsH(internalParam->param.pBmpData,internalParam->bmpDataCopy,width,height,m_bandSize,m_bandNum,internalParam->frameNow-1);
		}
		else
		{
			CBlinds::BlindsV(internalParam->param.pBmpData,internalParam->bmpDataCopy,width,height,m_bandSize,m_bandNum,internalParam->frameNow-1);
		}
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CBlindsEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	if(internalParam->param.animationEffect == 13)
	{
		m_bandSize = internalParam->param.bmpSize.cy/10;
		if (m_bandSize > 20)
		{
			m_bandSize = 20;
		}

		m_bandNum = ((internalParam->param.bmpSize.cy % m_bandSize == 0) ? internalParam->param.bmpSize.cy / m_bandSize : (internalParam->param.bmpSize.cy / m_bandSize + 1));
	}
	else
	{
		m_bandSize = internalParam->param.bmpSize.cx/10;
		if (m_bandSize > 20)
		{
			m_bandSize = 20;
		}

		m_bandNum = ((internalParam->param.bmpSize.cx % m_bandSize == 0) ? internalParam->param.bmpSize.cx / m_bandSize : (internalParam->param.bmpSize.cx / m_bandSize + 1));
	}
	
	m_effectFrame = m_bandSize;
}

void CBlindsEffect::ReleaseEffectParam()
{

}

void CSepcialBlindsEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	if(internalParam->param.animationEffect == 15)
	{
		CalculateFrame(internalParam->param.bmpSize.cy);		
	}
	else
	{
		CalculateFrame(internalParam->param.bmpSize.cx);
	}

	m_effectFrame = m_bandNum;
}

void CSepcialBlindsEffect::CalculateFrame(int length)
{
	m_specialBandSize.clear();

	int sum = 0;
	float delta = 0;
	if (length >= 210)
	{
		m_bandNum = 20;
		delta = length/210.0f;
	}
	else
	{
		m_bandNum = 10;
		delta = length/55.0f;
	}

	for(int i = 0; i < m_bandNum - 1; i++)
	{
		int bandSizeI = (int)((i+1)*delta + 0.5f);
		m_specialBandSize.push_back(bandSizeI);
		sum += bandSizeI;
	}
	m_specialBandSize.push_back(length - sum);

	sort(m_specialBandSize.begin(),m_specialBandSize.end());

	// 每次要处理的尺寸
	m_bandSize = (int)ceil(delta);
	m_nowSumRowCol = 0;
}

void CSepcialBlindsEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long width = internalParam->param.bmpSize.cx;
	long height = internalParam->param.bmpSize.cy;

	if(!internalParam->param.bShow)
	{
		if(internalParam->param.animationEffect == 15)
		{	
			int rowStart = m_nowSumRowCol;
			m_nowSumRowCol += m_specialBandSize[internalParam->frameNow-1];
			
			for(UINT32 i = internalParam->frameNow-1; i < m_specialBandSize.size(); i++)
			{
				RECT rc = { 0,rowStart+(internalParam->frameNow-1)*m_bandSize,width, rowStart + internalParam->frameNow*m_bandSize};

				rowStart += m_specialBandSize[i];
				rc.bottom = min(rc.bottom,rowStart);

				if(rc.top < rc.bottom)
				{
					CMemery::SetPixels(internalParam->param.pBmpData,width*4,rc.left,rc.top,rc.right,rc.bottom,0);
				}
			}
		}
		else
		{
			int colStart = m_nowSumRowCol;
			m_nowSumRowCol += m_specialBandSize[internalParam->frameNow-1];

			for(UINT32 i = internalParam->frameNow-1; i < m_specialBandSize.size(); i++)
			{
				RECT rc = {colStart+(internalParam->frameNow-1)*m_bandSize,0, colStart + internalParam->frameNow*m_bandSize,height};

				colStart += m_specialBandSize[i];
				rc.right = min(rc.right,colStart);

				if(rc.left < rc.right)
				{	
					CMemery::SetPixels(internalParam->param.pBmpData,width*4,rc.left,rc.top,rc.right,rc.bottom,0);
				}
			}	
		}
	}
	else
	{
		if(internalParam->frameNow == 1)
		{
			memset(internalParam->param.pBmpData,0,width*height*4);
		}

		if(internalParam->param.animationEffect == 15)
		{	
			int rowStart = m_nowSumRowCol;
			m_nowSumRowCol += m_specialBandSize[internalParam->frameNow-1];

			for(UINT32 i = internalParam->frameNow-1; i < m_specialBandSize.size(); i++)
			{
				RECT rc = { 0,rowStart+(internalParam->frameNow-1)*m_bandSize,width, rowStart + internalParam->frameNow*m_bandSize};

				rowStart += m_specialBandSize[i];
				rc.bottom = min(rc.bottom,rowStart);

				if(rc.top < rc.bottom)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,rc.left,rc.top,rc.right,rc.bottom,internalParam->bmpDataCopy,width*4,rc.left,rc.top);
				}
			}
		}
		else
		{
			int colStart = m_nowSumRowCol;
			m_nowSumRowCol += m_specialBandSize[internalParam->frameNow-1];

			for(UINT32 i = internalParam->frameNow-1; i < m_specialBandSize.size(); i++)
			{
				RECT rc = {colStart+(internalParam->frameNow-1)*m_bandSize,0, colStart + internalParam->frameNow*m_bandSize,height};

				colStart += m_specialBandSize[i];
				rc.right = min(rc.right,colStart);

				if(rc.left < rc.right)
				{	
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,rc.left,rc.top,rc.right,rc.bottom,internalParam->bmpDataCopy,width*4,rc.left,rc.top);
				}
			}	
		}
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CSepcialBlindsEffect::ReleaseEffectParam()
{
	m_specialBandSize.clear();
}

void CScanEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;

	if (!internalParam->param.bShow)
	{	
		if(internalParam->param.animationEffect == 17)
		{
			int right = width - (internalParam->frameNow-1)*m_step; 
			int left = max(right - m_step,0);

			CMemery::SetPixels(internalParam->param.pBmpData,width*4,left,0, right ,height,0);	
		}
		else if(internalParam->param.animationEffect == 18)
		{
			int left = (internalParam->frameNow-1)*m_step;
			int right = min(left + m_step, width); 

			CMemery::SetPixels(internalParam->param.pBmpData,width*4,left,0, right ,height,0);
		}
		else if(internalParam->param.animationEffect == 19)
		{
			int top = (internalParam->frameNow-1)*m_step;
			int bottom =  min(top + m_step, height);

			CMemery::SetPixels(internalParam->param.pBmpData,width*4,0,top,width,bottom,0);	
		}
		else/* if(internalParam->param.animationEffect == 20)*/
		{
			int bottom = height - (internalParam->frameNow-1)*m_step; 
			int top = max(bottom - m_step,0);

			CMemery::SetPixels(internalParam->param.pBmpData,width*4,0,top,width,bottom,0);	
		}
	}
	else
	{
		if(internalParam->frameNow == 1)
		{
			memset(internalParam->param.pBmpData,0,width*height*4);
		}

		if(internalParam->param.animationEffect == 17)
		{
			int right = width - (internalParam->frameNow-1)*m_step; 
			int left = max(right - m_step,0);

			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,left, 0,right ,height,internalParam->bmpDataCopy,width*4,left,0);
		}
		else if(internalParam->param.animationEffect == 18)
		{
			int left = (internalParam->frameNow-1)*m_step;
			int right = min(left + m_step, width);

			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,left, 0,right ,height,internalParam->bmpDataCopy,width*4,left,0);
		}
		else if(internalParam->param.animationEffect == 19)
		{
			int top = (internalParam->frameNow-1)*m_step;
			int bottom =  min(top + m_step, height);

			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, top,width ,bottom,internalParam->bmpDataCopy,width*4,0,top);
		}
		else/* if(internalParam->param.animationEffect == 20)*/
		{
			int bottom = height - (internalParam->frameNow-1)*m_step; 
			int top = max(bottom - m_step,0);

			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, top,width ,bottom,internalParam->bmpDataCopy,width*4,0,top);
		}
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CScanEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	int frame = 10;
	if(internalParam->param.animationEffect == 17 || internalParam->param.animationEffect == 18)
	{
		CalculateFrame(internalParam->param.bmpSize.cx);
	}
	else
	{
		CalculateFrame(internalParam->param.bmpSize.cy);
	}
}

void CScanEffect::ReleaseEffectParam()
{
}

void CScanEffect::CalculateFrame(int length)
{
	int frame = 10;

	m_step = length/frame;
	if(length%frame != 0)
	{
		m_effectFrame = frame + 1;
	}
	else
	{
		m_effectFrame = frame;
	}
}

void CInertiaSlideEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;

	memset(internalParam->param.pBmpData,0,width*height*4);

	if (!internalParam->param.bShow)
	{	
		if(internalParam->param.animationEffect == 21)
		{
			RECT rcCopy = { (internalParam->frameNow-1)*m_step, 0, width,height }; 
			if(rcCopy.left < rcCopy.right)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0, width-rcCopy.left ,height,internalParam->bmpDataCopy,width*4,rcCopy.left,rcCopy.top);
			}
		}
		else if(internalParam->param.animationEffect == 22)
		{
			RECT rcCopy = { (internalParam->frameNow-1)*m_step, 0, width,height }; 
			if(rcCopy.left < rcCopy.right)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,rcCopy.left,0, width ,height,internalParam->bmpDataCopy,width*4,0,0);
			}
		}
		else if(internalParam->param.animationEffect == 23)
		{
			RECT rcCopy = { 0,(internalParam->frameNow-1)*m_step, width,height }; 
			if(rcCopy.top < rcCopy.bottom)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,rcCopy.top,width,height,internalParam->bmpDataCopy,width*4,0,0);
			}
		}
		else/* if(internalParam->param.animationEffect == 24)*/
		{
			RECT rcCopy = { 0,(internalParam->frameNow-1)*m_step, width,height }; 
			if(rcCopy.top < rcCopy.bottom)
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0,width,height-rcCopy.top,internalParam->bmpDataCopy,width*4,rcCopy.left,rcCopy.top);
			}
		}

		if ((int)internalParam->frameNow == m_effectFrame)
		{
			internalParam->bLastFrame = TRUE;
		}
	}
	else
	{
		if ((int)internalParam->frameNow <= m_effectFrame)
		{		
			if(internalParam->param.animationEffect == 21)
			{
				RECT rcCopy = { (internalParam->frameNow-1)*m_step, 0, width,height }; 
				if(rcCopy.left < rcCopy.right)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,width-rcCopy.left, 0,width ,height,internalParam->bmpDataCopy,width*4,0,0);
				}
				else
				{
					memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
				}
			}
			else if(internalParam->param.animationEffect == 22)
			{
				RECT rcCopy = { (internalParam->frameNow-1)*m_step, 0, width,height }; 
				if(rcCopy.left < rcCopy.right)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0, rcCopy.left ,height,internalParam->bmpDataCopy,width*4,width-rcCopy.left,rcCopy.top);
				}
				else
				{
					memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
				}
			}
			else if(internalParam->param.animationEffect == 23)
			{
				RECT rcCopy = { 0,(internalParam->frameNow-1)*m_step, width,height }; 
				if(rcCopy.top < rcCopy.bottom)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0,width,rcCopy.top,internalParam->bmpDataCopy,width*4,rcCopy.left,height - rcCopy.top);
				}
				else
				{
					memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
				}
			}
			else/* if(internalParam->param.animationEffect == 24)*/
			{
				RECT rcCopy = { 0,(internalParam->frameNow-1)*m_step, width,height }; 
				if(rcCopy.top < rcCopy.bottom)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,height-rcCopy.top,width,height,internalParam->bmpDataCopy,width*4,0,0);
				}
				else
				{
					memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
				}
			}	
		}
		else
		{
			Motion::CUnderDamp* pUnderDamp = (Motion::CUnderDamp*)m_inertiaInstance;
			int inertiaDistance = (int)pUnderDamp->GetNowAmplitude(m_tInertiaTime);

			if(internalParam->param.animationEffect == 21)
			{
				if(inertiaDistance > 0)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, 0,width-inertiaDistance ,height,internalParam->bmpDataCopy,width*4,inertiaDistance,0);
				}
				else
				{
					inertiaDistance = -inertiaDistance;
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,inertiaDistance, 0,width ,height,internalParam->bmpDataCopy,width*4,0,0);
				}
			}
			else if(internalParam->param.animationEffect == 22)
			{
				if(inertiaDistance < 0)
				{
					inertiaDistance = -inertiaDistance;
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, 0,width-inertiaDistance ,height,internalParam->bmpDataCopy,width*4,inertiaDistance,0);
				}
				else
				{
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,inertiaDistance, 0,width ,height,internalParam->bmpDataCopy,width*4,0,0);
				}
			}
			else if(internalParam->param.animationEffect == 23)
			{	
				if(inertiaDistance > 0)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, inertiaDistance,width ,height,internalParam->bmpDataCopy,width*4,0,0);
				}
				else
				{
					inertiaDistance = -inertiaDistance;
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, 0,width ,height-inertiaDistance,internalParam->bmpDataCopy,width*4,0,inertiaDistance);
				}
			}
			else/* if(internalParam->param.animationEffect == 24)*/
			{
				if(inertiaDistance < 0)
				{
					inertiaDistance = -inertiaDistance;
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, inertiaDistance,width ,height,internalParam->bmpDataCopy,width*4,0,0);
				}
				else
				{
					CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, 0,width ,height-inertiaDistance,internalParam->bmpDataCopy,width*4,0,inertiaDistance);
				}
			}	
			
			m_tInertiaTime += M_PI/pUnderDamp->GetAngularFrequency()/m_tInertiacoeffecient;
			// 惯性运动结束条件
			if (inertiaDistance == (int)pUnderDamp->GetNowAmplitude(m_tInertiaTime))
			{
				internalParam->bLastFrame = TRUE;
			}
		}
	}
}

void CInertiaSlideEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	int frame = 10;
	if(internalParam->param.animationEffect == 21 || internalParam->param.animationEffect == 22)
	{
		CalculateFrame(internalParam->param.bmpSize.cx);
	}
	else
	{
		CalculateFrame(internalParam->param.bmpSize.cy);
	}

	m_tInertiaTime = 0;
	m_tInertiacoeffecient = 6.0f;

	Motion::CUnderDamp* pUnderDamp = new Motion::CUnderDamp();
	m_inertiaInstance = pUnderDamp;

	pUnderDamp->SetAmplitude((real)m_step);
	pUnderDamp->SetDamp(0.9f);
	pUnderDamp->SetAngularFrequency(2.0f);
}

void CInertiaSlideEffect::ReleaseEffectParam()
{
	if(m_inertiaInstance != NULL)
	{
		Motion::CUnderDamp* pUnderDamp = (Motion::CUnderDamp*)m_inertiaInstance;
		delete pUnderDamp;
	}
}

void CFadeEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{

	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;

	if (!internalParam->param.bShow)
	{		
		CAlpha::SetAlpha(internalParam->param.pBmpData,width,height,1.0f-internalParam->frameNow*0.1f);
	}
	else
	{
		memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
		CAlpha::SetAlpha(internalParam->param.pBmpData,width,height,internalParam->frameNow*0.1f);
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CFadeEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 10;
}

void CFadeEffect::ReleaseEffectParam()
{

}

void CZoomEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};

	// 构造要扭曲的矩形数据下标
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};
	CdRect rc(rcSrc);
	
	if(!internalParam->param.bShow)
	{
		m_rcNow.InflateRect(ToNearestInt(rc.Width()/10),ToNearestInt(rc.Height()/10));

		WarpRect rcWarp(m_rcNow);
		CWarp::WarpImage((BYTE*)pImgData,rcSrc,rcWarp,rcClient,(BYTE*)internalParam->param.pBmpData,Color(0,0,0,0),TRUE);
	}
	else
	{	
		if((int)internalParam->frameNow == m_effectFrame)
		{
			m_rcNow = rc;
		}
		else
		{
			m_rcNow.InflateRect(ToNearestInt(rc.Width()*0.04),ToNearestInt(rc.Height()*0.04));
		}

		WarpRect rcWarp(m_rcNow);
		CWarp::WarpImage((BYTE*)pImgData,rcSrc,rcWarp,rcClient,(BYTE*)internalParam->param.pBmpData,Color(0,0,0,0),TRUE);
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CZoomEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 10;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};
	m_rcNow = CdRect(rcSrc);

	if(internalParam->param.bShow)
	{
		m_rcNow.DeflateRect(ToNearestInt(m_rcNow.Width()*0.4),ToNearestInt(m_rcNow.Height()*0.4));
	}
}

void CZoomEffect::ReleaseEffectParam()
{

}

void CShrinkEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};

	// 构造要扭曲的矩形数据下标
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};
	CdRect rc(rcSrc);

	if(!internalParam->param.bShow)
	{
		if((int)internalParam->frameNow == m_effectFrame)
		{
			memset((BYTE*)internalParam->param.pBmpData,0,internalParam->param.bmpSize.cx*internalParam->param.bmpSize.cy*4);
		}
		else
		{
			m_rcNow.DeflateRect(rc.Width()/20,rc.Height()/20);
			WarpRect rcWarp(m_rcNow);
			CWarp::WarpImage((BYTE*)pImgData,rcSrc,rcWarp,rcClient,(BYTE*)internalParam->param.pBmpData,Color(0,0,0,0),TRUE);
		}		
	}
	else
	{	
		if((int)internalParam->frameNow == m_effectFrame)
		{
			m_rcNow = rc;
		}
		else
		{
			m_rcNow.DeflateRect(ToNearestInt(rc.Width()*0.1),ToNearestInt(rc.Height()*0.1));
		}

		WarpRect rcWarp(m_rcNow);
		CWarp::WarpImage((BYTE*)pImgData,rcSrc,rcWarp,rcClient,(BYTE*)internalParam->param.pBmpData,Color(0,0,0,0),TRUE);
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CShrinkEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 10;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};
	m_rcNow = CdRect(rcSrc);

	if(internalParam->param.bShow)
	{
		m_rcNow.InflateRect(ToNearestInt(m_rcNow.Width()),ToNearestInt(m_rcNow.Height()));
	}
}

void CShrinkEffect::ReleaseEffectParam()
{

}

void CSwingEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};

	CdRect rc(rcSrc);
	WarpRect rcWarp;
	CProjection mtxProject;

	CdPoint ptCenter(rc.right,rc.GetCenter().y);
	if(!internalParam->param.bShow)
	{
		memset(internalParam->param.pBmpData,0,rcClient.right*rcClient.bottom*4);

		if(internalParam->frameNow != m_effectFrame)
		{
			mtxProject.Reset();
			mtxProject.RotateY(-((int)internalParam->frameNow-1)/24.0f*M_PI);	

			mtxProject.ProjectRect(rc,rcWarp,ptCenter);

			// 构造外包框
			RECT boundRect = rcWarp.GetBoundRect().ToRECT();
			RECT rcBound = {boundRect.left,boundRect.top,boundRect.right+1,boundRect.bottom+1};

			int wrapWidth = (rcBound.right - rcBound.left);
			int wrapHeight = (rcBound.bottom - rcBound.top);
			BYTE* dataWrap = new BYTE[wrapWidth*wrapHeight*4];

			CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcBound,dataWrap,Color(0,0,0,0),TRUE);

			CMemery::CopyPixels(internalParam->param.pBmpData,rcClient.right*4,0,0,wrapWidth,wrapHeight,dataWrap,wrapWidth*4,0,0);

			delete []dataWrap;
		}
	}
	else
	{
		mtxProject.Reset();
		mtxProject.RotateY(-(m_effectFrame-(int)internalParam->frameNow)/24.0f*M_PI);	

		mtxProject.ProjectRect(rc,rcWarp,ptCenter);

		// 构造外包框
		RECT boundRect = rcWarp.GetBoundRect().ToRECT();
		RECT rcBound = {boundRect.left,boundRect.top,boundRect.right+1,boundRect.bottom+1};

		int wrapWidth = (rcBound.right - rcBound.left);
		int wrapHeight = (rcBound.bottom - rcBound.top);
		BYTE* dataWrap = new BYTE[wrapWidth*wrapHeight*4];
		memset(internalParam->param.pBmpData,0,rcClient.right*rcClient.bottom*4);

		CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcBound,dataWrap,Color(0,0,0,0),TRUE);

		CMemery::CopyPixels(internalParam->param.pBmpData,rcClient.right*4,0,0,wrapWidth,wrapHeight,dataWrap,wrapWidth*4,0,0);

		delete []dataWrap;			
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}

}

void CSwingEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 12;
}

void CSwingEffect::ReleaseEffectParam()
{

}

void CFunnelEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};

	CdRect rc(rcSrc);
	WarpRect rcWarp(rc);
	CProjection mtxProject;

	CdPoint ptCenter(rc.right,rc.GetCenter().y);
	if(!internalParam->param.bShow)
	{
		int step;
		if(internalParam->frameNow <= 3)
		{
			step = (int)(rc.Height()/8.0f);
			rcWarp.topLeft.Offset(0,step*internalParam->frameNow);
			rcWarp.bottomLeft.Offset(0,-(int)(step*internalParam->frameNow));
			CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color(0,0,0,0),TRUE);
		}
		else if((int)internalParam->frameNow < m_effectFrame)
		{
			step = (int)(rc.Width()/8.0f);
			rcWarp.topLeft.Offset((int)step*(internalParam->frameNow-3),(int)(rc.Height()/8.0f)*3);
			rcWarp.bottomLeft.Offset((int)step*(internalParam->frameNow-3),-(int)(rc.Height()/8.0f)*3);
			real k1 = (real)(rc.top - rcWarp.topLeft.y)/rc.Width();
			real k2 = (real)(rc.bottom - rcWarp.bottomLeft.y)/rc.Width();

			rcWarp.topRight = CdPoint(rcClient.right-1,ToNearestInt(k1*(rcClient.right-(int)step*(internalParam->frameNow-3))+rcWarp.topLeft.y));
			rcWarp.bottomRight = CdPoint(rcClient.right-1,ToNearestInt(k2*(rcClient.right-(int)step*(internalParam->frameNow-3))+rcWarp.bottomLeft.y));

			RECT rcSrc = {(int)step*(internalParam->frameNow-3),rcClient.top,rcClient.right-1,rcClient.bottom-1};
			RECT rcBound = {rcSrc.left,rcSrc.top,rcSrc.right+1,rcSrc.bottom+1};
			int wrapWidth = (rcBound.right - rcBound.left);
			int wrapHeight = (rcBound.bottom - rcBound.top);
			BYTE* dataWrap = new BYTE[wrapWidth*wrapHeight*4];

			memset(internalParam->param.pBmpData,0,rcClient.right*rcClient.bottom*4);
			CWarp::WarpImage(pImgData,rcClient.right,rcClient.bottom,rcSrc,rcWarp,rcBound,dataWrap,Color(0,0,0,0),TRUE);

			CMemery::CopyPixels(internalParam->param.pBmpData,rcClient.right*4,0,0,wrapWidth,wrapHeight,dataWrap,wrapWidth*4,0,0);

			delete []dataWrap;			

		}
		else
		{
			memset(internalParam->param.pBmpData,0,internalParam->param.bmpSize.cx*internalParam->param.bmpSize.cy*4);
		}
	}
	else
	{
		int step;
		if ((int)internalParam->frameNow == m_effectFrame)
		{
			memcpy(internalParam->param.pBmpData,pImgData,internalParam->param.bmpSize.cx*internalParam->param.bmpSize.cy*4);
		}
		else if(internalParam->frameNow > 7)
		{
			step = (int)(rc.Height()/8.0f);
			rcWarp.topLeft.Offset(0,step*(m_effectFrame-internalParam->frameNow));
			rcWarp.bottomLeft.Offset(0,-(int)(step*(m_effectFrame-internalParam->frameNow)));
			CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color(0,0,0,0),TRUE);
		}
		else if((int)internalParam->frameNow < m_effectFrame)
		{
			step = (int)(rc.Width()/8.0f);
			rcWarp.topLeft.Offset((int)step*(8-internalParam->frameNow),(int)(rc.Height()/8.0f)*3);
			rcWarp.bottomLeft.Offset((int)step*(8-internalParam->frameNow),-(int)(rc.Height()/8.0f)*3);
			real k1 = (real)(rc.top - rcWarp.topLeft.y)/rc.Width();
			real k2 = (real)(rc.bottom - rcWarp.bottomLeft.y)/rc.Width();

			rcWarp.topRight = CdPoint(rcClient.right-1,ToNearestInt(k1*(int)step*(internalParam->frameNow)+rcWarp.topLeft.y));
			rcWarp.bottomRight = CdPoint(rcClient.right-1,ToNearestInt(k2*(int)step*(internalParam->frameNow)+rcWarp.bottomLeft.y));

			RECT rcSrc = {(int)step*(8-internalParam->frameNow),rcClient.top,rcClient.right-1,rcClient.bottom-1};
			RECT rcBound = {rcSrc.left,rcSrc.top,rcSrc.right+1,rcSrc.bottom+1};
			int wrapWidth = (rcBound.right - rcBound.left);
			int wrapHeight = (rcBound.bottom - rcBound.top);
			BYTE* dataWrap = new BYTE[wrapWidth*wrapHeight*4];

			memset(internalParam->param.pBmpData,0,rcClient.right*rcClient.bottom*4);
			CWarp::WarpImage(pImgData,rcClient.right,rcClient.bottom,rcSrc,rcWarp,rcBound,dataWrap,Color(0,0,0,0),TRUE);

			CMemery::CopyPixels(internalParam->param.pBmpData,rcClient.right*4,0,0,wrapWidth,wrapHeight,dataWrap,wrapWidth*4,0,0);

			delete []dataWrap;			

		}
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CFunnelEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 11;
}

void CFunnelEffect::ReleaseEffectParam()
{

}

void CNoiseEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long width = internalParam->param.bmpSize.cx;
	long height = internalParam->param.bmpSize.cy;

	if(!internalParam->param.bShow)
	{		
		BYTE *ptrSrc = internalParam->param.pBmpData;
		memcpy(ptrSrc,internalParam->bmpDataCopy,width*4*height);

		m_u += 4;
		for(long i = 0; i < height; i++)
		{
			for(long j = 0; j < width;j++)
			{
				BYTE valueChange = (BYTE)(m_u * (rand()*6.1037019e-5 - 1.0f));
				ptrSrc[0] += valueChange;
				ptrSrc[1] += valueChange;
				ptrSrc[2] += valueChange;

				ptrSrc += 4;
			}
		}
		ptrSrc += 4;
	}
	else
	{
		BYTE *ptrSrc = internalParam->param.pBmpData;
		memcpy(ptrSrc,internalParam->bmpDataCopy,width*4*height);

		m_u -= 4;
		for(long i = 0; i < height; i++)
		{
			for(long j = 0; j < width;j++)
			{
				BYTE valueChange = (BYTE)(m_u * (rand()*6.1037019e-5 - 1.0f));
				ptrSrc[0] += valueChange;
				ptrSrc[1] += valueChange;
				ptrSrc[2] += valueChange;

				ptrSrc += 4;
			}
		}
		ptrSrc += 4;

	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}

}

void CNoiseEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 25;
	if(!internalParam->param.bShow)
	{
		m_u = 0;
	}
	else
	{
		m_u = 100;
	}

	srand((unsigned)time(NULL));
}

void CNoiseEffect::ReleaseEffectParam()
{

}

void CCubeEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};

	CdRect rc(rcSrc);
	WarpRect rcWarp[2];
	CProjection mtxProject;
	
	memset(internalParam->param.pBmpData,0,rcClient.right*rcClient.bottom*4);

	if(internalParam->param.animationEffect == 31)
	{
		mtxProject.Reset();
		mtxProject.RotateY(-(int)(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.CubeProjectRect(rc,rcWarp[0],0,rc.Width()/2);
		mtxProject.CubeProjectRect(rc,rcWarp[1],1,rc.Width()/2);

		if(rcWarp[0].topRight.x > rcWarp[0].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

		if(rcWarp[1].topRight.x > rcWarp[1].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}
	else if(internalParam->param.animationEffect == 32)
	{
		mtxProject.Reset();
		mtxProject.RotateY((internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.CubeProjectRect(rc,rcWarp[0],0,rc.Width()/2);
		mtxProject.CubeProjectRect(rc,rcWarp[1],2,rc.Width()/2);


		if(rcWarp[0].topRight.x > rcWarp[0].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

		if(rcWarp[1].topRight.x > rcWarp[1].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

	}
	else if(internalParam->param.animationEffect == 34)
	{
		mtxProject.Reset();
		mtxProject.RotateX(-(int)(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.CubeProjectRect(rc,rcWarp[0],0,rc.Height()/2);
		mtxProject.CubeProjectRect(rc,rcWarp[1],5,rc.Height()/2);

		if(rcWarp[0].topLeft.y < rcWarp[0].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

		if(rcWarp[1].topLeft.y < rcWarp[1].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}
	else /*if(internalParam->param.animationEffect == 33)*/
	{
		mtxProject.Reset();
		mtxProject.RotateX((internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.CubeProjectRect(rc,rcWarp[0],0,rc.Height()/2);
		mtxProject.CubeProjectRect(rc,rcWarp[1],4,rc.Height()/2);

		if(rcWarp[0].topLeft.y < rcWarp[0].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

		if(rcWarp[1].topLeft.y < rcWarp[1].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CCubeEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 13;
}

void CCubeEffect::ReleaseEffectParam()
{
}

void CGradualEraseEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;

	if(!internalParam->param.bShow)
	{
		if(internalParam->frameNow == m_effectFrame)
		{
			memset(internalParam->param.pBmpData,0,width*height*4);
		}
		else
		{
			if(internalParam->param.animationEffect == 35)
			{
				CMemery::SetPixels(internalParam->param.pBmpData,width*4,m_lastStart,0, m_lastEnd ,height,0);

				int right = width - (internalParam->frameNow-1)*m_step; 
				int left = max(right - m_step,0);

				for (int i = left; i < right; i++)
				{
					CAlpha::SetAlpha(internalParam->param.pBmpData,width*4,i,0,i+1,height,1-(i-left)*m_alphaStep);
				}

				m_lastStart = left;
				m_lastEnd = right;
			}
			else if(internalParam->param.animationEffect == 36)
			{
				CMemery::SetPixels(internalParam->param.pBmpData,width*4,m_lastStart,0, m_lastEnd ,height,0);

				int left = (internalParam->frameNow-1)*m_step;
				int right = min(left + m_step, width); 

				for (int i = left; i < right; i++)
				{
					CAlpha::SetAlpha(internalParam->param.pBmpData,width*4,i,0,i+1,height,(i-left)*m_alphaStep);
				}

				m_lastStart = left;
				m_lastEnd = right;
			}
			else if(internalParam->param.animationEffect == 37)
			{
				CMemery::SetPixels(internalParam->param.pBmpData,width*4,0,m_lastStart,width,m_lastEnd,0);	

				int top = (internalParam->frameNow-1)*m_step;
				int bottom =  min(top + m_step, height);

				for (int i = top; i < bottom; i++)
				{
					CAlpha::SetAlpha(internalParam->param.pBmpData,width*4,0,i,width,i+1,(i-top)*m_alphaStep);
				}

				m_lastStart = top;
				m_lastEnd = bottom;
		
			}
			else/* if(internalParam->param.animationEffect == 38)*/
			{
				CMemery::SetPixels(internalParam->param.pBmpData,width*4,0,m_lastStart,width,m_lastEnd,0);	

				int bottom = height - (internalParam->frameNow-1)*m_step; 
				int top = max(bottom - m_step,0);

				for (int i = top; i < bottom; i++)
				{
					CAlpha::SetAlpha(internalParam->param.pBmpData,width*4,0,i,width,i+1,1-(i-top)*m_alphaStep);
				}

				m_lastStart = top;
				m_lastEnd = bottom;
			}
		}
	}
	else
	{
		if(internalParam->param.animationEffect == 35)
		{
			if(internalParam->frameNow == 1)
			{
				memset(internalParam->param.pBmpData,0,width*height*4);
			}
			else
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,m_lastStart, 0,m_lastEnd ,height,internalParam->bmpDataCopy,width*4,m_lastStart,0);
			}

			int right = width - (internalParam->frameNow-1)*m_step; 
			int left = max(right - m_step,0);

			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,left, 0,right ,height,internalParam->bmpDataCopy,width*4,left,0);

			for (int i = left; i < right; i++)
			{
				CAlpha::SetAlpha(internalParam->param.pBmpData,width*4,i,0,i+1,height,(i-left)*m_alphaStep);
			}

			m_lastStart = left;
			m_lastEnd = right;
		}
		else if(internalParam->param.animationEffect == 36)
		{
			if(internalParam->frameNow == 1)
			{
				memset(internalParam->param.pBmpData,0,width*height*4);
			}
			else
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,m_lastStart, 0,m_lastEnd ,height,internalParam->bmpDataCopy,width*4,m_lastStart,0);
			}

			int left = (internalParam->frameNow-1)*m_step;
			int right = min(left + m_step, width);

			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,left, 0,right ,height,internalParam->bmpDataCopy,width*4,left,0);

			for (int i = left; i < right; i++)
			{
				CAlpha::SetAlpha(internalParam->param.pBmpData,width*4,i,0,i+1,height,1-(i-left)*m_alphaStep);
			}

			m_lastStart = left;
			m_lastEnd = right;
		}
		else if(internalParam->param.animationEffect == 37)
		{
			if(internalParam->frameNow == 1)
			{
				memset(internalParam->param.pBmpData,0,width*height*4);
			}
			else
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,m_lastStart,width,m_lastEnd,internalParam->bmpDataCopy,width*4,0,m_lastStart);
			}

			int top = (internalParam->frameNow-1)*m_step;
			int bottom =  min(top + m_step, height);

			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, top,width ,bottom,internalParam->bmpDataCopy,width*4,0,top);

			for (int i = top; i < bottom; i++)
			{
				CAlpha::SetAlpha(internalParam->param.pBmpData,width*4,0,i,width,i+1,1-(i-top)*m_alphaStep);
			}

			m_lastStart = top;
			m_lastEnd = bottom;
		}
		else /*if(internalParam->param.animationEffect == 38)*/
		{
			if(internalParam->frameNow == 1)
			{
				memset(internalParam->param.pBmpData,0,width*height*4);
			}
			else
			{
				CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,m_lastStart,width,m_lastEnd,internalParam->bmpDataCopy,width*4,0,m_lastStart);
			}

			int bottom = height - (internalParam->frameNow-1)*m_step; 
			int top = max(bottom - m_step,0);

			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, top,width ,bottom,internalParam->bmpDataCopy,width*4,0,top);

			for (int i = top; i < bottom; i++)
			{
				CAlpha::SetAlpha(internalParam->param.pBmpData,width*4,0,i,width,i+1,(i-top)*m_alphaStep);
			}

			m_lastStart = top;
			m_lastEnd = bottom;
		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CGradualEraseEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	int frame = 10;
	if(internalParam->param.animationEffect == 35 || internalParam->param.animationEffect == 36)
	{
		CalculateFrame(internalParam->param.bmpSize.cx);
	}
	else
	{
		CalculateFrame(internalParam->param.bmpSize.cy);
	}

	m_alphaStep = 1.0f/m_step;

	m_lastStart = m_lastEnd = 0;
	
}

void CGradualEraseEffect::ReleaseEffectParam()
{

}

void CPushEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};

	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};
	CdRect rc(rcSrc);

	if(!internalParam->param.bShow)
	{
		if(internalParam->param.animationEffect == 39)
		{
			rc.right = max(0,rc.right-(int)internalParam->frameNow*rcClient.right/10.0f);
		}
		else if(internalParam->param.animationEffect == 40)
		{
			rc.left = min(rc.right,(int)internalParam->frameNow*rcClient.right/10.0f);
		}
		else if(internalParam->param.animationEffect == 42)
		{
			rc.bottom = max(0,rc.bottom-(int)internalParam->frameNow*rcClient.bottom/10.0f);
		}
		else /*if(internalParam->param.animationEffect == 41)*/
		{
			rc.top = min(rc.bottom,(int)internalParam->frameNow*rcClient.bottom/10.0f);
		}
	}
	else
	{
		if(internalParam->param.animationEffect == 39)
		{
			rc.left = max(0,rc.right-(int)internalParam->frameNow*rcClient.right/10.0f);
		}
		else if(internalParam->param.animationEffect == 40)
		{
			rc.right = min(rc.right,(int)internalParam->frameNow*rcClient.right/10.0f);
		}
		else if(internalParam->param.animationEffect == 42)
		{
			rc.top = max(0,rc.bottom-(int)internalParam->frameNow*rcClient.bottom/10.0f);
		}
		else /*if(internalParam->param.animationEffect == 41)*/
		{
			rc.bottom = min(rc.bottom,(int)internalParam->frameNow*rcClient.bottom/10.0f);
		}
	}

	WarpRect rcWarp(rc);
	CWarp::WarpImage((BYTE*)pImgData,rcSrc,rcWarp,rcClient,(BYTE*)internalParam->param.pBmpData,Color(0,0,0,0),TRUE);

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}	
}

void CPushEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 11;
}

void CPushEffect::ReleaseEffectParam()
{
}

void CDissolveEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;
	int size = width*height;
	int index;

	if(!internalParam->param.bShow)
	{
		if ((int)internalParam->frameNow == m_effectFrame)
		{
			memset(internalParam->param.pBmpData,0,m_size*4);
		}
		else
		{
			int loop = m_framePixel;
			while(loop)
			{
				if(m_pDissolveArray[index = (rand()*rand())%m_size] == 0)
				{
					internalParam->param.pBmpData[(index<<2)+3] = 0;
					loop--;
					m_pDissolveArray[index] = 1;
				}
			}
		}		
	}
	else
	{
		if ((int)internalParam->frameNow == 1)
		{
			memset(internalParam->param.pBmpData,0,m_size*4);
		}

		if ((int)internalParam->frameNow == m_effectFrame)
		{
			memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,m_size*4);
		}
		else
		{
			int loop = m_framePixel;
			while(loop)
			{
				if(m_pDissolveArray[index = (rand()*rand())%m_size] == 0)
				{
					memcpy(internalParam->param.pBmpData+(index<<2),internalParam->bmpDataCopy+(index<<2),4);
					loop--;
					m_pDissolveArray[index] = 1;
				}
			}
		}
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}

}

void CDissolveEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 8;

	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;
	m_size = width*height;

	m_pDissolveArray = new BYTE[m_size];
	memset(m_pDissolveArray,0,m_size);	

	m_framePixel = m_size/m_effectFrame;

	srand((unsigned)time( NULL ) );
}

void CDissolveEffect::ReleaseEffectParam()
{
	delete []m_pDissolveArray;
}

void CCurtainEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};

	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};
	CdRect rc(rcSrc);

	if(!internalParam->param.bShow)
	{
		rc.right = max(0,rc.right-(int)internalParam->frameNow*rcClient.right/20.0f);
		rc.left = min(rc.right,(int)internalParam->frameNow*rcClient.right/20.0f);
	}
	else
	{
		rc.left = max(0,rc.right/2-(int)internalParam->frameNow*rcClient.right/20.0f);
		rc.right = min(rc.right,rc.right/2+(int)internalParam->frameNow*rcClient.right/20.0f);
	}

	WarpRect rcWarp(rc);
	CWarp::WarpImage((BYTE*)pImgData,rcSrc,rcWarp,rcClient,(BYTE*)internalParam->param.pBmpData,Color(0,0,0,0),TRUE);

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}	
}

void CCurtainEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 11;
}

void CCurtainEffect::ReleaseEffectParam()
{
}

void CTriangularprismEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};

	CdRect rc(rcSrc);
	WarpRect rcWarp[2];
	CProjection mtxProject;

	memset(internalParam->param.pBmpData,0,rcClient.right*rcClient.bottom*4);

	if(internalParam->param.animationEffect == 45)
	{
		mtxProject.Reset();
		mtxProject.RotateY(-(int)(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.HorzPrismProjectRect(rc,rcWarp[0],0);
		mtxProject.HorzPrismProjectRect(rc,rcWarp[1],1);

		if(rcWarp[0].topRight.x > rcWarp[0].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

		if(rcWarp[1].topRight.x > rcWarp[1].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}
	else if(internalParam->param.animationEffect == 46)
	{
		mtxProject.Reset();
		mtxProject.RotateY((int)(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.HorzPrismProjectRect(rc,rcWarp[0],0);
		mtxProject.HorzPrismProjectRect(rc,rcWarp[1],2);

		if(rcWarp[0].topRight.x > rcWarp[0].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

		if(rcWarp[1].topRight.x > rcWarp[1].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}
	else if(internalParam->param.animationEffect == 48)
	{
		mtxProject.Reset();
		mtxProject.RotateX(-(int)(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.VeriPrismProjectRect(rc,rcWarp[0],0);
		mtxProject.VeriPrismProjectRect(rc,rcWarp[1],2);

		if(rcWarp[0].topLeft.y < rcWarp[0].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

		if(rcWarp[1].topLeft.y < rcWarp[1].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}
	else /*if(internalParam->param.animationEffect == 47)*/
	{
		mtxProject.Reset();
		mtxProject.RotateX((int)(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.VeriPrismProjectRect(rc,rcWarp[0],0);
		mtxProject.VeriPrismProjectRect(rc,rcWarp[1],1);

		if(rcWarp[0].topLeft.y < rcWarp[0].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

		if(rcWarp[1].topLeft.y < rcWarp[1].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}

	if ((int)internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CTriangularprismEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 17;
}

void CTriangularprismEffect::ReleaseEffectParam()
{
}

void CCubeInsideEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;

	RECT rcClient = {0,0,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy};
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};

	CdRect rc(rcSrc);
	WarpRect rcWarp[2];
	CProjection mtxProject;

	memset(internalParam->param.pBmpData,0,rcClient.right*rcClient.bottom*4);

	if(internalParam->param.animationEffect == 49)
	{
		mtxProject.Reset();
		mtxProject.RotateY(int(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.CubeInsideProjectRect(rc,rcWarp[0],3,-rc.Width()/2);
		mtxProject.CubeInsideProjectRect(rc,rcWarp[1],1,-rc.Width()/2);

	//	if(rcWarp[0].topRight.x > rcWarp[0].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

	//	if(rcWarp[1].topRight.x > rcWarp[1].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}
	else if(internalParam->param.animationEffect == 50)
	{
		mtxProject.Reset();
		mtxProject.RotateY(-int(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.CubeInsideProjectRect(rc,rcWarp[0],3,-rc.Width()/2);
		mtxProject.CubeInsideProjectRect(rc,rcWarp[1],2,-rc.Width()/2);


	//	if(rcWarp[0].topRight.x > rcWarp[0].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

	//	if(rcWarp[1].topRight.x > rcWarp[1].topLeft.x)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

	}
	else if(internalParam->param.animationEffect == 52)
	{
		mtxProject.Reset();
		mtxProject.RotateX((int)(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.CubeInsideProjectRect(rc,rcWarp[0],3,-rc.Height()/2);
		mtxProject.CubeInsideProjectRect(rc,rcWarp[1],5,-rc.Height()/2);

	//	if(rcWarp[0].topLeft.y < rcWarp[0].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

	//	if(rcWarp[1].topLeft.y < rcWarp[1].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}
	else /*if(internalParam->param.animationEffect == 51)*/
	{
		mtxProject.Reset();
		mtxProject.RotateX(-(int)(internalParam->frameNow-1)/24.0f*M_PI);	

		mtxProject.CubeInsideProjectRect(rc,rcWarp[0],3,-rc.Height()/2);
		mtxProject.CubeInsideProjectRect(rc,rcWarp[1],4,-rc.Height()/2);

	//	if(rcWarp[0].topLeft.y < rcWarp[0].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}

	//	if(rcWarp[1].topLeft.y < rcWarp[1].bottomLeft.y)
		{
			CWarp::WarpImage(pImgData,rcSrc,rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CCubeInsideEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 13;
}

void CCubeInsideEffect::ReleaseEffectParam()
{
}

void CSpatialmotionEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	BYTE *pImgData  = internalParam->bmpDataCopy;
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;

	RECT rcClient = {0,0,width,height};
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};

	CdRect rc(rcSrc);
	WarpRect rcWarp;
	CProjection mtxProject;

	m_zOffset = mtxProject.GetEyePos().m_vector[2]/6.0f;
	memset(internalParam->param.pBmpData,0,width*height*4);

	if(internalParam->param.animationEffect == 53)
	{
		if(internalParam->param.bShow)
		{
			if(internalParam->frameNow <= 6)
			{			
				mtxProject.Reset();
				mtxProject.Translate(m_xyOffset*internalParam->frameNow,0,m_zOffset*int(internalParam->frameNow)-2000);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(-m_xyOffset*int(internalParam->frameNow),0,-m_zOffset*int(internalParam->frameNow));
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
			else
			{	
				int i = (int)internalParam->frameNow - 6;
				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(m_xyOffset*i-0.5*width,0,-1000-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.Translate(0.5*width-m_xyOffset*i,0,-1000+m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
		}
		else
		{
			if(internalParam->frameNow <= 6)
			{	
				int i = (int)internalParam->frameNow;

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(m_xyOffset*i,0,m_zOffset*i-2000);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.Translate(-m_xyOffset*i,0,-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

			}

			else
			{	
				int i = (int)internalParam->frameNow - 6;

				mtxProject.Reset();		
				mtxProject.Translate(m_xyOffset*i-0.5*width,0,-1000-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0.5*width-m_xyOffset*i,0,-1000+m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
		}
	}
	else if(internalParam->param.animationEffect == 54)
	{
		if(internalParam->param.bShow)
		{
			if(internalParam->frameNow <= 6)
			{	
				int i = (int)internalParam->frameNow;

				mtxProject.Reset();
				mtxProject.Translate(-m_xyOffset*i,0,m_zOffset*i-2000);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(m_xyOffset*i,0,-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
			else
			{
				int i = (int)internalParam->frameNow - 6;

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0.5*width-m_xyOffset*i,0,-1000-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.Translate(m_xyOffset*i-0.5*width,0,-1000+m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
		}
		else
		{
			if(internalParam->frameNow <= 6)
			{	
				int i = (int)internalParam->frameNow;

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(-m_xyOffset*i,0,m_zOffset*i-2000);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.Translate(m_xyOffset*i,0,-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
			else
			{
				int i = (int)internalParam->frameNow - 6;

				mtxProject.Reset();		
				mtxProject.Translate(0.5*width-m_xyOffset*i,0,-1000-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(m_xyOffset*i-0.5*width,0,-1000+m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
		}
	}
	else if(internalParam->param.animationEffect == 56)
	{
		if(internalParam->param.bShow)
		{
			if(internalParam->frameNow <= 6)
			{	
				int i = (int)internalParam->frameNow;

				mtxProject.Reset();
				mtxProject.Translate(0,m_xyOffset*i,m_zOffset*i-2000);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0,-m_xyOffset*i,-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
			else
			{
				int i = (int)internalParam->frameNow - 6;

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0,m_xyOffset*i-0.5*height,-1000-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.Translate(0,0.5*height-m_xyOffset*i,-1000+m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

			}
		}
		else
		{
			if(internalParam->frameNow <= 6)
			{	
				int i = (int)internalParam->frameNow;

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0,m_xyOffset*i,m_zOffset*i-2000);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.Translate(0,-m_xyOffset*i,-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
			else
			{
				int i = (int)internalParam->frameNow - 6;


				mtxProject.Reset();		
				mtxProject.Translate(0,m_xyOffset*i-0.5*height,-1000-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0,0.5*height-m_xyOffset*i,-1000+m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
			}
		}
	}
	else /*if(internalParam->param.animationEffect == 55)*/
	{
		if(internalParam->param.bShow)
		{
			if(internalParam->frameNow <= 6)
			{	
				int i = (int)internalParam->frameNow;

				mtxProject.Reset();
				mtxProject.Translate(0,-m_xyOffset*i,m_zOffset*i-2000);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0,m_xyOffset*i,-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

			}
			else
			{
				int i = (int)internalParam->frameNow - 6;

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0,0.5*height-m_xyOffset*i,-1000-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.Translate(0,m_xyOffset*i-0.5*height,-1000+m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

			}
		}
		else
		{
			if(internalParam->frameNow <= 6)
			{	
				int i = (int)internalParam->frameNow;
				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0,-m_xyOffset*i,m_zOffset*i-2000);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.Translate(0,m_xyOffset*i,-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

			}
			else
			{
				int i = (int)internalParam->frameNow - 6;

				mtxProject.Reset();		
				mtxProject.Translate(0,0.5*height-m_xyOffset*i,-1000-m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

				mtxProject.Reset();
				mtxProject.RotateY(M_PI);
				mtxProject.Translate(0,m_xyOffset*i-0.5*height,-1000+m_zOffset*i);
				mtxProject.ProjectRect(rc,rcWarp,rc.GetCenter());	

				CWarp::WarpImage(pImgData,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

			}
		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CSpatialmotionEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 12;

	if(internalParam->param.animationEffect == 53 || internalParam->param.animationEffect == 54)
	{
		m_xyOffset = internalParam->param.bmpSize.cx/12.0f;
	}
	else
	{
		m_xyOffset = internalParam->param.bmpSize.cy/12.0f;
	}
}

void CSpatialmotionEffect::ReleaseEffectParam()
{
}

void CPatchFlipEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;
	RECT rcClient = {0,0,width,height};

	// 构造要扭曲的矩形数据下标
	POINT ptCenter = {width/2,height/2};

	RECT rcSrc[4] = { 0 };

	rcSrc[0].left = rcSrc[0].top = 0;
	rcSrc[0].right = ptCenter.x - 1;
	rcSrc[0].bottom = ptCenter.y -1;

	rcSrc[1].left  = ptCenter.x;
	rcSrc[1].top = 0;
	rcSrc[1].right = width - 1;
	rcSrc[1].bottom = ptCenter.y -1;

	rcSrc[2].left = 0;
	rcSrc[2].top = ptCenter.y;
	rcSrc[2].right = ptCenter.x - 1;
	rcSrc[2].bottom = height -1;

	rcSrc[3].left = ptCenter.x;
	rcSrc[3].top = ptCenter.y;
	rcSrc[3].right = width - 1;
	rcSrc[3].bottom = height  -1;

	CdRect rc[4];
	for(int i = 0; i < 4; i++)
	{
		rc[i] = CdRect(rcSrc[i]);
	}

	// 扭曲矩形与投影矩阵
	WarpRect rcWarp[4];
	CProjection mtxProject[4];

	memset(internalParam->param.pBmpData,0,width*height*4);

	if(!internalParam->param.bShow)
	{
		for(int j = 0; j < 4; j++)
		{
			mtxProject[j].Reset();
			if(j==0 || j==3)
			{
				mtxProject[j].RotateX(internalParam->frameNow/24.0f*M_PI);
			}
			else
			{
				mtxProject[j].RotateY(internalParam->frameNow/24.0f*M_PI);
			}

			mtxProject[j].ProjectRect(rc[j],rcWarp[j],rc[j].GetCenter());

			CWarp::WarpImage(internalParam->bmpDataCopy,width,height,rcSrc[j],rcWarp[j],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}
	else
	{
		for(int j = 0; j < 4; j++)
		{
			mtxProject[j].Reset();
			if(j==0 || j==3)
			{
				mtxProject[j].RotateX((internalParam->frameNow+24)/24.0f*M_PI);
			}
			else
			{
				mtxProject[j].RotateY((internalParam->frameNow+24)/24.0f*M_PI);
			}

			mtxProject[j].ProjectRect(rc[j],rcWarp[j],rc[j].GetCenter());

			CWarp::WarpImage(internalParam->bmpDataCopy,width,height,rcSrc[j],rcWarp[j],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CPatchFlipEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 24;
}

void CPatchFlipEffect::ReleaseEffectParam()
{
}

void CSpatialRotateEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;
	RECT rcClient = {0,0,width,height};
	RECT rcSrc = {rcClient.left,rcClient.top,rcClient.right-1,rcClient.bottom-1};
	CdRect rc(rcSrc);
	WarpRect rcWarp;

	CProjection mtxProject;
	if(internalParam->param.animationEffect == 58)
	{
		mtxProject.Reset();
		mtxProject.RotateY(-((int)internalParam->frameNow-1)/12.0f*M_PI);	

		mtxProject.CubeProjectRect(rc,rcWarp,0,rc.Width()/2);
	}
	else if(internalParam->param.animationEffect == 59)
	{
		mtxProject.Reset();
		mtxProject.RotateY(((int)internalParam->frameNow-1)/12.0f*M_PI);	

		mtxProject.CubeProjectRect(rc,rcWarp,0,rc.Width()/2);
	}
	else if(internalParam->param.animationEffect == 61)
	{
		mtxProject.Reset();
		mtxProject.RotateX(-((int)internalParam->frameNow-1)/12.0f*M_PI);	

		mtxProject.CubeProjectRect(rc,rcWarp,0,rc.Width()/2);
	}
	else/* if(internalParam->param.animationEffect == 60)*/
	{
		mtxProject.Reset();
		mtxProject.RotateX(((int)internalParam->frameNow-1)/12.0f*M_PI);	

		mtxProject.CubeProjectRect(rc,rcWarp,0,rc.Width()/2);
	}

	memset(internalParam->param.pBmpData,0,width*height*4);
	CWarp::WarpImage(internalParam->bmpDataCopy,rcSrc,rcWarp,rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CSpatialRotateEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 25;
}

void CSpatialRotateEffect::ReleaseEffectParam()
{
}

void CDoorEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;
	RECT rcClient = {0,0,width,height};

	POINT ptCenter = {width/2,height/2};
	RECT rcSrc[2] = { 0 };

	rcSrc[0].left = rcSrc[0].top = 0;
	rcSrc[0].right = ptCenter.x - 1;
	rcSrc[0].bottom = height -1;

	rcSrc[1].left  = ptCenter.x;
	rcSrc[1].top = 0;
	rcSrc[1].right = width - 1;
	rcSrc[1].bottom = height -1;

	CdRect rc[2];
	CdPoint ptRotate[2];
	for(int i = 0; i < 2; i++)
	{
		rc[i] = CdRect(rcSrc[i]);
		if(!i)
		{
			ptRotate[i] = CdPoint(rcSrc[i].left,ptCenter.y);
		}
		else
		{
			ptRotate[i] = CdPoint(rcSrc[i].right,ptCenter.y);
		}
	}

	WarpRect rcWarp[2];
	CProjection mtxProject[2];

	memset(internalParam->param.pBmpData,0,width*height*4);

	if(!internalParam->param.bShow)
	{
		int i = (int)internalParam->frameNow;

		mtxProject[0].Reset();
		mtxProject[0].RotateY(i/24.0f*M_PI);
		mtxProject[0].ProjectRect(rc[0],rcWarp[0],ptRotate[0]);

		CWarp::WarpImage(internalParam->bmpDataCopy,width,height,rcSrc[0],rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

		mtxProject[1].Reset();
		mtxProject[1].RotateY(-i/24.0f*M_PI);
		mtxProject[1].ProjectRect(rc[1],rcWarp[1],ptRotate[1]);

		CWarp::WarpImage(internalParam->bmpDataCopy,width,height,rcSrc[1],rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

	}
	else
	{
		int i = m_effectFrame - (int)internalParam->frameNow;

		mtxProject[0].Reset();
		mtxProject[0].RotateY(i/24.0f*M_PI);
		mtxProject[0].ProjectRect(rc[0],rcWarp[0],ptRotate[0]);

		CWarp::WarpImage(internalParam->bmpDataCopy,width,height,rcSrc[0],rcWarp[0],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);

		mtxProject[1].Reset();
		mtxProject[1].RotateY(-i/24.0f*M_PI);
		mtxProject[1].ProjectRect(rc[1],rcWarp[1],ptRotate[1]);

		CWarp::WarpImage(internalParam->bmpDataCopy,width,height,rcSrc[1],rcWarp[1],rcClient,internalParam->param.pBmpData,Color::Transparent,FALSE);
		
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CDoorEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 12;
}

void CDoorEffect::ReleaseEffectParam()
{
}

void CWaveEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	int width = internalParam->param.bmpSize.cx;
	int height = internalParam->param.bmpSize.cy;

	if(!internalParam->param.bShow)
	{
		if(internalParam->frameNow == m_effectFrame)
		{
			memset(internalParam->param.pBmpData,0,width*height*4);
		}
		else
		{
			m_pWater->WaterImage(internalParam->bmpDataCopy,internalParam->param.pBmpData);	
			CAlpha::SetAlpha(internalParam->param.pBmpData, width, height, (255-internalParam->frameNow*5)/255.0f);
		}
	}
	else
	{
		if(internalParam->frameNow == m_effectFrame)
		{
			memcpy(internalParam->param.pBmpData,internalParam->bmpDataCopy,width*height*4);
		}
		else
		{
			m_pWater->WaterImage(internalParam->bmpDataCopy,internalParam->param.pBmpData);
			//alpha
			CAlpha::SetAlpha(internalParam->param.pBmpData, width, height, internalParam->frameNow/58.0f);
		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CWaveEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	int width = internalParam->param.bmpSize.cx;
	int height = internalParam->param.bmpSize.cy;

	m_pWater = new CWater();
	m_pWater->Create(width,height);

	int x = width/2;
	int y = height/2;

	m_pWater->SetDensity(6);
	m_pWater->AmplitudeBlob(x,y,x<y?x:y,800);

	for(int i = 0; i < 5; i++)
	{
		m_pWater->WaterImage(internalParam->bmpDataCopy,internalParam->param.pBmpData);
		Sleep(5);
	}

	m_effectFrame = 51;
}

void CWaveEffect::ReleaseEffectParam()
{
	m_pWater->Destory();
	delete m_pWater;
}

void CScatterEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	int width = internalParam->param.bmpSize.cx;
	int height = internalParam->param.bmpSize.cy;
	RECT rcBound = {0,0,width,height};

	POINT ptCenter = {width/2,height/2};

	RECT rcSrc[4] = { 0 };

	rcSrc[0].left = rcSrc[0].top = 0;
	rcSrc[0].right = ptCenter.x - 1;
	rcSrc[0].bottom = ptCenter.y -1;

	rcSrc[1].left  = ptCenter.x;
	rcSrc[1].top = 0;
	rcSrc[1].right = width - 1;
	rcSrc[1].bottom = ptCenter.y -1;

	rcSrc[2].left = 0;
	rcSrc[2].top = ptCenter.y;
	rcSrc[2].right = ptCenter.x - 1;
	rcSrc[2].bottom = height -1;

	rcSrc[3].left = ptCenter.x;
	rcSrc[3].top = ptCenter.y;
	rcSrc[3].right = width - 1;
	rcSrc[3].bottom = height  -1;

	CdRect rcAnimation[4];
	for(int i = 0; i < 4; i++)
	{
		rcAnimation[i] = CdRect(rcSrc[i]);
	}

	WarpRect rcWarp[4];
	CProjection mtxProject[4];

	memset(internalParam->param.pBmpData,0,width*height*4);
	if(!internalParam->param.bShow)
	{
		int i = internalParam->frameNow;
		for(int j = 0; j < 4; j++)
		{
			mtxProject[j].Reset();
			mtxProject[j].Translate(m_vx[j]*i,m_vy[j]*i,0);
			mtxProject[j].ProjectRect(rcAnimation[j],rcWarp[j],rcAnimation[j].GetCenter());

			CWarp::WarpImage(internalParam->bmpDataCopy,width,height,rcSrc[j],rcWarp[j],rcBound,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}
	else
	{
		int i = internalParam->frameNow + 6;
		for(int j = 0; j < 4; j++)
		{
			mtxProject[j].Reset();	
			mtxProject[j].Translate(m_vx[j]*(12-i),m_vy[j]*(12-i),0);
			mtxProject[j].ProjectRect(rcAnimation[j],rcWarp[j],rcAnimation[j].GetCenter());

			CWarp::WarpImage(internalParam->bmpDataCopy,width,height,rcSrc[j],rcWarp[j],rcBound,internalParam->param.pBmpData,Color::Transparent,FALSE);
		}
	}


	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}

}

void CScatterEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	int width = internalParam->param.bmpSize.cx;
	int height = internalParam->param.bmpSize.cy;

	real ratio = (real)width/height;
	real speedY = height/12.0f;
	real speedX = speedY*ratio;

	m_vx[0] = -speedX;
	m_vx[1] = speedX;
	m_vx[2] = -speedX;
	m_vx[3] = speedX;
	m_vy[0] = -speedY;
	m_vy[1] = -speedY;
	m_vy[2] = speedY;
	m_vy[3] = speedY;

	m_effectFrame = 6;
}

void CScatterEffect::ReleaseEffectParam()
{
}

void CColorFadeEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	int width = internalParam->param.bmpSize.cx;
	int height = internalParam->param.bmpSize.cy;

	if(!internalParam->param.bShow)
	{		
		CColorHelper::SetHueSaturation(internalParam->param.pBmpData,width,height, 0, -(int)internalParam->frameNow*5);
	}
	else
	{
		CColorHelper::SetHueSaturation(internalParam->param.pBmpData,width,height, 0, (m_effectFrame-internalParam->frameNow)*5);
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CColorFadeEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 13;

	if(internalParam->param.bShow)
	{
		CColorHelper::SetHueSaturation(internalParam->param.pBmpData,internalParam->param.bmpSize.cx,internalParam->param.bmpSize.cy, 0, -200);
	}
}

void CColorFadeEffect::ReleaseEffectParam()
{
}

void CDampSlideEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;

	Motion::CCriticalDamp* pCriticalDamp = (Motion::CCriticalDamp*)m_inertiaInstance;
	real inertiaDistance = pCriticalDamp->GetNowAmplitude(m_tInertiaTime);

	real x = inertiaDistance*cos(atan(m_ratio));
	real y = inertiaDistance*sin(atan(m_ratio));

	int xInt,yInt;

	REAL2INT(x,xInt);
	REAL2INT(y,yInt);

	xInt--;
	yInt--;

	if (!internalParam->param.bShow)
	{	
		memset(internalParam->param.pBmpData,0,width*height*4);

		if(internalParam->param.animationEffect == 66)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, 0,xInt ,yInt,internalParam->bmpDataCopy,width*4,width-xInt,height-yInt);
		}
		else if(internalParam->param.animationEffect == 67)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,width-xInt, 0,width,yInt,internalParam->bmpDataCopy,width*4,0,height-yInt);
		}
		else if(internalParam->param.animationEffect == 68)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, height-yInt,xInt,height,internalParam->bmpDataCopy,width*4,width-xInt,0);
		}
		else if(internalParam->param.animationEffect == 69)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,width-xInt, height-yInt,width,height,internalParam->bmpDataCopy,width*4,0,0);
		}
		else if(internalParam->param.animationEffect == 70)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, 0,xInt,height,internalParam->bmpDataCopy,width*4,width-xInt,0);
		}
		else if(internalParam->param.animationEffect == 71)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,width-xInt, 0,width,height,internalParam->bmpDataCopy,width*4,0,0);
		}
		else if(internalParam->param.animationEffect == 72)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, height-yInt,width,height,internalParam->bmpDataCopy,width*4,0,0);
		}
		else if(internalParam->param.animationEffect == 73)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0,width,yInt,internalParam->bmpDataCopy,width*4,0,height-yInt);
		}
	}
	else
	{
		if(internalParam->frameNow == 1)
		{
			memset(internalParam->param.pBmpData,0,width*height*4);
		}

		if(internalParam->param.animationEffect == 66)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, 0,width-xInt ,height-yInt,internalParam->bmpDataCopy,width*4,xInt,yInt);
		}
		else if(internalParam->param.animationEffect == 67)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,xInt, 0,width, height-yInt,internalParam->bmpDataCopy,width*4,0,yInt);
		}
		else if(internalParam->param.animationEffect == 68)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0, yInt,width-xInt ,height,internalParam->bmpDataCopy,width*4,xInt,0);
		}
		else if(internalParam->param.animationEffect == 69)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,xInt, yInt,width, height,internalParam->bmpDataCopy,width*4,0,0);
		}
		else if(internalParam->param.animationEffect == 70)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,xInt,0,width,height,internalParam->bmpDataCopy,width*4,0,0);
		}
		else if(internalParam->param.animationEffect == 71)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0,width-xInt,height,internalParam->bmpDataCopy,width*4,xInt,0);
		}
		else if(internalParam->param.animationEffect == 72)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,0,width,height-yInt,internalParam->bmpDataCopy,width*4,0,yInt);
		}
		else if(internalParam->param.animationEffect == 73)
		{
			CMemery::CopyPixels(internalParam->param.pBmpData,width*4,0,yInt,width,height,internalParam->bmpDataCopy,width*4,0,0);
		}
	
	}
	
	m_tInertiaTime += M_PI/pCriticalDamp->GetAngularFrequency()/m_tInertiacoeffecient;

	// 惯性运动结束条件
	if(!xInt && !yInt || (inertiaDistance<1.0f))
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CDampSlideEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	real amplitude = sqrt((real)internalParam->param.bmpSize.cx * internalParam->param.bmpSize.cy);
	m_ratio = (real)internalParam->param.bmpSize.cx / internalParam->param.bmpSize.cy;

	m_tInertiacoeffecient = 6.0f;
	m_tInertiaTime = 0;

	Motion::CCriticalDamp* pCriticalDamp = new Motion::CCriticalDamp();
	m_inertiaInstance = pCriticalDamp;

	pCriticalDamp->SetAmplitudeA(amplitude);
	pCriticalDamp->SetAmplitudeB(0.0f);
	pCriticalDamp->SetDamp(0.9f);
	pCriticalDamp->SetAngularFrequency(2.0f);
}

void CDampSlideEffect::ReleaseEffectParam()
{
	if(m_inertiaInstance != NULL)
	{
		Motion::CCriticalDamp* pCriticalDamp = (Motion::CCriticalDamp*)m_inertiaInstance;
		delete pCriticalDamp;
	}
}

void CWhirlPoolEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;

	memset(internalParam->param.pBmpData, 0, width*height*4);

	if(!internalParam->param.bShow)
	{ 
		if(internalParam->frameNow != m_effectFrame)
		{
			CWarp::WarpImage(internalParam->bmpDataCopy, width, height, internalParam->param.pBmpData, 0.1f*(internalParam->frameNow-1));
		}
	}
	else
	{
		CWarp::WarpImage(internalParam->bmpDataCopy, width, height, internalParam->param.pBmpData, 0.1f*(10-internalParam->frameNow));
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CWhirlPoolEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	if(!internalParam->param.bShow)
	{
		m_effectFrame = 11;
	}
	else
	{
		m_effectFrame = 10;
	}
}

void CWhirlPoolEffect::ReleaseEffectParam()
{
}

void CClockWipeEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;	

	PointF ptCenter(width/2.0f,height/2.0f);
	Point ptRotate(width/2,-100000);

	CdRect rcClient(0, 0, width, height);
	memset(m_pBits, 0, width*height*4);

	if (!internalParam->param.bShow)
	{	
		
		if(internalParam->frameNow == m_effectFrame)
		{
			internalParam->bLastFrame = TRUE;	
			memset(internalParam->param.pBmpData, 0, width*height*4);
		}
		else
		{
			Point pt(ptRotate);
			GraphicsPath path;

			Matrix mtx;
			mtx.RotateAt((internalParam->frameNow)*30,ptCenter);
			mtx.TransformPoints(&pt);

			CdLine lineRotate(ptCenter.X,ptCenter.Y,pt.X,pt.Y);
			CdLine lineTop(rcClient.left,rcClient.top,rcClient.right,rcClient.top);
			CdLine lineRight(rcClient.right,rcClient.top,rcClient.right,rcClient.bottom);
			CdLine lineBottom(rcClient.left,rcClient.bottom,rcClient.right,rcClient.bottom);
			CdLine lineLeft(rcClient.left,rcClient.top,rcClient.left,rcClient.bottom);
			CdPoint ptIntersect;
			if (GetSegIntersect(lineRotate,lineTop,ptIntersect))
			{
				path.AddLine((int)rcClient.left,(int)rcClient.top,(int)ptRotate.X,(int)rcClient.top);
				path.AddLine((int)ptRotate.X,(int)rcClient.top,(int)ptCenter.X,(int)ptCenter.Y);
				path.AddLine((int)ptCenter.X,(int)ptCenter.Y,(int)ptIntersect.x,(int)ptIntersect.y);
				path.AddLine((int)ptIntersect.x,(int)ptIntersect.y,(int)rcClient.right,(int)rcClient.top);
				path.AddLine((int)rcClient.right,(int)rcClient.top,(int)rcClient.right,(int)rcClient.bottom);
				path.AddLine((int)rcClient.right,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.bottom);
				path.AddLine((int)rcClient.left,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.top);

			} 
			else if (GetSegIntersect(lineRotate,lineRight,ptIntersect))
			{
				path.AddLine((int)rcClient.left,(int)rcClient.top,(int)ptRotate.X,(int)rcClient.top);
				path.AddLine((int)ptRotate.X,(int)rcClient.top,(int)ptCenter.X,(int)ptCenter.Y);
				path.AddLine((int)ptCenter.X,(int)ptCenter.Y,(int)ptIntersect.x,(int)ptIntersect.y);
				path.AddLine((int)ptIntersect.x,(int)ptIntersect.y,(int)rcClient.right,(int)rcClient.bottom);
				path.AddLine((int)rcClient.right,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.bottom);
				path.AddLine((int)rcClient.left,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.top);

			} 
			else if(GetSegIntersect(lineRotate,lineBottom,ptIntersect))
			{
				path.AddLine((int)rcClient.left,(int)rcClient.top,(int)ptRotate.X,(int)rcClient.top);
				path.AddLine((int)ptRotate.X,(int)rcClient.top,(int)ptCenter.X,(int)ptCenter.Y);
				path.AddLine((int)ptCenter.X,(int)ptCenter.Y,(int)ptIntersect.x,(int)ptIntersect.y);
				path.AddLine((int)ptIntersect.x,(int)ptIntersect.y,(int)rcClient.left,(int)rcClient.bottom);
				path.AddLine((int)rcClient.left,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.top);
			}
			else if(GetSegIntersect(lineRotate,lineLeft,ptIntersect))
			{
				path.AddLine((int)rcClient.left,(int)rcClient.top,(int)ptRotate.X,(int)rcClient.top);
				path.AddLine((int)ptRotate.X,(int)rcClient.top,(int)ptCenter.X,(int)ptCenter.Y);
				path.AddLine((int)ptCenter.X,(int)ptCenter.Y,(int)ptIntersect.x,(int)ptIntersect.y);
				path.AddLine((int)ptIntersect.x,(int)ptIntersect.y,(int)rcClient.left,(int)rcClient.top);
			}

#ifdef new
#undef new
#endif
			Bitmap *pBitmap = new Bitmap(width, height, width*4, PixelFormat32bppARGB, m_pBits);

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
			Graphics graphics(pBitmap);
			graphics.SetClip(&path);
			graphics.DrawImage(m_pBitmap, 0, 0);

			memcpy(internalParam->param.pBmpData, m_pBits, width*height*4);

			delete pBitmap;
		}
	}
	else
	{
		if(internalParam->frameNow == m_effectFrame)
		{
			internalParam->bLastFrame = TRUE;	
			memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);
		}
		else
		{
			Point pt(ptRotate);
			GraphicsPath path;

			Matrix mtx;
			mtx.RotateAt((internalParam->frameNow)*30,ptCenter);
			mtx.TransformPoints(&pt);

			CdLine lineRotate(ptCenter.X,ptCenter.Y,pt.X,pt.Y);
			CdLine lineTop(rcClient.left,rcClient.top,rcClient.right,rcClient.top);
			CdLine lineRight(rcClient.right,rcClient.top,rcClient.right,rcClient.bottom);
			CdLine lineBottom(rcClient.left,rcClient.bottom,rcClient.right,rcClient.bottom);
			CdLine lineLeft(rcClient.left,rcClient.top,rcClient.left,rcClient.bottom);
			CdPoint ptIntersect;
			if (GetSegIntersect(lineRotate,lineTop,ptIntersect))
			{
				path.AddLine((int)rcClient.left,(int)rcClient.top,(int)ptRotate.X,(int)rcClient.top);
				path.AddLine((int)ptRotate.X,(int)rcClient.top,(int)ptCenter.X,(int)ptCenter.Y);
				path.AddLine((int)ptCenter.X,(int)ptCenter.Y,(int)ptIntersect.x,(int)ptIntersect.y);
				path.AddLine((int)ptIntersect.x,(int)ptIntersect.y,(int)rcClient.right,(int)rcClient.top);
				path.AddLine((int)rcClient.right,(int)rcClient.top,(int)rcClient.right,(int)rcClient.bottom);
				path.AddLine((int)rcClient.right,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.bottom);
				path.AddLine((int)rcClient.left,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.top);

			} 
			else if (GetSegIntersect(lineRotate,lineRight,ptIntersect))
			{
				path.AddLine((int)rcClient.left,(int)rcClient.top,(int)ptRotate.X,(int)rcClient.top);
				path.AddLine((int)ptRotate.X,(int)rcClient.top,(int)ptCenter.X,(int)ptCenter.Y);
				path.AddLine((int)ptCenter.X,(int)ptCenter.Y,(int)ptIntersect.x,(int)ptIntersect.y);
				path.AddLine((int)ptIntersect.x,(int)ptIntersect.y,(int)rcClient.right,(int)rcClient.bottom);
				path.AddLine((int)rcClient.right,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.bottom);
				path.AddLine((int)rcClient.left,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.top);

			} 
			else if(GetSegIntersect(lineRotate,lineBottom,ptIntersect))
			{
				path.AddLine((int)rcClient.left,(int)rcClient.top,(int)ptRotate.X,(int)rcClient.top);
				path.AddLine((int)ptRotate.X,(int)rcClient.top,(int)ptCenter.X,(int)ptCenter.Y);
				path.AddLine((int)ptCenter.X,(int)ptCenter.Y,(int)ptIntersect.x,(int)ptIntersect.y);
				path.AddLine((int)ptIntersect.x,(int)ptIntersect.y,(int)rcClient.left,(int)rcClient.bottom);
				path.AddLine((int)rcClient.left,(int)rcClient.bottom,(int)rcClient.left,(int)rcClient.top);
			}
			else if(GetSegIntersect(lineRotate,lineLeft,ptIntersect))
			{
				path.AddLine((int)rcClient.left,(int)rcClient.top,(int)ptRotate.X,(int)rcClient.top);
				path.AddLine((int)ptRotate.X,(int)rcClient.top,(int)ptCenter.X,(int)ptCenter.Y);
				path.AddLine((int)ptCenter.X,(int)ptCenter.Y,(int)ptIntersect.x,(int)ptIntersect.y);
				path.AddLine((int)ptIntersect.x,(int)ptIntersect.y,(int)rcClient.left,(int)rcClient.top);
			}

			Rect rcTemp(0, 0, width, height);
			Region rgn(rcTemp);
			rgn.Exclude(&path);
			
	#ifdef new
	#undef new
	#endif
			Bitmap *pBitmap = new Bitmap(width, height, width*4, PixelFormat32bppARGB, m_pBits);
	

	#ifdef _DEBUG
	#define new DEBUG_CLIENTBLOCK
	#endif
			Graphics graphics(pBitmap);
			graphics.SetClip(&rgn);
			graphics.DrawImage(m_pBitmap, 0, 0);

			memcpy(internalParam->param.pBmpData, m_pBits, width*height*4);

			delete pBitmap;
		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CClockWipeEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;	

	m_effectFrame = 11;

#ifdef new
#undef new
#endif
	m_pBitmap = new Bitmap(width, height, width*4, PixelFormat32bppARGB, internalParam->bmpDataCopy);

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

	m_pBits = new BYTE[width*height*4];
}

void CClockWipeEffect::ReleaseEffectParam()
{
	if(m_pBits != NULL)
	{
		delete []m_pBits;
	}

	if(m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CPinWheelEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;	
	CdRect rcClient(0, 0, width, height);
	memset(m_pBits, 0, width*height*4);

	PointF ptCenter(rcClient.left+width/2,rcClient.top+height/2);
	Point ptLeftTop(rcClient.left,rcClient.top);
	Point ptTop(rcClient.left+width/2,rcClient.top);
	Point ptRightTop(rcClient.right,rcClient.top);	
	Point ptLeft(rcClient.left,rcClient.top+height/2);
	Point ptLeftBottom(rcClient.left,rcClient.bottom);	
	Point ptRight(rcClient.right,rcClient.top+height/2);
	Point ptRightBottom(rcClient.right,rcClient.bottom);	
	Point ptBottom(rcClient.left+width/2,rcClient.bottom);

	CdLine  lines[4] = {
		CdLine/* lineTop*/(rcClient.left,rcClient.top,rcClient.right,rcClient.top),	
		CdLine/* lineRight*/(rcClient.right,rcClient.top,rcClient.right,rcClient.bottom),
		CdLine/*lineLeft*/(rcClient.left,rcClient.top,rcClient.left,rcClient.bottom),
		CdLine/* lineBottom*/(rcClient.left,rcClient.bottom,rcClient.right,rcClient.bottom)
	};

	Point ptRotates[8] = {
		Point/* ptLeftTopRotate*/(ptLeftTop),Point/* ptTopRotate*/(rcClient.left+width/2,rcClient.top-5000),Point/* ptRightTopRotate*/(ptRightTop),
		Point/* ptLeftRotate*/(rcClient.left-5000,rcClient.top+height/2),Point/* ptRightRotate*/(rcClient.right+5000,rcClient.top+height/2),
		Point/* ptLeftBottomRotate*/(ptLeftBottom),Point/* ptBottomRotate*/(rcClient.left+width/2,rcClient.bottom+5000),Point/* ptRightBottomRotate*/(ptRightBottom)
	};

	if(!internalParam->param.bShow)
	{
		if(internalParam->frameNow == m_effectFrame)
		{
			internalParam->bLastFrame = TRUE;	
			memset(internalParam->param.pBmpData, 0, width*height*4);
		}
		else
		{
			Point ptTemp[8];
			memcpy(ptTemp, ptRotates, sizeof(ptRotates));

			Matrix mtx;
			mtx.RotateAt((internalParam->frameNow)*9, ptCenter);
			mtx.TransformPoints(ptTemp,8);

			CdPoint ptSections[8];
			for(int j = 0; j < 4; j++)
			{
				CdLine line(ptTemp[j].X, ptTemp[j].Y, ptTemp[7-j].X, ptTemp[7-j].Y);
				if (j < 2)
				{
					IntersectLine(line, lines[0], NULL, NULL, &ptSections[j]);
					IntersectLine(line, lines[3], NULL, NULL, &ptSections[7-j]);
				} 
				else
				{
					IntersectLine(line, lines[1], NULL, NULL, &ptSections[j]);
					IntersectLine(line, lines[2], NULL, NULL, &ptSections[7-j]);
				}		
			}

			GraphicsPath path;

			path.AddLine(ptSections[0].x, ptSections[0].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptTop.X, ptTop.Y);
			path.AddLine(ptTop.X,ptTop.Y, (int)ptSections[0].x, (int)ptSections[0].y);

			path.AddLine(ptSections[1].x, ptSections[1].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptRightTop.X, ptRightTop.Y);
			path.AddLine(ptRightTop.X, ptRightTop.Y, (int)ptSections[1].x, (int)ptSections[1].y);

			path.StartFigure();

			path.AddLine(ptSections[2].x, ptSections[2].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptRight.X, ptRight.Y);
			path.AddLine(ptRight.X,ptRight.Y, (int)ptSections[2].x, (int)ptSections[2].y);

			path.AddLine(ptSections[3].x, ptSections[3].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptRightBottom.X, ptRightBottom.Y);
			path.AddLine(ptRightBottom.X, ptRightBottom.Y, (int)ptSections[3].x, (int)ptSections[3].y);

			path.StartFigure();
			path.AddLine(ptSections[7].x, ptSections[7].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptBottom.X, ptBottom.Y);
			path.AddLine(ptBottom.X, ptBottom.Y, (int)ptSections[7].x, (int)ptSections[7].y);

			path.AddLine(ptSections[6].x, ptSections[6].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptLeftBottom.X, ptLeftBottom.Y);
			path.AddLine(ptLeftBottom.X, ptLeftBottom.Y, (int)ptSections[6].x, (int)ptSections[6].y);

			path.StartFigure();
			path.AddLine(ptSections[5].x, ptSections[5].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptLeft.X, ptLeft.Y);
			path.AddLine(ptLeft.X, ptLeft.Y, (int)ptSections[5].x, (int)ptSections[5].y);

			path.AddLine(ptSections[4].x, ptSections[4].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptLeftTop.X, ptLeftTop.Y);
			path.AddLine(ptLeftTop.X, ptLeftTop.Y, (int)ptSections[4].x, (int)ptSections[4].y);

			//path.CloseAllFigures();

	#ifdef new
	#undef new
	#endif
			Bitmap *pBitmap = new Bitmap(width, height, width*4, PixelFormat32bppARGB, m_pBits);
	

	#ifdef _DEBUG
	#define new DEBUG_CLIENTBLOCK
	#endif
			Graphics graphics(pBitmap);
			graphics.SetClip(&path);
			graphics.DrawImage(m_pBitmap, 0, 0);

			memcpy(internalParam->param.pBmpData, m_pBits, width*height*4);

			delete pBitmap;
		}
	}
	else
	{
		if(internalParam->frameNow == m_effectFrame)
		{
			internalParam->bLastFrame = TRUE;	
			memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);
		}
		else
		{
			Point ptTemp[8];
			memcpy(ptTemp, ptRotates, sizeof(ptRotates));

			Matrix mtx;
			mtx.RotateAt((5-internalParam->frameNow)*9, ptCenter);
			mtx.TransformPoints(ptTemp, 8);

			CdPoint ptSections[8];
			for(int j = 0; j < 4; j++)
			{
				CdLine line(ptTemp[j].X, ptTemp[j].Y, ptTemp[7-j].X, ptTemp[7-j].Y);
				if (j < 2)
				{
					IntersectLine(line, lines[0], NULL, NULL, &ptSections[j]);
					IntersectLine(line, lines[3], NULL, NULL, &ptSections[7-j]);
				} 
				else
				{
					IntersectLine(line, lines[1], NULL, NULL, &ptSections[j]);
					IntersectLine(line, lines[2], NULL, NULL, &ptSections[7-j]);
				}		
			}

			GraphicsPath path;

			path.AddLine(ptSections[0].x, ptSections[0].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptTop.X, ptTop.Y);
			path.AddLine(ptTop.X, ptTop.Y, (int)ptSections[0].x, (int)ptSections[0].y);

			path.AddLine(ptSections[1].x, ptSections[1].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptRightTop.X, ptRightTop.Y);
			path.AddLine(ptRightTop.X, ptRightTop.Y, (int)ptSections[1].x, (int)ptSections[1].y);

			path.StartFigure();

			path.AddLine(ptSections[2].x, ptSections[2].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptRight.X, ptRight.Y);
			path.AddLine(ptRight.X, ptRight.Y, (int)ptSections[2].x, (int)ptSections[2].y);

			path.AddLine(ptSections[3].x, ptSections[3].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptRightBottom.X, ptRightBottom.Y);
			path.AddLine(ptRightBottom.X, ptRightBottom.Y, (int)ptSections[3].x, (int)ptSections[3].y);

			path.StartFigure();
			path.AddLine(ptSections[7].x, ptSections[7].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptBottom.X, ptBottom.Y);
			path.AddLine(ptBottom.X, ptBottom.Y, (int)ptSections[7].x, (int)ptSections[7].y);

			path.AddLine(ptSections[6].x, ptSections[6].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptLeftBottom.X, ptLeftBottom.Y);
			path.AddLine(ptLeftBottom.X, ptLeftBottom.Y, (int)ptSections[6].x, (int)ptSections[6].y);

			path.StartFigure();
			path.AddLine(ptSections[5].x, ptSections[5].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptLeft.X, ptLeft.Y);
			path.AddLine(ptLeft.X, ptLeft.Y, (int)ptSections[5].x, (int)ptSections[5].y);

			path.AddLine(ptSections[4].x, ptSections[4].y, ptCenter.X, ptCenter.Y);
			path.AddLine((int)ptCenter.X, (int)ptCenter.Y, ptLeftTop.X, ptLeftTop.Y);
			path.AddLine(ptLeftTop.X, ptLeftTop.Y, (int)ptSections[4].x, (int)ptSections[4].y);

			//path.CloseAllFigures();

	#ifdef new
	#undef new
	#endif
			Bitmap *pBitmap = new Bitmap(width, height, width*4, PixelFormat32bppARGB, m_pBits);
	

	#ifdef _DEBUG
	#define new DEBUG_CLIENTBLOCK
	#endif
			Graphics graphics(pBitmap);
			graphics.SetClip(&path);
			graphics.DrawImage(m_pBitmap, 0, 0);

			memcpy(internalParam->param.pBmpData, m_pBits, width*height*4);

			delete pBitmap;
		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CPinWheelEffect::InitEffectParam(InternalAnimationParam *internalParam)
{	
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;	

	m_effectFrame = 6;

#ifdef new
#undef new
#endif
	m_pBitmap = new Bitmap(width, height, width*4, PixelFormat32bppARGB, internalParam->bmpDataCopy);

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

	m_pBits = new BYTE[width*height*4];
}

void CPinWheelEffect::ReleaseEffectParam()
{
	if(m_pBits != NULL)
	{
		delete []m_pBits;
	}

	if(m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CPagePeelEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;	

	CdRect rcClient(0, 0, width, height);
	memset(m_pBits, 0, width*height*4);

	//画图的区域和卷角的区域
	GraphicsPath rg1, rg2;

	Point pageCorner = Point(rcClient.right, rcClient.bottom);//页角

	Point ptCenter(rcClient.left, rcClient.bottom+2*rcClient.Width());
	Matrix mtx1;
	mtx1.Translate(-ptCenter.X, -ptCenter.Y, Gdiplus::MatrixOrderAppend);
	real angle = atanf(0.25f)/8*180;
	int frame = 8;

	if(!internalParam->param.bShow)
	{	
		if(internalParam->frameNow == m_effectFrame)
		{
			internalParam->bLastFrame = TRUE;	
			memset(internalParam->param.pBmpData, 0, width*height*4);
		}
		else
		{
			//计算点和区域

			//计算页角
			Matrix mtx2;
			mtx2.Rotate(-angle*(real)internalParam->frameNow);
			mtx2.Multiply(&mtx1);
			mtx2.Translate(ptCenter.X, ptCenter.Y, Gdiplus::MatrixOrderAppend);

			Point pgcorn(pageCorner);
			mtx2.TransformPoints(&pgcorn);

			CdPoint pt1(pgcorn.X, pgcorn.Y);
			CdPoint pt2(pgcorn.X+100, pgcorn.Y-100*m_nK);
			CdLine line1(pt1, pt2);
			line1.OffsetNormal(-rcClient.Width()*(real)internalParam->frameNow/10.0f);
			pt1 = CdPoint(rcClient.right, rcClient.top);
			pt2 = CdPoint(rcClient.right, rcClient.bottom);
			CdLine line2(pt1, pt2);
			pt1 = CdPoint(rcClient.left, rcClient.bottom);
			pt2 = CdPoint(rcClient.right, rcClient.bottom);
			CdLine line3(pt1, pt2);

			IntersectLine(line1, line2, NULL, NULL, &pt1);
			IntersectLine(line1, line3, NULL, NULL, &pt2);
			Point ptintsect1(pt1.x, pt1.y);
			Point ptintsect2(pt2.x, pt2.y);
			Point ptintsect3(ptintsect1);
			if(pt1.y < rcClient.top)
			{
				m_intersectTop = TRUE;
				pt1 = CdPoint(rcClient.left, rcClient.top);
				pt2 = CdPoint(rcClient.right, rcClient.top);
				line2 = CdLine(pt1, pt2);
				IntersectLine(line1, line2, NULL, NULL, &pt1);
				ptintsect1 = Point(pt1.x, pt1.y);

				//调整k
				m_nK = 3;
				//调整页角
				pgcorn.X += rcClient.Width()*internalParam->frameNow/80.0f;
			}

			if (m_intersectTop)
			{
				rg1.Reset();
				rg1.AddLine((INT)rcClient.left, (INT)rcClient.top, ptintsect1.X, ptintsect1.Y);
				rg1.AddLine(ptintsect1, ptintsect2);
				rg1.AddLine(ptintsect2.X, ptintsect2.Y, (INT)rcClient.left, (INT)rcClient.bottom);
				rg1.AddLine((INT)rcClient.left, (INT)rcClient.bottom, (INT)rcClient.left, (INT)rcClient.top);
				rg1.CloseAllFigures();

				//计算弧线
				ptintsect1 = ptintsect3;		
				pt1 = CdPoint(pgcorn.X, pgcorn.Y);
				pt2 = CdPoint(ptintsect1.X, ptintsect1.Y);
				line1 = CdLine(pt1, pt2);
				line1.OffsetNormal(-20);
				CdPoint pt3 = line1.GetLambdaPoint(1.0f/3);
				CdPoint pt4 = line1.GetLambdaPoint(2.0f/3);

				rg2.Reset();
				rg2.AddBezier(pt1.x, pt1.y, pt3.x, pt3.y, pt4.x, pt4.y, pt2.x, pt2.y);
				rg2.AddLine(ptintsect1, ptintsect2);

				pt1 = CdPoint(pgcorn.X, pgcorn.Y);
				pt2 = CdPoint(ptintsect2.X, ptintsect2.Y);
				line1 = CdLine(pt1, pt2);
				line1.OffsetNormal(10);
				pt3 = line1.GetLambdaPoint(1.0f/3);
				pt4 = line1.GetLambdaPoint(2.0f/3);

				rg2.AddBezier(pt2.x, pt2.y, pt4.x, pt4.y, pt3.x, pt3.y, pt1.x, pt1.y);
				rg2.CloseAllFigures();
			}
			else
			{
				rg1.Reset();
				rg1.AddLine((INT)rcClient.left, (INT)rcClient.top, (INT)rcClient.right, (INT)rcClient.top);
				rg1.AddLine((INT)rcClient.right, (INT)rcClient.top, ptintsect1.X, ptintsect1.Y);
				rg1.AddLine(ptintsect1, ptintsect2);
				rg1.AddLine(ptintsect2.X, ptintsect2.Y, (INT)rcClient.left, (INT)rcClient.bottom);
				rg1.AddLine((INT)rcClient.left, (INT)rcClient.bottom, (INT)rcClient.left, (INT)rcClient.top);
				rg1.CloseAllFigures();

				//计算弧线

				pt1 = CdPoint(pgcorn.X, pgcorn.Y);
				pt2 = CdPoint(ptintsect1.X, ptintsect1.Y);
				line1 = CdLine(pt1, pt2);
				line1.OffsetNormal(-20);
				CdPoint pt3 = line1.GetLambdaPoint(1.0f/3);
				CdPoint pt4 = line1.GetLambdaPoint(2.0f/3);

				rg2.Reset();
				rg2.AddBezier(pt1.x, pt1.y, pt3.x, pt3.y, pt4.x, pt4.y, pt2.x, pt2.y);
				rg2.AddLine(ptintsect1, ptintsect2);

				pt1 = CdPoint(pgcorn.X, pgcorn.Y);
				pt2 = CdPoint(ptintsect2.X, ptintsect2.Y);
				line1 = CdLine(pt1, pt2);
				line1.OffsetNormal(10);
				pt3 = line1.GetLambdaPoint(1.0f/3);
				pt4 = line1.GetLambdaPoint(2.0f/3);

				rg2.AddBezier(pt2.x, pt2.y, pt4.x, pt4.y, pt3.x, pt3.y, pt1.x, pt1.y);
				rg2.CloseAllFigures();

			}

			//生产动画帧
	#ifdef new
	#undef new
	#endif
			Bitmap *pBitmap = new Bitmap(width, height, width*4, PixelFormat32bppARGB, m_pBits);
	

	#ifdef _DEBUG
	#define new DEBUG_CLIENTBLOCK
	#endif
			// 画图
			Graphics graphics(pBitmap);
			graphics.SetClip(&rg1);
			graphics.DrawImage(m_pBitmap, 0, 0);

			// 画纸
			graphics.SetClip(&rg2);
			graphics.FillRectangle(&SolidBrush(Color::White), 0, 0, width, height);

			//memcpy(internalParam->param.pBmpData, m_pBits, width*height*4);
			for(int row = 0; row < height; row++)
			{
				memcpy(internalParam->param.pBmpData+row*width*4, m_pBits+(height-row-1)*width*4, width*4);
			}
	
			delete pBitmap;

		}

	}
	else
	{

		if(internalParam->frameNow == m_effectFrame)
		{
			internalParam->bLastFrame = TRUE;	
			//memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);
			for(int row = 0; row < height; row++)
			{
				memcpy(internalParam->param.pBmpData+row*width*4, internalParam->bmpDataCopy+(height-row-1)*width*4, width*4);
			}
		}
		else
		{
			//计算点和区域

			//计算页角
			Matrix mtx2;
			mtx2.Rotate(-angle*(m_effectFrame-(real)internalParam->frameNow));
			mtx2.Multiply(&mtx1);
			mtx2.Translate(ptCenter.X, ptCenter.Y, Gdiplus::MatrixOrderAppend);

			Point pgcorn(pageCorner);
			mtx2.TransformPoints(&pgcorn);

			CdPoint pt1(pgcorn.X, pgcorn.Y);
			CdPoint pt2(pgcorn.X+100, pgcorn.Y-100*m_nK);
			CdLine line1(pt1, pt2);
			line1.OffsetNormal(-rcClient.Width()*(m_effectFrame-(real)internalParam->frameNow)/10.0f);
			pt1 = CdPoint(rcClient.right, rcClient.top);
			pt2 = CdPoint(rcClient.right, rcClient.bottom);
			CdLine line2(pt1, pt2);
			pt1 = CdPoint(rcClient.left, rcClient.bottom);
			pt2 = CdPoint(rcClient.right, rcClient.bottom);
			CdLine line3(pt1, pt2);


			IntersectLine(line1, line2, NULL, NULL, &pt1);
			IntersectLine(line1, line3, NULL, NULL, &pt2);
			Point ptintsect1(pt1.x, pt1.y);
			Point ptintsect2(pt2.x, pt2.y);
			Point ptintsect3(ptintsect1);
			if(pt1.y > rcClient.top)
			{
				m_intersectTop = FALSE;

				if(rcClient.Width() < rcClient.Height()/1.5f)
				{
					//调整k
					m_nK = 2;
				}
			}
			else
			{
				pt1 = CdPoint(rcClient.left, rcClient.top);
				pt2 = CdPoint(rcClient.right, rcClient.top);
				line2 = CdLine(pt1, pt2);
				IntersectLine(line1, line2, NULL, NULL, &pt1);
				ptintsect1 = Point(pt1.x, pt1.y);
				//调整页角
				pgcorn.X += rcClient.Width()*(m_effectFrame-(real)internalParam->frameNow)/80.0f;
			}

			if (m_intersectTop)
			{
				rg1.Reset();
				rg1.AddLine((INT)rcClient.left, (INT)rcClient.top, ptintsect1.X, ptintsect1.Y);
				rg1.AddLine(ptintsect1, ptintsect2);
				rg1.AddLine(ptintsect2.X, ptintsect2.Y, (INT)rcClient.left, (INT)rcClient.bottom);
				rg1.AddLine((INT)rcClient.left, (INT)rcClient.bottom, (INT)rcClient.left, (INT)rcClient.top);
				rg1.CloseAllFigures();

				//计算弧线
				ptintsect1 = ptintsect3;		
				pt1 = CdPoint(pgcorn.X, pgcorn.Y);
				pt2 = CdPoint(ptintsect1.X, ptintsect1.Y);
				line1 = CdLine(pt1, pt2);
				line1.OffsetNormal(-20);
				CdPoint pt3 = line1.GetLambdaPoint(1.0f/3);
				CdPoint pt4 = line1.GetLambdaPoint(2.0f/3);

				rg2.Reset();
				rg2.AddBezier(pt1.x, pt1.y, pt3.x, pt3.y, pt4.x, pt4.y, pt2.x, pt2.y);
				rg2.AddLine(ptintsect1, ptintsect2);

				pt1 = CdPoint(pgcorn.X, pgcorn.Y);
				pt2 = CdPoint(ptintsect2.X, ptintsect2.Y);
				line1 = CdLine(pt1, pt2);
				line1.OffsetNormal(10);
				pt3 = line1.GetLambdaPoint(1.0f/3);
				pt4 = line1.GetLambdaPoint(2.0f/3);

				rg2.AddBezier(pt2.x, pt2.y, pt4.x, pt4.y, pt3.x, pt3.y, pt1.x, pt1.y);
				rg2.CloseAllFigures();
			}
			else
			{
				rg1.Reset();
				rg1.AddLine((INT)rcClient.left, (INT)rcClient.top, (INT)rcClient.right, (INT)rcClient.top);
				rg1.AddLine((INT)rcClient.right, (INT)rcClient.top, ptintsect1.X, ptintsect1.Y);
				rg1.AddLine(ptintsect1, ptintsect2);
				rg1.AddLine(ptintsect2.X, ptintsect2.Y, (INT)rcClient.left, (INT)rcClient.bottom);
				rg1.AddLine((INT)rcClient.left, (INT)rcClient.bottom, (INT)rcClient.left, (INT)rcClient.top);
				rg1.CloseAllFigures();

				//计算弧线

				pt1 = CdPoint(pgcorn.X, pgcorn.Y);
				pt2 = CdPoint(ptintsect1.X, ptintsect1.Y);
				line1 = CdLine(pt1, pt2);
				line1.OffsetNormal(-20);
				CdPoint pt3 = line1.GetLambdaPoint(1.0f/3);
				CdPoint pt4 = line1.GetLambdaPoint(2.0f/3);

				rg2.Reset();
				rg2.AddBezier(pt1.x, pt1.y, pt3.x, pt3.y, pt4.x, pt4.y, pt2.x, pt2.y);
				rg2.AddLine(ptintsect1, ptintsect2);

				pt1 = CdPoint(pgcorn.X, pgcorn.Y);
				pt2 = CdPoint(ptintsect2.X, ptintsect2.Y);
				line1 = CdLine(pt1, pt2);
				line1.OffsetNormal(10);
				pt3 = line1.GetLambdaPoint(1.0f/3);
				pt4 = line1.GetLambdaPoint(2.0f/3);

				rg2.AddBezier(pt2.x, pt2.y, pt4.x, pt4.y, pt3.x, pt3.y, pt1.x, pt1.y);
				rg2.CloseAllFigures();

			}

			//生产动画帧
	#ifdef new
	#undef new
	#endif
			Bitmap *pBitmap = new Bitmap(width, height, width*4, PixelFormat32bppARGB, m_pBits);
	

	#ifdef _DEBUG
	#define new DEBUG_CLIENTBLOCK
	#endif
			// 画图
			Graphics graphics(pBitmap);
			graphics.SetClip(&rg1);
			graphics.DrawImage(m_pBitmap, 0, 0);

			// 画纸
			graphics.SetClip(&rg2);
			graphics.FillRectangle(&SolidBrush(Color::White), 0, 0, width, height);

			//memcpy(internalParam->param.pBmpData, m_pBits, width*height*4);
			for(int row = 0; row < height; row++)
			{
				memcpy(internalParam->param.pBmpData+row*width*4, m_pBits+(height-row-1)*width*4, width*4);
			}
	
			delete pBitmap;
		}
	}
}

void CPagePeelEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;	

	m_effectFrame = 8;
	m_nK = 2.0f;
	if(width < height/1.5f)
	{
		m_effectFrame = 12;
		m_nK = 3;
	}
	if(!internalParam->param.bShow)
	{
		m_intersectTop = FALSE;
	}
	else
	{
		m_nK = 3.0f;
		m_intersectTop = TRUE;
	}

#ifdef new
#undef new
#endif
	m_pBitmap = new Bitmap(width, height, width*4, PixelFormat32bppARGB, internalParam->bmpDataCopy);

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

	m_pBits = new BYTE[width*height*4];

	for(int row = 0; row < height; row++)
	{
		memcpy(internalParam->bmpDataCopy+(height-row-1)*width*4, internalParam->param.pBmpData+row*width*4, width*4);
	}
}

void CPagePeelEffect::ReleaseEffectParam()
{
	if(m_pBits != NULL)
	{
		delete []m_pBits;
	}

	if(m_pBitmap)
	{
		delete m_pBitmap;
	}
}

void CBlurEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;	
	memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);

	if (!internalParam->param.bShow)
	{
		if(internalParam->frameNow == m_effectFrame)
		{
			internalParam->bLastFrame = TRUE;	
			memset(internalParam->param.pBmpData, 0, width*height*4);
		}
		else
		{
			int *weights = NULL;
			CGaussBlur::InitWeights(2, internalParam->frameNow-1, &weights);
			CGaussBlur::ImageGaussiabBlur(internalParam->param.pBmpData, width, height, internalParam->frameNow-1, weights);

			//alpha
			CAlpha::SetAlpha(internalParam->param.pBmpData, width, height, (255-(internalParam->frameNow-1)*5)/255.0f);
		}

	}
	else
	{
		if(internalParam->frameNow == m_effectFrame)
		{
			internalParam->bLastFrame = TRUE;	
			memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);
		}
		else
		{
			int *weights = NULL;
			CGaussBlur::InitWeights(2, 10-internalParam->frameNow, &weights);
			CGaussBlur::ImageGaussiabBlur(internalParam->param.pBmpData, width, height, 10-internalParam->frameNow, weights);

			//alpha
			CAlpha::SetAlpha(internalParam->param.pBmpData, width, height, (205+(internalParam->frameNow-1)*5)/255.0f);	 
		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CBlurEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 10;
}

void CBlurEffect::ReleaseEffectParam()
{
}

void CZoomBlurEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;	
	memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);

	if (!internalParam->param.bShow)
	{
		if(internalParam->frameNow != m_effectFrame)
		{
			CTraileBlur::TraileBlur(internalParam->param.pBmpData, internalParam->bmpDataCopy, width, height, internalParam->frameNow*9);
			//alpha
			CAlpha::SetAlpha(internalParam->param.pBmpData, width, height, (255-(internalParam->frameNow)*25)/255.0f);
		}
		else
		{
			memset(internalParam->param.pBmpData, 0, width*height*4);
		}
	}
	else
	{
		CTraileBlur::TraileBlur(internalParam->param.pBmpData, internalParam->bmpDataCopy, width, height, (4-internalParam->frameNow)*9);
		//alpha
		CAlpha::SetAlpha(internalParam->param.pBmpData, width, height, (255-(4-internalParam->frameNow)*25)/255.0f);
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CZoomBlurEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_effectFrame = 4;
}

void CZoomBlurEffect::ReleaseEffectParam()
{
}

void CFoldEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	int width = internalParam->param.bmpSize.cx;
	int height = internalParam->param.bmpSize.cy;
	RECT rcBound = { 0, 0, width, height };
	int frame = internalParam->frameNow;
	if(internalParam->param.bShow)
	{
		frame = m_effectFrame - frame;
	}
	memset(internalParam->param.pBmpData, 0, width*height*4);
	CProjection project;

	if(internalParam->param.animationEffect == 80)
	{
		for(int j = 0; j < m_foldNum; j++)
		{
			project.Reset();
			if(j%2)
			{
				project.RotateY(frame/24.0f*M_PI);
			}
			else
			{
				project.RotateY(-frame/24.0f*M_PI);
			}

			project.ProjectRect(m_foldRect[j], m_warpRect[j], m_foldRect[j].GetCenter());
		}

		// 计算总宽度
		int totalWidth = 0;
		for(int j = 0; j < m_foldNum; j++)
		{
			totalWidth += m_warpRect[j].bottomRight.x - m_warpRect[j].bottomLeft.x;
		}

		int offSet = (width - totalWidth) / 2;
		for(int j = 0; j < m_foldNum; j++)
		{
			CdSize szOffSet(offSet - m_warpRect[j].bottomLeft.x, 0);
			m_warpRect[j].Offset(szOffSet);
			offSet += m_warpRect[j].bottomRight.x - m_warpRect[j].bottomLeft.x;
		}

		for(int j = 0; j < m_foldNum; j++)
		{
			CWarp::WarpImage(internalParam->bmpDataCopy, width, height, m_foldRect[j].ToRECT(), m_warpRect[j], rcBound, internalParam->param.pBmpData, Color(0, 0, 0, 0), FALSE);
		}
	}
	else
	{
		for(int j = 0; j < m_foldNum; j++)
		{
			project.Reset();
			if(j%2)
			{
				project.RotateX(frame/24.0f*M_PI);
			}
			else
			{
				project.RotateX(-frame/24.0f*M_PI);
			}

			project.ProjectRect(m_foldRect[j], m_warpRect[j], m_foldRect[j].GetCenter());
		}

		// 计算总高度
		int totalHeight = 0;
		for(int j = 0; j < m_foldNum; j++)
		{
			totalHeight += m_warpRect[j].bottomRight.y - m_warpRect[j].topRight.y;
		}

		int offSet = (height - totalHeight) / 2;
		for(int j = 0; j < m_foldNum; j++)
		{
			CdSize szOffSet(0, offSet - m_warpRect[j].topLeft.y);
			m_warpRect[j].Offset(szOffSet);
			offSet += m_warpRect[j].bottomRight.y - m_warpRect[j].topRight.y;
		}

		for(int j = 0; j < m_foldNum; j++)
		{
			CWarp::WarpImage(internalParam->bmpDataCopy, width, height, m_foldRect[j].ToRECT(), m_warpRect[j], rcBound, internalParam->param.pBmpData, Color(0, 0, 0, 0), FALSE);
		}
	}

	if(internalParam->frameNow == m_effectFrame)
	{
		internalParam->bLastFrame = TRUE;
	}
}

void CFoldEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	if(internalParam->param.animationEffect == 81)
	{
		int foldSize = internalParam->param.bmpSize.cy/10;
		if (foldSize > 60)
		{
			foldSize = 60;
		}
		else if(foldSize < 20)
		{
			foldSize = 20;
		}

		m_foldNum = ((internalParam->param.bmpSize.cy % foldSize == 0) ? internalParam->param.bmpSize.cy / foldSize : (internalParam->param.bmpSize.cy / foldSize + 1));

		for(int i = 0; i < m_foldNum; i++)
		{
			RECT rc = { 0, i*foldSize, internalParam->param.bmpSize.cx-1, min((i+1)*foldSize, internalParam->param.bmpSize.cy-1) };
			m_foldRect.push_back(CdRect(rc));
		}
	}
	else
	{
		int foldSize = internalParam->param.bmpSize.cx/10;
		if (foldSize > 60)
		{
			foldSize = 60;
		}
		else if(foldSize < 20)
		{
			foldSize = 20;
		}

		m_foldNum = ((internalParam->param.bmpSize.cx % foldSize == 0) ? internalParam->param.bmpSize.cx / foldSize : (internalParam->param.bmpSize.cx / foldSize + 1));

		for(int i = 0; i < m_foldNum; i++)
		{
			RECT rc = { i*foldSize, 0, min((i+1)*foldSize, internalParam->param.bmpSize.cx-1), internalParam->param.bmpSize.cy-1 };
			m_foldRect.push_back(CdRect(rc));
		}
	}

	m_effectFrame = 12;

	WarpRect warpRc;
	m_warpRect.resize(m_foldNum, warpRc);

}

void CFoldEffect::ReleaseEffectParam()
{
	m_foldRect.clear();
	m_warpRect.clear();
}

void CStackEffect::ComputeOneFrame(InternalAnimationParam *internalParam)
{
	long &width = internalParam->param.bmpSize.cx;
	long &height = internalParam->param.bmpSize.cy;	

	if (!internalParam->param.bShow)
	{
		// 从左往右堆
		if (internalParam->param.animationEffect == 82)
		{
			if(internalParam->frameNow == m_effectFrame)
			{
				memset(internalParam->param.pBmpData, 0, width*height*4);
				internalParam->bLastFrame = true;
			}
			else
			{
				RECT rcSrc = { 0 };
				rcSrc.left = (m_bandNum-m_bandAnimating-1)*m_bandSize;
				rcSrc.right = min(width, rcSrc.left + m_bandSize);
				rcSrc.bottom = height;

				
				RECT rcDes = { 0 };
				rcDes.left = min(width, rcSrc.left + (m_bandSubFrame-1)*(width-rcSrc.right)/m_bandFrameNum[m_bandAnimating]);
				rcDes.right = min(width, rcDes.left + m_bandSize);
				rcDes.bottom = height;

				if (m_bandAnimating != 0)
				{
					CMemery::SetPixels(internalParam->param.pBmpData, width*4, (m_bandNum-m_bandAnimating-1)*m_bandSize, 0, width, height, 0);
				}
			
				if(m_bandFrameNum[m_bandAnimating] != 1)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData, width*4, rcDes.left, rcDes.top, rcDes.right, rcDes.bottom, internalParam->bmpDataCopy, width*4, rcSrc.left, 0);
				}

				m_bandFrameSum ++;
				m_bandSubFrame ++;
				if (m_bandSubFrame > m_bandFrameNum[m_bandAnimating])
				{
					m_bandAnimating++;
					m_bandSubFrame = 1;
				}
			}
		}
		else if (internalParam->param.animationEffect == 83)
		{
			if(internalParam->frameNow == m_effectFrame)
			{
				memset(internalParam->param.pBmpData, 0, width*height*4);
				internalParam->bLastFrame = true;
			}
			else
			{
				RECT rcSrc = { 0 };
				rcSrc.left = (m_bandAnimating-1)*m_bandSize;
				rcSrc.right = min(width, rcSrc.left + m_bandSize);
				rcSrc.bottom = height;

				
				RECT rcDes = { 0 };
				rcDes.left = max(0, rcSrc.left - (m_bandSubFrame-1)*rcSrc.left/m_bandFrameNum[m_bandAnimating]);
				rcDes.right = min(width, rcDes.left + m_bandSize);
				rcDes.bottom = height;

				if (m_bandAnimating != 0)
				{
					CMemery::SetPixels(internalParam->param.pBmpData, width*4, 0, 0, min(width, (m_bandAnimating)*m_bandSize), height, 0);
				}
			
				if(m_bandFrameNum[m_bandAnimating] != 1)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData, width*4, rcDes.left, rcDes.top, rcDes.right, rcDes.bottom, internalParam->bmpDataCopy, width*4, rcSrc.left, 0);
				}

				m_bandFrameSum ++;
				m_bandSubFrame ++;
				if (m_bandSubFrame > m_bandFrameNum[m_bandAnimating])
				{
					m_bandAnimating++;
					m_bandSubFrame = 1;
				}
			}
		}
		else if (internalParam->param.animationEffect == 85)
		{
			if(internalParam->frameNow == m_effectFrame)
			{
				memset(internalParam->param.pBmpData, 0, width*height*4);
				internalParam->bLastFrame = true;
			}
			else
			{
				RECT rcSrc = { 0 };
				rcSrc.top = (m_bandNum-m_bandAnimating-1)*m_bandSize;
				rcSrc.bottom = min(height, rcSrc.top + m_bandSize);
				rcSrc.right = width;
				
				RECT rcDes = { 0 };
				rcDes.top = min(height, rcSrc.top + (m_bandSubFrame-1)*(height-rcSrc.top)/m_bandFrameNum[m_bandAnimating]);
				rcDes.bottom = min(height, rcDes.top + m_bandSize);
				rcDes.right = width;

				if (m_bandAnimating != 0)
				{
					CMemery::SetPixels(internalParam->param.pBmpData, width*4, 0, (m_bandNum-m_bandAnimating-1)*m_bandSize, width, height, 0);
				}
			
				if(m_bandFrameNum[m_bandAnimating] != 1)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData, width*4, rcDes.left, rcDes.top, rcDes.right, rcDes.bottom, internalParam->bmpDataCopy, width*4, 0, rcSrc.top);
				}

				m_bandFrameSum ++;
				m_bandSubFrame ++;
				if (m_bandSubFrame > m_bandFrameNum[m_bandAnimating])
				{
					m_bandAnimating++;
					m_bandSubFrame = 1;
				}
			}
		}
		else if (internalParam->param.animationEffect == 84)
		{
			if(internalParam->frameNow == m_effectFrame)
			{
				memset(internalParam->param.pBmpData, 0, width*height*4);
				internalParam->bLastFrame = true;
			}
			else
			{
				RECT rcSrc = { 0 };
				rcSrc.top = (m_bandAnimating-1)*m_bandSize;
				rcSrc.bottom = min(height, rcSrc.top + m_bandSize);
				rcSrc.right = width;

				
				RECT rcDes = { 0 };
				rcDes.top = max(0, rcSrc.top - (m_bandSubFrame-1)*rcSrc.top/m_bandFrameNum[m_bandAnimating]);
				rcDes.bottom = min(height, rcDes.top + m_bandSize);
				rcDes.right = width;

				if (m_bandAnimating != 0)
				{
					CMemery::SetPixels(internalParam->param.pBmpData, width*4, 0, 0, width, min(width, (m_bandAnimating)*m_bandSize), 0);
				}
			
				if(m_bandFrameNum[m_bandAnimating] != 1)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData, width*4, rcDes.left, rcDes.top, rcDes.right, rcDes.bottom, internalParam->bmpDataCopy, width*4, 0, rcSrc.top);
				}

				m_bandFrameSum ++;
				m_bandSubFrame ++;
				if (m_bandSubFrame > m_bandFrameNum[m_bandAnimating])
				{
					m_bandAnimating++;
					m_bandSubFrame = 1;
				}
			}
		}

	}
	else
	{
		// 从左往右堆
		if (internalParam->param.animationEffect == 82)
		{
			memset(internalParam->param.pBmpData, 0, width*height*4);
			if(internalParam->frameNow == m_effectFrame)
			{
				memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);
				internalParam->bLastFrame = true;
			}
			else
			{
				RECT rcSrc = { 0 };
				rcSrc.left = m_bandAnimating*m_bandSize;
				rcSrc.right = min(width, rcSrc.left + m_bandSize);
				rcSrc.bottom = height;

				RECT rcDes = { 0 };
				rcDes.left = max(m_bandAnimating*m_bandSize, width - m_bandSubFrame*(width - m_bandAnimating*m_bandSize)/m_bandFrameNum[m_bandNum-m_bandAnimating-1]);
				rcDes.right = min(width, rcDes.left + m_bandSize);
				rcDes.bottom = height;

				if (m_bandAnimating != 0)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData, width*4, 0, 0, min(m_bandAnimating*m_bandSize,width) , height, internalParam->bmpDataCopy, width*4, 0, 0);
				}
			
				CMemery::CopyPixels(internalParam->param.pBmpData, width*4, rcDes.left, rcDes.top, rcDes.right, rcDes.bottom, internalParam->bmpDataCopy, width*4, rcSrc.left, 0);
				
				m_bandFrameSum ++;
				m_bandSubFrame ++;
				if (m_bandSubFrame > m_bandFrameNum[m_bandNum-m_bandAnimating-1])
				{
					m_bandAnimating++;
					m_bandSubFrame = 1;
				}
			}
		}
		else if (internalParam->param.animationEffect == 83)
		{
			memset(internalParam->param.pBmpData, 0, width*height*4);
			if(internalParam->frameNow == m_effectFrame)
			{
				memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);
				internalParam->bLastFrame = true;
			}
			else
			{
				RECT rcSrc = { 0 };
				rcSrc.left = max(width-(m_bandAnimating+1)*m_bandSize, 0);
				rcSrc.right = max(width-(m_bandAnimating)*m_bandSize, 0);
				rcSrc.bottom = height;

				RECT rcDes = { 0 };
				rcDes.left = max(0, m_bandSubFrame*(width - (m_bandAnimating+1)*m_bandSize)/m_bandFrameNum[m_bandNum-m_bandAnimating-1]);
				rcDes.right = min(width, rcDes.left + m_bandSize);
				rcDes.bottom = height;

				if (m_bandAnimating != 0)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData, width*4, max(0, width-m_bandAnimating*m_bandSize), 0, width, height, internalParam->bmpDataCopy, width*4, max(0, width-m_bandAnimating*m_bandSize), 0);
				}
			
				CMemery::CopyPixels(internalParam->param.pBmpData, width*4, rcDes.left, rcDes.top, rcDes.right, rcDes.bottom, internalParam->bmpDataCopy, width*4, rcSrc.left, 0);
				
				m_bandFrameSum ++;
				m_bandSubFrame ++;
				if (m_bandSubFrame > m_bandFrameNum[m_bandNum-m_bandAnimating-1])
				{
					m_bandAnimating++;
					m_bandSubFrame = 1;
				}
			}
		}
		else if (internalParam->param.animationEffect == 84)
		{
			memset(internalParam->param.pBmpData, 0, width*height*4);
			if(internalParam->frameNow == m_effectFrame)
			{
				memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);
				internalParam->bLastFrame = true;
			}
			else
			{
				RECT rcSrc = { 0 };
				rcSrc.top = max(height-(m_bandAnimating+1)*m_bandSize, 0);
				rcSrc.bottom = max(height-(m_bandAnimating)*m_bandSize, 0);
				rcSrc.right = width;

				RECT rcDes = { 0 };
				rcDes.top = max(0, m_bandSubFrame*(height - (m_bandAnimating+1)*m_bandSize)/m_bandFrameNum[m_bandNum-m_bandAnimating-1]);
				rcDes.bottom = min(height, rcDes.top + m_bandSize);
				rcDes.right = width;

				if (m_bandAnimating != 0)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData, width*4, 0, max(0, height-m_bandAnimating*m_bandSize), width, height, internalParam->bmpDataCopy, width*4, 0, max(0, height-m_bandAnimating*m_bandSize));
				}
			
				CMemery::CopyPixels(internalParam->param.pBmpData, width*4, rcDes.left, rcDes.top, rcDes.right, rcDes.bottom, internalParam->bmpDataCopy, width*4, 0, rcSrc.top);
				
				m_bandFrameSum ++;
				m_bandSubFrame ++;
				if (m_bandSubFrame > m_bandFrameNum[m_bandNum-m_bandAnimating-1])
				{
					m_bandAnimating++;
					m_bandSubFrame = 1;
				}
			}
		}
		if (internalParam->param.animationEffect == 85)
		{
			memset(internalParam->param.pBmpData, 0, width*height*4);
			if(internalParam->frameNow == m_effectFrame)
			{
				memcpy(internalParam->param.pBmpData, internalParam->bmpDataCopy, width*height*4);
				internalParam->bLastFrame = true;
			}
			else
			{
				RECT rcSrc = { 0 };
				rcSrc.top = m_bandAnimating*m_bandSize;
				rcSrc.bottom = min(height, rcSrc.top + m_bandSize);
				rcSrc.right = width;

				RECT rcDes = { 0 };
				rcDes.top = max(m_bandAnimating*m_bandSize, height - m_bandSubFrame*(height - m_bandAnimating*m_bandSize)/m_bandFrameNum[m_bandNum-m_bandAnimating-1]);
				rcDes.bottom = min(height, rcDes.top + m_bandSize);
				rcDes.right = width;

				if (m_bandAnimating != 0)
				{
					CMemery::CopyPixels(internalParam->param.pBmpData, width*4, 0, 0, width, min(m_bandAnimating*m_bandSize,height), internalParam->bmpDataCopy, width*4, 0, 0);
				}
			
				CMemery::CopyPixels(internalParam->param.pBmpData, width*4, rcDes.left, rcDes.top, rcDes.right, rcDes.bottom, internalParam->bmpDataCopy, width*4, 0, rcSrc.top);
				
				m_bandFrameSum ++;
				m_bandSubFrame ++;
				if (m_bandSubFrame > m_bandFrameNum[m_bandNum-m_bandAnimating-1])
				{
					m_bandAnimating++;
					m_bandSubFrame = 1;
				}
			}
		}
	}
}

void CStackEffect::InitEffectParam(InternalAnimationParam *internalParam)
{
	m_bandAnimating = 0;
	m_bandFrameSum = 0;
	m_bandSubFrame = 1;
	
	if (internalParam->param.animationEffect == 82 || internalParam->param.animationEffect == 83)
	{
		m_bandSize = internalParam->param.bmpSize.cx/10;
		m_bandNum = ((internalParam->param.bmpSize.cx % m_bandSize == 0) ? internalParam->param.bmpSize.cx / m_bandSize : (internalParam->param.bmpSize.cx / m_bandSize + 1));

		// 初始化每个带状的帧数
		if (m_bandNum == 11)
		{
			m_bandFrameNum.push_back(1);
		}
		m_bandFrameNum.push_back(1);
		m_bandFrameNum.push_back(1);
		m_bandFrameNum.push_back(2);
		m_bandFrameNum.push_back(2);
		m_bandFrameNum.push_back(3);
		m_bandFrameNum.push_back(3);
		m_bandFrameNum.push_back(4);
		m_bandFrameNum.push_back(4);
		if (m_bandNum == 11)
		{
			m_bandFrameNum.push_back(4);
		}
		else
		{
			m_bandFrameNum.push_back(5);
		}
		m_bandFrameNum.push_back(5);
	}
	else if (internalParam->param.animationEffect == 84 || internalParam->param.animationEffect == 85)
	{
		m_bandSize = internalParam->param.bmpSize.cy/10;
		m_bandNum = ((internalParam->param.bmpSize.cy % m_bandSize == 0) ? internalParam->param.bmpSize.cy / m_bandSize : (internalParam->param.bmpSize.cy / m_bandSize + 1));

		// 初始化每个带状的帧数
		if (m_bandNum == 11)
		{
			m_bandFrameNum.push_back(1);
		}
		m_bandFrameNum.push_back(1);
		m_bandFrameNum.push_back(1);
		m_bandFrameNum.push_back(2);
		m_bandFrameNum.push_back(2);
		m_bandFrameNum.push_back(3);
		m_bandFrameNum.push_back(3);
		m_bandFrameNum.push_back(4);
		m_bandFrameNum.push_back(4);
		if (m_bandNum == 11)
		{
			m_bandFrameNum.push_back(4);
		}
		else
		{
			m_bandFrameNum.push_back(5);
		}
		m_bandFrameNum.push_back(5);
	}

	m_effectFrame = 30;

}

void CStackEffect::ReleaseEffectParam()
{
	m_bandFrameNum.clear();
}