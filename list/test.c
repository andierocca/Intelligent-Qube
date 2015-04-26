#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//to test valgrind
//valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./test

/* Here we are going to write some functions to support a linked list that stores
 * Person data (name, age)
 */
typedef struct person_t
{
    char* name;
    int age;
} Person;

/* Example functions given to you. If you want to truly grasp these concepts, try
 * writing your own structs and functions as well!
 */

// Create a new Person
Person* create_person(const char* name, int age)
{
    Person* p = (Person*) malloc(sizeof(Person));
    p->name = strdup(name); // Uses malloc!
    p->age = age;
    return p;
}

// Make a deep copy of a Person
void* copy_person(const void* data)
{
    Person *p = (Person*) data;
    return create_person(p->name, p->age);
}

// Print a Person
void print_person(void* data)
{
    Person *p = (Person*) data;
    printf("%s, %d\n", p->name, p->age);
}

// Free a Person
void free_person(void* data)
{
    // This is safe because we should only be passing in Person struct pointers
    Person *p = (Person*) data;
    // free any malloc'd pointers contained in the Person struct (just name)
    free(p->name);
    // Now free the struct itself; this takes care of non-malloc'd data, like age.
    free(p);
}

// Return 1 if the person's name is 8+ characters long
int long_name(const void *data)
{
	Person *p = (Person*) data;
	return strlen(p->name) > 7;
}

/* Account struct
 * Account data (holder, savings, checkings)
 */
typedef struct account_t
{
  char* holder;
  int savings;
  int checkings;
} Account;

// Create a new Account
Account* create_account(const char* holder, int savings, int checkings)
{
  Account* a = (Account*) malloc(sizeof(Account));
  a->holder = strdup(holder);
  a->savings = savings;
  a->checkings  = checkings;
  return a;
}

// Make a deep copy of an Account
void* copy_account(const void* data)
{
  Account *a = (Account*) data;
  return create_account(a->holder, a->savings, a->checkings);
}

// Print an account
void print_account(void* data)
{
  Account *a = (Account*) data;
  printf("\nAccount Holder %s\nSavings $%d\nCheckings $%d\n",
  a->holder, a->savings, a->checkings);
}

// Free an Account
void free_account(void* data)
{
  Account *a = (Account*) data;
  // free any malloc'd pointers contained in the struct
  free(a->holder);
  // Now free the struct itself; this takes care of non-malloc'd data
  free(a);
}

//Predicate to determine if in debt
int in_debt(const void *data)
{
  Account *a = (Account*) data;
  return (a->savings < 0);
}

//Buy a car
void purchase_car(void* data)
{
  Account *a = (Account*) data;
  a->checkings -= 4000;
  if (a->checkings < 0) {
    a->savings += a->checkings;
    a->checkings = 0;
  }
}

