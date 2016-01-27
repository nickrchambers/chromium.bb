/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GeolocationClient_h
#define GeolocationClient_h

#include "modules/ModulesExport.h"
#include "platform/heap/Handle.h"

namespace blink {

class Geolocation;
class GeolocationController;
class GeolocationPosition;
class LocalFrame;

class GeolocationClient : public NoBaseWillBeGarbageCollectedFinalized<GeolocationClient> {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(GeolocationClient);
public:
    virtual ~GeolocationClient() { }

    virtual void startUpdating() = 0;
    virtual void stopUpdating() = 0;
    virtual void setEnableHighAccuracy(bool) = 0;
    virtual GeolocationPosition* lastPosition() = 0;

    virtual void requestPermission(Geolocation*) = 0;
    virtual void cancelPermissionRequest(Geolocation*) = 0;

    virtual void controllerForTestAdded(GeolocationController*) { }
    virtual void controllerForTestRemoved(GeolocationController*) { }

    DEFINE_INLINE_VIRTUAL_TRACE() { }
};

MODULES_EXPORT void provideGeolocationTo(LocalFrame&, GeolocationClient*);

} // namespace blink

#endif // GeolocationClient_h