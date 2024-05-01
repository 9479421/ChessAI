#include "pch.h"
#include "QEdit.h"

void QEdit::PreSubclassWindow()
{
	DWORD dwStyle = GetStyle();
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_OWNERDRAW | SS_NOTIFY);


	CStatic::PreSubclassWindow();
}
