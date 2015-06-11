#pragma once
#include "LableEx.h"

class CGameItemUI : public DuiLib::COptionUI
{
public:
	CGameItemUI();
	~CGameItemUI(void);

public:
	void DoEvent(DuiLib::TEventUI& event);
	virtual void PaintStatusImage(HDC hDC);

public:
	void SetIcon(HICON hIcon);

private:
	HICON m_hIcon;
};

