#include "stdafx.h"
#include "AnimationHelper.h"

///////////////////////////////////////////////////////////////////////////////////////
DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{ 
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
typedef struct
{ 
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);
///////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, \
		int *comp, int req_comp);
	extern void     stbi_image_free(void *retval_from_stbi_load);

};

namespace DuiLib
{
	/************************************************************************/
	/* CAnimation                                                           */
	/************************************************************************/

	//////////////////////////////////////////////////////////////////////
	// Nested structures member functions
	//////////////////////////////////////////////////////////////////////
	inline int CAnimation::TGIFControlExt::GetPackedValue(enum ControlExtValues Value)
	{
		int nRet = (int) m_cPacked;
		switch(Value)
		{
		case GCX_PACKED_DISPOSAL:
			nRet = (nRet & 28) >> 2;
			break;

		case GCX_PACKED_USERINPUT:
			nRet = (nRet & 2) >> 1;
			break;

		case GCX_PACKED_TRANSPCOLOR:
			nRet &= 1;
			break;
		};

		return nRet;
	}

	inline int CAnimation::TGIFLSDescriptor::GetPackedValue(enum LSDPackedValues Value)
	{
		int nRet = (int) m_cPacked;

		switch(Value)
		{
		case LSD_PACKED_GLOBALCT:
			nRet = nRet >> 7;
			break;

		case LSD_PACKED_CRESOLUTION:
			nRet = ((nRet & 0x70) >> 4) + 1;
			break;

		case LSD_PACKED_SORT:
			nRet = (nRet & 8) >> 3;
			break;

		case LSD_PACKED_GLOBALCTSIZE:
			nRet &= 7;
			break;
		};

		return nRet;
	}

	inline int CAnimation::TGIFImageDescriptor::GetPackedValue(enum IDPackedValues Value)
	{
		int nRet = (int) m_cPacked;

		switch(Value)
		{
		case ID_PACKED_LOCALCT:
			nRet >>= 7;
			break;

		case ID_PACKED_INTERLACE:
			nRet = ((nRet & 0x40) >> 6);
			break;

		case ID_PACKED_SORT:
			nRet = (nRet & 0x20) >> 5;
			break;

		case ID_PACKED_LOCALCTSIZE:
			nRet &= 7;
			break;
		};

		return nRet;
	}

	//////////////////////////////////////////////////////////////////////
	// ctor && dtor
	//////////////////////////////////////////////////////////////////////
	CAnimation::CAnimation(IAnimationCallback* pCallback) : m_pCallback(pCallback)
	{
		// check structures size
		assert(sizeof(TGIFImageDescriptor) == 10);
		assert(sizeof(TGIFAppExtension) == 14);
		assert(sizeof(TGIFPlainTextExt) == 15);
		assert(sizeof(TGIFLSDescriptor) == 7);
		assert(sizeof(TGIFControlExt) == 8);
		assert(sizeof(TGIFCommentExt) == 2);
		assert(sizeof(TGIFHeader) == 6);

		m_pRawData = NULL;
		m_nDataSize = 0;
		m_pGIFHeader = NULL;
		m_pGIFLSDescriptor = NULL;
		m_nGlobalCTSize = 0;
		m_PictureSize.cx = m_PictureSize.cy = 0;
		m_clrBackground = RGB(255, 255, 255); // white by default
		m_nCurrFrame = 0;
		m_nCurrOffset = 0;
		m_hThread = NULL;
		m_bExitThread = true;

		m_hDrawEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(m_hDrawEvent);
		m_pvFrames = new VTFRAME;
		assert(m_pvFrames);
	}

	CAnimation::~CAnimation()
	{
		UnLoad();
		delete m_pvFrames;
		m_pvFrames = NULL;
		CloseHandle(m_hDrawEvent);
		m_hDrawEvent = NULL;
	}

