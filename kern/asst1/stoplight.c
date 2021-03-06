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
int no_check = 1;
struct lock *lock_lefts;
struct lock *just_two;

// lock for printing vehicle info
struct lock *lock_print;

// lock/counter for thread completion check
struct lock *lock_threads; 
int threads_done = 0;


// lock/ints for truck priority check
struct lock *lock_truck_c_A;
struct lock *lock_truck_s_A;
struct lock *lock_truck_c_B;
struct lock *lock_truck_s_B;
struct lock *lock_truck_c_C;
struct lock *lock_truck_s_C;
int loop_A =0;
int loop_B =0;
int loop_C =0;


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
	char *turn = turn_directions[turndirection];

	lock_acquire(lock_print);

	kprintf("\n#: %-2lu  Action: %-11s  Type: %-5s  Direction: %-1c  Turn: %-5s  Dest.: %-1c  Location: %-2s", 
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
	just_two = lock_create("Just Two");

	// printing lock
	lock_print = lock_create("Lock Printing");

	// lock for thread completion check
	lock_threads = lock_create("Lock Threads");


	// locks for checking truck priority and stopping appropriately
	lock_truck_c_A = lock_create("Truck Ckeck A");
	lock_truck_s_A = lock_create("Truck Stop A");
	lock_truck_c_B = lock_create("Truck Ckeck B");
	lock_truck_s_B = lock_create("Truck Stop B");
	lock_truck_c_C = lock_create("Truck Ckeck C");
	lock_truck_s_C = lock_create("Truck Stop C");

	 	
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
	// kprintf("\nLeft attempt\n");

	/*
	 * Avoid unused variable warnings.
	 */

	(void) vehicledirection;
	(void) vehiclenumber;
	(void) vehicletype;


	struct lock *lock1 = get_lock_1(vehicledirection);
	struct lock *lock2 = left_lock_2(vehicledirection);
	struct lock *lock_cars;
	struct lock *lock_truck_check; //(3)
        struct lock *lock_truck_stop;  //(2)
	int *cars_count;
	int *in_loop;
	int i_hold_the_lock = 0;


	// get necessary locks/counters/ints for direction/type
	if(vehicledirection == 0) {
		// from A

		// define cars lock and counter for A
		lock_cars = lock_cars_A;
		cars_count = &cars_A;
	   	lock_truck_check = lock_truck_c_A;
       	lock_truck_stop =  lock_truck_s_A;
       	in_loop = &loop_A;	
	}
	else if (vehicledirection == 1) {
		// from B

		// define cars lock and counter for B
		lock_cars = lock_cars_B;
		cars_count = &cars_B;
		lock_truck_check = lock_truck_c_B;
       	lock_truck_stop =  lock_truck_s_B;
       	in_loop = &loop_B;
	}
	else {
		// from C

		// define cars lock and counter for C
		lock_cars = lock_cars_C;
		cars_count = &cars_C;
		lock_truck_check = lock_truck_c_C;
       	lock_truck_stop =  lock_truck_s_C;
       	in_loop = &loop_C;
	}


	//****** Truck/car check priority to cars ******//

	lock_acquire(lock_cars);
        if(vehicletype == 0)
                (*cars_count)++;  // cars count, trucks sholud yield in presence of cars

	lock_release(lock_cars);

        lock_acquire(lock_truck_check);   // truck/car check begins 

        if(vehicletype==0){
                lock_release(lock_truck_check);  //It's a car! It can continue to intersection 

	}else{ // It's a truck, it must wait if cars are present
               int second = 0;
               if(*in_loop ){// a truck is in the loop that means cars are present the truck will be lock, it must release the truck/car check point 
                        second = 1;
                        lock_release(lock_truck_check); // release of truck/car check                       
               }

                lock_acquire(lock_truck_stop);// truck stop if cars are present 

                if(second)
                          lock_acquire(lock_truck_check);

                int not_first = 0;

                while(1){// one truck loops until cars are no longer present  

                         *in_loop = 1;

                         if(not_first)
                                lock_acquire(lock_truck_check);//

                         lock_acquire(lock_cars); // 

                         if(*cars_count == 0){// no cars present truck can continue
                                *in_loop =0;
                                break;
                        }
                        
                        lock_release(lock_cars); // 
                        lock_release(lock_truck_check);//
                        not_first = 1;
                }
                lock_release(lock_cars); //
                lock_release(lock_truck_stop);//
                lock_release(lock_truck_check);//
        }


 //*********** This avoids a 3-way deadlock, only two threads or less allowed to procede at the same time **********//

	lock_acquire(lock_lefts);//A
		lefts_count++;

	if(no_check){// this thread gets to proceed if it is the first tread or if the previous thread that got to proceed without a lock has exited
           	no_check = 0;
	  
	}else if(lefts_count == 2 ){
		lock_acquire(just_two); // thread allowed in with lock
		i_hold_the_lock =1;
	}else if (lefts_count > 2 ){ // thread will be stop on lock
		lock_release(lock_lefts);
                lock_acquire(just_two); /////////////////////////////////
		i_hold_the_lock  =1;
		lock_acquire(lock_lefts);
	} 

	lock_release(lock_lefts);//R

//****************************************************************************//

		//*********** wait on intersection ************//

		// enter first section
		lock_acquire(lock1);

		print_vehicle("Entered (1)", vehicledirection,
   			vehiclenumber, vehicletype, 0, lock1->name);

	//	lock_release(lock1);// I think this has to be inside lock2

		// enter second section
		lock_acquire(lock2);
		lock_release(lock1);// here
		print_vehicle("Entered (2)", vehicledirection,
            vehiclenumber, vehicletype, 0, lock2->name);	

		lock_acquire(lock_lefts);
		lefts_count--;

               
		if(i_hold_the_lock){
			lock_release(just_two);// allow another thread in from the lock if any
		}else{
           		no_check = 1; //allows a second thread in without lock
		}

		lock_release(lock_lefts);

		lock_release(lock2);

		// leaves intersection
		char dest[] = {get_dest(vehicledirection, 0), '\0'};

		print_vehicle("Exited", vehicledirection,
            vehiclenumber, vehicletype, 0, dest);

		// decrease car count if applicable 
		if(vehicletype==0){
			lock_acquire(lock_cars);
			(*cars_count)--;
			lock_release(lock_cars);
		}
	
		// another completed thread
		lock_acquire(lock_threads);
        threads_done++;
        lock_release(lock_threads);
	
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
	struct lock *lock_truck_check; //(3)
	struct lock *lock_truck_stop;  //(2)
	int *cars_count;
	int *in_loop;

	// get necessary locks/counters/ints for direction and type	
	if(vehicledirection == 0) {
		// from A	

		lock_cars = lock_cars_A;
		cars_count = &cars_A;
		lock_truck_check = lock_truck_c_A;
		lock_truck_stop =  lock_truck_s_A;
		in_loop = &loop_A;
	}
	if(vehicledirection == 1) {
    	// from B
	 
		lock_cars = lock_cars_B;
      		cars_count = &cars_B;
		lock_truck_check = lock_truck_c_B;
                lock_truck_stop =  lock_truck_s_B;
		in_loop = &loop_B;
    }
	else {
    	// from C 
	
	 	lock_cars = lock_cars_C;
        	cars_count = &cars_C;
		lock_truck_check = lock_truck_c_C;
                lock_truck_stop =  lock_truck_s_C;
		in_loop = &loop_C;
    }



    //****** Truck/car check priority to cars ******//
	
         lock_acquire(lock_cars);                
         if(vehicletype == 0) 
		(*cars_count)++;  // cars count, trucks sholud yield in presence of cars

	 lock_release(lock_cars);
	
  	lock_acquire(lock_truck_check);   // truck/car check begins 

	if(vehicletype==0){ 
		lock_release(lock_truck_check);  //It's a car! It can continue to intersection 

	}else{ // It's a truck, it must wait if cars are present
               int second = 0;
               if(*in_loop ){// a truck is in the loop that means cars are present the truck will be lock, it must release the truck/car check point 
			second = 1;
			lock_release(lock_truck_check); // release of truck/car check                       
               }

   		lock_acquire(lock_truck_stop);// truck stop if cars are present 
                
		if(second)      
                          lock_acquire(lock_truck_check);  
                
 		int not_first = 0;

 		while(1){// one truck loops until cars are no longer present  
         
                        *in_loop = 1;   

  			if(not_first)
             			lock_acquire(lock_truck_check);//

			lock_acquire(lock_cars); // 
		         
			if(*cars_count == 0){// no cars present truck can continue
				*in_loop =0;	
				break; 	
			}

			lock_release(lock_cars); // 
                	lock_release(lock_truck_check);//
			not_first = 1;
                        

		}
		lock_release(lock_cars); //
		lock_release(lock_truck_stop);//
		lock_release(lock_truck_check);//
	
	}  
		
		//******** wait on intersection ***********//
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

		// leaves intersection
		char dest[] = {get_dest(vehicledirection, 1), '\0'};

		print_vehicle("Exited", vehicledirection,
            vehiclenumber, vehicletype, 1, dest);

		// another thread done
		lock_acquire(lock_threads);
        threads_done++;
        lock_release(lock_threads);


			 
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
	
	
/*	// NOT RANDOM
	vehicledirection = random() % 3;// 
	turndirection = 0;// random() % 2;
	vehicletype = random() % 2;
*/
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

		
        if (threads_done == NVEHICLES)
                kprintf("\n\nIntersection is empty!!! Test completed.\n\n");                  
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
	threads_done = 0;


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


	while(1) {// avoids premature ending of thread
               

                lock_acquire(lock_threads);
                if(threads_done == NVEHICLES) {
                        lock_release(lock_threads);
                        break;
                }
                lock_release(lock_threads);

        }

	// destroy locks
		lock_destroy(lock_AB);
		lock_destroy(lock_BC);
		lock_destroy(lock_CA);  
		lock_destroy(lock_cars_A);
		lock_destroy(lock_cars_B);
		lock_destroy(lock_cars_C);
		lock_destroy(lock_lefts);
		lock_destroy(lock_truck_c_A);
		lock_destroy(lock_truck_s_A);
		lock_destroy(lock_truck_c_B);
		lock_destroy(lock_truck_s_B);
		lock_destroy(lock_truck_c_C);
		lock_destroy(lock_truck_s_C);
		lock_destroy(lock_threads);
		lock_destroy(just_two);
		lock_destroy(lock_print);

	return 0;
}








