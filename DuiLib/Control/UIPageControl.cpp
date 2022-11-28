#include "StdAfx.h"
#include "UIPageControl.h"

#pragma warning(disable:4996)
namespace DuiLib
{
    IMPLEMENT_DUICONTROL(CPageControlUI)

#define TEXT_MORE _T("...")
#define PAGE_OPT_GROUP _T("PAGE_OPT_GROUP")

        DuiLib::CPageControlUI::CPageControlUI()
        : m_BtnPrevious(new CButtonUI)
        , m_BtnNext(new CButtonUI)
        , m_BtnNextMore(new CButtonUI)
        , m_BtnGoto(new CButtonUI)
        , m_EdtPageNo(new CEditUI)
        , m_ConCurSel(nullptr)
        , m_nMaxPage(0)
        , m_nSelPageNo(-1)
        , m_dwHotBkColor(0xffc3c3c3)
        , m_dwSelTextColor(0xff00ff00)
        , m_dwNormalTextColor(0xff000000)
        , m_dwNormalBkColor(0)
        , m_dwHotTextColor(0xff000000)
        , m_dwSelectedBkColor(0)
        , m_nFont(-1)
    {
        SetPageNoSize();
        SetGotoEditSize();

        m_BtnPrevious->SetText(_T("<"));
        m_BtnPrevious->SetHotBkColor(m_dwHotBkColor);
        m_BtnPrevious->OnNotify += MakeDelegate(this, &CPageControlUI::OnBtnClick);
        Add(m_BtnPrevious);

        m_BtnNext->SetText(_T(">"));
        m_BtnNext->SetHotBkColor(m_dwHotBkColor);
        m_BtnNext->OnNotify += MakeDelegate(this, &CPageControlUI::OnBtnClick);
        Add(m_BtnNext);

        m_BtnNextMore->SetText(_T("..."));
        m_BtnNextMore->SetHotBkColor(m_dwHotBkColor);
        m_BtnNextMore->OnNotify += MakeDelegate(this, &CPageControlUI::OnBtnClick);
        Add(m_BtnNextMore);

        m_BtnGoto->SetText(_T("Go to"));
        m_BtnGoto->SetHotBkColor(m_dwHotBkColor);
        m_BtnGoto->OnNotify += MakeDelegate(this, &CPageControlUI::OnBtnClick);
        Add(m_BtnGoto);
        Add(m_EdtPageNo);
        m_EdtPageNo->SetNumberOnly(true);
        SetMaxChar();
        SetMaxPages(1);
    }

    DuiLib::CPageControlUI::~CPageControlUI()
    {
        RemoveAll();
    }

    void DuiLib::CPageControlUI::SetMaxPages(int nPages, int nShowNum /*= 6*/)
    {
        if (m_nShowPage == nShowNum && m_nMaxPage == nPages)
        {
            return;
        }

        m_nMaxPage = max(nPages, 1);
        m_nShowPage = max(nShowNum, 5);
        ResetAllPages();
    }

    void DuiLib::CPageControlUI::SetMaxChar(int nNum /*= 2*/)
    {
        m_EdtPageNo->SetMaxChar(nNum);
    }

    void DuiLib::CPageControlUI::SetFont(int nFont)
    {
        m_nFont = nFont;
        m_BtnPrevious->SetFont(nFont);
        m_BtnNext->SetFont(nFont);
        m_BtnGoto->SetFont(nFont);
        m_EdtPageNo->SetFont(nFont);
        m_BtnNextMore->SetFont(nFont);

        for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
        {
            COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
            page->SetFont(nFont);
        }
    }

    void DuiLib::CPageControlUI::SetPageSelectedTextColor(DWORD cr /*= 0xff00ff00*/)
    {
        m_dwSelTextColor = cr;
        for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
        {
            COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
            page->SetSelectedTextColor(cr);
        }
    }

    void DuiLib::CPageControlUI::SetPageSelectedBkColor(DWORD cr)
    {
        m_dwSelectedBkColor = cr;
        for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
        {
            COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
            page->SetSelectedBkColor(m_dwSelectedBkColor);
        }
    }

    void DuiLib::CPageControlUI::SetPageNormalTextColor(DWORD cr /*= 0xff000000*/)
    {
        m_dwNormalTextColor = cr;
        for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
        {
            COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
            page->SetTextColor(cr);
        }
    }

