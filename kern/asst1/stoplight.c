/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use any synchronization primitives available to solve
 * the stoplight problem in this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>

#include <synch.h>


/*
 *
 * Constants
 *
 */

/*
 * Number of vehicles created.
 */

#define NVEHICLES 20


/*
 * Car parameters meanings:
 * 
 * vehicletype [0,1]
 *	- 0: car
 *	- 1: truck
 *
 * vehicledirection [0,2]
 *	- 0: A
 *	- 1: B
 *	- 2: C
 *
 * turndirection [0,1]
 *  - 0: left
 *  - 1: right
 *
 */


/*
 *
 * Function Definitions
 *
 */


/*
 * START of added variables/functions
 */

// variables

// to convert int to string/char
char car_types[2][6] = {"Car", "Truck"};
char car_directions[] = "ABC";
char turn_directions[2][6] = {"Left", "Right"};

// section locks
static struct lock *lock_AB;
static struct lock *lock_BC;
static struct lock *lock_CA;

// car count locks and ints
int cars_A = 0;
int cars_B = 0;
int cars_C = 0;
struct lock *lock_cars_A;
struct lock *lock_cars_B;
struct lock *lock_cars_C;

// left turn counts and ints
int lefts_count = 0;
struct lock *lock_lefts;

// lock for printing vehicle info
struct lock *lock_print;

// lock and counter for completed threads
int threads_done = 0;
struct lock *lock_threads;


// function prototypes

void print_vehicle(const char *action, unsigned long vehicledirection,
	unsigned long vehiclenumber, unsigned long vehicletype,
	unsigned long turndirection, char *location);
char get_dest(unsigned long vehicledirection, unsigned long turndirection);
void stoplight_init();
struct lock *get_lock_1(unsigned long vehicledirection);
struct lock *left_lock_2(unsigned long vehicledirection);
void print_synch(const char *text);
void print_threads_done();

// functions

// Prints all necessary vehicle info
void print_vehicle(const char *action, unsigned long vehicledirection,
    unsigned long vehiclenumber, unsigned long vehicletype,
	unsigned long turndirection, char *location) {
	char direction = car_directions[vehicledirection];
	//char *turn = turn_directions[turndirection];
	char *type = car_types[vehicletype];
	char dest = get_dest(vehicledirection, turndirection);
	char *turn = turn_directions[turndirection];

	lock_acquire(lock_print);

	kprintf("\n#: %lu, Action: %s, Type: %s, Direction: %c, Turn: %s, Dest.: %c, Location: %s\n", 
		vehiclenumber, action, type, direction, turn, dest, location);

	lock_release(lock_print);
}

// Gets char of destination from given direction and turn values
char get_dest(unsigned long vehicledirection, unsigned long turndirection) {
	int idx = turndirection==0 ? vehicledirection-1 : vehicledirection+1;
	char dest;

	// find char result
	if(idx > 2) dest = 'A';
	else if(idx < 0) dest = 'C';
	else dest = car_directions[idx];

	return dest;
}

// initialize program globals
void stoplight_init() {
	// section locks
	lock_AB = lock_create("AB");
	lock_BC = lock_create("BC");
	lock_CA = lock_create("CA");

	// car count locks
	lock_cars_A = lock_create("Lock Cars A");
	lock_cars_B = lock_create("Lock Cars B");
	lock_cars_C = lock_create("Lock Cars C");

	// left turn counts
	lock_lefts = lock_create("Lock Lefts");

	// printing lock
	lock_print = lock_create("Lock Printing");

	// lock for thread completion check
	lock_threads = lock_create("Lock Threads");
	
	threads_done = 0;
	cars_A = 0;
	cars_B = 0;
	cars_C = 0;
	lefts_count = 0;
}

// Get first lock needed for left-turn from vehcledirection OR
// 		Get lock needed for right-turn
struct lock *get_lock_1(unsigned long vehicledirection){
	if(vehicledirection == 0) {
		// from A
		return lock_AB;
	}
	else if(vehicledirection == 1) {
		// from B
		return lock_BC;
	}
	else {
		// from C
		return lock_CA;
	}
}

// Get second lock needed for left-turn from vehcledirection
struct lock *left_lock_2(unsigned long vehicledirection){
    if(vehicledirection == 0) {
        // from A
        return lock_BC;
    }
    else if(vehicledirection == 1) {
        // from B
        return lock_CA;
    }
    else {
        // from C
        return lock_AB;
    }
}

// synchronized printing to terminal
void print_synch(const char *text) {
	lock_acquire(lock_print);
	kprintf("%s", text);
	lock_release(lock_print);
}

