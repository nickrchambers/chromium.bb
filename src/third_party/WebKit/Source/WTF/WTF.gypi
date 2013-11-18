{
    'variables': {
        'project_dir': ['.'],
        'wtf_privateheader_files': [
            'wtf/ASCIICType.h',
            'wtf/AVLTree.h',
            'wtf/Alignment.h',
            'wtf/AlwaysInline.h',
            'wtf/Assertions.h',
            'wtf/Atomics.h',
            'wtf/BitArray.h',
            'wtf/BitVector.h',
            'wtf/Bitmap.h',
            'wtf/BlockStack.h',
            'wtf/BloomFilter.h',
            'wtf/BumpPointerAllocator.h',
            'wtf/ByteOrder.h',
            'wtf/CheckedArithmetic.h',
            'wtf/CheckedBoolean.h',
            'wtf/Compiler.h',
            'wtf/Complex.h',
            'wtf/CryptographicallyRandomNumber.h',
            'wtf/CurrentTime.h',
            'wtf/DataLog.h',
            'wtf/DateMath.h',
            'wtf/DecimalNumber.h',
            'wtf/Decoder.h',
            'wtf/Deque.h',
            'wtf/DisallowCType.h',
            'wtf/DoublyLinkedList.h',
            'wtf/Encoder.h',
            'wtf/FastAllocBase.h',
            'wtf/FastMalloc.h',
            'wtf/FilePrintStream.h',
            'wtf/FixedArray.h',
            'wtf/Forward.h',
            'wtf/Functional.h',
            'wtf/GetPtr.h',
            'wtf/GregorianDateTime.h',
            'wtf/HashCountedSet.h',
            'wtf/HashFunctions.h',
            'wtf/HashIterators.h',
            'wtf/HashMap.h',
            'wtf/HashSet.h',
            'wtf/HashTable.h',
            'wtf/HashTraits.h',
            'wtf/HexNumber.h',
            'wtf/ListHashSet.h',
            'wtf/ListRefPtr.h',
            'wtf/Locker.h',
            'wtf/MD5.h',
            'wtf/MainThread.h',
            'wtf/MathExtras.h',
            'wtf/MemoryInstrumentation.cpp',
            'wtf/MemoryInstrumentation.h',
            'wtf/MemoryInstrumentationArrayBufferView.h',
            'wtf/MemoryInstrumentationHashCountedSet.h',
            'wtf/MemoryInstrumentationHashMap.h',
            'wtf/MemoryInstrumentationHashSet.h',
            'wtf/MemoryInstrumentationListHashSet.h',
            'wtf/MemoryInstrumentationParsedURL.h',
            'wtf/MemoryInstrumentationSequence.h',
            'wtf/MemoryInstrumentationString.h',
            'wtf/MemoryInstrumentationVector.h',
            'wtf/MemoryObjectInfo.h',
            'wtf/MessageQueue.h',
            'wtf/NonCopyingSort.h',
            'wtf/Noncopyable.h',
            'wtf/NotFound.h',
            'wtf/NullPtr.h',
            'wtf/OSAllocator.h',
            'wtf/OwnArrayPtr.h',
            'wtf/OwnPtr.h',
            'wtf/OwnPtrCommon.h',
            'wtf/PageAllocation.h',
            'wtf/PageAllocationAligned.h',
            'wtf/PageBlock.h',
            'wtf/PageReservation.h',
            'wtf/PassOwnArrayPtr.h',
            'wtf/PassOwnPtr.h',
            'wtf/PassRefPtr.h',
            'wtf/PassTraits.h',
            'wtf/Platform.h',
            'wtf/PossiblyNull.h',
            'wtf/PrintStream.h',
            'wtf/RandomNumber.h',
            'wtf/RawPointer.h',
            'wtf/RefCounted.h',
            'wtf/RefCountedLeakCounter.h',
            'wtf/RefPtr.h',
            'wtf/RefPtrHashMap.h',
            'wtf/RetainPtr.h',
            'wtf/SaturatedArithmetic.h',
            'wtf/SentinelLinkedList.h',
            'wtf/SinglyLinkedList.h',
            'wtf/StackBounds.h',
            'wtf/StaticConstructors.h',
            'wtf/StdLibExtras.h',
            'wtf/StringExtras.h',
            'wtf/StringHasher.h',
            'wtf/StringPrintStream.h',
            'wtf/TemporaryChange.h',
            'wtf/ThreadRestrictionVerifier.h',
            'wtf/ThreadSafeRefCounted.h',
            'wtf/ThreadSpecific.h',
            'wtf/Threading.h',
            'wtf/ThreadingPrimitives.h',
            'wtf/TypeTraits.h',
            'wtf/UnusedParam.h',
            'wtf/VMTags.h',
            'wtf/ValueCheck.h',
            'wtf/Vector.h',
            'wtf/VectorTraits.h',
            'wtf/WTFThreadData.h',
            'wtf/WeakPtr.h',
            'wtf/dtoa.h',
            'wtf/dtoa/bignum-dtoa.h',
            'wtf/dtoa/bignum.h',
            'wtf/dtoa/cached-powers.h',
            'wtf/dtoa/diy-fp.h',
            'wtf/dtoa/double-conversion.h',
            'wtf/dtoa/double.h',
            'wtf/dtoa/fast-dtoa.h',
            'wtf/dtoa/fixed-dtoa.h',
            'wtf/dtoa/strtod.h',
            'wtf/dtoa/utils.h',
            'wtf/text/ASCIIFastPath.h',
            'wtf/text/AtomicString.h',
            'wtf/text/AtomicStringHash.h',
            'wtf/text/AtomicStringImpl.h',
            'wtf/text/Base64.h',
            'wtf/text/CString.h',
            'wtf/text/IntegerToStringConversion.h',
            'wtf/text/StringBuffer.h',
            'wtf/text/StringBuilder.h',
            'wtf/text/StringConcatenate.h',
            'wtf/text/StringHash.h',
            'wtf/text/StringImpl.h',
            'wtf/text/StringOperators.h',
            'wtf/text/TextPosition.h',
            'wtf/text/WTFString.h',
            'wtf/threads/BinarySemaphore.h',
            'wtf/unicode/CharacterNames.h',
            'wtf/unicode/Collator.h',
            'wtf/unicode/UTF8.h',
            'wtf/unicode/Unicode.h',
            'wtf/unicode/icu/UnicodeIcu.h',
        ],
        'wtf_files': [
            'wtf/ArrayBuffer.cpp',
            'wtf/ArrayBuffer.h',
            'wtf/ArrayBufferView.cpp',
            'wtf/ArrayBufferView.h',
            'wtf/Assertions.cpp',
            'wtf/BitVector.cpp',
            'wtf/CryptographicallyRandomNumber.cpp',
            'wtf/CurrentTime.cpp',
            'wtf/DataLog.cpp',
            'wtf/DateMath.cpp',
            'wtf/DecimalNumber.cpp',
            'wtf/DynamicAnnotations.cpp',
            'wtf/DynamicAnnotations.h',
            'wtf/FastMalloc.cpp',
            'wtf/FilePrintStream.cpp',
            'wtf/Float32Array.h',
            'wtf/Float64Array.h',
            'wtf/GregorianDateTime.cpp',
            'wtf/HashTable.cpp',
            'wtf/Int16Array.h',
            'wtf/Int32Array.h',
            'wtf/Int8Array.h',
            'wtf/IntegralTypedArrayBase.h',
            'wtf/MD5.cpp',
            'wtf/MainThread.cpp',
            'wtf/MallocZoneSupport.h',
            'wtf/MediaTime.h',
            'wtf/MediaTime.cpp',
            'wtf/MetaAllocator.cpp',
            'wtf/MetaAllocator.h',
            'wtf/NullPtr.cpp',
            'wtf/NumberOfCores.cpp',
            'wtf/NumberOfCores.h',
            'wtf/RAMSize.cpp',
            'wtf/RAMSize.h',
            'wtf/OSAllocatorPosix.cpp',
            'wtf/OSAllocatorWin.cpp',
            'wtf/OSRandomSource.cpp',
            'wtf/OSRandomSource.h',
            'wtf/PageAllocationAligned.cpp',
            'wtf/PageBlock.cpp',
            'wtf/ParallelJobs.h',
            'wtf/ParallelJobsGeneric.cpp',
            'wtf/ParallelJobsGeneric.h',
            'wtf/ParallelJobsLibdispatch.h',
            'wtf/ParallelJobsOpenMP.h',
            'wtf/PrintStream.cpp',
            'wtf/RandomNumber.cpp',
            'wtf/RandomNumberSeed.h',
            'wtf/RefCountedLeakCounter.cpp',
            'wtf/SHA1.cpp',
            'wtf/SHA1.h',
            'wtf/SegmentedVector.h',
            'wtf/SizeLimits.cpp',
            'wtf/StackBounds.cpp',
            'wtf/StringExtras.cpp',
            'wtf/StringPrintStream.cpp',
            'wtf/TCPackedCache.h',
            'wtf/TCPageMap.h',
            'wtf/TCSpinLock.h',
            'wtf/TCSystemAlloc.cpp',
            'wtf/TCSystemAlloc.h',
            'wtf/ThreadFunctionInvocation.h',
            'wtf/ThreadIdentifierDataPthreads.cpp',
            'wtf/ThreadIdentifierDataPthreads.h',
            'wtf/ThreadSpecificWin.cpp',
            'wtf/Threading.cpp',
            'wtf/ThreadingPthreads.cpp',
            'wtf/ThreadingWin.cpp',
            'wtf/TypeTraits.cpp',
            'wtf/TypedArrayBase.h',
            'wtf/Uint16Array.h',
            'wtf/Uint32Array.h',
            'wtf/Uint8Array.h',
            'wtf/WTFThreadData.cpp',
            'wtf/chromium/ChromiumThreading.h',
            'wtf/chromium/MainThreadChromium.cpp',
            'wtf/dtoa.cpp',
            'wtf/dtoa/bignum-dtoa.cc',
            'wtf/dtoa/bignum.cc',
            'wtf/dtoa/cached-powers.cc',
            'wtf/dtoa/diy-fp.cc',
            'wtf/dtoa/double-conversion.cc',
            'wtf/dtoa/fast-dtoa.cc',
            'wtf/dtoa/fixed-dtoa.cc',
            'wtf/dtoa/strtod.cc',
            'wtf/efl/MainThreadEfl.cpp',
            'wtf/gobject/GOwnPtr.cpp',
            'wtf/gobject/GOwnPtr.h',
            'wtf/gobject/GRefPtr.cpp',
            'wtf/gobject/GRefPtr.h',
            'wtf/gobject/GTypedefs.h',
            'wtf/gtk/MainThreadGtk.cpp',
            'wtf/mac/MainThreadMac.mm',
            'wtf/qt/MainThreadQt.cpp',
            'wtf/qt/StringQt.cpp',
            'wtf/text/AtomicString.cpp',
            'wtf/text/Base64.cpp',
            'wtf/text/CString.cpp',
            'wtf/text/StringBuilder.cpp',
            'wtf/text/StringImpl.cpp',
            'wtf/text/StringStatics.cpp',
            'wtf/text/WTFString.cpp',
            'wtf/threads/BinarySemaphore.cpp',
            'wtf/unicode/CollatorDefault.cpp',
            'wtf/unicode/ScriptCodesFromICU.h',
            'wtf/unicode/UTF8.cpp',
            'wtf/unicode/UnicodeMacrosFromICU.h',
            'wtf/unicode/glib/UnicodeGLib.cpp',
            'wtf/unicode/glib/UnicodeGLib.h',
            'wtf/unicode/icu/CollatorICU.cpp',
            'wtf/unicode/wchar/UnicodeWchar.cpp',
            'wtf/unicode/wchar/UnicodeWchar.h',
            'wtf/win/MainThreadWin.cpp',
            'wtf/win/OwnPtrWin.cpp',
            'wtf/wince/FastMallocWinCE.h',
            'wtf/wince/MemoryManager.cpp',
            'wtf/wince/MemoryManager.h',
            'wtf/wx/MainThreadWx.cpp',
            'wtf/wx/StringWx.cpp',
        ],
    }
}