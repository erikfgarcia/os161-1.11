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

char car_types[2][6] = {"Car", "Truck"};
char car_directions[] = "ABC";
char turn_directions[2][6] = {"Left", "Right"};


// function prototypes

void print_vehicle(char *action, unsigned long vehicledirection,
	unsigned long vehiclenumber, unsigned long vehicletype,
	unsigned long turndirection, char *location);
char get_dest(unsigned long vehicledirection, unsigned long turndirection);
void stoplight_init();

// functions

// Prints all necessary vehicle info
void print_vehicle(char *action, unsigned long vehicledirection,
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
	/*car_types[0] = "Car";
	car_types[1] = "Truck";

	car_directions = "ABC";

	turn_directions[0] = "Left";
	turn_directions[1] = "Right";*/
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

	return 0;
}












