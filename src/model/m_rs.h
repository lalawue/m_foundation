
#ifndef rs_h
#define rs_h

#ifdef __cplusplus
extern "C" {
#endif
    
#define RS_MAX_MSG_LEN 255

typedef struct s_rs rs_t;

/* max length RS_MAX_MSG_LEN, correct error bytes <= parity_bytes/2
 * @param data_len: data bytes
 * @param parity_bytes: parity bytes, more than 2
 */
rs_t* rs_init(unsigned data_len, unsigned parity_bytes);
void rs_fini(rs_t *rs);

/* encode & decode data with parity symbol
 * @param data: data input
 * @param c: buffer to store parity symbol
 */
int rs_encode(rs_t*, unsigned char *data, unsigned char *parity);
int rs_decode(rs_t*, unsigned char *data, unsigned char *parity);

#ifdef __cplusplus
}
#endif

#endif
