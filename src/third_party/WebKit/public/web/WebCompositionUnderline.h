/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebCompositionUnderline_h
#define WebCompositionUnderline_h

#include "../platform/WebColor.h"

namespace blink {

// Class WebCompositionUnderline is intended to be used with WebWidget's
// setComposition() method.
struct WebCompositionUnderline {
    WebCompositionUnderline()
        : startOffset(0)
        , endOffset(0)
        , thick(false)
        , backgroundColor(0) { }

    WebCompositionUnderline(unsigned s, unsigned e, bool t, WebColor bc)
        : startOffset(s)
        , endOffset(e)
        , thick(t)
        , backgroundColor(bc) { }

    bool operator<(const WebCompositionUnderline& other) const
    {
        return startOffset != other.startOffset ? startOffset < other.startOffset : endOffset < other.endOffset;
    }

    // Need to update IPC_STRUCT_TRAITS_BEGIN(blink::WebCompositionUnderline)
    // if members change.
    unsigned startOffset;
    unsigned endOffset;
    bool thick;
    WebColor backgroundColor;
};

} // namespace blink

#endif