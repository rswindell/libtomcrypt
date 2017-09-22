/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 */

#include "tomcrypt.h"

#ifdef LTC_MECC

int ecc_set_dp_oid(unsigned long *oid, unsigned long oidsize, ecc_key *key)
{
   int i;

   LTC_ARGCHK(oid != NULL);
   LTC_ARGCHK(oidsize > 0);

   for(i = 0; ltc_ecc_sets[i].size != 0; i++) {
      if ((oidsize == ltc_ecc_sets[i].oid.OIDlen) &&
          (XMEM_NEQ(oid, ltc_ecc_sets[i].oid.OID, sizeof(unsigned long) * ltc_ecc_sets[i].oid.OIDlen) == 0)) {
         break;
      }
   }
   if (ltc_ecc_sets[i].name == NULL) return CRYPT_ERROR; /* not found */
   return ecc_set_dp(&ltc_ecc_sets[i], key);
}

int ecc_set_dp_copy(ecc_key *srckey, ecc_key *key)
{
   unsigned long i;
   int err;

   LTC_ARGCHK(key != NULL);
   LTC_ARGCHK(srckey != NULL);

   if ((err = mp_init_multi(&key->dp.prime, &key->dp.order, &key->dp.A, &key->dp.B,
                            &key->dp.base.x, &key->dp.base.y, &key->dp.base.z,
                            &key->pubkey.x, &key->pubkey.y, &key->pubkey.z, &key->k,
                            NULL)) != CRYPT_OK) {
      return err;
   }

   /* A, B, order, prime, Gx, Gy */
   if ((err = mp_copy(srckey->dp.prime,  key->dp.prime )) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(srckey->dp.order,  key->dp.order )) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(srckey->dp.A,      key->dp.A     )) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(srckey->dp.B,      key->dp.B     )) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(srckey->dp.base.x, key->dp.base.x)) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(srckey->dp.base.y, key->dp.base.y)) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(srckey->dp.base.z, key->dp.base.z)) != CRYPT_OK) { goto error; }
   /* cofactor & size */
   key->dp.cofactor = srckey->dp.cofactor;
   key->dp.size     = srckey->dp.size;
   /* OID */
   key->dp.oid.OIDlen = srckey->dp.oid.OIDlen;
   for (i = 0; i < key->dp.oid.OIDlen; i++) key->dp.oid.OID[i] = srckey->dp.oid.OID[i];
   /* success */
   return CRYPT_OK;

error:
   ecc_free(key);
   return err;
}

int ecc_set_dp_bn(void *a, void *b, void *prime, void *order, void *gx, void *gy, unsigned long cofactor, ecc_key *key)
{
   int err;

   LTC_ARGCHK(key   != NULL);
   LTC_ARGCHK(a     != NULL);
   LTC_ARGCHK(b     != NULL);
   LTC_ARGCHK(prime != NULL);
   LTC_ARGCHK(order != NULL);
   LTC_ARGCHK(gx    != NULL);
   LTC_ARGCHK(gy    != NULL);

   /* A, B, order, prime, Gx, Gy */
   if ((err = mp_copy(prime, key->dp.prime )) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(order, key->dp.order )) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(a,     key->dp.A     )) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(b,     key->dp.B     )) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(gx,    key->dp.base.x)) != CRYPT_OK) { goto error; }
   if ((err = mp_copy(gy,    key->dp.base.y)) != CRYPT_OK) { goto error; }
   if ((err = mp_set(key->dp.base.z, 1)) != CRYPT_OK)      { goto error; }
   /* cofactor & size */
   key->dp.cofactor = cofactor;
   key->dp.size = mp_unsigned_bin_size(prime);
   /* no OID - XXX-TODO perhaps try find order/prime/a/b in ltc_ecc_sets */
   key->dp.oid.OIDlen = 0;
   /* success */
   return CRYPT_OK;

error:
   ecc_free(key);
   return err;
}

#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
