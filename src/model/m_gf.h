// gf.h

#ifndef _GF_H_
#define _GF_H_

#ifdef __cplusplus
extern "C" {
#endif

//typedef unsigned gu;  /* or 16bit */
typedef unsigned char gu;       /* 8 bit */
   
typedef struct s_gf gf_t;   

gf_t* gf_init(void);
void gf_fini(gf_t*);

gu gf_add(gf_t *gf, gu a, gu b);
gu gf_mul(gf_t *gf, gu a, gu b);
gu gf_div(gf_t *gf, gu a, gu b);
gu gf_inv(gf_t *gf, gu a);
gu gf_pow(gf_t *gf, gu a, int n);
gu gf_poly(gf_t *gf, gu p[], int n, gu a);

gu* gf_alpha_to_array(gf_t*);
gu* gf_index_of_array(gf_t*);

#ifdef __cplusplus
}
#endif

#endif
