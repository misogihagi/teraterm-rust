﻿project(teraterm)

add_library(
  common_static
  STATIC
  asprintf.cpp
  asprintf.h
  codeconv.cpp
  codeconv.h
  compat_win.cpp
  compat_win.h
  dlglib.c
  dlglib.h
  dlglib_cpp.cpp
  dlglib_tmpl.cpp
  dllutil.cpp
  dllutil.h
  fileread.cpp
  fileread.h
  getcontent.cpp
  getcontent.h
  i18n.h
  i18n_static.c
  layer_for_unicode.cpp
  layer_for_unicode.h
  layer_for_unicode_comctl32.cpp
  tipwin.cpp
  tipwin.h
  tmfc.cpp
  tmfc.h
  tmfc_frame.cpp
  tmfc_property.cpp
  ttlib.h
  ttlib_static.c
  ttlib_static_cpp.cpp
  win16api.c
  win16api.h
  )

target_include_directories(
  common_static
  PRIVATE
  .
  )
