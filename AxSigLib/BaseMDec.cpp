/*! \file
    \brief AxSigLib - Short Elliptic Curve Digital Signature Algorithm et. al.

    @(#) $Id$

    Decode BaseM strings. These are strings any character set specified by defining
    encoding/decoding vectors like these (for Base34):

    static const byte s_vecUpper[] = "ABCDEFGHIJKLMNPQRSTUVWXYZ123456789";
    static const byte s_vecLower[] = "abcdefghijklmnpqrstuvwxyz123456789";

    The definition of these control the encoding along with the AnyBase and BitPrecision
    parameters.

    Copyright (C) 2005 Svante Seleborg/Axon Data, All rights reserved.

    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation;
    either version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with this program;
    if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA 02111-1307 USA

    The author may be reached at mailto:software@axantum.com and http://www.axantum.com
----
*/

#include "stdafx.h"
#include "BaseMDec.h"
#include "integer.h"

void
BaseM_Decoder::IsolatedInitialize(const NameValuePairs &parameters) {
    parameters.GetRequiredParameter("BaseM_Decoder", Name::DecodingLookupArray(), m_lookup);

    parameters.GetRequiredIntParameter("BaseM_Decoder", Name::AnyBase(), m_base);
    parameters.GetRequiredIntParameter("BaseM_Decoder", Name::BitPrecision(), m_bits);
    if (m_base <= 0 || m_base >= 256) {
        throw InvalidArgument("BaseM_Decoder: Invalid base");
    }
}

// We'll be getting a BaseM string, with most significant bytes first. We'll have to get the whole
// string before converting to a large integer and then outputting.
// We need to recreate the original length in byte, as we don't know how it will be interpreted
// later. The string we get will be zero-padded in the high order positions.
// Assuming, which we do, that the input string is always an even number 8-bits, i.e. bytes and
// also assuming, which we enforce, that the base 'M' is less than 256, there's no ambigousness
// about how long the output after decoding is to be.
size_t
BaseM_Decoder::Put2(const byte *begin, size_t length, int messageEnd, bool blocking) {
    SecByteBlock outBuf;
    size_t lBufferSize = m_buffer.size();
    Integer v, s;

    FILTER_BEGIN;
    if (length) {
        m_buffer.Grow(lBufferSize + length);
        memcpy(m_buffer + lBufferSize, begin, length);
        lBufferSize += length;
    }

    // This I'm a bit unsure about.. Don't really like these kinds of tricky, tricky, macros.
    // Especially when there's absolutely no documentation...
    // Apparently this will generate a case statement, matching the switch generated by
    // FILTER_BEGIN, but I can't really see how this works.
    FILTER_OUTPUT(1, outBuf, 0, 0);
    if (messageEnd) {
        v = 0;
        s = 1;
        for (size_t i = 0; i < lBufferSize; i++ ) {
            unsigned int value = m_lookup[m_buffer[i]];
            if (value >= 256) {
                continue;
            }
            v *= m_base;
            v += value;

            // Keep track of the total order, even if we have leading high-order zeroes.
            s *= m_base;
        }
        if (m_bits) {
            outBuf.New((m_bits - 1) / 8 + 1);
        } else {
            // Now v is the value, but possibly too short. s is the magnitude expressed as an even
            // power of m_base;
            // We know that the binary representation of s-1 will require exactly the maximum length
            // of the original representation of v. Therefore we use this length for the output.
            // This assumes that we're using the most efficent representation.
            s -= 1;
            outBuf.New(s.MinEncodedSize());
        }

        // Encode pads high order bytes
        v.Encode(outBuf, outBuf.size());
        FILTER_OUTPUT(2, outBuf, outBuf.size(), messageEnd);
    }
    FILTER_END_NO_MESSAGE_END;
}

void BaseM_Decoder::InitializeDecodingLookupArray(int *lookup, const byte *alphabet, unsigned int base, bool caseInsensitive) {
    std::fill(lookup, lookup+256, -1);

    for (unsigned int i=0; i<base; i++) {
        if (caseInsensitive && isalpha(alphabet[i])) {
            assert(lookup[toupper(alphabet[i])] == -1);
            lookup[toupper(alphabet[i])] = i;
            assert(lookup[tolower(alphabet[i])] == -1);
            lookup[tolower(alphabet[i])] = i;
        } else {
            assert(lookup[alphabet[i]] == -1);
            lookup[alphabet[i]] = i;
        }
    }
}