	const TImageInfo* CAnimation::LoadGIF(LPCTSTR bitmap, LPCTSTR type/* = NULL*/, DWORD mask/* = 0*/)
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;
		if( type != NULL && isdigit(*bitmap) )
		{
			LPTSTR pstr = NULL;
			int iIndex = _tcstol(bitmap, &pstr, 10);
			HRSRC hPicture = ::FindResource(CPaintManagerUI::GetResourceDll(), MAKEINTRESOURCE(iIndex), type);
			if (!hPicture)
				return NULL;
			HGLOBAL hResData;
			if (!(hResData = ::LoadResource(CPaintManagerUI::GetResourceDll(), hPicture)))
			{
				::FreeResource(hPicture);
				return NULL;
			};
			dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(),hPicture);
			if( dwSize < sizeof(TGIFHeader))
			{
				::FreeResource(hPicture);
				return NULL;
			}
			pData = static_cast<LPBYTE>( malloc(dwSize*sizeof(BYTE)) );
			if (!pData)
			{
				::FreeResource(hPicture);
				return NULL;
			}
			LPBYTE pSrc = static_cast<LPBYTE>( ::LockResource(hResData) );
			if (!pSrc)
			{
				free(pData);
				::FreeResource(hPicture);
				return NULL;
			}
			::CopyMemory(pData, pSrc, dwSize);
			::FreeResource(hPicture);
		}
		else
		{
			CDuiString sFile = CPaintManagerUI::GetResourcePath();
			if( CPaintManagerUI::GetResourceZip().IsEmpty() ) {
				sFile += bitmap;
				HANDLE hFile = INVALID_HANDLE_VALUE;
				DWORD dwRead=0;
				hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
					FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
				if(INVALID_HANDLE_VALUE == hFile)
					return NULL;
				dwSize = ::GetFileSize(hFile, NULL);
				if (dwSize < sizeof(TGIFHeader))
				{
					CloseHandle(hFile);
					return NULL;
				}
				pData = static_cast<LPBYTE>( malloc(dwSize*sizeof(BYTE)) );
				if (!pData)
				{
					CloseHandle(hFile);
					return NULL;
				}
				::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
				::CloseHandle(hFile);
			}
			else 
			{
				sFile += CPaintManagerUI::GetResourceZip();
				HZIP hz = NULL;
				if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
				else hz = OpenZip((void*)sFile.GetData(), 0, 2);
				if( hz == NULL ) 
					return NULL;
				ZIPENTRY ze; 
				int i; 
				if( FindZipItem(hz, bitmap, true, &i, &ze) != 0 ) 
				{
					if( !CPaintManagerUI::IsCachedResourceZip() ) 
						CloseZip(hz);
					return NULL;
				}
				dwSize = ze.unc_size;
				if (dwSize < sizeof(TGIFHeader))
				{
					if( !CPaintManagerUI::IsCachedResourceZip() ) 
						CloseZip(hz);
					return NULL;
				}
				pData = static_cast<LPBYTE>( malloc(dwSize*sizeof(BYTE)) );
				if (!pData)
				{
					if( !CPaintManagerUI::IsCachedResourceZip() ) 
						CloseZip(hz);
					return NULL;
				}
				int res = UnzipItem(hz, i, pData, dwSize, 3);
				if( res != 0x00000000 && res != 0x00000600) {
					free(pData);
					if( !CPaintManagerUI::IsCachedResourceZip() ) 
						CloseZip(hz);
					return NULL;
				}
				if( !CPaintManagerUI::IsCachedResourceZip() ) 
					CloseZip(hz);
			}
		}

		const TImageInfo* data = LoadGIF(pData, dwSize);
		free(pData);
		return data;
	}

	const TImageInfo* CAnimation::LoadGIF(const LPBYTE pData, DWORD dwSize, DWORD mask/* = 0*/)
	{
		if (!pData || dwSize==0)
			return NULL;

		UnLoad();

		if(!(m_pRawData = const_cast<unsigned char*>(pData)))
			return NULL;

		m_nDataSize = dwSize;
		m_pGIFHeader = (TGIFHeader*)m_pRawData;

		if((memcmp(&m_pGIFHeader->m_cSignature, "GIF", 3) != 0)
			&& ((memcmp(&m_pGIFHeader->m_cVersion, "87a", 3) != 0)
			|| (memcmp(&m_pGIFHeader->m_cVersion, "89a", 3) != 0)))
		{
			// it's neither GIF87a nor GIF89a
			// do nothing
			// clear GIF variables
			m_pRawData = NULL;
			m_pGIFHeader = NULL;
			m_nDataSize = 0;

			return NULL;
		}

		m_pGIFLSDescriptor = (TGIFLSDescriptor*)(m_pRawData + sizeof(TGIFHeader));
		if(m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT) == 1)
		{
			// calculate the globat color table size
			m_nGlobalCTSize = static_cast<int>(3 * (1
				<< (m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE)
				+ 1)));
			// get the background color if GCT is present
			unsigned char * pBkClr = m_pRawData
				+ sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor)
				+ 3 * m_pGIFLSDescriptor->m_cBkIndex;
			m_clrBackground = RGB(pBkClr[0], pBkClr[1], pBkClr[2]);
		};

		// store the picture's size
		m_PictureSize.cx = m_pGIFLSDescriptor->m_wWidth;
		m_PictureSize.cy = m_pGIFLSDescriptor->m_wHeight;

		// determine frame count for this picture
		UINT nFrameCount = 0;
		ResetDataPointer();
		while(SkipNextGraphicBlock())
			nFrameCount++;

