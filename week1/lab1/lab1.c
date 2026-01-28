/*********************************************************************
 *
 * File header comments go here... including student name, class, date, 
 * assignment, description, etc.
 * Student Name: Badr Almadhi
 * Class CS6013
 * Jan 19
 * Lab1 Warmup Lab
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> // For strlen()
#include <assert.h>

/*********************************************************************
 *
 * The C functions in this lab use patterns and functionality often
 * found in operating system code. Your job is to implement them.
 * Additionally, write some test cases for each function (stick them in
 * functions [called xyzTests(), etc or similar]) Call your abcTests(),
 * etc functions in main().
 *
 * Write your own tests for each function you are testing, then
 * share/combine tests with a classmate.  Try to come up with tests that
 * will break people's code!  Easy tests don't catch many bugs! [Note
 * this is one specific case where you may share code directly with
 * another student.  The test function(s) from other students must be
 * clearly marked (add '_initials' to the function name) and commented
 * with the other student's name.
 *
 * Note: you may NOT use any global variables in your solution.
 * 
 * Errata:
 *   - You can use global vars in your testing functions (if necessary).
 *   - Don't worry about testing the free_list(), or the draw_me() functions.
 *
 * You must compile in C mode (not C++).  If you compile from the
 * commandline run clang, not clang++. Submit your solution files on
 * Canvas.
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 * 
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 * Ah, the joys of using bitwise operators!
 *
 * Hint: you may want to write helper functions for these two functions
 *
 *********************************************************************/

 /**
  * break down this 0x (0403deadbeef0201)
  * 0 (lowest) = 01
  * 1 = 02
  * 2 = ef 
  * 3 = be
  * 4 = ad
  * 5 = de
  * 6 = 03
  * 7 (highest) = 04
  */

  // function sort them smallest to highest retrun long with sorted bytes with lowest first

unsigned long byte_sort( unsigned long arg )
{

  // create an array that takes only 8 bytes
  unsigned char bytes[8];

  // using for loop to shift by 0 rightmost and 7 leftmost
  for (int i = 0; i < 8; i++){
    // arg >> (8*i) moves target down to the lowest position
    // & 0xFF keeps only the lowest 8 bits (one byte) and clear the rest
    bytes[i] = (arg >> (8 * i)) & 0xFF;
  }

  // sort 8 bytes using selection sort i position
  // compare j with min_index
  for (int i = 0; i < 8; i++){
    int min_index = i; 
    for (int j = i + 1; j < 8; j++){
      if (bytes[j] < bytes[min_index]){
        min_index = j;
      }
    }

    unsigned char temp = bytes[i];
    bytes[i] = bytes[min_index];
    bytes[min_index] = temp;
  }

    unsigned long result = 0;

    // why use ( |= not = ) using = will overwrite lose previous bytes, but |= add bytes while keeping existing bytes
    for (int i = 0; i < 8; i++)
    {
        result |= ((unsigned long)bytes[i]) << (8 * i);
    }
 

  return result;
}

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of 16 4-bit
 * numbers, and returns a new unsigned long integer containing the same nibbles,
 * sorted numerically, with smaller-valued nibbles towards the "small end" of
 * the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 * 
 * EXAMPLE: nibble_sort( 0x0403deadbeef0201 ) returns 0xfeeeddba43210000
 *
 *********************************************************************/

 // a nibble is 4 bits (half a byte )
 // 64-bit number contains 16 nibbles

unsigned long nibble_sort( unsigned long arg )
{
  // store 16 nibbles each nibble is 0-15
  unsigned char nibbles[16];

  for (int i = 0; i < 16; i++){
    nibbles[i] = (arg >> (4 * i)) & 0xF; 
  }

  for (int i = 0; i < 16; i++){
    int min_index = i; 
    for (int j = i + 1; j < 16; j++){
      if (nibbles[j] < nibbles[min_index]) {
        min_index = j;
      }
    }

      unsigned char temp = nibbles[i];
      nibbles[i] = nibbles[min_index];
      nibbles[min_index] = temp;
  }

  unsigned long result = 0; 
  for (int i = 0; i < 16; i++){
    result |= ((unsigned long)nibbles[i] << (4 * i));
  }

  return result;
}

/*********************************************************************/

// create a linked list on the heap pointer
// each node stores: 1- one character (val) 2- pointer to next node (link)
typedef struct elt {
  char val;
  struct elt *link;
} Elt;

/*********************************************************************/

/* Forward declaration of "free_list()"...
 *    This allows you to call free_list() in name_list() [if you'd like].
 */

void free_list( Elt* head ); // [No code goes here!]

/*********************************************************************
 *
 * name_list( name )
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the
 *   first letter of the passed in 'name'; the second element the second
 *   letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of the passed in name and its "link" field should be a
 *   null pointer.
 *
 * - each element must be dynamically allocated using a malloc() call
 * 
 * - you must use the "name" variable (change it to be your name).
 *
 * Note, since we're using C, not C++ you can't use new/delete!
 * The analog to delete is the free() function
 *
 * - if any call to malloc() fails, your function must return NULL and must
 *   also free any heap memory that has been allocated so far; that is, it
 *   must not leak memory when allocation fails.
 *
 * Implement print_list and free_list which should do what you expect.
 * Printing or freeing a nullptr should do nothing.
 *
 * Note: free_list() might be useful for error handling for name_list()... 
 *
 *********************************************************************/

 // this function uses the link list we create 
