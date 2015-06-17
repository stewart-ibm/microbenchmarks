/* Strongly derived from InnoDB code.
 * i.e. I only wrote main()
 *
 * This takes the "new" InnoDB checksum algorithm and microbenchmarks it.
 *
 * CFLAGS=-O3 make mysql-innodb-new-checksum ; time ./mysql-innod-new-checksum
 */

/*****************************************************************************

Copyright (c) 1994, 2009, Oracle and/or its affiliates. All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA

*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define UNIV_INLINE static inline
#define UNIV_INTERN static
#define ut_ad(x) do { }while(0)
typedef unsigned long long ulint;
typedef char byte;
#define UT_HASH_RANDOM_MASK	1463735687
#define UT_HASH_RANDOM_MASK2	1653893711
#define FIL_PAGE_OFFSET              4
#define FIL_PAGE_FILE_FLUSH_LSN     26
#define FIL_PAGE_END_LSN_OLD_CHKSUM 8
#define FIL_PAGE_DATA               38
#define UNIV_PAGE_SIZE 16384

UNIV_INLINE
ulint
ut_fold_ulint_pair(
/*===============*/
	ulint	n1,	/*!< in: ulint */
	ulint	n2)	/*!< in: ulint */
{
	return(((((n1 ^ n2 ^ UT_HASH_RANDOM_MASK2) << 8) + n1)
		^ UT_HASH_RANDOM_MASK) + n2);
}

/*************************************************************//**
Folds a binary string.
@return	folded value */
UNIV_INLINE
ulint
ut_fold_binary(
/*===========*/
	const byte*	str,	/*!< in: string of bytes */
	ulint		len)	/*!< in: length */
{
	ulint		fold = 0;
	const byte*	str_end	= str + (len & 0xFFFFFFF8);

	ut_ad(str || !len);

	while (str < str_end) {
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
	}

	switch (len & 0x7) {
	case 7:
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
	case 6:
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
	case 5:
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
	case 4:
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
	case 3:
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
	case 2:
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
	case 1:
		fold = ut_fold_ulint_pair(fold, (ulint)(*str++));
	}

	return(fold);
}

UNIV_INTERN
ulint
buf_calc_page_new_checksum(
/*=======================*/
	const byte*	page)	/*!< in: buffer page */
{
	ulint checksum;

	/* Since the field FIL_PAGE_FILE_FLUSH_LSN, and in versions <= 4.1.x
	FIL_PAGE_ARCH_LOG_NO_OR_SPACE_ID, are written outside the buffer pool
	to the first pages of data files, we have to skip them in the page
	checksum calculation.
	We must also skip the field FIL_PAGE_SPACE_OR_CHKSUM where the
	checksum is stored, and also the last 8 bytes of page because
	there we store the old formula checksum. */

	checksum = ut_fold_binary(page + FIL_PAGE_OFFSET,
				  FIL_PAGE_FILE_FLUSH_LSN - FIL_PAGE_OFFSET)
		+ ut_fold_binary(page + FIL_PAGE_DATA,
				 UNIV_PAGE_SIZE - FIL_PAGE_DATA
				 - FIL_PAGE_END_LSN_OLD_CHKSUM);
	checksum = checksum & 0xFFFFFFFFUL;

	return(checksum);
}

int main()
{
  byte *page;
  int i;
  volatile ulint csum=0; /* volatile to force GCC not to optimize out loop */

  page = malloc(UNIV_PAGE_SIZE);

  srand(42);

  for (i=0; i< UNIV_PAGE_SIZE; i++)
    page[i] = rand();

  for (i=0; i<100000; i++)
    csum+= buf_calc_page_new_checksum(page);

  free(page);

  if (csum != 105765277000000ULL) {
	  printf("FAILED, expected 105765277000000 got %llu \n", csum);
	  return 1;
  }

  return 0;
}