// prints number of threads currently completed
void print_threads_done() {
	lock_acquire(lock_print);
	lock_acquire(lock_threads);
	kprintf("THREADS: %d\n", threads_done);
	lock_release(lock_threads);
	lock_release(lock_print);
}


/*
 * END of added variables/functions
 */


/*
 * turnleft()
 *
 * Arguments:
 *      unsigned long vehicledirection: the direction from which the vehicle
 *              approaches the intersection.
 *      unsigned long vehiclenumber: the vehicle id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnleft(unsigned long vehicledirection,
		unsigned long vehiclenumber,
		unsigned long vehicletype)
{
	// kprintf("\nLeft attempt\n");

	/*
	 * Avoid unused variable warnings.
	 */

	(void) vehicledirection;
	(void) vehiclenumber;
	(void) vehicletype;

	// ADDED

	struct lock *lock1 = get_lock_1(vehicledirection);
	struct lock *lock2 = left_lock_2(vehicledirection);
	struct lock *lock_cars;
	int *cars_count;


	// check direction, define corresponding locks/counters
	if(vehicledirection == 0) {
		// from A

		// define cars lock and counter for A
		lock_cars = lock_cars_A;
		cars_count = &cars_A;
	}
	else if (vehicledirection == 1) {
		// from B

		// define cars lock and counter for B
		lock_cars = lock_cars_B;
		cars_count = &cars_B;
	}
	else {
		// from C

		// define cars lock and counter for C
		lock_cars = lock_cars_C;
		cars_count = &cars_C;
	}


	// continue until vehicle enters intersection and completes route
	int has_entered = 0;
	while(has_entered == 0) {
		//print_synch("LEFT\n");
		// check if able to enter		
		lock_acquire(lock_lefts);
		lock_acquire(lock_cars);
		if(lefts_count>=2 || !(vehicletype==0 || (*cars_count)==0)) {
			// retry
			lock_release(lock_cars);
			lock_release(lock_lefts);
<<<<<<< HEAD
	
			/*lock_acquire(lock_print);
			kprintf("DEBUG -- lefts=%d, type=%lu, cars=%d\n", lefts_count, vehicletype, 
				*cars_count);
			lock_release(lock_print);*/

			//thread_yield();
=======

>>>>>>> parent of 62f92f2... minor print update, still error present with OS printing during runtime
			continue;
		}
		else {
			// success
			if(vehicletype==0) {
				(*cars_count)++;
			}
			lefts_count++;

			lock_release(lock_cars);
			lock_release(lock_lefts);
		}


		// entered intersection
		// wait on required locks

		// enter first section
		lock_acquire(lock1);

		print_vehicle("Entered", vehicledirection,
   			vehiclenumber, vehicletype, 0, lock1->name);

		lock_release(lock1);
		
		// enter second section
		lock_acquire(lock2);
		print_vehicle("Entered", vehicledirection,
            vehiclenumber, vehicletype, 0, lock2->name);
		lock_release(lock2);


		// leaves
		char dest[] = {get_dest(vehicledirection, 0), '\0'};

		//kprintf("\nDEST: %s\n", dest);

		print_vehicle("Exited", vehicledirection,
            vehiclenumber, vehicletype, 0, dest);


		// decrease counts
		lock_acquire(lock_lefts);
		lock_acquire(lock_cars);

		if(vehicletype==0) {
			(*cars_count)--;
		}
		lefts_count--;

		lock_release(lock_cars);
		lock_release(lock_lefts);

		// success, exit loop
		has_entered = 1;
	}
}