#ifdef GIF_TRACING
		OutputDebugString(_T(" -= GIF encountered\n"
			"Logical Screen dimensions = %dx%d\n"
			"Global color table = %d\n"
			"Color depth = %d\n"
			"Sort flag = %d\n"
			"Size of Global Color Table = %d\n"
			"Background color index = %d\n"
			"Pixel aspect ratio = %d\n"
			"Frame count = %d\n"
			"Background color = %06Xh\n\n"),
			m_pGIFLSDescriptor->m_wWidth,
			m_pGIFLSDescriptor->m_wHeight,
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_CRESOLUTION),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_SORT),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE),
			m_pGIFLSDescriptor->m_cBkIndex,
			m_pGIFLSDescriptor->m_cPixelAspect,
			nFrameCount,
			m_clrBackground);
		EnumGIFBlocks();
#endif

		if(nFrameCount <= 1) 
		{
			// now check the frame count
			// if there's an empty GIF or only one frame, 
			// no need to animate this GIF
			// therefore, treat it like any other pic
			m_pRawData = NULL;
			return NULL;
		}

		// if, on the contrary, there are several frames
		// then store separate frames in an array
		TFrame frame;
		UINT nBlockLen = 0;
		LPBYTE pFrameData = NULL;
		UINT nCurFrame = 0;
		//////////////////////////////////////////////////////////////////////////
		// Before rendering a frame we should take care of what's 
		// behind that frame. TFrame::m_nDisposal will be our guide:
		//   0 - no disposal specified (do nothing)
		//   1 - do not dispose (again, do nothing)
		//   2 - restore to background color (m_clrBackground)
		//   3 - restore to previous
		//   background color for first frame
		LPBYTE pDispData = static_cast<LPBYTE>(malloc(m_PictureSize.cx * m_PictureSize.cy * sizeof(COLORREF)));
		if (!pDispData)
		{
			m_pRawData = NULL;
			return NULL;
		}
		for (int n = 0; n < m_PictureSize.cx*m_PictureSize.cy; n++)
			((COLORREF*)pDispData)[n] = m_clrBackground;

		ResetDataPointer();
		while(pFrameData = GetNextGraphicBlock(&nBlockLen,
			&frame.m_nDelay,
			&frame.m_frameSize,
			&frame.m_frameOffset,
			&frame.m_nDisposal))
		{
#ifdef GIF_TRACING
			//////////////////////////////////////////////
			// uncomment the following strings if you want
			// to write separate frames on disk
			//
			//  CString szName;
			//  szName.Format(_T("%.4d.gif"),nCurFrame);
			//  WriteDataOnDisk(szName,hFrameData,nBlockLen);
			//  nCurFrame++;
#endif // GIF_TRACING
			if (!pFrameData)
				continue;

			LPBYTE pImage = NULL;
			int x,y,n;
			pImage = stbi_load_from_memory(pFrameData, nBlockLen, &x, &y, &n, 4);
			free(pFrameData);
			pFrameData = NULL;
			BITMAPINFO bmi;
			::ZeroMemory(&bmi, sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = x;
			bmi.bmiHeader.biHeight = -y;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = x * y * 4;

			bool bAlphaChannel = false;
			LPBYTE pDest = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
			if( !hBitmap ) 
			{
				stbi_image_free(pImage);
				continue;
			}

			for( int i = 0; i < x * y; i++ ) 
			{
				DWORD dwPixel = pImage[i*4+3]==BYTE(0) ? ((DWORD*)pDispData)[i] : ((DWORD*)pImage)[i];;
				LPBYTE pPixel = (LPBYTE)(&dwPixel);
				pDest[i*4 + 3] = pPixel[3];
				if( pDest[i*4 + 3] < 255 )
				{
					pDest[i*4] = (BYTE)(DWORD(pPixel[2])*pPixel[3]/255);
					pDest[i*4 + 1] = (BYTE)(DWORD(pPixel[1])*pPixel[3]/255);
					pDest[i*4 + 2] = (BYTE)(DWORD(pPixel[0])*pPixel[3]/255); 
					bAlphaChannel = true;
				}
				else
				{
					pDest[i*4] = pPixel[2];
					pDest[i*4 + 1] = pPixel[1];
					pDest[i*4 + 2] = pPixel[0]; 
				}

				if( *(DWORD*)(&pDest[i*4]) == mask ) {
					pDest[i*4] = (BYTE)0;
					pDest[i*4 + 1] = (BYTE)0;
					pDest[i*4 + 2] = (BYTE)0; 
					pDest[i*4 + 3] = (BYTE)0;
					bAlphaChannel = true;
				}
			}
			stbi_image_free(pImage);

			switch (frame.m_nDisposal)
			{
			case 0:
				memset(pDispData, 0, m_PictureSize.cx*m_PictureSize.cy*sizeof(COLORREF));
				break;
			case 1:
				memcpy(pDispData, pDest, x * y * 4);
				break;
			case 2:
				for (int n = 0; n < m_PictureSize.cx*m_PictureSize.cy; n++)
					((COLORREF*)pDispData)[n] = m_clrBackground;
				break;
			case 3:
				break;
			}

			frame.m_pImage = new TImageInfo;
			frame.m_pImage->hBitmap = hBitmap;
			frame.m_pImage->nX = x;
			frame.m_pImage->nY = y;
			frame.m_pImage->alphaChannel = bAlphaChannel;

			nCurFrame++;
			// everything went well, add this frame
			m_pvFrames->push_back(frame);
		};

		// clean after ourselves
		free(pDispData); pDispData = NULL;
		m_pRawData = NULL;
		m_nDataSize = 0;
		m_pGIFHeader = NULL;
		m_pGIFLSDescriptor = NULL;
		m_nGlobalCTSize = 0;
		m_nCurrOffset = 0;

		return m_pvFrames->empty() ? NULL : ((*m_pvFrames)[0]).m_pImage;
	}

	void CAnimation::UnLoad()
	{
		Stop();

		std::vector<TFrame>::iterator it;
		for(it = m_pvFrames->begin();it < m_pvFrames->end();it++)
		{
			if ((*it).m_pImage)
			{
				::DeleteObject((*it).m_pImage->hBitmap);
				delete (*it).m_pImage;
			}
		}
		m_pvFrames->clear();
		m_PictureSize.cx = m_PictureSize.cy = 0;
		m_clrBackground = RGB(255, 255, 255); // white by default
		m_nCurrFrame = 0;
	}

	bool CAnimation::Play()
	{
		if(!m_bExitThread)
			return false;

		if (m_hThread != NULL)
		{
			::WaitForSingleObject(m_hThread, 5000);
			::CloseHandle(m_hThread);
			m_hThread = NULL;
		}

		DWORD nDummy;
		m_bExitThread = false;
		m_hThread = (HANDLE) ::CreateThread(NULL,
			0,
			_ThreadAnimation,
			(void *)this,
			CREATE_SUSPENDED,
			&nDummy);
		if(!m_hThread)
		{
			m_bExitThread = true;
			return false;
		}
		else
			::ResumeThread(m_hThread);

		return true;
	}

	void CAnimation::Stop()
	{
		m_bExitThread = true;
		::SetEvent(m_hDrawEvent);
		// we'll wait for 5 seconds then continue execution
		::WaitForSingleObject(m_hThread, 5000);
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	bool CAnimation::IsPlaying() const
	{
		return !m_bExitThread;
	}

	SIZE CAnimation::GetSize() const
	{
		return m_PictureSize;
	}

	int CAnimation::GetFrameCount() const
	{
		if (m_pvFrames)
			return m_pvFrames->size();
		else
			return 0;
	}

	COLORREF CAnimation::GetBkColor() const
	{
		return m_clrBackground;
	}

	const TImageInfo* CAnimation::GetCurImage()
	{
		TImageInfo* data = ((*m_pvFrames)[m_nCurrFrame]).m_pImage;
		if (!IsPlaying())
			Play();
		::SetEvent(m_hDrawEvent);
		return data;
	}

	//////////////////////////////////////////////////////////////////////
	// protected methods
	//////////////////////////////////////////////////////////////////////
	int CAnimation::GetNextBlockLen() const
	{
		GIFBlockTypes nBlock = GetNextBlock();

		int nTmp;

		switch(nBlock)
		{
		case BLOCK_UNKNOWN:
			return -1;
			break;

		case BLOCK_TRAILER:
			return 1;
			break;

		case BLOCK_APPEXT:
			nTmp = GetSubBlocksLen(m_nCurrOffset + sizeof(TGIFAppExtension));
			if(nTmp > 0)
				return sizeof(TGIFAppExtension) + nTmp;
			break;

		case BLOCK_COMMEXT:
			nTmp = GetSubBlocksLen(m_nCurrOffset + sizeof(TGIFCommentExt));
			if(nTmp > 0)
				return sizeof(TGIFCommentExt) + nTmp;
			break;

		case BLOCK_CONTROLEXT:
			return sizeof(TGIFControlExt);
			break;

		case BLOCK_PLAINTEXT:
			nTmp = GetSubBlocksLen(m_nCurrOffset + sizeof(TGIFPlainTextExt));
			if(nTmp > 0)
				return sizeof(TGIFPlainTextExt) + nTmp;
			break;

		case BLOCK_IMAGE:
			TGIFImageDescriptor* pIDescr = reinterpret_cast<TGIFImageDescriptor*>(&m_pRawData[m_nCurrOffset]);
			int nLCTSize = (int)
				(pIDescr->GetPackedValue(ID_PACKED_LOCALCT) * 3 * (1
				<< (pIDescr->GetPackedValue(ID_PACKED_LOCALCTSIZE)
				+ 1)));

			int nTmp = GetSubBlocksLen(m_nCurrOffset + sizeof(TGIFImageDescriptor) + nLCTSize + 1);
			if(nTmp > 0)
				return sizeof(TGIFImageDescriptor) + nLCTSize + 1 + nTmp;
			break;
		};

		return 0;
	}

	BOOL CAnimation::SkipNextBlock()
	{
		if(!m_pRawData)
			return FALSE;

		int nLen = GetNextBlockLen();
		if((nLen <= 0) || ((m_nCurrOffset + nLen) > m_nDataSize))
			return FALSE;

		m_nCurrOffset += nLen;
		return TRUE;
	}

	BOOL CAnimation::SkipNextGraphicBlock()
	{
		if(!m_pRawData)
			return FALSE;

		// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

		enum GIFBlockTypes nBlock;

		nBlock = GetNextBlock();

		while((nBlock != BLOCK_CONTROLEXT)
			&& (nBlock != BLOCK_IMAGE)
			&& (nBlock != BLOCK_PLAINTEXT)
			&& (nBlock != BLOCK_UNKNOWN)
			&& (nBlock != BLOCK_TRAILER))
		{
			if(!SkipNextBlock())
				return NULL;
			nBlock = GetNextBlock();
		};

		if((nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER))
			return FALSE;

		// it's either a control ext.block, an image or a plain text

		if(GetNextBlockLen() <= 0)
			return FALSE;

		if(nBlock == BLOCK_CONTROLEXT)
		{
			if(!SkipNextBlock())
				return FALSE;
			nBlock = GetNextBlock();

			// skip everything until we meet an image block or a plain-text block
			while((nBlock != BLOCK_IMAGE)
				&& (nBlock != BLOCK_PLAINTEXT)
				&& (nBlock != BLOCK_UNKNOWN)
				&& (nBlock != BLOCK_TRAILER))
			{
				if(!SkipNextBlock())
					return NULL;
				nBlock = GetNextBlock();
			};

			if((nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER))
				return FALSE;
		};

		// skip the found data block (image or plain-text)
		if(!SkipNextBlock())
			return FALSE;

		return TRUE;
	}

	void CAnimation::ResetDataPointer()
	{
		// skip header and logical screen descriptor
		m_nCurrOffset = sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor) + m_nGlobalCTSize;
	}

	enum CAnimation::GIFBlockTypes CAnimation::GetNextBlock() const
	{
		switch(m_pRawData[m_nCurrOffset])
		{
		case 0x21:
			// extension block
			switch(m_pRawData[m_nCurrOffset + 1])
			{
			case 0x01:
				// plain text extension
				return BLOCK_PLAINTEXT;
				break;

			case 0xF9:
				// graphic control extension
				return BLOCK_CONTROLEXT;
				break;

			case 0xFE:
				// comment extension
				return BLOCK_COMMEXT;
				break;

			case 0xFF:
				// application extension
				return BLOCK_APPEXT;
				break;
			};
			break;

		case 0x3B:
			// trailer
			return BLOCK_TRAILER;
			break;

		case 0x2C:
			// image data
			return BLOCK_IMAGE;
			break;
		};

		return BLOCK_UNKNOWN;
	}

	UINT CAnimation::GetSubBlocksLen(UINT nStartingOffset) const
	{
		UINT nRet = 0;
		UINT nCurOffset = nStartingOffset;

		while(m_pRawData[nCurOffset] != 0 )
		{
			nRet += m_pRawData[nCurOffset] + 1;
			nCurOffset += m_pRawData[nCurOffset] + 1;
		};

		return nRet + 1;
	}

	LPBYTE CAnimation::GetNextGraphicBlock(UINT* pBlockLen, UINT* pDelay, SIZE* pBlockSize, SIZE* pBlockOffset, UINT* pDisposal)
	{
		if(!m_pRawData)
			return NULL;

		// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

		*pDisposal = 0;
		enum GIFBlockTypes nBlock;
		nBlock = GetNextBlock();

		while((nBlock != BLOCK_CONTROLEXT)
			&& (nBlock != BLOCK_IMAGE)
			&& (nBlock != BLOCK_PLAINTEXT)
			&& (nBlock != BLOCK_UNKNOWN)
			&& (nBlock != BLOCK_TRAILER))
		{
			if(!SkipNextBlock())
				return NULL;
			nBlock = GetNextBlock();
		};

		if((nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER))
			return NULL;

		// it's either a control ext.block, an image or a plain text

		int nStart = m_nCurrOffset;
		int nBlockLen = GetNextBlockLen();

		if(nBlockLen <= 0)
			return NULL;

		if(nBlock == BLOCK_CONTROLEXT)
		{
			// get the following data
			TGIFControlExt* pControl = reinterpret_cast<TGIFControlExt*>(&m_pRawData[m_nCurrOffset]);
			// store delay time
			*pDelay = pControl->m_wDelayTime;
			// store disposal method
			*pDisposal = pControl->GetPackedValue(GCX_PACKED_DISPOSAL);

			if(!SkipNextBlock())
				return NULL;
			nBlock = GetNextBlock();

			// skip everything until we find data to display
			// (image block or plain-text block)

			while((nBlock != BLOCK_IMAGE)
				&& (nBlock != BLOCK_PLAINTEXT)
				&& (nBlock != BLOCK_UNKNOWN)
				&& (nBlock != BLOCK_TRAILER))
			{
				if(!SkipNextBlock())
					return NULL;
				nBlock = GetNextBlock();
				nBlockLen += GetNextBlockLen();
			};

			if((nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER))
				return NULL;
			nBlockLen += GetNextBlockLen();
		}
		else
			*pDelay = -1; // to indicate that there was no delay value

		if(nBlock == BLOCK_IMAGE)
		{
			// store size and offsets
			TGIFImageDescriptor* pImage = reinterpret_cast<TGIFImageDescriptor*>(&m_pRawData[m_nCurrOffset]);
			pBlockSize->cx = pImage->m_wWidth;
			pBlockSize->cy = pImage->m_wHeight;
			pBlockOffset->cx = pImage->m_wLeftPos;
			pBlockOffset->cy = pImage->m_wTopPos;
		};

		if(!SkipNextBlock())
			return NULL;

		LPBYTE pData = static_cast<LPBYTE>(malloc(sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor) + m_nGlobalCTSize + nBlockLen + 1));
		if (!pData)
			return NULL;

		int nOffset = 0;

		CopyMemory(pData, m_pRawData, sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor) + m_nGlobalCTSize);
		nOffset += sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor) + m_nGlobalCTSize;

		CopyMemory(pData + nOffset, &m_pRawData[nStart], nBlockLen);
		nOffset += nBlockLen;

		pData[nOffset] = 0x3B; // trailer
		nOffset++;

		*pBlockLen = nOffset;

		return pData;
	}

