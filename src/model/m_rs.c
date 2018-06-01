/* 
 * Copyright (c) 2018 lalawue
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

/* the source comes from https://github.com/nimrody/rs */

#include <stdlib.h>
#include <string.h>

#include "m_gf.h"
#include "m_rs.h"

#include "m_mem.h"
#include "mfoundation_import.h"

#if M_FOUNDATION_IMPORT_MODEL_RS

// rs constrol structure
struct s_rs {
   gf_t *gf;
   gu t;               // parity byte shift
   gu k;               // sysmbols to be encoded
   gu n;               // k + 2*t;
   gu *g_arr;
   unsigned char uncorrectable;
};

/*
 * Generate the coefficients of a Reed-Solomon generator
 * polynomial of degree n (n = 2t).
 *
 * In our case the roots are a^0 to a^n:
 *   p_2T(x) = (x+1)(x+a^1)(x+a^2)....(x+a^n)
 *
 * So we use the recursion:
 *   p_0(x)  = (x+1)
 *   p_1(x)  = (x+1)(x+a^1) = p_1(x)*x + p(x)*a^1
 *                           `--shift--'
 *   or ..
 *   p_k(x)  = p_(k-1)(x)*x + p_(k-1)(x)*a^k
 *
 *   and k goes from 0 to n-1 (k+1 is the degree of the
 *   resultant polynomial)
 */
void
_generate_rs(rs_t *rs) {
   int i, k;
   int n = 2 * rs->t;
   gf_t *gf = rs->gf;
   gu *poly_arr = mm_malloc(n * (n+1));
    
#define _poly(x, y) poly_arr[(x)*(n+1) + (y)]

   _poly(0, 0) = 1;     /* the x^0 coefficient of x+1 is 1*/
   _poly(0, 1) = 1;     /* the x^1 coefficient of x+1 is 1*/
   gu *alpha_to = gf_alpha_to_array(gf);

   for(k=1; k<n; k++) {
      /*
       * first we generate the p_(k-1)(x)*x term by
       * shifting the previous polynomial. we loop
       * over all terms although many of them are 0
       * just to make sure we generate the 0's for the
       * new polynomial as well.
       */
      for(i=n; i>0; i--)  {
         gu c = _poly(k-1, i-1);
         _poly(k, i) = c;
      }
      _poly(k, 0) = 0;

      /* 
       * now add the p_(k-1)(x)*a^k term coefficient by
       * coefficient
       */
      for(i=0; i<=k; i++) {
         gu c  = _poly(k-1, i);
         c  = gf_mul(gf, c, alpha_to[k]);  /* c = c*a^k */
         _poly(k, i) = gf_add(gf, _poly(k, i), c);
      }

   }

   for(i=0; i<n; i++) {
      rs->g_arr[i] = _poly(n-1, i);
   }
    
   mm_free(poly_arr);
#undef _poly
}