    void DuiLib::CPageControlUI::SetPageBkColor(DWORD cr /*= 0x00000000*/)
    {
        m_dwNormalBkColor = cr;
        for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
        {
            COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
            page->SetBkColor(cr);
        }
    }

    void DuiLib::CPageControlUI::SetPageHotBkColor(DWORD cr)
    {
        m_dwHotBkColor = cr;
        m_BtnPrevious->SetHotBkColor(m_dwHotBkColor);
        m_BtnNext->SetHotBkColor(m_dwHotBkColor);
        m_BtnGoto->SetHotBkColor(m_dwHotBkColor);
        m_BtnNextMore->SetHotBkColor(m_dwHotBkColor);

        for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
        {
            COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
            page->SetHotBkColor(m_dwHotBkColor);
        }
    }

    void DuiLib::CPageControlUI::SetPageHotTextColor(DWORD cr)
    {
        m_dwHotTextColor = cr;
        m_BtnPrevious->SetHotTextColor(m_dwHotTextColor);
        m_BtnNext->SetHotTextColor(m_dwHotTextColor);
        m_BtnGoto->SetHotTextColor(m_dwHotTextColor);
        m_BtnNextMore->SetHotTextColor(m_dwHotTextColor);

        for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
        {
            COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
            page->SetHotTextColor(m_dwHotTextColor);
        }
    }



    void DuiLib::CPageControlUI::SetPageNoSize(int w /*= 30*/, int h /*= 30*/)
    {
        m_szPage.cx = w;
        m_szPage.cy = h;
        UpdatePosition();
    }

    void DuiLib::CPageControlUI::SetGotoEditSize(int w /*= 50*/, int h /*= 30*/)
    {
        m_szGotoEdit.cx = w;
        m_szGotoEdit.cy = h;
        UpdatePosition();
    }

    void DuiLib::CPageControlUI::SetGotoEditBorderColor(DWORD cr /*= 0x00000000*/)
    {
        m_EdtPageNo->SetBorderColor(cr);
    }

    void DuiLib::CPageControlUI::SetGotoEditBorderSize(int size /*= 1*/)
    {
        m_EdtPageNo->SetBorderSize(size);
    }

