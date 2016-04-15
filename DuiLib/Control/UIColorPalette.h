/*********************************************************************
*  这是为duilib开发的动态调色板控件，完全模仿酷狗和QQ的调色板功能
*  使用了HSB绘制模式，结合我自己研究出的一个算法绘制界面
*  这个调色板可以大致模仿出酷狗调色板的样式，但是还有差别
*  如果有哪位朋友知道更好的算法或者有类似的代码，或者此代码有bug，请联系我
*  By:Redrain  QQ：491646717   2014.8.19
*  如果有类似的需求，可以修改这个类
*  thumbimage属性指定选择颜色的光标素材的位置，素材我附加到了压缩包中，请自己修改
*  sample:<ColorPalette name="Pallet" width="506" height="220" palletheight="200" barheight="14" padding="8,5,0,0" bkcolor="#FFFFFFFF" thumbimage="UI\skin\cursor.png" />
*
*  此模块是在我原来的代码上由网友“风之羽翼”重新编写的，修改了算法，大幅提升了动态调色板的性能，之后我又进行了一点bug，再次感谢风之羽翼的代码
*********************************************************************/


#ifndef UI_PALLET_H
#define UI_PALLET_H
#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CColorPaletteUI : public CControlUI
	{
		DECLARE_DUICONTROL(CColorPaletteUI)
	public:
		CColorPaletteUI();
		virtual ~CColorPaletteUI();

		//获取最终被选择的颜色，可以直接用于设置duilib背景色
		DWORD GetSelectColor();
		void SetSelectColor(DWORD dwColor);

		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		//设置/获取 Pallet（调色板主界面）的高度
		void SetPalletHeight(int nHeight);
		int	GetPalletHeight() const;

		//设置/获取 下方Bar（亮度选择栏）的高度
		void SetBarHeight(int nHeight);
		int GetBarHeight() const;
		//设置/获取 选择图标的路径
		void SetThumbImage(LPCTSTR pszImage);
		LPCTSTR GetThumbImage() const;

		virtual void SetPos(RECT rc);
		virtual void DoInit();
		virtual void DoEvent(TEventUI& event);
		virtual void PaintBkColor(HDC hDC);
		virtual void PaintPallet(HDC hDC);

	protected:
		//更新数据
		void UpdatePalletData();
		void UpdateBarData();

	private:
		HDC			m_MemDc;
		HBITMAP		m_hMemBitmap;
		BITMAP		m_bmInfo;
		BYTE		*m_pBits;
		UINT		m_uButtonState;
		bool		m_bIsInBar;
		bool		m_bIsInPallet;
		int			m_nCurH;
		int			m_nCurS;
		int			m_nCurB;

		int			m_nPalletHeight;
		int			m_nBarHeight;
		CDuiPoint		m_ptLastPalletMouse;
		CDuiPoint		m_ptLastBarMouse;
		CDuiString  m_strThumbImage;
	};
}

#endif // UI_PALLET_H