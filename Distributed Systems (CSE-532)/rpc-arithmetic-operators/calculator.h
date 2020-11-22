/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _CALCULATOR_H_RPCGEN
#define _CALCULATOR_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct Argument {
	int a;
	int b;
};
typedef struct Argument Argument;

#define CALCULATOR_PROG 0x77777777
#define CALCULATOR_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define ADD 1
extern  int * add_1(Argument *, CLIENT *);
extern  int * add_1_svc(Argument *, struct svc_req *);
#define SUBTRACT 2
extern  int * subtract_1(Argument *, CLIENT *);
extern  int * subtract_1_svc(Argument *, struct svc_req *);
#define MULTIPLY 3
extern  int * multiply_1(Argument *, CLIENT *);
extern  int * multiply_1_svc(Argument *, struct svc_req *);
#define DIVIDE 4
extern  int * divide_1(Argument *, CLIENT *);
extern  int * divide_1_svc(Argument *, struct svc_req *);
extern int calculator_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define ADD 1
extern  int * add_1();
extern  int * add_1_svc();
#define SUBTRACT 2
extern  int * subtract_1();
extern  int * subtract_1_svc();
#define MULTIPLY 3
extern  int * multiply_1();
extern  int * multiply_1_svc();
#define DIVIDE 4
extern  int * divide_1();
extern  int * divide_1_svc();
extern int calculator_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Argument (XDR *, Argument*);

#else /* K&R C */
extern bool_t xdr_Argument ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_CALCULATOR_H_RPCGEN */
