/*
	cache.c
		caching of tensor coefficients in
		dynamically allocated memory
		this file is part of LoopTools
		last modified 14 Apr 18 th
*/


#define MUTEX

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef MUTEX
#include <pthread.h>
#endif
#include "cexternals.h"

#if NOUNDERSCORE
#define cacheindex_ cacheindex
#define cachecopy_ cachecopy
#define ltcache_ ltcache
#endif

#if QUAD
#define MSB (1-BIGENDIAN)
#else
#define MSB 0
#endif


typedef long long dblint;

typedef unsigned long long udblint;

typedef struct { dblint part[QUAD+1]; } RealType;

typedef const RealType cRealType;

typedef struct { RealType re, im; } ComplexType;


typedef long long memindex;


extern struct {
  int cmpbits;
} ltcache_;


enum { ncaches = 10 };

#ifdef MUTEX
static pthread_mutex_t mutex[ncaches] = {
  PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER
};
#endif


static inline int IDim(const int i) {
  return i & (-i >> (8*sizeof i - 1));
}


static inline int SignBit(const dblint i) {
  return (udblint)i >> (8*sizeof i - 1);
}


static inline memindex PtrDiff(const void *a, const void *b) {
  return (char *)a - (char *)b;
}


static dblint CmpPara(cRealType *para1, cRealType *para2,
  int n, const dblint mask)
{
  while( n-- ) {
    const dblint c = (mask & para1->part[MSB]) -
                     (mask & para2->part[MSB]);
    if( c ) return c;
    ++para1;
    ++para2;
  }
  return 0;
}


#if QUAD

static dblint CmpParaLo(cRealType *para1, cRealType *para2,
  int n, const dblint mask)
{
  while( n-- ) {
    dblint c = para1->part[MSB] - para2->part[MSB];
    if( c ) return c;
    c = (mask & para1->part[1-MSB]) - (mask & para2->part[1-MSB]);
    if( c ) return c;
    ++para1;
    ++para2;
  }
  return 0;
}

#endif


static void *Lookup(cRealType *para, double *base,
  void (*calc)(RealType *, cRealType *),
  const int npara, const int nval, const int cacheno)
{
  typedef struct node {
    struct node *next[2], *succ;
    int serial;
    RealType para[2];
  } Node;

#define base_valid (int *)&base[0]
#define base_last (Node ***)&base[1]
#define base_first (Node **)&base[2]

  const int valid = *base_valid;
  Node **last = *base_last;
  Node **next = base_first;
  Node *node;

#ifdef MUTEX
  pthread_mutex_t *mx = &mutex[cacheno];
#endif

  if( last == NULL ) last = next;

  {
    dblint mask = -(1ULL << IDim(64 - ltcache_.cmpbits));
#if QUAD
    dblint (*cmp)(cRealType *, cRealType *, int, const dblint) = CmpPara;
    if( ltcache_.cmpbits >= 64 ) {
      mask = -(1ULL << IDim(128 - ltcache_.cmpbits));
      cmp = CmpParaLo;
    }
#else
#define cmp CmpPara
#endif

    while( (node = *next) && node->serial < valid ) {
      const dblint i = cmp(para, node->para, npara, mask);
      if( i == 0 ) goto x0;
      next = &node->next[SignBit(i)];
    }

#ifdef MUTEX
    pthread_mutex_lock(mx);

    while( (node = *next) && node->serial < valid ) {
      const dblint i = cmp(para, node->para, npara, mask);
      if( i == 0 ) goto x1;
      next = &node->next[SignBit(i)];
    }
#endif
  }

  node = *last;

  if( node == NULL ) {
	/* The "RealType para[2]" bit in Node is effectively an extra
	   Complex for alignment so that node can be reached with
	   an integer index into base */
    size_t mem = sizeof(Node) +
      npara*sizeof(RealType) + nval*sizeof(ComplexType);
    assert( (node = malloc(mem)) );
    node = (Node *)((char *)node +
      (PtrDiff(base, &node->para[npara]) & (sizeof(ComplexType) - 1)));
    node->succ = NULL;
    node->serial = valid;
    *last = node;
  }

  *next = node;
  *base_last = &node->succ;
  *base_valid = valid + 1;

  node->next[0] = NULL;
  node->next[1] = NULL;

  memcpy(node->para, para, npara*sizeof(RealType));
  calc(&node->para[npara], para);

#ifdef MUTEX
x1:
  pthread_mutex_unlock(mx);
#endif

x0:
  return &node->para[npara];
}


memindex cacheindex_(cRealType *para, double *base,
  void (*calc)(RealType *, cRealType *),
  const int *pnpara, const int *pnval, const int *pcacheno)
{
  ComplexType *val = Lookup(para, base, calc, *pnpara, *pnval, *pcacheno);
  return PtrDiff(val, base)/(long)sizeof(ComplexType);
}


void cachecopy_(ComplexType *dest, cRealType *para, double *base,
  void (*calc)(RealType *, cRealType *),
  const int *pnpara, const int *pnval, const int *pcacheno)
{
  ComplexType *val = Lookup(para, base, calc, *pnpara, *pnval, *pcacheno);
  memcpy(dest, val, *pnval*sizeof *dest);
}

