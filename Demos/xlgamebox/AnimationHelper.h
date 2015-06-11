#ifndef _EUI_ANIMATION_HELPER__H__
#define	_EUI_ANIMATION_HELPER__H__

#include <vector>

namespace DuiLib
{
	class IAnimationCallback
	{
	public:
		virtual void OnFrame() = 0;
	};

	class CAnimation
	{
#pragma pack(1)   // turn byte alignment on
		enum GIFBlockTypes
		{
			BLOCK_UNKNOWN,
			BLOCK_APPEXT,
			BLOCK_COMMEXT,
			BLOCK_CONTROLEXT,
			BLOCK_PLAINTEXT,
			BLOCK_IMAGE,
			BLOCK_TRAILER
		};

		enum ControlExtValues // graphic control extension packed field values
		{
			GCX_PACKED_DISPOSAL,  // disposal method
			GCX_PACKED_USERINPUT,
			GCX_PACKED_TRANSPCOLOR
		};

		enum LSDPackedValues  // logical screen descriptor packed field values
		{
			LSD_PACKED_GLOBALCT,
			LSD_PACKED_CRESOLUTION,
			LSD_PACKED_SORT,
			LSD_PACKED_GLOBALCTSIZE
		};

		enum IDPackedValues   // image descriptor packed field values
		{
			ID_PACKED_LOCALCT,
			ID_PACKED_INTERLACE,
			ID_PACKED_SORT,
			ID_PACKED_LOCALCTSIZE
		};

		struct TGIFHeader     // GIF header
		{
			char   m_cSignature[3]; // Signature - Identifies the GIF Data Stream
			// This field contains the fixed value 'GIF'
			char   m_cVersion[3];   // Version number. May be one of the following:
			// "87a" or "89a"
		};

		struct TGIFLSDescriptor // Logical Screen Descriptor
		{
			WORD          m_wWidth;         // 2 bytes. Logical screen width
			WORD          m_wHeight;        // 2 bytes. Logical screen height
			unsigned char m_cPacked;        // packed field
			unsigned char m_cBkIndex;       // 1 byte. Background color index
			unsigned char m_cPixelAspect;   // 1 byte. Pixel aspect ratio
			inline int    GetPackedValue(enum LSDPackedValues Value);
		};

		struct TGIFAppExtension // application extension block
		{
			unsigned char m_cExtIntroducer;     // extension introducer (0x21)
			unsigned char m_cExtLabel;          // app. extension label (0xFF)
			unsigned char m_cBlockSize;         // fixed value of 11
			char          m_cAppIdentifier[8];  // application identifier
			char          m_cAppAuth[3];        // application authentication code
		};

		struct TGIFControlExt // graphic control extension block
		{
			unsigned char m_cExtIntroducer; // extension introducer (0x21)
			unsigned char m_cControlLabel;  // control extension label (0xF9)
			unsigned char m_cBlockSize;     // fixed value of 4
			unsigned char m_cPacked;        // packed field
			WORD          m_wDelayTime;     // delay time
			unsigned char m_cTColorIndex;   // transparent color index
			unsigned char m_cBlockTerm;     // block terminator (0x00)
			inline int    GetPackedValue(enum ControlExtValues Value);
		};

		struct TGIFCommentExt  // comment extension block
		{
			unsigned char m_cExtIntroducer; // extension introducer (0x21)
			unsigned char m_cCommentLabel;  // comment extension label (0xFE)
		};

		struct TGIFPlainTextExt // plain text extension block
		{
			unsigned char m_cExtIntroducer;     // extension introducer (0x21)
			unsigned char m_cPlainTextLabel;    // text extension label (0x01)
			unsigned char m_cBlockSize;         // fixed value of 12
			WORD          m_wLeftPos;           // text grid left position
			WORD          m_wTopPos;            // text grid top position
			WORD          m_wGridWidth;         // text grid width
			WORD          m_wGridHeight;        // text grid height
			unsigned char m_cCellWidth;         // character cell width
			unsigned char m_cCellHeight;        // character cell height
			unsigned char m_cFgColor;           // text foreground color index
			unsigned char m_cBkColor;           // text background color index
		};

