#ifndef __WPPANEL_H__
#define __WPPANEL_H__
#include <stdlib.h>
#include <time.h>

namespace DuiLib
{
	class CWPPanel : public CContainerUI
	{
	public:
		CWPPanel()
		{
			// ≤ºæ÷º∆À„
			int aaLayouts[4][4] = {{4,1,1,4},{1,4,4,1},{4,4,1,1},{1,1,4,4}};
			srand(time(NULL));
			int nLayout = rand() % 4;
			for(int i = 0; i < 4; i++)
			{
				m_aLayouts[i] = aaLayouts[nLayout][i];
			}
		}
		~CWPPanel(void)
		{
		}

	public:
		void SetPos(RECT rc, bool bNeedInvalidate)
		{
			CControlUI::SetPos(rc, bNeedInvalidate);
			RECT rcItem = m_rcItem;

			rcItem.left += m_rcInset.left;
			rcItem.top += m_rcInset.top;
			rcItem.right -= m_rcInset.right;
			rcItem.bottom -= m_rcInset.bottom;

			int nWidth = rcItem.right - rcItem.left;
			int nHeight = rcItem.bottom - rcItem.top;
			int nBlockWidth = nWidth / 2;
			int nBlockHeight = nHeight / 2;
			int nSBlockWidth = nBlockWidth / 2;
			int nSBlockHeight = nBlockHeight / 2;
			
			int it = 0;
			for (int it1 = 0; it1 < 4; ++it1) {
				int nX = it1 % 2 * nBlockWidth;
				int nY = it1 / 2 * nBlockHeight;
				RECT rcBlock = {rcItem.left + nX, rcItem.top + nY, rcItem.left + nX + nBlockWidth, rcItem.top + nY + nBlockHeight};

				int nBlocks = m_aLayouts[it1];
				if(nBlocks == 1) {
					CControlUI* pBlock = (CControlUI*)m_items.GetAt(it);
					if(pBlock != NULL) {
						pBlock->SetPos(rcBlock);
					}
				}
				else if(nBlocks == 4) {
					for(int it2 = 0; it2 < nBlocks; ++it2) {
						int nSX = it2 % 2 * nSBlockWidth;
						int nSY = it2 / 2 * nSBlockHeight;
						RECT rcSBlock = {rcBlock.left + nSX, rcBlock.top + nSY, rcBlock.left + nSX + nSBlockWidth, rcBlock.top + nSY + nSBlockHeight};
						CControlUI* pBlock = (CControlUI*)m_items.GetAt(it + it2);
						if(pBlock != NULL) {
							pBlock->SetPos(rcSBlock);
						}
					}
				}
				it += nBlocks;
			}
		}
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
		{
			CContainerUI::SetAttribute(pstrName, pstrValue);
		}

	private:
		int m_aLayouts[4];
	};
}

#endif __WPPANEL_H__
