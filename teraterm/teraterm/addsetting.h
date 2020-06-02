/*
 * (C) 2008-2019 TeraTerm Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "tmfc.h"
#include "tt_res.h"
#include "teraterm.h"
#include "tipwin.h"

typedef struct {
	const char *name;
	LPCTSTR id;
} mouse_cursor_t;

extern const mouse_cursor_t MouseCursor[];

// General Page
class CGeneralPropPageDlg : public TTCPropertyPage
{
public:
	CGeneralPropPageDlg(HINSTANCE inst);
	virtual ~CGeneralPropPageDlg();
private:
	void OnInitDialog();
	void OnOK();
	enum { IDD = IDD_TABSHEET_GENERAL };
	void OnHelp();
};

// Control Sequence Page
class CSequencePropPageDlg : public TTCPropertyPage
{
public:
	CSequencePropPageDlg(HINSTANCE inst);
	virtual ~CSequencePropPageDlg();
private:
	void OnInitDialog();
	void OnOK();
	enum { IDD = IDD_TABSHEET_SEQUENCE };
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnHelp();
};

// Copypaste Page
class CCopypastePropPageDlg : public TTCPropertyPage
{
public:
	CCopypastePropPageDlg(HINSTANCE inst);
	virtual ~CCopypastePropPageDlg();
private:
	void OnInitDialog();
	void OnOK();
	enum { IDD = IDD_TABSHEET_COPYPASTE };
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnHelp();
};

// Visual Page
class CVisualPropPageDlg : public TTCPropertyPage
{
public:
	CVisualPropPageDlg(HINSTANCE inst);
	virtual ~CVisualPropPageDlg();
private:
	void OnInitDialog();
	void OnOK();
	HBRUSH OnCtlColor(HDC hDC, HWND hWnd);
	enum { IDD = IDD_TABSHEET_VISUAL };
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnHScroll(UINT nSBCode, UINT nPos, HWND pScrollBar);
	void SetupRGBbox(int index);
	void OnHelp();
	CTipWin* TipWin;
};

// Log Page
class CLogPropPageDlg : public TTCPropertyPage
{
public:
	CLogPropPageDlg(HINSTANCE inst);
	virtual ~CLogPropPageDlg();
private:
	void OnInitDialog();
	void OnOK();
	enum { IDD = IDD_TABSHEET_LOG };
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnHelp();
};

// Cygwin Page
class CCygwinPropPageDlg : public TTCPropertyPage
{
public:
	CCygwinPropPageDlg(HINSTANCE inst);
	virtual ~CCygwinPropPageDlg();
private:
	void OnInitDialog();
	void OnOK();
	enum { IDD = IDD_TABSHEET_CYGWIN };
	cygterm_t settings;
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnHelp();
};

// Property Sheet
class CAddSettingPropSheetDlg
{
public:
	CAddSettingPropSheetDlg(HINSTANCE hInstance, HWND hParentWnd);
	virtual ~CAddSettingPropSheetDlg();
	INT_PTR DoModal();

private:
	static int CALLBACK PropSheetProc(HWND hWnd, UINT msg, LPARAM lParam);
	static HINSTANCE ghInstance;
	static class CAddSettingPropSheetDlg *gTTCPS;

	PROPSHEETHEADERW m_psh;
	HWND m_hWnd;
	HWND m_hParentWnd;
	HINSTANCE m_hInst;

	int m_PageCount;
	HPROPSHEETPAGE hPsp[9];

	int m_PageCountCPP;
	TTCPropertyPage *m_Page[7];
};