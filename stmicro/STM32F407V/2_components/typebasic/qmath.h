#ifndef __QMATH_H__
#define __QMATH_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "typebasic.h"

#define Q0              0
#define Q1              1
#define Q2              2
#define Q3              3
#define Q4              4
#define Q5              5
#define Q6              6
#define Q7              7
#define Q8              8
#define Q9              9
#define Q10             10
#define Q11             11
#define Q12             12
#define Q13             13
#define Q14             14
#define Q15             15
#define Q16             16
#define Q17             17
#define Q18             18
#define Q19             19
#define Q20             20
#define Q21             21
#define Q22             22
#define Q23             23
#define Q24             24
#define Q25             25
#define Q26             26
#define Q27             27
#define Q28             28
#define Q29             29
#define Q29             29
#define Q30             30
#define Q31             31

#define Q2F(q, n)       ((f32)((f32)(q) / (f32)(1ul << (n))))
#define F2Q(f, n)       ((u32)((f32)(f) * (f32)(1ul << (n))))
#define Q2D(q, n)       ((q) >> (n))

#define QAdd(q1, q2)    ((q1) + (q2))           // Qn+Qn=Qn
#define QSub(q1, q2)    ((q1) - (q2))           // Qn-Qn=Qn
#define QMul(q1, q2, n) (((q1) * (q2)) >> (n))  // Qn1*Qn2=Q(n1+n2)
#define QDiv(q1, q2, n) (((q1) << (n)) / (q2))  // Qn1/Qn2=Q(n1-n2)

#define QDiv2(q)        ((q) >> 1ul)
#define QMul2(q)        ((q) << 1ul)

void SinCosQ15(u16 theta, s16* sine, s16* cosine);
s16  SinQ15(s16 theta);
s16  CosQ15(s16 theta);
u16  SqrtQ12(u32 value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
