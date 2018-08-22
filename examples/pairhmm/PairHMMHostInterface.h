#ifndef PAIRHMM_HOST_INTERFACE_H
#define PAIRHMM_HOST_INTERFACE_H
#include <cstdint>
#include <string>
#include <vector>

typedef struct {
  int   len;
  char* _b;
  char* _q;
  char* _i;
  char* _d;
  char* _c;
} read_t;

typedef struct {
  int len;
  char* _b;
} hap_t;

static void alloc_data(read_t *v, int len) {
  v->len = len;
  v->_b = (char*)malloc(len);
  v->_q = (char*)malloc(len);
  v->_i = (char*)malloc(len);
  v->_d = (char*)malloc(len);
  v->_c = (char*)malloc(len);
}

static void alloc_data(hap_t *v, int len) {
  v->len = len;
  v->_b = (char*)malloc(len);
}

static void free_reads(read_t *r, int n) {
  for (int i = 0; i < n; i++) {
    free(r[i]._b);
    free(r[i]._q);
    free(r[i]._i);
    free(r[i]._d);
    free(r[i]._c);
  }
  free(r);
}

static void free_haps(hap_t *h, int n) {
  for (int i = 0; i < n; i++) {
    free(h[i]._b);
  }
  free(h);
}

uint64_t serialize(void* buf, const read_t* reads, int num);
uint64_t serialize(void* buf, const hap_t* haps, int num);

int deserialize(const void* buf, read_t* &reads);
int deserialize(const void* buf, hap_t* &haps);
#endif