Elt *name_list( char * name )
{
  if (name == NULL) {
    return NULL;
  }

  // we use head and tail to organize memory
  Elt* head = NULL;
  Elt* tail = NULL;

  // loop through name start at 0 until end of name end of name '\0' to store each character
  for (int i = 0; name[i] != '\0'; i++){
    Elt *node = (Elt *)malloc(sizeof(Elt)); // reserve a space in RAM size of Elt = 16 bytes
    if (node == NULL) {
      free_list(head); // free everything allocated
      return NULL; 
    }

    // go in struct memory and access val
    node->val = name[i];
    node->link = NULL;

    if (head == NULL){
      head = node; 
      tail = node;
    } else {
      tail->link = node;
      tail = node;
    }

  }


   return head;
}

/*********************************************************************/

void print_list( Elt* head )
{
  Elt *curr = head;
  while(curr != NULL){
    printf("%c", curr->val);
    curr = curr->link;
  }
  printf("\n");
}

/*********************************************************************/

void free_list( Elt* head )
{
  while (head != NULL) {
    Elt *next = head->link;
    free(head);
    head = next;
  }
}

/*********************************************************************
 *
 * draw_me()
 *
 * This function creates a file called 'me.txt' which contains an ASCII-art
 * picture of you (it does not need to be very big).
 * 
 * Use the C stdlib functions: fopen, fclose, fprintf, etc which live in stdio.h
 * - Don't use C++ iostreams
 *
 *********************************************************************/

void draw_me()
{
  FILE *fp = fopen("me.txt", "w");
    if (fp == NULL) {
        perror("fopen");
        return;
    }

    fprintf(fp, "   ^_^   \n");
    fprintf(fp, "  (o o)  \n");
    fprintf(fp, "   \\_/   \n");
    fprintf(fp, "  /|_|\\  \n");
    fprintf(fp, "   / \\   \n");
    fprintf(fp, "  Badr   \n");

    fclose(fp);
} 

/*********************************************************************
 *
 * Test Code - Place your test functions in this section:
 *
 *     Remember, when testing name_list(), you should create a 'myName'
 *     variable and pass it in.
 */

void byteSortTests()
{
    unsigned long x1 = 0x0403deadbeef0201UL;
    unsigned long y1 = byte_sort(x1);
    assert(y1 == 0xefdebead04030201UL);

    unsigned long x2 = 0x0807060504030201UL;
    unsigned long y2 = byte_sort(x2);
    assert(y2 == 0x0807060504030201UL);

    unsigned long x3 = 0x0102030405060708UL;
    unsigned long y3 = byte_sort(x3);
    assert(y3 == 0x0807060504030201UL);

    unsigned long x4 = 0xAAAAAAAAAAAAAAAAUL;
    unsigned long y4 = byte_sort(x4);
    assert(y4 == 0xAAAAAAAAAAAAAAAAUL);
}

void nibbleSortTests()
{
    unsigned long x1 = 0x0403deadbeef0201UL;
    unsigned long y1 = nibble_sort(x1);
    assert(y1 == 0xfeeeddba43210000UL);

    unsigned long x2 = 0xFEDCBA9876543210UL;
    unsigned long y2 = nibble_sort(x2);
    assert(y2 == 0xFEDCBA9876543210UL);

    unsigned long x3 = 0x0123456789ABCDEFUL;
    unsigned long y3 = nibble_sort(x3);
    assert(y3 == 0xFEDCBA9876543210UL);

    unsigned long x4 = 0x1111111111111111UL;
    unsigned long y4 = nibble_sort(x4);
    assert(y4 == 0x1111111111111111UL);

    unsigned long x5 = 0x0UL;
    unsigned long y5 = nibble_sort(x5);
    assert(y5 == 0x0UL);
}

void nameListTests()
{
    // Normal string
    char name1[] = "Badr";
    Elt *h1 = name_list(name1);
    print_list(h1);      // should print: Badr
    free_list(h1);

    // Single character
    char name2[] = "Z";
    Elt *h2 = name_list(name2);
    print_list(h2);      // should print: Z
    free_list(h2);

    // With spaces and punctuation
    char name3[] = "Badr Almadhi!";
    Elt *h3 = name_list(name3);
    print_list(h3);      // should print: Badr Almadhi!
    free_list(h3);

    // Empty string (your implementation returns NULL — acceptable)
    char name4[] = "";
    Elt *h4 = name_list(name4);
    assert(h4 == NULL);

    // NULL input
    Elt *h5 = name_list(NULL);
    assert(h5 == NULL);

    // Safety checks (should not crash)
    print_list(NULL);
    free_list(NULL);
}


/*********************************************************************
 *
 * main()
 *
 * The main driver program.  You can place your main() method in this
 * file to make compilation easier, or have it in a separate file.
 *
 *********************************************************************/

int main()
{
  

    byteSortTests();
    nibbleSortTests();
    nameListTests();

    draw_me();
    return 0;

   
}
