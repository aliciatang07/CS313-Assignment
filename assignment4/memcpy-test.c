#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "delaymemcpy.h"

#define MAX_ARRAY_SIZE 0x40000000 // 2^30=1GB

/* Arrays declared as global, as this allows program to use more
   memory. Aligned to page boundaries so other global variables don't
   end up in same page. */
unsigned char __attribute__ ((aligned (0x1000))) array[MAX_ARRAY_SIZE];
unsigned char __attribute__ ((aligned (0x1000))) copy[MAX_ARRAY_SIZE];
unsigned char __attribute__ ((aligned (0x1000))) C[MAX_ARRAY_SIZE];
/* Initializes the array with a random set of values. */
void *random_array(void *array, size_t size) {
  
  int i;
  size /= sizeof(long);
  for (i = 0; i < size; i++)
    ((long *)array)[i] = random();
  
  return array;
}

/* Prints all values of the array */
void print_array(unsigned char *array, int size) {
  int i;
  for (i = 0; i < size; i++)
    printf("%02x ", array[i]);
  printf("\n");
}

int main(void) {
  
  srandom(time(NULL));
  
  initialize_delay_memcpy_data();

  printf("\nCopying one page of data, trigger copy via read dst...\n");
  random_array(array, 0x1000);
  printf("Before copy: ");
  print_array(array, 20);

  delay_memcpy(copy, array, 0x1000);
  printf("After copy : ");
  print_array(array, 20); // Should not trigger copy
  printf("Destination: ");
  print_array(copy, 20);  // Triggers copy

  printf("\nCopying one page of data, trigger copy via write dst...\n");
  random_array(array, 0x1000);
  printf("Before copy: ");
  print_array(array, 20);

  delay_memcpy(copy, array, 0x1000);
  copy[0]++; // Triggers copy
  printf("After copy : ");
  print_array(array, 20);
  printf("Destination: ");
  print_array(copy, 20);

  printf("\nCopying one page of data, trigger copy via write src...\n");
  random_array(array, 0x1000);
  printf("Before copy: ");
  print_array(array, 20);

  delay_memcpy(copy, array, 0x1000);
  array[0]++; // Triggers copy
  printf("After copy : ");
  print_array(array, 20);
  printf("Destination: ");
  print_array(copy, 20);

  printf("\nCopying two pages of data...\n");
  random_array(array, 0x2000);
  printf("Before copy: ");
  print_array(array, 20);

  delay_memcpy(copy, array, 0x2000);
  printf("After copy : ");
  print_array(array, 20);
  printf("Destination: ");
  print_array(copy, 20);  // Triggers copy of first page only
  printf("2nd page:\nAfter copy : ");
  print_array(array + 0x1800, 20);
  printf("Destination: ");
  print_array(copy + 0x1800, 20);  // Triggers copy of second page

  printf("\nCopying unaligned page of data...\n");
  random_array(array, 0x2000);
  printf("Before copy: ");
  print_array(array + 0x400, 20);

  delay_memcpy(copy + 0x400, array + 0x400, 0x1000);
  printf("After copy : ");
  print_array(array + 0x400, 20);
  printf("Destination: ");
  print_array(copy + 0x400, 20);  // Triggers copy of first page only

  //MORE TESTS COME HERE

    printf("\nTEST1Copying two pages of data, trigger via wrtie dst...\n");
    random_array(array, 0x8000);
    printf("Before copy: ");
    print_array(array + 0x7fec, 20);

    delay_memcpy(copy, array, 0x8000);
    //copy[7980]++; // Triggers copy of the last page
    printf("2nd page:\nAfter copy : ");
    print_array(array + 0x7fec, 20);
    printf("Destination: ");
    print_array(copy + 0x7fec, 20);
    printf("1st page:\nAfter copy : ");
    print_array(array, 20);
    printf("Destination: ");
    print_array(copy, 20); // Triggers the copy of the first page

    // Test 2
    printf("\nTEST2Copying two pages of data, trigger via wrtie src...\n");
    random_array(array, 0x5000);
    printf("Before copy: ");
    print_array(array + 0x1800, 20);

    delay_memcpy(copy, array, 0x5000);
    array[300]++; // Triggers copy of the first page
    printf("2nd page:\nAfter copy : ");
    print_array(array + 0x1800, 20);
    printf("Destination: ");
    print_array(copy + 0x1800, 20);
    printf("1st page:\nAfter copy : ");
    print_array(array, 20);
    printf("Destination: ");
    print_array(copy, 20); // Triggers the copy of the first page

    // Test 3
    printf("\nTEST3Copying five pages of data...\n");
    random_array(array, 0x6000);
    printf("Before copy: ");
    print_array(array, 20);

    delay_memcpy(copy, array, 0x6000);
    printf("After copy : ");
    print_array(array, 20);
    printf("Destination: ");
    print_array(copy, 20);  // Triggers copy of first page only
    printf("2nd page:\nAfter copy : ");
    print_array(array + 0x1800, 20);
    printf("Destination: ");
    print_array(copy + 0x1800, 20);  // Triggers copy of second page
    printf("3rd page:\nAfter copy : ");
    print_array(array + 0x2800, 20);
    printf("Destination: ");
    print_array(copy + 0x2800, 20);  // Triggers copy of second page
    printf("4th page:\nAfter copy : ");
    print_array(array + 0x3800, 20);
    printf("Destination: ");
    print_array(copy + 0x3800, 20);  // Triggers copy of second page
    printf("5th page:\nAfter copy : ");
    print_array(array + 0x4800, 20);
    printf("Destination: ");
    print_array(copy + 0x4800, 20);  // Triggers copy of second page

    // Test 4
    printf("\nTEST4Copying unaligned page of data...\n");
    random_array(array, 0x6000);
    printf("Before copy: ");
    print_array(array + 0x2500, 20);

    delay_memcpy(copy + 0x400, array + 0x400, 0x6000);
    printf("After copy : ");
    print_array(array + 0x2500, 20);
    printf("Destination: ");
    print_array(copy + 0x2500, 20);  // Triggers copy of first page only


    //Test 5
    printf("\nTEST5Copying unaligned page of data with size < page_size...\n");
    random_array(array, 0x2000);
    printf("Before copy: ");
    print_array(array + 0x400, 20);

    delay_memcpy(copy + 0x400, array + 0x400, 0x600);
    printf("After copy : ");
    print_array(array + 0x400, 20);
    printf("Destination: ");
    print_array(copy + 0x400, 20);  // Triggers copy of specific range
    printf("After copy  (out of range): ");
    print_array(array + 0x1000, 20);
    printf("Destination (out of range): ");
    print_array(copy + 0x1000, 20); // Should not trigger copy, different values


    // Test 6
    printf("\nTEST6Copying unaligned page of data with ptr out of range (before src/dst)...\n");
    random_array(array, 0x2000);
    printf("Before copy: ");
    print_array(array + 0x400, 20);

    delay_memcpy(copy + 0x400, array + 0x400, 0x1000);
    copy[0]++;
    printf("After copy : ");
    print_array(array + 0x400, 20);
    printf("Destination: ");
    print_array(copy + 0x400, 20);  // trigger the copy of data in the specified page only
    printf("After copy  (out of range): ");
    print_array(array, 20);
    printf("Destination (out of range): ");
    print_array(copy, 20); // Should not trigger copy, different values   // #####fail

    //Test7
    printf("\nCopying unaligned pages of data with ptr out of range (after src/dst)...\n");
    random_array(array, 0x5000);
    printf("Before copy: ");
    print_array(array + 0x1400, 20);

    delay_memcpy(copy + 0x1400, array + 0x1400, 0x3000);
    copy[4500]++;
    printf("After copy : ");
    print_array(array + 0x1400, 20);
    printf("Destination: ");
    print_array(copy + 0x1400, 20);  // trigger the copy of data in the specified page only
    printf("After copy  (out of range): ");
    print_array(array + 0x4500, 20);
    printf("Destination (out of range): ");
    print_array(copy + 0x4500, 20);  // Should not trigger copy, different values  //##fail


    // Test 8
    printf("\nTEST7Copying unaligned page of data with address not in the same page as src/dst...\n");
    random_array(array, 0x3000);
    printf("Before copy: ");
    print_array(array + 0x1500, 20);

    delay_memcpy(copy + 0x400, array + 0x400, 0x2000);
    copy[1500]++;
    printf("After copy : ");
    print_array(array + 0x1500, 20);
    printf("Destination: ");
    print_array(copy + 0x1500, 20);  // trigger the copy of data in the specified page only

    // Test 9
    printf("\nTEST8Copying unaligned page of data with address in the same page as src/dst...\n");
    random_array(array, 0x2000);
    printf("Before copy: ");
    print_array(array + 0x400, 20);

    delay_memcpy(copy + 0x400, array + 0x400, 0x1000);
    copy[500]++;
    printf("After copy : ");
    print_array(array + 0x400, 20);
    printf("Destination: ");
    print_array(copy + 0x400, 20);  // trigger the copy of data in the specified page only


    // Test 10
    printf("\nTEST9Copying unaligned multiple pages of data...\n");
    random_array(array, 0x2000);
    printf("Before copy: ");
    print_array(array + 0x400, 20);

    delay_memcpy(copy + 0x400, array + 0x400, 0x2000);
    printf("After copy : ");
    print_array(array + 0x400, 20);
    printf("Destination: ");
    print_array(copy + 0x400, 20);  // Triggers copy of first page only

    //Test 11
    printf("\nTEST10Copying unaligned multiple pages of data...\n");
    random_array(array, 0x6000);
    printf("Before copy: ");
    print_array(array + 0x2400, 20);

    delay_memcpy(copy + 0x400, array + 0x400, 0x5000);
    printf("After copy : ");
    print_array(array + 0x2400, 20);
    printf("Destination: ");
    print_array(copy + 0x2400, 20);  // Triggers copy of second page only
    printf("\nMultiple copies are ongoing\n");
    random_array(array, 0x8000);

    delay_memcpy(copy, array, 0x1000);  // Copy No.1
    delay_memcpy(copy + 0x2000, array + 0x2000, 0x2000);  // Copy No.2
    delay_memcpy(copy + 0x5000, array + 0x5000, 0x3000);  // Copy No.3
    printf("Copy No.2:\nBefore copy: ");
    print_array(array + 0x2000, 20);
    printf("After copy : ");
    print_array(array + 0x2000, 20); // Should not trigger copy
    printf("Destination: ");
    print_array(copy + 0x2000, 20);  // Triggers copy
    printf("Copy No.1:\nBefore copy: ");
    print_array(array, 20);
    printf("After copy : ");
    print_array(array, 20); // Should not trigger copy
    printf("Destination: ");
    print_array(copy, 20);  // Triggers copy
    printf("Copy No.3:\nBefore copy: ");
    print_array(array + 0x5000, 20);
    printf("After copy : ");
    print_array(array + 0x5000, 20); // Should not trigger copy
    printf("Destination: ");
    print_array(copy + 0x5000, 20);  // Triggers copy


    // Test 13
    printf("\nTEST11Adding more than 20 pending copies\n");
    random_array(array, 0x30000);
    printf("Before copy: ");
    print_array(array + 0x20000, 20);

    delay_memcpy(copy, array, 0x500);  // Copy No.1
    delay_memcpy(copy + 0x1000, array + 0x1000, 0x500);  // Copy No.2
    delay_memcpy(copy + 0x2000, array + 0x2000, 0x500);  // Copy No.3
    delay_memcpy(copy + 0x3000, array + 0x3000, 0x500);  // Copy No.4
    delay_memcpy(copy + 0x4000, array + 0x4000, 0x500);  // Copy No.5
    delay_memcpy(copy + 0x5000, array + 0x5000, 0x500);  // Copy No.6
    delay_memcpy(copy + 0x6000, array + 0x6000, 0x500);  // Copy No.7
    delay_memcpy(copy + 0x7000, array + 0x7000, 0x500);  // Copy No.8
    delay_memcpy(copy + 0x8000, array + 0x8000, 0x500);  // Copy No.9
    delay_memcpy(copy + 0x9000, array + 0x9000, 0x500);      // Copy No.10
    delay_memcpy(copy + 0x10000, array + 0x10000, 0x500);  // Copy No.11
    delay_memcpy(copy + 0x11000, array + 0x11000, 0x500);  // Copy No.12
    delay_memcpy(copy + 0x12000, array + 0x12000, 0x500);  // Copy No.13
    delay_memcpy(copy + 0x13000, array + 0x13000, 0x500);  // Copy No.14
    delay_memcpy(copy + 0x14000, array + 0x14000, 0x500);  // Copy No.15
    delay_memcpy(copy + 0x15000, array + 0x15000, 0x500);  // Copy No.16
    delay_memcpy(copy + 0x16000, array + 0x16000, 0x500);  // Copy No.17
    delay_memcpy(copy + 0x17000, array + 0x17000, 0x500);  // Copy No.18
    delay_memcpy(copy + 0x18000, array + 0x18000, 0x500);  // Copy No.19
    delay_memcpy(copy + 0x19000, array + 0x19000, 0x1000);  // Copy No.20
    delay_memcpy(copy + 0x20000, array + 0x20000, 0x1000);  // Copy No.21

    printf("After copy : ");
    print_array(array + 0x20000, 20);
    printf("Destination: ");
    print_array(copy + 0x20000, 20);  // Triggers copy of the last page
    printf("After copy : ");
    print_array(array + 0x10000, 20);
    printf("Destination: ");
    print_array(copy + 0x10000, 20);  // Triggers copy of the last page
    printf("After copy : ");
    print_array(array + 0x20100, 20);
    printf("Destination: ");
    print_array(copy + 0x20100, 20);  // Triggers copy of the last page



    // test12
    printf("\nTEST13A->B and B->C dependency\n");

    random_array( C , 0x2000);

    printf("Before copy A: ");
    print_array(C, 20);

     printf("Before copy B: ");
     print_array(array, 20);


    delay_memcpy( array , C , 0x2000); // Copy No.1 A->B
    delay_memcpy( copy , array , 0x2000);  // Copy No.2 B->C
    copy[0]++; // Triggers copy

    printf("After copy A is: ");
    print_array(C, 20);


    printf("Destination C is: ");
    print_array(copy, 20);




    printf("\nTEST13A->B and B->C，C->D dependency\n");
    random_array( C , 0x2000);
    printf("Before copy A: ");
    print_array(C, 20);
    printf("Before copy B: ");
    print_array(array, 20);
    delay_memcpy( array , C , 0x2000); // Copy No.1 A->B
    delay_memcpy( copy , array , 0x2000);  // Copy No.2 B->C
    delay_memcpy( copy+0x2000 , copy , 0x2000);  // Copy No.2 C->D

    copy[0]++; // Triggers copy

    printf("After copy A is: ");
    print_array(C, 20);
    printf("Destination C is: ");
    print_array(copy+0x2000 , 20);


    printf("\nTEST14 A->B and C-> B dependency\n");

    random_array( array , 0x1500);
    random_array( copy , 0x1500);
    printf("Before copy A is:");
    print_array(array, 20);
    printf("Before copy C is:");
    print_array(copy, 20);
    delay_memcpy( C , array,  0x2500); // Copy No.1 A->B
    delay_memcpy( C , copy , 0x2500);  // Copy No.2 C->B


    printf("After copy A is : ");
    print_array(array, 20);

    printf("After copy C is : ");
    print_array(copy, 20);

    printf("Destination B: ");
    print_array(C, 20);

    //test 13
    printf("\nA->B and C-> A dependency\n");

    random_array( array , 0x20000);
    random_array( copy , 0x40000);

    printf("Before copy A is:");
    print_array(array, 20);

    printf("Before copy C is:");
    print_array(copy, 20);

    delay_memcpy( C, array , 0x3000); // Copy No.1 A->B
    delay_memcpy( array , copy , 0x3000);  // Copy No.2 C->A

    copy[0]++; // Triggers copy
    printf("After copy A is : ");
    print_array(array, 20);
    printf("After copy C is : ");
    print_array(copy, 20);

    printf("Destination B: ");
    print_array(array, 20);


    //test15
    printf("\nunbalanced case 15 when src,dst have different offset and src is at the beginning multiple page:\n");
    random_array( array , 0x20000);
    printf("Before copy is:");
    print_array(array, 20);
    delay_memcpy(copy+0x100 , array , 0x4000);
    printf("After copy A is : ");
    print_array(array, 20);
    printf("Destination B: ");
    print_array(copy+0x100, 20);
    printf("After copy A is : ");
    print_array(array+0x1000, 20);
    printf("Destination B: ");
    print_array(copy+0x1100, 20);
    printf("After copy A is : ");
    print_array(array+0x2100, 20);
    printf("Destination B: ");
    print_array(copy+0x2200, 20);

    printf("\nTEST16 A->B and endofB-> C dependency\n");

    random_array( array , 0x1500);

    printf("Before copy A is:");
    print_array(array, 20);

    delay_memcpy( C , array,  0x200); // Copy No.1 A->B
    delay_memcpy( copy , C+0x500 , 0x1500);  // Copy No.2 C->B

    array[0]++; // Triggers copy

    printf("After copy A is : ");
    print_array(array, 20);
    //check the length of pending copy
    printf("Destination: ");
    print_array(copy,20);


    printf("\nTEST13A->B and B->C，C-》A dependency\n");

    random_array( C , 0x1000);

    printf("Before copy A: ");
    print_array(C, 20);

    printf("Before copy B: ");
    print_array(array, 20);


    delay_memcpy( array , C , 0x1000); // Copy No.1 A->B
    delay_memcpy( copy , array , 0x1000);  // Copy No.2 B->C
    delay_memcpy( C , copy , 0x1000);
    //copy[0]++; // Triggers copy

    printf("After copy A is: ");
    print_array(C, 20);

    printf("Destination C is: ");
    print_array(copy, 20);


     printf("\n Actual Seg fault: Copying one page of data, trigger copy via write dst...\n");
    random_array(array, 0x1000);
    printf("Before copy: ");
    print_array(array, 20);

    delay_memcpy(NULL, array, 0x1000);
    printf("After copy : ");
    print_array(array, 20);
    printf("Destination: ");
    print_array(NULL, 20);

    

  return 0;
}
