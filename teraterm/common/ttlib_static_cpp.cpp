/*
 * Copyright (C) 2020 TeraTerm Project
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

#include <windows.h>
#include <stdio.h>
#include <string.h>
#if !defined(_CRTDBG_MAP_ALLOC)
#define _CRTDBG_MAP_ALLOC
#endif
#include <stdlib.h>
#include <crtdbg.h>
#include <assert.h>

#include "i18n.h"
#include "layer_for_unicode.h"
#include "asprintf.h"

#include "ttlib.h"

/**
 *	GetI18nStrW() の動的バッファ版
 */
wchar_t *TTGetLangStrW(const char *section, const char *key, const wchar_t *def, const char *UILanguageFile)
{
	wchar_t *buf = (wchar_t *)malloc(MAX_UIMSG * sizeof(wchar_t));
	size_t size = GetI18nStrW(section, key, buf, MAX_UIMSG, def, UILanguageFile);
	buf = (wchar_t *)realloc(buf, size * sizeof(wchar_t));
	return buf;
}

/**
 *	MessageBoxを表示する
 *
 *	@param[in]	hWnd			親 window
 *	@param[in]	info			タイトル、メッセージ
 *	@param[in]	uType			MessageBoxの uType
 *	@param[in]	UILanguageFile	lngファイル
 *	@param[in]	...				フォーマット引数
 *
 *	info.message を書式化文字列として、
 *	UILanguageFileより後ろの引数を出力する
 *
 *	info.message_key, info.message_default 両方ともNULLの場合
 *		可変引数の1つ目を書式化文字列として使用する
 */
int TTMessageBoxW(HWND hWnd, const TTMessageBoxInfoW *info, UINT uType, const char *UILanguageFile, ...)
{
	const char *section = info->section;
	wchar_t *title;
	if (info->title_key == NULL) {
		title = _wcsdup(info->title_default);
	}
	else {
		title = TTGetLangStrW(section, info->title_key, info->title_default, UILanguageFile);
	}

	wchar_t *message = NULL;
	if (info->message_key == NULL && info->message_default == NULL) {
		wchar_t *format;
		va_list ap;
		va_start(ap, UILanguageFile);
		format = va_arg(ap, wchar_t *);
		vaswprintf(&message, format, ap);
	}
	else {
		wchar_t *format = TTGetLangStrW(section, info->message_key, info->message_default, UILanguageFile);
		va_list ap;
		va_start(ap, UILanguageFile);
		vaswprintf(&message, format, ap);
		free(format);
	}

	int r = _MessageBoxW(hWnd, message, title, uType);

	free(title);
	free(message);

	return r;
}

/**
 *	クリップボードからwchar_t文字列を取得する
 *	文字列長が必要なときはwcslen()すること
 *	@param	hWnd
 *	@param	emtpy	TRUEのときクリップボードを空にする
 *	@retval	文字列へのポインタ 使用後free()すること
 *			文字がない(またはエラー時)はNULL
 */
wchar_t *GetClipboardTextW(HWND hWnd, BOOL empty)
{
	UINT Cf;
	wchar_t *str_w = NULL;
	size_t str_w_len;
	HGLOBAL TmpHandle;

	if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		Cf = CF_UNICODETEXT;
	}
	else if (IsClipboardFormatAvailable(CF_TEXT)) {
		Cf = CF_TEXT;
	}
	else if (IsClipboardFormatAvailable(CF_OEMTEXT)) {
		Cf = CF_OEMTEXT;
	}
	else {
		return NULL;
	}

 	if (!OpenClipboard(hWnd)) {
		return NULL;
	}
	TmpHandle = GetClipboardData(Cf);
	if (TmpHandle == NULL) {
		return NULL;
	}
	if (Cf == CF_UNICODETEXT) {
		const wchar_t *str_cb = (wchar_t *)GlobalLock(TmpHandle);
		if (str_cb != NULL) {
			size_t str_cb_len = GlobalSize(TmpHandle);	// bytes
			str_w_len = str_cb_len / sizeof(wchar_t);
			str_w = (wchar_t *)malloc((str_w_len + 1) * sizeof(wchar_t));	// +1 for terminator
			if (str_w != NULL) {
				memcpy(str_w, str_cb, str_cb_len);
				str_w[str_w_len] = L'\0';
			}
		}
	}
	else {
		const char *str_cb = (char *)GlobalLock(TmpHandle);
		if (str_cb != NULL) {
			size_t str_cb_len = GlobalSize(TmpHandle);
			str_w_len = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, str_cb, (int)str_cb_len, NULL, 0);
			str_w = (wchar_t *)malloc(sizeof(wchar_t) * (str_w_len + 1));	// +1 for terminator
			if (str_w != NULL) {
				str_w_len = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, str_cb, (int)str_cb_len, str_w, (int)str_w_len);
				str_w[str_w_len] = L'\0';
			}
		}
	}
	GlobalUnlock(TmpHandle);
	if (empty) {
		EmptyClipboard();
	}
	CloseClipboard();
	return str_w;
}

