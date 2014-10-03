// Copyright (C) 2010-2014 Joshua Boyce.
// See the file COPYING for copying permission.

#pragma once

#include "static_assert.hpp"
#include "std_string.hpp"

namespace divinity
{

struct TriString
{
  void* vtable_;
  char* fixed_narrow_;
  StdStringA std_narrow_;
  StdStringW std_wide_;
};

HADESMEM_DETAIL_STATIC_ASSERT_X86(sizeof(TriString) == 0x40);

struct TriStringPairPoly
{
  void* vtable_;
  TriString name_1_;
  TriString name_2_;
};

HADESMEM_DETAIL_STATIC_ASSERT_X86(sizeof(TriStringPairPoly) == 0x84);
}