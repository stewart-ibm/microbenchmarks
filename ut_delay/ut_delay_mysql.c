/* Strongly derived from MySQL, so their copyright applies to near all of this.
 *
 * This benchmark is designed to count CPU cycles for various delays.
 */

/*
   Copyright (c) 2000, 2013, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/
#include <stdio.h>

volatile int ut_always_false = 0;

static inline unsigned long mftb(void)
{
	unsigned long tb;

	/* We use a memory clobber to avoid this being
         * moved in the instruction stream
         */
	asm volatile("mftb %0" : "=r"(tb) : : "memory");
	return tb;
}
typedef unsigned long ulint;
typedef long int lint;
typedef int ibool;

#define UT_LOW_PRIORITY_CPU() __asm__ __volatile__ ("or 1,1,1")
#define UT_RESUME_PRIORITY_CPU() __asm__ __volatile__ ("or 2,2,2")

# define os_compare_and_swap(ptr, old_val, new_val) \
        __sync_bool_compare_and_swap(ptr, old_val, new_val)

# define os_compare_and_swap_lint(ptr, old_val, new_val) \
        os_compare_and_swap(ptr, old_val, new_val)


#define UT_RELAX_CPU() do { \
	volatile lint volatile_var; \
	os_compare_and_swap_lint(&volatile_var, 0, 1); \
	} while (0)


static ulint
ut_delay(ulint delay) /*!< in: delay in microseconds on 100 MHz Pentium */
{
ulint i, j;

UT_LOW_PRIORITY_CPU();

j = 0;

for (i = 0; i < delay * 50; i++)
{ j += i; UT_RELAX_CPU(); }

if (ut_always_false)
{ ut_always_false = (ibool) j; }

UT_RESUME_PRIORITY_CPU();

return(j);
}

int main()
{
	unsigned long before, after, total=0;
	int i;
	for (i=0; i<1000000; i++) {
		before = mftb();
		ut_delay(6);
		after = mftb();
		total+=after-before;
	}
	printf("tb change (avg over %d): %d\n", i, total/i);
}