/**
 *	クリップボードからANSI文字列を取得する
 *	文字列長が必要なときはstrlen()すること
 *	@param	hWnd
 *	@param	emtpy	TRUEのときクリップボードを空にする
 *	@retval	文字列へのポインタ 使用後free()すること
 *			文字がない(またはエラー時)はNULL
 */
char *GetClipboardTextA(HWND hWnd, BOOL empty)
{
	HGLOBAL hGlobal;
	const char *lpStr;
	size_t length;
	char *pool;

    OpenClipboard(hWnd);
    hGlobal = (HGLOBAL)GetClipboardData(CF_TEXT);
    if (hGlobal == NULL) {
        CloseClipboard();
		return NULL;
    }
    lpStr = (const char *)GlobalLock(hGlobal);
	length = GlobalSize(hGlobal);
	if (length == 0) {
		pool = NULL;
	} else {
		pool = (char *)malloc(length + 1);	// +1 for terminator
		memcpy(pool, lpStr, length);
		pool[length] = '\0';
	}
	GlobalUnlock(hGlobal);
	if (empty) {
		EmptyClipboard();
	}
	CloseClipboard();

	return pool;
}

/**
 *	クリップボードにテキストをセットする
 *	str_w	クリップボードにセットする文字列へのポインタ
 *			NULLのときクリップボードを空にする(str_lenは参照されない)
 *	str_len	文字列長
 *			0のとき文字列長が自動で算出される
 */
BOOL CBSetTextW(HWND hWnd, const wchar_t *str_w, size_t str_len)
{
	HGLOBAL CBCopyWideHandle;

	if (str_w == NULL) {
		str_len = 0;
	} else {
		if (str_len == 0)
			str_len = wcslen(str_w);
	}

	if (!OpenClipboard(hWnd)) {
		return FALSE;
	}

	EmptyClipboard();
	if (str_len == 0) {
		CloseClipboard();
		return TRUE;
	}

	{
		// 文字列をコピー、最後のL'\0'も含める
		wchar_t *CBCopyWidePtr;
		const size_t alloc_bytes = (str_len + 1) * sizeof(wchar_t);
		CBCopyWideHandle = GlobalAlloc(GMEM_MOVEABLE, alloc_bytes);
		if (CBCopyWideHandle == NULL) {
			CloseClipboard();
			return FALSE;
		}
		CBCopyWidePtr = (wchar_t *)GlobalLock(CBCopyWideHandle);
		if (CBCopyWidePtr == NULL) {
			CloseClipboard();
			return FALSE;
		}
		memcpy(CBCopyWidePtr, str_w, alloc_bytes - sizeof(wchar_t));
		CBCopyWidePtr[str_len] = L'\0';
		GlobalUnlock(CBCopyWideHandle);
	}

	SetClipboardData(CF_UNICODETEXT, CBCopyWideHandle);

	// TODO 9x系では自動でCF_TEXTにセットされないらしい?
	// ttl_gui.c の TTLVar2Clipb() ではつぎの2つが行われていた
	//		SetClipboardData(CF_TEXT, hText);
	//		SetClipboardData(CF_UNICODETEXT, wide_hText);
	CloseClipboard();

	return TRUE;
}

