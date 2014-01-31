/*
 * Copyright (C) 2013 Bloomberg Finance L.P.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "config.h"
#include "core/page/BBDragData.h"
#include "core/dom/Clipboard.h"

namespace WebCore {

BBDragData::BBDragData()
: d_clipboard(0)
{

}

void BBDragData::setClipboard(Clipboard* clipboard)
{
    d_clipboard = clipboard;
}

void BBDragData::setHtml(const String& html)
{
    if (d_clipboard) {
        d_clipboard->setAccessPolicy(ClipboardWritable);
        d_clipboard->setData("text/html", html);
        d_clipboard->setAccessPolicy(ClipboardImageWritable);
    }
}

String BBDragData::html() const
{
    if (d_clipboard) {
        return d_clipboard->getData("text/html");
    }

    return String();
}

} // namespace WebCore