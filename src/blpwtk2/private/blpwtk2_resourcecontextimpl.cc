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

#include <blpwtk2_resourcecontextimpl.h>

#include <net/dns/host_resolver.h>
#include <net/url_request/url_request_context_getter.h>

namespace blpwtk2 {

ResourceContextImpl::ResourceContextImpl() : d_requestContextGetter(NULL)
{
}

ResourceContextImpl::~ResourceContextImpl()
{
}

void ResourceContextImpl::setRequestContextGetter(
    net::URLRequestContextGetter* getter)
{
    d_requestContextGetter = getter;
}

net::HostResolver* ResourceContextImpl::GetHostResolver()
{
    if (!d_hostResolver.get())
        d_hostResolver = net::HostResolver::CreateDefaultResolver(0);
    return d_hostResolver.get();
}

net::URLRequestContext* ResourceContextImpl::GetRequestContext()
{
    return d_requestContextGetter->GetURLRequestContext();
}

}  // close namespace blpwtk2
