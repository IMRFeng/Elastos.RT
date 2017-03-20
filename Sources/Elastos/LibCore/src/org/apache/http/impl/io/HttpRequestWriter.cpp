//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "org/apache/http/impl/io/HttpRequestWriter.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Utility::CCharArrayBuffer;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

HttpRequestWriter::HttpRequestWriter(
    /* [in] */ ISessionOutputBuffer* buffer,
    /* [in] */ ILineFormatter* formatter,
    /* [in] */ IHttpParams* params)
    : AbstractMessageWriter(buffer, formatter, params)
{}

ECode HttpRequestWriter::WriteHeadLine(
    /* [in] */ IHttpMessage* message)
{
    AutoPtr<IRequestLine> requestLine;
    IHttpRequest::Probe(message)->GetRequestLine((IRequestLine**)&requestLine);
    AutoPtr<ICharArrayBuffer> buffer;
    mLineFormatter->FormatRequestLine(mLineBuffer, requestLine, (ICharArrayBuffer**)&buffer);
    return mSessionBuffer->WriteLine(buffer);
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org