#pragma once

class CShortCutUI : public DuiLib::CButtonUI
{
public:
	CShortCutUI();
	~CShortCutUI(void);

public:
	virtual void PaintStatusImage(HDC hDC);
	virtual void SetText(LPCTSTR pstrText);

public:
	void SetIcon(HICON hIcon);

private:
	HICON m_hIcon;
};

