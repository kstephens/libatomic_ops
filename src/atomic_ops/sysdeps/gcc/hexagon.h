/*
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 */

#include "../all_aligned_atomic_load_store.h"

#include "../test_and_set_t_is_ao_t.h"

/* There's also "isync" and "barrier"; however, for all current CPU     */
/* versions, "syncht" should suffice.  Likewise, it seems that the      */
/* auto-defined versions of *_acquire, *_release or *_full suffice for  */
/* all current ISA implementations.                                     */
AO_INLINE void
AO_nop_full(void)
{
  __asm__ __volatile__("syncht" : : : "memory");
}
#define AO_HAVE_nop_full

/* The Hexagon has load-locked, store-conditional primitives, and so    */
/* resulting code is very nearly identical to that of PowerPC.          */

AO_INLINE AO_t
AO_fetch_and_add(volatile AO_t *addr, AO_t incr)
{
  AO_t oldval;
  AO_t newval;
  __asm__ __volatile__(
     "1:\n"
     "  %0 = memw_locked(%3);\n"        /* load and reserve            */
     "  %1 = add (%0,%4);\n"            /* increment                   */
     "  memw_locked(%3,p1) = %1;\n"     /* store conditional           */
     "  if (!p1) jump 1b;\n"            /* retry if lost reservation   */
     : "=&r"(oldval), "=&r"(newval), "+m"(*addr)
     : "r"(addr), "r"(incr)
     : "memory", "p1");
  return oldval;
}
#define AO_HAVE_fetch_and_add

AO_INLINE AO_TS_VAL_t
AO_test_and_set(volatile AO_TS_t *addr)
{
  int oldval;
  int locked_value = 1;

  __asm__ __volatile__(
     "1:\n"
     "  %0 = memw_locked(%2);\n"        /* load and reserve            */
     "  {\n"
     "    p2 = cmp.eq(%0,#0);\n"        /* if load is not zero,        */
     "    if (!p2.new) jump:nt 2f; \n"  /* we are done                 */
     "  }\n"
     "  memw_locked(%2,p1) = %3;\n"     /* else store conditional      */
     "  if (!p1) jump 1b;\n"            /* retry if lost reservation   */
     "2:\n"                             /* oldval is zero if we set    */
     : "=&r"(oldval), "+m"(*addr)
     : "r"(addr), "r"(locked_value)
     : "memory", "p1", "p2");
  return (AO_TS_VAL_t)oldval;
}
#define AO_HAVE_test_and_set

AO_INLINE int
AO_compare_and_swap(volatile AO_t *addr, AO_t old, AO_t new_val)
{
  AO_t __oldval;
  int result = 0;
  __asm__ __volatile__(
     "1:\n"
     "  %0 = memw_locked(%3);\n"        /* load and reserve            */
     "  {\n"
     "    p2 = cmp.eq(%0,%4);\n"        /* if load is not equal to     */
     "    if (!p2.new) jump:nt 2f; \n"  /* old, fail                   */
     "  }\n"
     "  memw_locked(%3,p1) = %5;\n"     /* else store conditional      */
     "  if (!p1) jump 1b;\n"            /* retry if lost reservation   */
     "  %1 = #1\n"                      /* success, result = 1         */
     "2:\n"
     : "=&r" (__oldval), "+r" (result), "+m"(*addr)
     : "r" (addr), "r" (old), "r" (new_val)
     : "p1", "p2", "memory"
  );
  return result;
}
#define AO_HAVE_compare_and_swap

#include "../ao_t_is_int.h"
