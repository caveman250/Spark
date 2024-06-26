/* Bcj2.h -- Converter for x86 code (BCJ2)
2008-10-04 : Igor Pavlov : Public domain */

#ifndef __BCJ2_H
#define __BCJ2_H

#include "LzmaTypes.h"

namespace crnlib {

/*
Conditions:
  outSize <= FullOutputSize,
  where FullOutputSize is full size of output stream of x86_2 filter.

If buf0 overlaps outBuf, there are two required conditions:
  1) (buf0 >= outBuf)
  2) (buf0 + size0 >= outBuf + FullOutputSize).

Returns:
  SZ_OK
  SZ_ERROR_DATA - Data error
*/

int Bcj2_Decode(
    const Byte* buf0, SizeT size0,
    const Byte* buf1, SizeT size1,
    const Byte* buf2, SizeT size2,
    const Byte* buf3, SizeT size3,
    Byte* outBuf, SizeT outSize);
}

#endif