/* This main function does a little testing
   Like all good CS Majors you should test
   your code here. There is no substitute for testing
   and you should be sure to test for all edge cases
   e.g., calling remove_front on an empty list.
*/
int main(void)
{
	/* Now to make use of all of this stuff */
	list* llist = create_list();

  /* What does an empty list contain?  Lets use our handy traversal function */
  printf("TEST CASE 1\nAn Empty list should print nothing here:\n");
  traverse(llist, print_person);
  printf("\n");

  /* Lets add a person and then print */
  push_front(llist, create_person("Andrew", 24));
  printf("TEST CASE 2\nA List with one person should print that person:\n");
  traverse(llist, print_person);
  printf("\n");

  /* Lets remove that person and then print */
  remove_front(llist, free_person);
  printf("TEST CASE 3\nAnother Empty list should print nothing here:\n");
  traverse(llist, print_person);
  printf("\n");

  /* Lets add two people and then print */
  push_front(llist, create_person("Nick", 22));
  push_front(llist, create_person("Randal", 21));
  printf("TEST CASE 4\nA List with two people should print those two people:\n");
  traverse(llist, print_person);
  printf("\n");

  /* Lets copy this list */
  list* llist2 = copy_list(llist, copy_person);
  printf("TEST CASE 5\nA copied list should print out the same two people:\n");
  traverse(llist2, print_person);
  printf("\n");

    /* Lets kill the list */
  empty_list(llist, free_person);
  printf("TEST CASE 6\nAfter freeing all nodes the list should be empty:\n");
  traverse(llist, print_person);
  printf("\n");

  /* Let's make a list of people, and remove certain ones! */
  /* Should remove anyone whose name is 8+ characters long */
  push_front(llist, create_person("Josephine", 27));
  push_front(llist, create_person("Dave", 34));
  push_front(llist, create_person("Benjamin", 23));
  push_front(llist, create_person("Lisa", 41));
  push_front(llist, create_person("Maximilian", 24));
  remove_if(llist, long_name, free_person);
  printf("TEST CASE 7\nShould only print 2 people with short names:\n");
  traverse(llist, print_person);

  /* YOU ARE REQUIRED TO MAKE MORE TEST CASES THAN THE ONES PROVIDED HERE */
  /* You will get points off if you do not you should at least test each function here */

  /* Testing over clean up*/
  empty_list(llist, free_person);
  free(llist);
  empty_list(llist2, free_person);
  free(llist2);

  printf("\nTESTING CASES FOR ACCOUNT STRUCT\n");

  list* alist = create_list();

  printf("\nTEST CASE 1\nAdd some accounts. Should print 2.\n");
  push_back(alist, create_account("John Smith", 20000, 5000));
  push_back(alist, create_account("Catherine Johnson", 4000, 2000));
  traverse(alist, print_account);
  printf("\nsize: %d\n", size(alist));

  printf("\nTEST CASE 2\nEmpty accounts. Should print nothing.\n");
  empty_list(alist, free_account);
  traverse(alist, print_account);
  printf("\nsize: %d\n", size(alist));
  int empty = is_empty(alist);
  printf ("Account list is empty? %d\n", empty);

  printf("\nTEST CASE 3\nAdd some more accounts. Should print 7 accounts.\n");
  push_back(alist, create_account("Sally Garcia", 5000, 2000));
  push_back(alist, create_account("Doug Brown", 1000, 0));
  push_back(alist, create_account("Greg Moore", 10000, 5000));
  push_back(alist, create_account("Herman Clark", 2000, 500));
  push_back(alist, create_account("Allison Harris", 15000, 6000));
  push_back(alist, create_account("Tom Anderson", 3000, 1000));
  push_back(alist, create_account("Bob Martin", 2300, 1500));
  traverse(alist, print_account);
  printf("\nsize: %d\n", size(alist));
  empty = is_empty(alist);
  printf ("Account list is empty? %d\n", empty);

  printf("\nTEST CASE 3.5\nRemove Bob and Sally. Should print 5 accounts.\n");
  remove_back(alist, free_account);
  remove_front(alist, free_account);
  traverse(alist, print_account);
  printf("size: %d\n", size(alist));

  printf("\nTEST CASE 4\nEveryone buys a car. Print account status after.\n");
  traverse(alist, purchase_car);
  traverse(alist, print_account);

  printf("\nTEST CASE 5\nRemove accounts if in debt. Should print 3 accounts\n");
  remove_if(alist, in_debt, free_account);
  traverse(alist, print_account);
  printf("\nsize: %d\n", size(alist));

  printf("\nTEST CASE 6\nGet first and last accounts\n");
  print_account(front(alist));
  print_account(back(alist));

  printf("\nTEST CASE 7\nAdd another account to the front and print front\n");
  push_front(alist, create_account("Sarah White", 4000, 3000));
  print_account(front(alist));

  printf("\nTEST CASE 8\nCopy account list and remove Sarah from the original\n");
  list* alist2 = copy_list(alist, copy_account);
  remove_front(alist, free_account);
  printf("\nOriginal list (Should not have Sarah)\n");
  traverse(alist, print_account);
  printf("\nsize: %d\n", size(alist));
  printf("\nCopied list (Should have Sarah)\n");
  traverse(alist2, print_account);
  printf("\nsize: %d\n", size(alist2));


  empty_list(alist, free_person);
  free(alist);
  empty_list(alist2, free_person);
  free(alist2);
  return 0;
}