/*
 * turnright()
 *
 * Arguments:
 *      unsigned long vehicledirection: the direction from which the vehicle
 *              approaches the intersection.
 *      unsigned long vehiclenumber: the vehicle id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnright(unsigned long vehicledirection,
		unsigned long vehiclenumber,
		unsigned long vehicletype)
{
	//kprintf("\nRight attempt\n");
	
	/*
	 * Avoid unused variable warnings.
	 */

	(void) vehicledirection;
	(void) vehiclenumber;
	(void) vehicletype;

	// ADDED
	
	struct lock *lock1 = get_lock_1(vehicledirection);
	struct lock *lock_cars;
	int *cars_count;

	// get cars lock and counter for direction	
	if(vehicledirection == 0) {
		// from A	

		lock_cars = lock_cars_A;
		cars_count = &cars_A;
	}
	if(vehicledirection == 1) {
    	// from B
	 
		lock_cars = lock_cars_B;
      	cars_count = &cars_B;
    }
	else {
    	// from C 
	
	 	lock_cars = lock_cars_C;
        cars_count = &cars_C;
    }


	int has_entered = 0;
	while(has_entered == 0) {
		//print_synch("RIGHT\n");
		// check if able to enter		
		lock_acquire(lock_cars);
		if(!(vehicletype==0 || (*cars_count)==0)) {
			// retry
						
			lock_release(lock_cars);
			continue;
		}
		else {
			// success
		
			if(vehicletype==0) (*cars_count)++;	
			lock_release(lock_cars);
		}
		
		
		// wait on section
		lock_acquire(lock1);

		// decrease cars_count at direction if car
		if(vehicletype == 0) { 
			lock_acquire(lock_cars);
        	(*cars_count)--;
        	lock_release(lock_cars);
		}
	
		print_vehicle("Entered", vehicledirection,
			vehiclenumber, vehicletype, 1, lock1->name);
	
		lock_release(lock1);	

		// leaves
		char dest[] = {get_dest(vehicledirection, 1), '\0'};
        
		//kprintf("\nDEST: %s\n", dest);

		print_vehicle("Exited", vehicledirection,
            vehiclenumber, vehicletype, 1, dest);
			
		// success, exit loop
		has_entered = 1;
	}
}


/*
 * approachintersection()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long vehiclenumber: holds vehicle id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by createvehicles().  Each one must choose a direction
 *      randomly, approach the intersection, choose a turn randomly, and then
 *      complete that turn.  The code to choose a direction randomly is
 *      provided, the rest is left to you to implement.  Making a turn
 *      or going straight should be done by calling one of the functions
 *      above.
 */

static
void
approachintersection(void * unusedpointer,
		unsigned long vehiclenumber)
{
	int vehicledirection, turndirection, vehicletype;

	/*
	 * Avoid unused variable and function warnings.
	 */

	(void) unusedpointer;
	(void) vehiclenumber;
	(void) turnleft;
	(void) turnright;

	/*
	 * vehicledirection is set randomly.
	 */

	vehicledirection = random() % 3;
	turndirection = random() % 2;
	vehicletype = random() % 2;
	

	
	// NOT RANDOM
	//vehicledirection = 0;// random() % 3;
	//turndirection = 0;// random() % 2;
	//vehicletype = 0;//random() % 2;
	

	// ADDED

	char start[] = {car_directions[vehicledirection], '\0'};
	print_vehicle("Approached", vehicledirection, vehiclenumber, 
		vehicletype, turndirection, start);

	if(turndirection == 0) {
		// turn left
		turnleft(vehicledirection, vehiclenumber, vehicletype);	
	}
	else {
		// turn right
		turnright(vehicledirection, vehiclenumber, vehicletype);
	}
	
	// thread done, increment count
	lock_acquire(lock_threads);
	threads_done++;
	lock_release(lock_threads);

	print_threads_done();

	// wait for all threads to finish
	/*while(1) {
		lock_acquire(lock_threads);
		if(threads_done >= NVEHICLES)
			break;
		lock_release(lock_threads);
	}*/
}


/*
 * createvehicles()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modiy this code as necessary for your solution.
 */

int
createvehicles(int nargs,
		char ** args)
{
	int index, error;

	// Added
	
	stoplight_init();
	
	// Added


	/*
	 * Avoid unused variable warnings.
	 */

	(void) nargs;
	(void) args;

	/*
	 * Start NVEHICLES approachintersection() threads.
	 */

	for (index = 0; index < NVEHICLES; index++) {

		error = thread_fork("approachintersection thread",
				NULL,
				index,
				approachintersection,
				NULL
				);

		/*
		 * panic() on error.
		 */

		if (error) {

			panic("approachintersection: thread_fork failed: %s\n",
					strerror(error)
				 );
		}
	}

	// ADDED

	// wait for all threads to finish
	while(1) {
		print_synch("COMPLETION CHECK\n");
		print_threads_done();	
	
		lock_acquire(lock_threads);
		if(threads_done >= NVEHICLES) {
			lock_release(lock_threads);
			break;
		}
		lock_release(lock_threads);

		print_synch("NOT COMPLETED\n");
	}

	
	// destroy locks
	/*lock_destroy(lock_AB);
	lock_destroy(lock_BC);
	lock_destroy(lock_CA);	
	lock_destroy(lock_cars_A);
	lock_destroy(lock_cars_B);
	lock_destroy(lock_cars_C);
	lock_destroy(lock_lefts);*/


	return 0;
}












