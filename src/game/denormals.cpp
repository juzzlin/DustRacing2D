// This file is part of Dust Racing 2D.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.
#include "denormals.hpp"

#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
#include <pmmintrin.h>
#include <xmmintrin.h>
#define DENORMALS_SSE
#elif defined(__aarch64__)
#include <cstdint>
#define DENORMALS_AARCH64
#endif

namespace Denormals {

void disable()
{
#if defined(DENORMALS_SSE)
    // Flush denormal results to zero and treat denormal inputs as zero
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#elif defined(DENORMALS_AARCH64)
    // Set the flush-to-zero bit (FZ) of the floating point control register
    uint64_t fpcr = 0;
    asm volatile("mrs %0, fpcr" : "=r"(fpcr));
    fpcr |= uint64_t { 1 } << 24;
    asm volatile("msr fpcr, %0" : : "r"(fpcr));
#endif
}

} // namespace Denormals