/*
 * Encoding:
 *
 * m(x) is the data polynomial:
 *   m(x) = m[0] + m[1]x + ... + m[k-1]x^(k-1)
 *
 * where k sysmbols are to be encoded.
 *
 * The degree of the generator polynomial is b = n-k
 *   g(x) = g[0] + g[1]x + ... + g[n-k]x^(n-k)
 *
 * We could encode simply by multiplying m(x) by g(x):
 *   c(x) = m(x)g(x) = c[0] + c[1]x .. + c[n-1]x^(n-1)
 *
 * But this would result a non-systematic encoder (i.e., the output
 * codeword would not contain the input data symbols explicitly).
 *
 * Instead we represent x^b*m(x) as a divisor and reminder of 
 * a division by g(x) - the generator polynomial (this is always
 * possible, of course):
 *
 *   x^b * m(x) = f(x)g(x) + r(x)
 *
 * deg r(x) < deg g(x) = b, of course, so we have:
 *
 *   x^b * m(x) - r(x) = f(x)g(x)
 *
 * So we conclude that x^b*m(x)-r(x) is a codeword since it is
 * a multiple of g(x) and is of the right degree:
 *
 *   deg x^b * m(x) = n-1
 *   deg r(x) < b = n-k <= n-1, so we have
 *   deg x^b*m(x) - r(x) = n-1
 *
 *  Looking at the resulting codeword:
 *
 *  c(x) = x^b * m(x) - x^b*m(x) % g(x) 
 *  x^b*m(x) = m[0]x^b + m[1]x^(b+1) + ... + m[k-1]x^(n-1)
 * -x^b*m(x) % g(x) = r[0] + r[1]x + ... + r[b-1]x^(b-1)
 *  
 *  So if we start by transmitting m[k-1] m[k-2] .. m[0]
 *  we can calculate r(x) and then transmit r[b-1] .. r[0]
 *
 * Shortening:
 *   If we want to shorten the code (transmit less data with the
 *   same number of check bytes, b) we assume m[k-1] .. m[k-u] = 0
 *   and simply skip their transmission.A
 *
 * How do we calculate the reminder in the division x^b * m(x) / g(x)?
 * 
 *
 *     ,---------+----------+--------------+--------<--.
 *     |         |          |              |           |
 *     V         V          V              V           |
 *   .----.    .----.      ...          .-------.   .------.
 *   |x g0|    |x g1|                   |xg[b-1]|   | /g[b]|
 *   `----'    `----'                   `-------'   `------'
 *     |-        |-                        |-          ^
 *     |  .--.   V   .--.                  V   .----.  |
 *     `--|D0|--[+]->|D1|              -->[+]->|Db-1|-[+]--> reminder
 *        `--'       `--'                      `----'  ^     may be extracted
 *                                                     |     when m[0] has
 *           0 .. 0  m[0] ....  m[k] m[k-1]    --------'     been processed
 *
 *   Initially, the registers D0..Db are set to 0.
 *
 *   Notes:
 *     1. In our case g[b]=1 so 1/g[b] is simply a short circuit
 *     2. in GF(2^m) addition and substruction are the
 *        same, so all the '-' signs after the multiplications
 *        by g[0]..g[b-1] are not necessary.
 */
int
rs_encode(rs_t *rs, unsigned char *data, unsigned char *parity) {
   if (!rs || !data || !parity) {
      return 0;
   }
    
   gu *m = (gu*)data;
   gu *c = (gu*)parity;
    
   int n = rs->n;
   int b = rs->n - rs->k;
   gu *g = rs->g_arr;
   gf_t *gf = rs->gf;
    
   int i, j, p;
   gu reg[b];

   /* initialize register to 0 */
   memset(reg, 0, sizeof(gu) * b);

   /*
    * p is the output pointer. Note that the output codeword
    * is c(x) = c[n-1]x^n-1 ... c[0]x^0
    * (i.e., the elements are arranged according to their
    * power. That's why c[n-1] comes out first.
    */
   p = n-1;

   /*
    * and now feed the data symbols one by one 
    * (data is fed from highest power (m[k-1]) to lowest (m[0])
    */
   for (i=rs->k-1;i>=0; i--) {

      gu feedback = gf_add(gf, reg[b-1], m[i]);

      /*
       * systematic encoder - the first outputs are the 
       * input data
       */
      //c[p--] = m[i];
      p--;

      for(j=b-1; j>0; j--) {
         reg[j] = gf_mul(gf, feedback, g[j]);
         reg[j] = gf_add(gf, reg[j], reg[j-1]);
      }
      /* for the leftmost branch no addition is necessary: */
      reg[0] = gf_mul(gf, feedback, g[0]);
   }
    
   if (p != (b-1)) {
      return 0;
   }

   /* finished with symbols. insert 0's and take out the reminder */
   for(i=b-1; i>=0 ;i--) {
      gu feedback = reg[i];
      c[p--] = feedback;
   }
   return (p==-1);
}