// from ttxssh
static void format_line_hexdump(char *buf, int buflen, int addr, int *bytes, int byte_cnt)
{
	int i, c;
	char tmp[128];

	buf[0] = 0;

	/* 先頭のアドレス表示 */
	_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "%08X : ", addr);
	strncat_s(buf, buflen, tmp, _TRUNCATE);

	/* バイナリ表示（4バイトごとに空白を挿入）*/
	for (i = 0; i < byte_cnt; i++) {
		if (i > 0 && i % 4 == 0) {
			strncat_s(buf, buflen, " ", _TRUNCATE);
		}

		_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "%02X", bytes[i]);
		strncat_s(buf, buflen, tmp, _TRUNCATE);
	}

	/* ASCII表示部分までの空白を補う */
	_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "   %*s%*s", (16 - byte_cnt) * 2 + 1, " ", (16 - byte_cnt + 3) / 4, " ");
	strncat_s(buf, buflen, tmp, _TRUNCATE);

	/* ASCII表示 */
	for (i = 0; i < byte_cnt; i++) {
		c = bytes[i];
		if (isprint(c)) {
			_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "%c", c);
			strncat_s(buf, buflen, tmp, _TRUNCATE);
		}
		else {
			strncat_s(buf, buflen, ".", _TRUNCATE);
		}
	}
}

void DebugHexDump(void (*f)(const char *s), const void *data_, size_t len)
{
	const char *data = (char *)data_;
	char buff[4096];
	int c, addr;
	int bytes[16], *ptr;
	int byte_cnt;

	addr = 0;
	byte_cnt = 0;
	ptr = bytes;
	for (size_t i = 0; i < len; i++) {
		c = data[i];
		*ptr++ = c & 0xff;
		byte_cnt++;

		if (byte_cnt == 16) {
			format_line_hexdump(buff, sizeof(buff), addr, bytes, byte_cnt);
			f(buff);

			addr += 16;
			byte_cnt = 0;
			ptr = bytes;
		}
	}

	if (byte_cnt > 0) {
		format_line_hexdump(buff, sizeof(buff), addr, bytes, byte_cnt);
		f(buff);
	}
}

static void OutputDebugHexDumpSub(const char *s)
{
	OutputDebugPrintf("%s\n", s);
}

void OutputDebugHexDump(const void *data, size_t len)
{
	DebugHexDump(OutputDebugHexDumpSub, data, len);
}

/**
 *	メニューを追加する
 *	InsertMenuA() とほぼ同じ動作
 *	before引数を FALSE にすると次の項目に追加できる
 *
 *	@param[in]	hMenu			メニューハンドル (InsertMenuA() の第1引数)
 *	@param[in]	targetItemID	このIDのメニューの前又は後ろにメニューを追加 (InsertMenuA() の第2引数)
 *	@param[in]	flags			メニューflag (InsertMenuA() の第3引数)
 *	@param[in]	newItemID		メニューID (InsertMenuA() の第4引数)
 *	@param[in]	text			メニュー文字列 (InsertMenuA() の第5引数)
 *	@param[in]	before			TRUE/FALSE 前に追加/後ろに追加 (TRUEのとき InsertMenuA() と同じ動作)
 */
void TTInsertMenuItemA(HMENU hMenu, UINT targetItemID, UINT flags, UINT newItemID, const char *text, BOOL before)
{
	assert((flags & MF_BYPOSITION) == 0);
	for (int i = GetMenuItemCount(hMenu) - 1; i >= 0; i--) {
		HMENU submenu = GetSubMenu(hMenu, i);

		for (int j = GetMenuItemCount(submenu) - 1; j >= 0; j--) {
			if (GetMenuItemID(submenu, j) == targetItemID) {
				const UINT position = (before == FALSE) ? j + 1 : j;
				InsertMenuA(submenu, position, MF_BYPOSITION | flags, newItemID, text);
				return;
			}
		}
	}
}