#ifdef GIF_TRACING
	void CAnimation::WriteDataOnDisk(CString szFileName, HGLOBAL hData, DWORD dwSize)
	{
		CFile file;

		if(!file.Open(szFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		{
			OutputDebugString(_T("WriteData: Error creating file %s\n"), szFileName);
			return;
		};

		char* pData = reinterpret_cast<char*>(GlobalLock(hData));
		if(!pData)
		{
			OutputDebugString(_T("WriteData: Error locking memory\n"));
			return;
		};

		TRY
		{
			file.Write(pData, dwSize);
		}
		CATCH(CFileException, e);
		{
			OutputDebugString(_T("WriteData: An exception occured while writing to the file %s\n"), szFileName);
			e->Delete();
			GlobalUnlock(hData);
			file.Close();
			return;
		}
		END_CATCH GlobalUnlock(hData);
		file.Close();
	}

	void CAnimation::EnumGIFBlocks()
	{
		enum GIFBlockTypes nBlock;
		ResetDataPointer();
		while(m_nCurrOffset < m_nDataSize)
		{
			nBlock = GetNextBlock();
			switch(nBlock)
			{
			case BLOCK_UNKNOWN:
				OutputDebugString(_T("- Unknown block\n"));
				return;
				break;
			case BLOCK_TRAILER:
				OutputDebugString(_T("- Trailer block\n"));
				break;
			case BLOCK_APPEXT:
				OutputDebugString(_T("- Application extension block\n"));
				break;
			case BLOCK_COMMEXT:
				OutputDebugString(_T("- Comment extension block\n"));
				break;
			case BLOCK_CONTROLEXT:
				{
					TGIFControlExt* pControl = reinterpret_cast<TGIFControlExt*>(&m_pRawData[m_nCurrOffset]);
					OutputDebugString(_T("- Graphic control extension block (delay %d, disposal %d)\n"),
						pControl->m_wDelayTime,
						pControl->GetPackedValue(GCX_PACKED_DISPOSAL));
				};
				break;
			case BLOCK_PLAINTEXT:
				OutputDebugString(_T("- Plain text extension block\n"));
				break;
			case BLOCK_IMAGE:
				TGIFImageDescriptor* pIDescr = reinterpret_cast<TGIFImageDescriptor*>(&m_pRawData[m_nCurrOffset]);
				OutputDebugString(_T("- Image data block (%dx%d  %d,%d)\n"),
					pIDescr->m_wWidth,
					pIDescr->m_wHeight,
					pIDescr->m_wLeftPos,
					pIDescr->m_wTopPos);
				break;
			};
			SkipNextBlock();
		};
		OutputDebugString(_T("\n"));
	}
#endif // GIF_TRACING

	DWORD CAnimation::ThreadAnimation()
	{
		while(!m_bExitThread)
		{
			if (m_pCallback) m_pCallback->OnFrame();
			// wait until currframe is drawn. if currframe is not drawn over 1min
			// this ani may be no use in the future. so exit this thread to stop 
			// the animation playing by herself.
			if (::WaitForSingleObject(m_hDrawEvent, 60*1000) != WAIT_OBJECT_0)
			{
				m_bExitThread = true;
				continue;
			}

			// if the delay time is too short (like in old GIFs), wait for 100ms
			if((*m_pvFrames)[m_nCurrFrame].m_nDelay < 5)
				Sleep(100);
			else
				Sleep(10 * (*m_pvFrames)[m_nCurrFrame].m_nDelay);

			::InterlockedIncrement(&m_nCurrFrame);
			if(m_nCurrFrame == m_pvFrames->size())
				::InterlockedExchange(&m_nCurrFrame, (LONG)0);
		}

		return 0;
	}

	DWORD __stdcall CAnimation::_ThreadAnimation(LPVOID pParam)
	{
		assert(pParam);
		CAnimation* pAni = static_cast<CAnimation*>(pParam);
		if (!pAni)
			return 1;

		return pAni->ThreadAnimation();    
	}

	//************************************************************************/
	//* CEUIAniHelper                                                        */
	//************************************************************************/


	bool CEUIAniHelper::DrawAniImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const CDuiString& sImageName, \
		const CDuiString& sImageResType, RECT rcItem, RECT rcBmpPart, RECT rcCorner, DWORD dwMask, BYTE bFade, \
		bool bHole, bool bTiledX, bool bTiledY)
	{
		const TImageInfo* data = NULL;
		if ( !(data=GetAniImage((LPCTSTR)sImageName)) && !(data=pManager->GetImage((LPCTSTR)sImageName)) )
		{
			// new Image
			// try GIF first
			data = AddAniImage(pManager->GetPaintDC(), (LPCTSTR)sImageName, (LPCTSTR)sImageResType, dwMask);
			if (!data)
			{
				// may be not GIF
				if( sImageResType.IsEmpty() ) {
					data = pManager->GetImageEx((LPCTSTR)sImageName, NULL, dwMask);
				}
				else {
					data = pManager->GetImageEx((LPCTSTR)sImageName, (LPCTSTR)sImageResType, dwMask);
				}
			}
		}

		if (!data)
			return false;

		if( rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0 ) {
			rcBmpPart.right = data->nX;
			rcBmpPart.bottom = data->nY;
		}
		if (rcBmpPart.right > data->nX) rcBmpPart.right = data->nX;
		if (rcBmpPart.bottom > data->nY) rcBmpPart.bottom = data->nY;

		RECT rcTemp;
		if( !::IntersectRect(&rcTemp, &rcItem, &rc) ) return true;
		if( !::IntersectRect(&rcTemp, &rcItem, &rcPaint) ) return true;

		CRenderEngine::DrawImage(hDC, data->hBitmap, rcItem, rcPaint, rcBmpPart, rcCorner, data->alphaChannel, bFade, bHole, bTiledX, bTiledY);

		return true; 
	}

	bool CEUIAniHelper::DrawImageEx(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, 
		LPCTSTR pStrImage, LPCTSTR pStrModify)
	{
		if ((pManager == NULL) || (hDC == NULL)) return false;

		// 1¡¢aaa.jpg
		// 2¡¢file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
		// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'

		CDuiString sImageName = pStrImage;
		CDuiString sImageResType;
		RECT rcItem = rc;
		RECT rcBmpPart = {0};
		RECT rcCorner = {0};
		DWORD dwMask = 0;
		BYTE bFade = 0xFF;
		bool bHole = false;
		bool bTiledX = false;
		bool bTiledY = false;

		int image_count = 0;

		CDuiString sItem;
		CDuiString sValue;
		LPTSTR pstr = NULL;

		for( int i = 0; i < 2; ++i,image_count = 0 ) {
			if( i == 1)
				pStrImage = pStrModify;

			if( !pStrImage ) continue;

			while( *pStrImage != _T('\0') ) {
				sItem.Empty();
				sValue.Empty();
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
					LPTSTR pstrTemp = ::CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sItem += *pStrImage++;
					}
				}
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				if( *pStrImage++ != _T('=') ) break;
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				if( *pStrImage++ != _T('\'') ) break;
				while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
					LPTSTR pstrTemp = ::CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sValue += *pStrImage++;
					}
				}
				if( *pStrImage++ != _T('\'') ) break;
				if( !sValue.IsEmpty() ) {
					if( sItem == _T("file") || sItem == _T("res") ) {
						if( image_count > 0 )
							DrawAniImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType,
							rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);

						sImageName = sValue;
						if( sItem == _T("file") )
							++image_count;
					}
					else if( sItem == _T("restype") ) {
						if( image_count > 0 )
							DrawAniImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType,
							rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);

						sImageResType = sValue;
						++image_count;
					}
					else if( sItem == _T("dest") ) {
						rcItem.left = rc.left + _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
						rcItem.top = rc.top + _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
						rcItem.right = rc.left + _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
						if (rcItem.right > rc.right) rcItem.right = rc.right;
						rcItem.bottom = rc.top + _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
						if (rcItem.bottom > rc.bottom) rcItem.bottom = rc.bottom;
					}
					else if( sItem == _T("source") ) {
						rcBmpPart.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
						rcBmpPart.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
						rcBmpPart.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
						rcBmpPart.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);  
					}
					else if( sItem == _T("corner") ) {
						rcCorner.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
						rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
						rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
						rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
					}
					else if( sItem == _T("mask") ) {
						if( sValue[0] == _T('#')) dwMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
						else dwMask = _tcstoul(sValue.GetData(), &pstr, 16);
					}
					else if( sItem == _T("fade") ) {
						bFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
					}
					else if( sItem == _T("hole") ) {
						bHole = (_tcscmp(sValue.GetData(), _T("true")) == 0);
					}
					else if( sItem == _T("xtiled") ) {
						bTiledX = (_tcscmp(sValue.GetData(), _T("true")) == 0);
					}
					else if( sItem == _T("ytiled") ) {
						bTiledY = (_tcscmp(sValue.GetData(), _T("true")) == 0);
					}
				}
				if( *pStrImage++ != _T(' ') ) break;
			}
		}

		DrawAniImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType,
			rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);

		return true;
	}

	const TImageInfo* CEUIAniHelper::GetAniImage(LPCTSTR bitmap)
	{
		CAnimation* data = static_cast<CAnimation*>(m_mAniHash.Find(bitmap));
		if (data)
			return data->GetCurImage();
		else
			return NULL;
	}

	const TImageInfo* CEUIAniHelper::AddAniImage(HDC hDC, LPCTSTR bitmap, LPCTSTR type/* = NULL*/, DWORD mask/* = 0*/)
	{
		CAnimation* pAniImage = new CAnimation(this);
		if (!pAniImage->LoadGIF(bitmap, type, mask))
		{
			delete pAniImage;
			return NULL;
		}
		if ( !m_mAniHash.Insert(bitmap, pAniImage) )
		{
			delete pAniImage;
			return NULL;
		}

		pAniImage->Play();
		return pAniImage->GetCurImage();
	}
}
