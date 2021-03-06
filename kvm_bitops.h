/*
 * GPL HEADER START
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * GPL HEADER END
 */

#ifndef _ASM_X86_BITOPS_H
#define	_ASM_X86_BITOPS_H

/*
 * Copyright 1992, Linus Torvalds.
 * Copyright 2011, Joyent, Inc.
 *
 * Note: inlines with more than a single statement should be marked
 * __always_inline to avoid problems with older gcc's inlining heuristics.
 */

#define	DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define	BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, 8 * sizeof (long))

/*
 * These have to be done with inline assembly: that way the bit-setting
 * is guaranteed to be atomic. All bit operations return 0 if the bit
 * was cleared before the operation and != 0 if it was not.
 *
 * bit 0 is the LSB of addr; bit 32 is the LSB of (addr+1).
 */
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 1)
/*
 * Technically wrong, but this avoids compilation errors on some gcc
 * versions.
 */
#define	BITOP_ADDR(x) "=m" (*(volatile long *) (x))
#else
#define	BITOP_ADDR(x) "+m" (*(volatile long *) (x))
#endif

/*
 * We do the locked ops that don't return the old value as
 * a mask operation on a byte.
 */
#define	IS_IMMEDIATE(nr)		(__builtin_constant_p(nr))
#define	CONST_MASK_ADDR(nr, addr)	\
	BITOP_ADDR((uintptr_t)(addr) + ((nr) >> 3))
#define	CONST_MASK(nr)			(1 << ((nr) & 7))

/*
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered.  See __set_bit()
 * if you do not require the atomic guarantees.
 *
 * Note: there are no guarantees that this function will not be reordered
 * on non x86 architectures, so if you are writing portable code,
 * make sure not to rely on its reordering guarantees.
 *
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
extern inline void set_bit(unsigned int, volatile unsigned long *);

/*
 * __set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
extern inline void __set_bit(int, volatile unsigned long *);

/*
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
 * in order to ensure changes are visible on other processors.
 */
extern inline void clear_bit(int, volatile unsigned long *);
extern inline void __clear_bit(int, volatile unsigned long *);

/*
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
extern inline int test_and_set_bit(int, volatile unsigned long *);

/*
 * __test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
extern inline int __test_and_set_bit(int, volatile unsigned long *);

/*
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
extern inline int test_and_clear_bit(int, volatile unsigned long *);

/*
 * __test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
extern inline int __test_and_clear_bit(int, volatile unsigned long *);

extern inline int constant_test_bit(unsigned int,
    const volatile unsigned long *);
extern inline int variable_test_bit(int, volatile const unsigned long *);

/*
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */

#define	test_bit(nr, addr)			\
	(__builtin_constant_p((nr))		\
	? constant_test_bit((nr), (addr))	\
	: variable_test_bit((nr), (addr)))

/*
 * __ffs - find first set bit in word
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
extern inline unsigned long __ffs(unsigned long);

/*
 * ffz - find first zero bit in word
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
extern inline unsigned long ffz(unsigned long);

/*
 * __fls: find last set bit in word
 * @word: The word to search
 *
 * Undefined if no set bit exists, so code should check against 0 first.
 */
extern inline unsigned long __fls(unsigned long);

#ifdef __KERNEL__
/*
 * ffs - find first set bit in word
 * @x: the word to search
 *
 * This is defined the same way as the libc and compiler builtin ffs
 * routines, therefore differs in spirit from the other bitops.
 *
 * ffs(value) returns 0 if value is 0 or the position of the first
 * set bit if value is nonzero. The first (least significant) bit
 * is at position 1.
 */
extern inline int ffs(int);

/*
 * fls - find last set bit in word
 * @x: the word to search
 *
 * This is defined in a similar way as the libc and compiler builtin
 * ffs, but returns the position of the most significant set bit.
 *
 * fls(value) returns 0 if value is 0 or the position of the last
 * set bit if value is nonzero. The last (most significant) bit is
 * at position 32.
 */
extern inline int fls(int);

#endif /* __KERNEL__ */

#endif /* _ASM_X86_BITOPS_H */