    void DuiLib::CPageControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (_tcsicmp(pstrName, _T("maxpage")) == 0)
        {
            LPTSTR pstr = NULL;
            int nMaxPageNo = _tcstol(pstrValue, &pstr, 10); ASSERT(pstr);
            int nShowPage = _tcstol(pstr + 1, &pstr, 10);
            SetMaxPages(nMaxPageNo, nShowPage);
            return;

        }
        else if (_tcsicmp(pstrName, _T("font")) == 0)
        {
            LPTSTR pstr = NULL;
            int nFont = _tcstol(pstrValue, &pstr, 10);
            SetFont(nFont);
            return;
        }
        else if (_tcsicmp(pstrName, _T("pageselectedtextcolor")) == 0)
        {
            while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetPageSelectedTextColor(clrColor);
            return;
        }
        else if (_tcsicmp(pstrName, _T("pageselectedbkcolor")) == 0)
        {
            while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetPageSelectedBkColor(clrColor);
            return;
        }
        else if (_tcsicmp(pstrName, _T("pagetextcolor")) == 0)
        {
            while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetPageNormalTextColor(clrColor);
            return;
        }
        else if (_tcsicmp(pstrName, _T("pagebkcolor")) == 0)
        {
            while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetPageBkColor(clrColor);
            return;
        }
        else if (_tcsicmp(pstrName, _T("pagehotbkcolor")) == 0)
        {
            while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetPageHotBkColor(clrColor);
            return;
        }
        else if (_tcsicmp(pstrName, _T("pagehottextcolor")) == 0)
        {
            while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetPageHotTextColor(clrColor);
            return;
        }
        else if (_tcsicmp(pstrName, _T("gotoeditbordercolor")) == 0)
        {
            while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetGotoEditBorderColor(clrColor);
            return;
        }
        else if (_tcsicmp(pstrName, _T("gotoeditbordersize")) == 0)
        {
            LPTSTR pstr = NULL;
            int nFont = _tcstol(pstrValue, &pstr, 10);
            SetGotoEditBorderSize(nFont);
            return;
        }
        else if (_tcsicmp(pstrName, _T("editmaxchar")) == 0)
        {
            LPTSTR pstr = NULL;
            int nNum = _tcstol(pstrValue, &pstr, 10);
            SetMaxChar(nNum);
            return;
        }
        CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
    }

    void DuiLib::CPageControlUI::ResetAllPages()
    {
        int nPages = m_OptPageNoArr.GetSize();
        if (nPages < m_nShowPage)
        {
            for (int i = nPages; i < m_nShowPage; i++)
            {
                auto page = new COptionUI();
                TCHAR szPageText[20];
                _sntprintf(szPageText, 20, _T("%u"), i + 1);
                page->SetText(szPageText);
                page->SetGroup(PAGE_OPT_GROUP);
                page->OnNotify += MakeDelegate(this, &CPageControlUI::OnOptionSelChanged);
                page->SetTag(i);
                page->SetFont(m_nFont);
                page->SetTextColor(m_dwNormalTextColor);
                page->SetBkColor(m_dwNormalBkColor);
                page->SetSelectedTextColor(m_dwSelTextColor);
                page->SetSelectedBkColor(m_dwSelectedBkColor);
                page->SetHotTextColor(m_dwHotTextColor);
                page->SetHotBkColor(m_dwHotBkColor);

                m_OptPageNoArr.Add(page);
                Add(page);
            }
        }
        else
        {
            for (int i = nPages - 1; i >= m_nShowPage; i--)
            {
                COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
                m_OptPageNoArr.Remove(i);
                Remove(page);
            }
        }

        //更新索引
        UpdateItemIndex();

        //更新页码
        UpdatePageNo(0, true);

        //选中
        if (m_nSelPageNo == -1 || m_nSelPageNo > m_nShowPage)
        {
            SelectPage(0);
        }

        UpdatePosition();
        UpdateMoreBtnVisiable();
    }

    void DuiLib::CPageControlUI::UpdatePosition()
    {
        int controlWidth = 0;
        m_BtnPrevious->SetFixedWidth(m_szPage.cx);
        m_BtnPrevious->SetFixedHeight(m_szPage.cy);
        controlWidth += m_szPage.cx;
        for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
        {
            if (i == m_OptPageNoArr.GetSize() - 1)
            {
                m_BtnNextMore->SetFixedWidth(m_szPage.cx);
                m_BtnNextMore->SetFixedHeight(m_szPage.cy);
                controlWidth += m_szPage.cx;
            }

            COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
            page->SetFixedWidth(m_szPage.cx);
            page->SetFixedHeight(m_szPage.cy);
            controlWidth += m_szPage.cx;
        }

        m_BtnNext->SetFixedWidth(m_szPage.cx);
        m_BtnNext->SetFixedHeight(m_szPage.cy);
        controlWidth += m_szPage.cx;
		m_BtnGoto->SetPadding(CDuiRect(m_szPage.cx + 10,0,0,0));
        m_BtnGoto->SetFixedWidth(m_szPage.cx + 10);
        m_BtnGoto->SetFixedHeight(m_szPage.cy);
        controlWidth += m_szPage.cx + m_szPage.cx + 10 + 10;	
        m_EdtPageNo->SetPadding(CDuiRect(5,(m_szPage.cy - m_szGotoEdit.cy) / 2,0,0));
        m_EdtPageNo->SetFixedWidth(m_szGotoEdit.cx);
        m_EdtPageNo->SetFixedHeight(m_szGotoEdit.cy);
        controlWidth += m_szGotoEdit.cx + 5;

        SetFixedWidth(controlWidth + GetBorderSize() * 2);
    }


    void DuiLib::CPageControlUI::UpdatePageNo(int iDelta, bool bReset)
    {
        if (bReset)
        {
            for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
            {
                COptionUI* page = (COptionUI*)m_OptPageNoArr[i];

                TCHAR szPage[20];
                _sntprintf(szPage, 20, _T("%u"), i + 1);
                page->SetText(szPage);
                page->SetVisible(true);

                if (i >= m_nMaxPage)
                {
                    page->SetVisible(false);
                }
            }

            COptionUI* page = (COptionUI*)m_OptPageNoArr[m_OptPageNoArr.GetSize() - 1];

            TCHAR szPage[20];
            _sntprintf(szPage, 20, _T("%u"), m_nMaxPage);
            page->SetText(szPage);
        }
        else
        {
            for (int i = 0; i < m_OptPageNoArr.GetSize() - 1; i++)
            {
                COptionUI* page = (COptionUI*)m_OptPageNoArr[i];
                int nPage = GetPageNoByControl(page);
                nPage += iDelta;

                TCHAR szPage[20];
                _sntprintf(szPage, 20, _T("%u"), nPage);
                page->SetText(szPage);
            }
        }

    }

    void DuiLib::CPageControlUI::UpdateItemIndex()
    {
        int nIndex = 1;
        for (int i = 0; i < m_OptPageNoArr.GetSize(); i++)
        {
            if (i == m_OptPageNoArr.GetSize() - 1)
            {
                SetItemIndex(m_BtnNextMore, nIndex++);
            }

            SetItemIndex((CControlUI*)m_OptPageNoArr[i], nIndex++);
        }

        SetItemIndex(m_BtnNext, nIndex++);
        SetItemIndex(m_BtnGoto, nIndex++);
        SetItemIndex(m_EdtPageNo, nIndex++);
    }

    void DuiLib::CPageControlUI::GotoPage(int nPageNo, bool bAutoGotoOneWhenError)
    {
        int nGotoPageNo = nPageNo;
        if (nGotoPageNo > 0 && nGotoPageNo <= m_nMaxPage)
        {
            int nExistsPageNoIdx = GetIndexByPageNo(nGotoPageNo);
            //页面存在但是不是最后一个页码
            if (nExistsPageNoIdx != -1 && nExistsPageNoIdx != m_OptPageNoArr.GetSize() - 1)
            {
                SelectPage(nExistsPageNoIdx);
                return;
            }

            int nFirstPageVal = 1;
            int nSelIndex = m_nMaxPage - nGotoPageNo + 1 - m_nShowPage;
            if (nSelIndex >= 0)
            {
                nFirstPageVal = nGotoPageNo;
                nSelIndex = 0;
            }
            else
            {
                nFirstPageVal = nGotoPageNo + nSelIndex;
            }

            int nFirstPageNo = GetPageNoByControl((CControlUI*)m_OptPageNoArr[0]);
            if (nFirstPageNo >= 0)
            {
                int delta = nFirstPageVal - nFirstPageNo;
                if (delta != 0)
                {
                    UpdatePageNo(delta);
                    UpdateMoreBtnVisiable();
                }
                SelectPage(-nSelIndex);
            }
        }
        else if (bAutoGotoOneWhenError)
        {
            DUITRACE(_T("索引越界，选中第一个页"));
            SelectPage(0);
        }
    }

    int DuiLib::CPageControlUI::GetIndexByPageNo(int nPageNo)
    {
        for (int i = 0; i < GetShowPageNum(); i++)
        {
            int pageNo = GetPageNoByControl((CControlUI*)m_OptPageNoArr[i]);
            if (nPageNo == pageNo)
            {
                return i;
            }
        }

        return -1;
    }

    void DuiLib::CPageControlUI::SelectPage(int nPageIdx)
    {
        m_EdtPageNo->SetText(_T(""));
        if (nPageIdx >= GetShowPageNum() || nPageIdx < 0)
        {
            nPageIdx = 0;
        }

        int nCurSelPageNo = m_nSelPageNo;
        int nNewPageNo = GetPageNoByControl((COptionUI*)m_OptPageNoArr[nPageIdx]);
        if (nNewPageNo == -1)
        {
            return;
        }

        if (nCurSelPageNo != nNewPageNo)
        {
            m_nSelPageNo = nNewPageNo;
            m_ConCurSel = (COptionUI*)m_OptPageNoArr[nPageIdx];
            if (!m_ConCurSel->IsSelected())
            {
                m_ConCurSel->Selected(true, false);
            }
            if (m_pManager)
            {
                m_pManager->SendNotify(this, DUI_MSGTYPE_PAGECHANED, nNewPageNo, nCurSelPageNo);
                DUITRACE(_T("select page changed %d -> %d"), nCurSelPageNo, nNewPageNo);
            }
        }

    }

    int DuiLib::CPageControlUI::GetCurSel()
    {
        return m_nSelPageNo;
    }

    void DuiLib::CPageControlUI::UpdateMoreBtnVisiable()
    {
        bool bVisiable = false;
        int nLastPageIdx = GetShowPageNum() - 1;
        int nPrevPageIdx = GetShowPageNum() - 2;
        if (nLastPageIdx < 0 || nPrevPageIdx < 0)
        {
            m_BtnNextMore->SetVisible(bVisiable);
            return;
        }

        COptionUI* lastPage = (COptionUI*)m_OptPageNoArr[nLastPageIdx];
        COptionUI* prevPage = (COptionUI*)m_OptPageNoArr[nPrevPageIdx];

        int iLastPageNo = GetPageNoByControl(lastPage);
        int iPrevPageNo = GetPageNoByControl(prevPage);

        bVisiable = (iLastPageNo - iPrevPageNo > 1 ? true : false);
        m_BtnNextMore->SetVisible(bVisiable);
    }

    bool DuiLib::CPageControlUI::IsExistNextMore()
    {
        bool bVisiable = m_BtnNextMore->IsVisible();
        return bVisiable;
    }

    bool DuiLib::CPageControlUI::OnOptionSelChanged(void* p)
    {
        TNotifyUI* notify = (TNotifyUI*)p;
        if (notify->sType == DUI_MSGTYPE_SELECTCHANGED)
        {
            COptionUI* page = (COptionUI*)notify->pSender->GetInterface(DUI_CTR_OPTION);
            if (page && page->IsSelected())
            {
                int iPageIndex = (int)notify->pSender->GetTag();
                if (iPageIndex == m_OptPageNoArr.GetSize() - 1)
                {
                    int nPrePageNo = GetPageNoByControl((CControlUI*)m_OptPageNoArr[m_OptPageNoArr.GetSize() - 2]);
                    //前N-1个更新到最后一页
                    int nDelta = m_nMaxPage - nPrePageNo - 1;
                    UpdatePageNo(nDelta);
                    //更新more按钮是否显示
                    UpdateMoreBtnVisiable();
                }

                SelectPage(iPageIndex);
            }
        }

        return true;
    }

    bool DuiLib::CPageControlUI::OnBtnClick(void* p)
    {
        TNotifyUI* notify = (TNotifyUI*)p;
        if (notify->sType == DUI_MSGTYPE_CLICK)
        {
            if (notify->pSender == m_BtnPrevious)
            {
                int iCurSelIndex = m_ConCurSel->GetTag();
                int nFirstPageNo = GetPageNoByControl((COptionUI*)m_OptPageNoArr[0]);
                int iPageIndex = ((COptionUI*)m_OptPageNoArr[0])->GetTag();

                //第一个页码是1
                if (nFirstPageNo == 1)
                {
                    //左移选中,当前选中不是第一页
                    if (iCurSelIndex != 0) SelectPage(--iCurSelIndex);
                }
                else
                {
                    if (iCurSelIndex < GetShowPageNum() - 1)
                    {
                        //if (iCurSelIndex >= 0)
                        {
                            //前N-1个页码-1
                            UpdatePageNo(-1);
                            //不移动更新选中
                            SelectPage(iCurSelIndex);
                        }

                    }
                    //(选中最后一个)
                    else
                    {
                        //左移选中
                        SelectPage(--iCurSelIndex);
                    }
                }

                UpdateMoreBtnVisiable();
            }
            else if (notify->pSender == m_BtnNext ||
                notify->pSender == m_BtnNextMore)
            {
                int nCurSelPage = GetCurSel();
                int nCurSelIdx = m_ConCurSel->GetTag();

                //当前选中是倒数第二个之前的页
                if (nCurSelIdx < GetShowPageNum() - 1)
                {
                    //不存在下一个更多按钮或者不是倒数第二个按钮
                    if (!IsExistNextMore() || nCurSelIdx != GetShowPageNum() - 2)
                    {
                        //右移选中
                        SelectPage(++nCurSelIdx);
                    }
                    else//存在更多按钮
                    {
                        //页码+1
                        UpdatePageNo(1);
                        //通知选中
                        SelectPage(nCurSelIdx);
                    }
                }

                UpdateMoreBtnVisiable();
            }
            else if (notify->pSender == m_BtnGoto)
            {
                int nGotoPageNo = GetPageNoByControl(m_EdtPageNo);
                GotoPage(nGotoPageNo, false);
            }
        }

        return true;
    }

    int DuiLib::CPageControlUI::GetPageNoByControl(CControlUI* pcon)
    {
        if (!pcon)
        {
            return -1;
        }

        int iPage = _ttoi(pcon->GetText().GetData());
        return iPage;
    }
}