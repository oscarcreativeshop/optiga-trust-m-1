/* rsa.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */



#ifndef WOLF_CRYPT_RSA_H
#define WOLF_CRYPT_RSA_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_RSA

/* allow for user to plug in own crypto */
#if !defined(HAVE_FIPS) && (defined(HAVE_USER_RSA) || defined(HAVE_FAST_RSA))
    #include "user_rsa.h"
#else

#ifdef HAVE_FIPS
/* for fips @wc_fips */
#include <cyassl/ctaocrypt/rsa.h>
#if defined(CYASSL_KEY_GEN) && !defined(WOLFSSL_KEY_GEN)
    #define WOLFSSL_KEY_GEN
#endif
#else
    #include <wolfssl/wolfcrypt/integer.h>
    #include <wolfssl/wolfcrypt/random.h>
#endif /* HAVE_FIPS */

/* header file needed for OAEP padding */
#include <wolfssl/wolfcrypt/hash.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* avoid redefinition of structs */
#if !defined(HAVE_FIPS)

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

enum {
    RSA_PUBLIC   = 0,
    RSA_PRIVATE  = 1,

    RSA_TYPE_UNKNOWN    = -1,
    RSA_PUBLIC_ENCRYPT  = 0,
    RSA_PUBLIC_DECRYPT  = 1,
    RSA_PRIVATE_ENCRYPT = 2,
    RSA_PRIVATE_DECRYPT = 3,

    RSA_BLOCK_TYPE_1 = 1,
    RSA_BLOCK_TYPE_2 = 2,

    RSA_MIN_SIZE = 512,
    RSA_MAX_SIZE = 4096,

    RSA_MIN_PAD_SZ   = 11      /* separator + 0 + pad value + 8 pads */
};


/* RSA */
typedef struct RsaKey {
    mp_int n, e, d, p, q, dP, dQ, u;
    int   type;                               /* public or private */
    void* heap;                               /* for user memory overrides */
    int   state;
    byte*  tmp;                               /* temp buffer for async RSA */
    word32 tmpLen;
    byte   tmpIsAlloc;
#ifdef WC_RSA_BLINDING
    WC_RNG* rng;                              /* for PrivateDecrypt blinding */
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    AsyncCryptDev asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
} RsaKey;
#endif /*HAVE_FIPS */

WOLFSSL_API int  wc_InitRsaKey(RsaKey* key, void* heap);
WOLFSSL_API int  wc_InitRsaKey_ex(RsaKey* key, void* heap, int devId);
WOLFSSL_API int  wc_FreeRsaKey(RsaKey* key);

WOLFSSL_LOCAL int wc_RsaFunction(const byte* in, word32 inLen, byte* out,
                           word32* outLen, int type, RsaKey* key, WC_RNG* rng);

WOLFSSL_API int  wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                                 word32 outLen, RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out,
                                        RsaKey* key);
WOLFSSL_API int  wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                  word32 outLen, RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out,
                                    RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key);
WOLFSSL_API int  wc_RsaEncryptSize(RsaKey* key);

#ifndef HAVE_FIPS /* to avoid asn duplicate symbols @wc_fips */
WOLFSSL_API int  wc_RsaPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                                               RsaKey*, word32);
WOLFSSL_API int  wc_RsaPublicKeyDecode(const byte* input, word32* inOutIdx,
                                                               RsaKey*, word32);
WOLFSSL_API int  wc_RsaPublicKeyDecodeRaw(const byte* n, word32 nSz,
                                        const byte* e, word32 eSz, RsaKey* key);
#ifdef WOLFSSL_KEY_GEN
    WOLFSSL_API int wc_RsaKeyToDer(RsaKey*, byte* output, word32 inLen);
#endif

WOLFSSL_API int wc_RsaSetRNG(RsaKey* key, WC_RNG* rng);

/*
   choice of padding added after fips, so not available when using fips RSA
 */

/* Mask Generation Function Identifiers */
#define WC_MGF1NONE   0
#define WC_MGF1SHA1   26
#define WC_MGF1SHA224 4
#define WC_MGF1SHA256 1
#define WC_MGF1SHA384 2
#define WC_MGF1SHA512 3

/* Padding types */
#define WC_RSA_PKCSV15_PAD 0
#define WC_RSA_OAEP_PAD    1

WOLFSSL_API int  wc_RsaPublicEncrypt_ex(const byte* in, word32 inLen, byte* out,
                   word32 outLen, RsaKey* key, WC_RNG* rng, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 lableSz);
WOLFSSL_API int  wc_RsaPrivateDecrypt_ex(const byte* in, word32 inLen,
                   byte* out, word32 outLen, RsaKey* key, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 lableSz);
WOLFSSL_API int  wc_RsaPrivateDecryptInline_ex(byte* in, word32 inLen,
                      byte** out, RsaKey* key, int type, enum wc_HashType hash,
                      int mgf, byte* label, word32 lableSz);
#endif /* HAVE_FIPS*/
WOLFSSL_API int  wc_RsaFlattenPublicKey(RsaKey*, byte*, word32*, byte*,
                                                                       word32*);

#ifdef WOLFSSL_KEY_GEN
    WOLFSSL_API int wc_RsaKeyToPublicDer(RsaKey*, byte* output, word32 inLen);
    WOLFSSL_API int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng);
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    WOLFSSL_API int  wc_RsaAsyncHandle(RsaKey* key, WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event);
    WOLFSSL_API int  wc_RsaAsyncWait(int ret, RsaKey* key);
#endif

#endif /* HAVE_USER_RSA */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_RSA */
#endif /* WOLF_CRYPT_RSA_H */
