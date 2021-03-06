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

#ifndef __ORG_CONSCRYPT_OPENSSLRSAKEYFACTORY_H__
#define __ORG_CONSCRYPT_OPENSSLRSAKEYFACTORY_H__

#include "_Org.Conscrypt.h"
#include "elastos/core/Object.h"

using Elastos::Security::IKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IKeyFactorySpi;
using Elastos::Security::Spec::IKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLRSAKeyFactory
    : public Object
    , public IKeyFactorySpi
    , public IOpenSSLRSAKeyFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI EngineGeneratePublic(
        /* [in] */ IKeySpec* keySpec,
        /* [out] */ IPublicKey** pubKey);

    CARAPI EngineGeneratePrivate(
        /* [in] */ IKeySpec* keySpec,
        /* [out] */ IPrivateKey** priKey);

    CARAPI EngineGetKeySpec(
        /* [in] */ IKey* key,
        /* [in] */ const ClassID& keySpec,
        /* [out] */ IKeySpec** retkeySpec);

    CARAPI EngineTranslateKey(
        /* [in] */ IKey* key,
        /* [out] */ IKey** translatedKey);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAKEYFACTORY_H__