		struct TGIFImageDescriptor // image descriptor block
		{
			unsigned char m_cImageSeparator;    // image separator byte (0x2C)
			WORD          m_wLeftPos;           // image left position
			WORD          m_wTopPos;            // image top position
			WORD          m_wWidth;             // image width
			WORD          m_wHeight;            // image height
			unsigned char m_cPacked;            // packed field
			inline int    GetPackedValue(enum IDPackedValues Value);
		};
#pragma pack() // turn byte alignment off
		struct TFrame    // structure that keeps a single frame info
		{
			TImageInfo* m_pImage;       // pointer to one frame image
			SIZE        m_frameSize;
			SIZE        m_frameOffset;
			UINT        m_nDelay;       // delay (in 1/100s of a second)
			UINT        m_nDisposal;    // disposal method
		};
		typedef std::vector<TFrame>     VTFRAME;

	public:
		CAnimation(IAnimationCallback* pCallback);
		~CAnimation();

		const TImageInfo* GetCurImage();
		// loads a picture from a file or a program resource
		const TImageInfo* LoadGIF(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);
		// loads a picture from a memory block (allocated by malloc)
		// Warning: this function DOES NOT free the memory, pointed to by pData
		const TImageInfo* LoadGIF(const LPBYTE pData, DWORD dwSize, DWORD mask = 0);
		SIZE GetSize() const;
		int GetFrameCount() const;
		COLORREF GetBkColor() const;

		bool Play();    // play the animation (just change m_nCurrFrame)
		void Stop();    // stops animation
		void UnLoad();  // stops animation plus releases all resources
		bool IsPlaying() const;

	protected:
		int GetNextBlockLen() const;
		BOOL SkipNextBlock();
		BOOL SkipNextGraphicBlock();
		void ResetDataPointer();
		enum GIFBlockTypes GetNextBlock() const;
		UINT GetSubBlocksLen(UINT nStartingOffset) const;
		// create a single-frame GIF from one of frames, and put in a memeory block (allocated by malloc)
		// Warning: we should free the memeory block, pointed to by return value
		LPBYTE GetNextGraphicBlock(UINT* pBlockLen, UINT* pDelay, SIZE* pBlockSize, SIZE* pBlockOffset, UINT* pDisposal);
#ifdef GIF_TRACING
		void EnumGIFBlocks();
		void WriteDataOnDisk(CString szFileName, HGLOBAL hData, DWORD dwSize);
#endif // GIF_TRACING

	private:
		unsigned char*    m_pRawData;           // 解析时用于暂存动画文件数据 解析结束后重置无效
		UINT              m_nDataSize;          // GIF文件大小
		TGIFHeader*       m_pGIFHeader;         // GIF文件头
		TGIFLSDescriptor* m_pGIFLSDescriptor;   // 逻辑屏幕标识符
		UINT              m_nGlobalCTSize;      // 全局颜色列表大小
		SIZE              m_PictureSize;        // 图像尺寸
		COLORREF          m_clrBackground;      // 背景色
		volatile long     m_nCurrFrame;         // 当前帧索引值
		UINT              m_nCurrOffset;        // 块读取偏移量
		VTFRAME*          m_pvFrames;           // 帧数组

		HANDLE            m_hThread;
		HANDLE            m_hDrawEvent;         // 绘图事件 代表当前帧已经被读取用于绘制 （防止由于渲染速度慢而导致跳帧）
		volatile bool     m_bExitThread;
		IAnimationCallback* m_pCallback;

		DWORD ThreadAnimation();
		static DWORD __stdcall _ThreadAnimation(LPVOID pParam);
	};

	class CEUIAniHelper : public IAnimationCallback
	{
	public:
		~CEUIAniHelper()
		{
			CAnimation* data = NULL;
			while (m_mAniHash.GetSize() > 0)
			{
				if(LPCTSTR key = m_mAniHash.GetAt(0)) {
					data = static_cast<CAnimation*>(m_mAniHash.Find(key));
					if (data)
					{
						delete data;
						data = NULL;
					}
					m_mAniHash.Remove(key);
				}
			}
		}
		/**
		* 绘图函数的增强版 替代原有CControlUI::DrawImage 和 CRenderEngine::DrawImageString函数
		* Note: 被PaintXXImage调用
		*/
		bool DrawImageEx(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, 
			LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
		/**
		* 绘图函数的增强版 替代原有DuiLib::DrawImage函数
		* Note: 包括判断文件类型、初始化读取GIF内容等
		*/
		bool DrawAniImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const CDuiString& sImageName, \
			const CDuiString& sImageResType, RECT rcItem, RECT rcBmpPart, RECT rcCorner, DWORD dwMask, BYTE bFade, \
			bool bHole, bool bTiledX, bool bTiledY);

	protected:
		const TImageInfo* GetAniImage(LPCTSTR bitmap);
		const TImageInfo* AddAniImage(HDC hDC, LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);

	protected:
		CStdStringPtrMap    m_mAniHash;
	};
}

#endif