/* Decoding:
 *
 * First we compute the syndroms. The syndroms are the result of evaluating
 * S[i] = c(alpha^i) where c(x) is the codeword polynomial and i = 0..2T-1 -
 * the roots of the code generator polynomial.
 *
 * If no errors occured, S[i]=0 for all i.
 *
 * For each syndrom calculation we use the following trick to evaluate
 * the polynomial:
 *
 * c(x) = c[n-1]x^n-1 + c[n-2]x^(n-1) + ... + c[0]x^0
 *
 * c_n      = 0
 * c_n-1(x) = c_n*x + c[n-1] = c[n-1]
 * c_n-2(x) = c_n-1(x)*x + c[n-2] = c[n-1]x + c[n-2]
 * c_n-3(x) = c_n-2(x)*x + c[n-3] = c[n-1]x^2 + c[n-2]*x + c[n-3]
 * ..
 * c(x)  = c_0(x) = c_1(x)*x + c[0]
 *
 * Or in hardware:
 *                   GF(256) multiplication by x        
 *                            .---.
 *                        .---| x |<--.
 *                        |   `---'   |
 *                        V   .----.  |
 * c[0]...c[n-2] c[n-1]->[+]->| Di |--'
 *                            `----'
 *
 * The register is initialized to 0 and then new symbols are
 * shifted in as they arrive.
 *
 * Adaptation to different RS(n,k):
 *   For n!=255
 *	   Same operation. Feed only the actual codeword
 *	   (no need to zero pad since the zeros would come
 *     *before* c[n] and will not affect the syndroms.
 *	
 *	 For t!=16
 *     Just make sure the extra syndroms are forced 
 *     to zero when transferred to the key equation
 *     solver.
 *
 */
#define kTT 16

static void
_rs_syn(rs_t *rs, gu *m, gu *c, gu *s) {
   int i, j;
   int n = rs->n;
   int t = rs->t;
   int t2 = t << 1;
   gf_t *gf = rs->gf;
   gu *alpha_to = gf_alpha_to_array(gf);

   /* evaluate 2t syndroms s_i = c(alpha^i), i=0..2T-1 */
   for(i=0; i<2*t; i++) {
      gu multiply_by = alpha_to[i];
      s[i] = 0;
      for (j=n-1; j>=t2; j--) {
         gu feedback = gf_mul(gf, s[i], multiply_by);
         s[i] = gf_add(gf, m[j-t2], feedback);
      }
      for (j=t2-1; j>=0; j--) {
         gu feedback = gf_mul(gf, s[i], multiply_by);
         s[i] = gf_add(gf, c[j], feedback);
      }
   }
   for(; i<2*kTT; i++) {
      s[i]=0;
   }
}

/* 
 *  Key equation solver (KES)

	 The PE1 processor: (i=0..2t-1)
                 ^wh_i
                 |
	             | .---.
	dhat_i[r]  <-+-| D |<---[+]----[X]<---+-- dhat_i+1[r]
                   `---'     ^      ^     |
                             |      |     |
	                         |      `-----)-- gamma[r]
                             |            |
	                        [X]<----------)-- delta[r]
                             |            |
					         |            |
					         `-----.      |
							       |      |
	                          .----)------'
	          .---.    ,----./ 1   |
	th_i[r] .-| D |<--< MUX |      |
			| `---'    `----'\_0   |
			|            |     |   |
			`------->----)-----+---'
				         |
						 ^
						 MC[r]
	

	  And the DC block:
                                                   ,---------.
	 ,------------------------------------------+->| control |
	 |                                          |  |         |
     |   wh0     wh1       wh_t-1               |  |         |
     |   ^       ^         ^                    |  `---------'
	 | ,----.   ,----.            ,----.        |    |  |
	 `-|PE1 |<--|PE1 |<--  ... <--|PE1 |<-- 0   |    |  V MC[r]
	   |0   |   |1   |            |2t-1|        |    | 
	   `----'   `----'            `----'        |    V gamma[r]
                                                V
	                                            delta[r]

		Initialization:												
		PE1_i = s_i, i=0..2t-1

		Output:
		wh0 .. wh_t-1 - error locator polynomial (after 2t iterations)


	 The PE0 processor: (i=t..0)
                                    ^ lambda_i
	                                |
                ,-------------------+-----.
				|                   |     |
	            |  .---.            V     |
	            `--| D |<---[+]----[X]    |
                   `---'     ^      ^     |
                             |      |     |
	                         |      `-----)-- gamma[r]
                             |            |
	                        [X]<----------)-- delta[r]
                             |            |
					         |            |
					         `-----.      V
							       |      |
	                          .----)------'
	           .---.   ,----./ 1   |
	B_i[r]  <--| D |<-< MUX |      |
			   `---'   `----'\ 0   |
			             |    `----+--------- B_i-1[r]
				         |
						 ^
						 MC[r]

	
		And the ELU block:

		 ^lambda_t  ^lambda_t-1      ^lambda1 ^lambda0
	   ,----.     ,----.            ,----.   ,----.
	   |PE0	|     |PE0 |            |PE0 |   |PE0 |
       |t   |<----|t-1 |<-- ...  <--|1   |<--|0   |<- 0
	   `----'     `----'            `----'   `----'

		Initialization:
		PE0_i = 0, i=1..t
		PE0_0 = 1

		Output:
		The error locator polynomial (after 2t iterations)
	

	Adaptation to N!=255, T!=16:

  	  No need to change anything. Just make sure
	  the extra syndroms were zeroed by the syndrom calculation
	  unit.

	  The number of iterations is of course 2*t (and not 2*16)
	
	  The extra lambda[] and omega[] symbols will be
	  zero and will not affect the forney/chien modules.
 *
 */

