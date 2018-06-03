/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

/* the source comes from https://github.com/nimrody/rs */

#include <stdlib.h>
#include <string.h>

#include "m_mem.h"
#include "m_gf.h"
#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_MODEL_GF

/* GF over 2^kMM
 * The field has 256 elements from 0 to kNN
 */
#define kMM  8
#define kNN  ((1 << kMM) - 1)

#if (kMM != 8)
#error "this function is specifically for GF(2^8)"
#endif

struct s_gf {
   gu *alpha_to;   /* index->polynomial form conversion table */
   gu *index_of;   /* Polynomial->index form conversion table */
};

gu*
gf_alpha_to_array(gf_t *gf) {
   return gf ? gf->alpha_to : (gu*)0;
}

gu*
gf_index_of_array(gf_t *gf) {
   return gf ? gf->index_of : (gu*)0;
}

/* Field generation and arithmetic operations *********************************/

int
_gen_256(gf_t *gf) {
    
   if ( !gf ) {
      return 0;
   }
    
   int i = 0;
   gu *arr = gf->alpha_to;
    
   /*
    * Generate the standard base. Note that alpha = 0x02 
    * 00000001 alpha^0
    * 00000010 alpha^1
    * 00000100 alpha^2
    * 00001000 alpha^3
    * 00010000 alpha^4
    * 00100000 alpha^5
    * 01000000 alpha^6
    * 10000000 alpha^7
    */
	
   for(i=0; i<kMM; i++) {
      arr[i] = 1 << i;
   }

   /*
    * Our generator polynomial is p(x) = 1+x^2+x^3+x^4+x^8.
    * But in GF(256) the polynomial p(x) == 0 (mod p(x)), so
    * we can write 0 = a^2+a^3+a^4+a^8
    *
    * If we substitute x=alpha (a for short) and regard the
    * coefficients as binary digits (addition equals substruction)
    * we have:
    *
    * a^8 = a^0 + a^2 + a^3 + a^4 or more generally, 
    * a^n = a^(n-8) + a^(n-6) + a^(n-5) + a^(n-4),  n=8..254
    *
    * (a^0 .. a^254 are 255 element so a^255=1)
    *
    * Where '+' operations denote binary xor, of course.
    */
	

   for(; i<kNN; i++) {
      arr[i] = arr[i-8] ^ arr[i-6] ^ arr[i-5] ^ arr[i-4];
   }


   /*
    * we regard kNN as infinity, and alpha^kNN stands for the
    * zero element which actually has no alpha^n representation
    */
   gf->alpha_to[kNN] = 0;

   /*
    * now generate the inverse table: numeric representation
    * to power of alpha (logarithm table)
    */

   gu *ain = gf->index_of;
   for(i=0; i<=kNN; i++) {
      gu s = arr[i];
      ain[s] = i;
   }
    
   return 1;
}


gu
gf_add(gf_t *gf, gu a, gu b) {
   return a ^ b;
}

gu
gf_mul(gf_t *gf, gu a, gu b) {
   if (a == 0 || b == 0)
      return 0;
   else {
      int na = gf->index_of[a];
      int nb = gf->index_of[b];
      int nc = na + nb;

      /* note that this is % kNN, since a^kNN = 1 (this is
       * unfortunate, as mod (kNN+1) can be accomplished by
       * a simple bitwise and with 0xFF */
      return gf->alpha_to[nc % kNN];
   }

}


gu
gf_div(gf_t *gf, gu a, gu b) {
   gu c = gf_inv(gf, b);
   gu result = gf_mul(gf, a, c);

   return result;
}

gu
gf_inv(gf_t *gf, gu a) {
   return gf_pow(gf, a, -1);
}

gu
gf_pow(gf_t *gf, gu a, int n) {
   int ia;

   if (a==0) {
      return 0;
   }

   if (n>=0 || (n<0 && a!=0)) {
      return 0;
   }

   while(n<0)
      n += kNN;

   ia = gf->index_of[a];
   ia = (ia * n) % kNN;

   return gf->alpha_to[ia];
}


/*
 * Compute p(x)
 *
 * p0 + p1*x + p2*x^2 + .. + p_n-1*x^n-1 =
 * (((p_n-1)*x + p_n-2)*x + .. )*x + p0;
 */

gu
gf_poly(gf_t *gf, gu p[], int n, gu a) {
   int i = n-1;
   gu acc = 0;

   while(i>=0)  {
      acc = gf_mul(gf, acc, a);
      acc = gf_add(gf, acc, p[i]);
      i--;
   }

   return acc;	
}

gf_t*
gf_init(void) {
   gf_t *gf = (gf_t*)mm_malloc(sizeof(struct s_gf));
   if (gf) {
      gf->alpha_to = (gu*)mm_malloc((kNN + 1) * sizeof(gu));
      gf->index_of = (gu*)mm_malloc((kNN + 1) * sizeof(gu));
      _gen_256(gf);
   }
   return gf;
}

void
gf_fini(gf_t *gf) {
   if (gf) {
      mm_free(gf->alpha_to);
      mm_free(gf->index_of);
      mm_free(gf);
   }
}

#endif // M_FOUNDATION_IMPORT_MODEL_GF
