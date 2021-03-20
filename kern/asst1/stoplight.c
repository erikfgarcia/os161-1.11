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


// function prototypes

void print_vehicle(const char *action, unsigned long vehicledirection,
	unsigned long vehiclenumber, unsigned long vehicletype,
	unsigned long turndirection, char *location);
char get_dest(unsigned long vehicledirection, unsigned long turndirection);
void stoplight_init();
struct lock *get_lock_1(unsigned long vehicledirection);
struct lock *left_lock_2(unsigned long vehicledirection);


// functions

// Prints all necessary vehicle info
void print_vehicle(const char *action, unsigned long vehicledirection,
    unsigned long vehiclenumber, unsigned long vehicletype,
	unsigned long turndirection, char *location) {
	char direction = car_directions[vehicledirection];
	//char *turn = turn_directions[turndirection];
	char *type = car_types[vehicletype];
	char dest = get_dest(vehicledirection, turndirection);

	kprintf("#: %lu, Action: %s, Type: %s, Direction: %c, Dest.: %c \
		Location: %s\n", vehiclenumber, action, type, direction,
		dest, location);
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
		lock_cars = lock_cars_A;
		cars_count = &cars_B;
	}
	else {
		// from C

		// define cars lock and counter for C
		lock_cars = lock_cars_B;
		cars_count = &cars_C;
	}


	// continue until vehicle enters intersection and completes route
	int has_entered = 0;
	while(has_entered == 0) {
		// prevent left-turn deadlock, only allow 2 lefts
		lock_acquire(lock_lefts);
		if(lefts_count > 1) {
			lock_release(lock_lefts);
			thread_yield();
			continue;
		}
		else {
			lefts_count++;
			lock_release(lock_lefts);
		}

		// special actions for trucks
		lock_acquire(lock_cars);
		if(vehicletype == 1) {
			// is a truck, check car_count for direction
			if((*cars_count) > 0){
				// truck waits on cars, retry
				lock_release(lock_cars);
				thread_yield();
				continue;
			}

			// truck enters
			lock_release(lock_cars);
		}
		else {
			// increment cars count for direction
			(*cars_count)++;
			lock_release(lock_cars);
		}

		// entered intersection
		// wait on required locks

		// enter first section
		lock_acquire(lock1);

		// decrease cars_count at direction if car
		if(vehicletype == 0) {
			lock_acquire(lock_cars);
			(*cars_count)--;
			lock_release(lock_cars);
		}

		print_vehicle("Entered", vehicledirection,
   			vehiclenumber, vehicletype, 0, lock1->name);
		lock_release(lock1);
		
		// enter second section
		lock_acquire(lock2);
		print_vehicle("Entered", vehicledirection,
            vehiclenumber, vehicletype, 0, lock2->name);
		lock_release(lock2);

		// leaves
		char dest[] = {get_dest(vehicledirection, 0)};
		print_vehicle("Exited", vehicledirection,
            vehiclenumber, vehicletype, 0, dest);

		// decrease lefts_count
		lock_acquire(lock_lefts);
		lefts_count--;
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
	if(vehicledirection == 0) {
    	// from C 
	
	 	lock_cars = lock_cars_C;
        cars_count = &cars_C;
    }


	int has_entered = 0;
	while(has_entered == 0) {
		// special actions for trucks
		lock_acquire(lock_cars);
		if(vehicletype == 1) {
			// is a truck, check car_count for direction
		//	lock_acquire(lock_cars); // no this will lock_acquire more than one 
			if((*cars_count) > 0){
				// truck waits on cars, retry
				lock_release(lock_cars);
				thread_yield();
				continue;
			}

			// truck enters
			lock_release(lock_cars);
		}
		else {
			// increment cars count for direction
			(*cars_count)++;
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
		char dest[] = {get_dest(vehicledirection, 1)};
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

	vehicledirection = 0;// random() % 3;
	turndirection = 1;// random() % 2;
	vehicletype = 0;//random() % 2;
	// I'm trying only cars turning rigth coming from A


	// ADDED

	char start[] = {car_directions[vehicledirection]};
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
	
	// destroy locks
/*	lock_destroy(lock_AB);  // don't do this some threads will destroy the lock when
 *	                         //the lock still need it for other threads 
	lock_destroy(lock_BC);
	lock_destroy(lock_CA);	
	lock_destroy(lock_cars_A);
	lock_destroy(lock_cars_B);
	lock_destroy(lock_cars_C);

*/
	return 0;
}