static void
_rs_kes(gf_t *gf, int n, int t, gu *s, gu *l, gu *w) {
   int i;

   /* r is the iteration count of the irBM algorithm */	
   int	r;

   /* PE1[i][r] state variables of PE1_i at iteration r */
   gu dhat[2*kTT][2*t+1];
   gu th  [2*kTT][2*t+1];

   /* PE0[i][r] state variables of PE0_i at iteration r */
   gu B     [kTT+1][2*t+1];
   gu lambda[kTT+1][2*t+1];

   /* control block signals (not all are registers!) */
   int kcnt [2*kTT+1];
   gu  gamma[2*kTT+1];
   gu  delta[2*kTT+1];
   int MC   [2*kTT+1];



   /* r is the iteration count. r = 0.. 2t-1 */
   r = 0; 

   /* initialize PE1 array (DC block) */
   for(i=0; i<2*kTT; i++) {
      dhat[i][r] = s[i];
      th[i][r]   = s[i];
   }

   /* initialize PE0 array (ELU block */
   for(i=kTT; i>0; i--)  {
      B[i][r]      = 0;
      lambda[i][r] = 0;
   }
   B[i][r] = 1;
   lambda[i][r] = 1;


   /* initialize the control block */
   kcnt[r] = 0;
   gamma[r] = 1;

   for(; r<2*t; r++) {

      /* control block */

      delta[r] = dhat[0][r];
      if (delta[r] != 0 && kcnt[r] >= 0)	 {
         MC[r] = 1;
      } else {
         MC[r] = 0;
      }

      if (MC[r] == 1) {
         gamma[r+1] = delta[r];
         kcnt[r+1] = -kcnt[r] - 1;
      } else {
         gamma[r+1] = gamma[r];
         kcnt[r+1] = kcnt[r] + 1;
      }

      /* ELU block (PE0 processor) */

      for(i=0; i<=kTT; i++) {
         gu m1, m2, prev;

         prev = (i > 0) ? B[i-1][r] : 0;

         if (MC[r])
            B[i][r+1] = lambda[i][r];
         else
            B[i][r+1] = prev;

         m1 = gf_mul(gf, lambda[i][r], gamma[r]);
         m2 = gf_mul(gf, prev, delta[r]);

         lambda[i][r+1] = gf_add(gf, m1, m2);
      }

      /* DC block (PE1 processor) */
		

      for(i=0; i<2*kTT; i++) {
         gu m1, m2, prev;

         prev = (i < 2*kTT-1) ? dhat[i+1][r] : 0;

         m1 = gf_mul(gf, prev, gamma[r]);
         m2 = gf_mul(gf, th[i][r], delta[r]);

         dhat[i][r+1] = gf_add(gf, m1, m2);

         if (MC[r]) {
            th[i][r+1] = prev;
         }
         else {
            th[i][r+1] = th[i][r];
         }
      }	

   }

   for(i=0; i<=kTT; i++) {
      l[i] = lambda[i][r];
   }
   for(i=0; i<kTT; i++) {
      w[i] = dhat[i][r];
   }
}

int
rs_decode(rs_t *rs, unsigned char *data, unsigned char *parity) {
   
   if (!rs || !data || !parity) {
      return 0;
   }
    
   gu *m = (gu*)data;
   gu *c = (gu*)parity;
    
   gf_t *gf = rs->gf;
   int i, roots, deg_lambda;
   int n = rs->n;
   int t = rs->t;
   int t2 = t << 1;

   gu syndrom[2*kTT];
   //gu syndrom_check[2*kTT];
   gu lambda[kTT+1];
   gu omega[kTT];
  
   gu lambda_odd[kTT+1];

   // compute the syndroms: s[0]..s[2t-1] 
   _rs_syn(rs, m, c, syndrom);
	
   // key equation solver
   _rs_kes(gf, n, t, syndrom, lambda, omega);

   // chien search

   // Adaptaion for N!=255, T!=16
   // No need to change anything. However z^(2*t) is
   // computed with the real t of course.
	
   deg_lambda = 0;
   for(i=0; i<kTT+1; i++) {
      lambda_odd[i] = (i%2 == 1) ? lambda[i] : 0;
      if (lambda[i] != 0) {
         deg_lambda = i;
      }
   }

   /* 
    * Calculate lambda[alpha^-i], i = n-1 .. 0
    *
    * lambda[a^-i] = l0 + l1*a^-i + l2*a^-i ... 
    *
    * If n=NN, i.e. n=2^m-1=255,
    * alpha^-(n-1) = alpha^-(NN-1) = alpha^(NN-(NN-1)) =  alpha^1 
    *
    * If, however n<NN: n = NN-s
    * alpha^-(n-1) = alpha^(NN-n+1) = alpha^(s+1)
    *
    * I.e., we must calculate alpha^s first
    */
   roots = 0;
   gu *alpha_to = gf_alpha_to_array(gf);
   for(i=n-1; i>=0; i--) {
      gu root = alpha_to[(RS_MAX_MSG_LEN - i) % (RS_MAX_MSG_LEN)];
      gu result;
      result = gf_poly(gf, lambda, kTT+1, root);

      if (result == 0) {
         gu den = gf_poly(gf, lambda_odd, kTT+1, root);
         gu num = gf_poly(gf, omega, kTT, root);
         gu z2t = gf_pow(gf, root, t2);
         gu result = gf_mul(gf, z2t, num);
			
         result = gf_div(gf, result, den);

         if (i < t2) {
            c[i] ^= result;
         } else {
            m[i - t2] ^= result;
         }

         roots++;
      }
   }

   rs->uncorrectable = (deg_lambda != roots);

//    if (0) {
//        int e = 0;
//        _rs_syn(rs, m, c, syndrom_check);
//        for (i=0; i<2*t; i++) {
//            e += syndrom[i] != syndrom_check[i];
//        }
//    }
    
   return !rs->uncorrectable;
}

rs_t*
rs_init(unsigned data_len, unsigned parity_bytes) {
   if ((data_len + parity_bytes <= RS_MAX_MSG_LEN) &&
       (parity_bytes % 2 == 0) &&
       (parity_bytes <= (2*kTT)))
   {
      rs_t *rs = mm_malloc(sizeof(*rs));
      rs->gf = gf_init();
      rs->t = parity_bytes >> 1;
      rs->k = data_len;
      rs->n = rs->k + parity_bytes;
      rs->g_arr = mm_malloc(2 * rs->t + 1);
      _generate_rs(rs);
      return rs;
   }
   return 0;
}

void
rs_fini(rs_t *rs) {
   if (rs) {
      gf_fini(rs->gf);
      mm_free(rs->g_arr);
      mm_free(rs);
   }
}

#undef kTT
#endif // M_FOUNDATION_IMPORT_MODEL_RS
