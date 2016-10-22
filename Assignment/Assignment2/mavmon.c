// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "train.h"

void trainArrives( uint32_t train_id, enum TRAIN_DIRECTION train_direction );
void trainCross  ( uint32_t train_id, enum TRAIN_DIRECTION train_direction );
void trainLeaves ( uint32_t train_id, enum TRAIN_DIRECTION train_direction );

// Current time of day in seconds since midnight
int32_t  current_time;
uint32_t clock_tick;

// Current train in the intersection
uint32_t in_intersection;
struct train_struct {
  int id;
  enum TRAIN_DIRECTION direction;
};


int train_count[NUM_DIRECTIONS];
int count [NUM_DIRECTIONS];
struct train_struct *ts;

//conditional variables & mutexes
pthread_cond_t cond[NUM_DIRECTIONS]; 
pthread_mutex_t intersection_mutex;
pthread_mutex_t direction_mutex[NUM_DIRECTIONS];

//Return 1 if in_intersection=-1, 0 otherwise
int isIntersectionEmpty() {
  return in_intersection == INTERSECTION_EMPTY;  
}

/*given a direction, increase its count on how many consecutive trains have
crossed the intersection from that direction
*/
void directionCount(enum TRAIN_DIRECTION dir){
   count[dir]++;
}

void * trainLogic( void * val ){
  struct train_struct *ts = val;
  int id = ts->id;
  enum TRAIN_DIRECTION direction = ts->direction;
 
  //pthread_cond_wait(&cond[direction], &direction_mutex[direction]);
  pthread_mutex_lock(&intersection_mutex);
  trainCross(id,direction);
  pthread_mutex_unlock(&intersection_mutex);

  free (ts);
  fflush(NULL);
}


void trainLeaves( uint32_t train_id, enum TRAIN_DIRECTION train_direction ) {
  fprintf( stdout, "Current time: %d MAV %d heading %s leaving the intersection\n", 
           current_time, train_id, directionAsString[ train_direction ] );
  train_count[train_direction]--;
  printf("Train count %s: %d \n",
   directionAsString[train_direction], train_count[train_direction] );
  
  in_intersection = INTERSECTION_EMPTY;
}

void trainCross( uint32_t train_id, enum TRAIN_DIRECTION train_direction ) {
  // TODO: Handle any crossing logic
  fprintf( stdout, "Current time: %d MAV %d heading %s entering the intersection\n", 
           current_time, train_id, directionAsString[ train_direction ] );

  if( in_intersection == INTERSECTION_EMPTY ){
    in_intersection = train_id;
  }else{
    fprintf( stderr, "CRASH: Train %d collided with train %d\n",
                      train_id, in_intersection );
    exit( EXIT_FAILURE );
  }

  // Sleep for 10 microseconds to simulate crossing the intersection
  usleep( 10 * 1000000 / clock_tick );

  // Leave the intersection
  trainLeaves( train_id, train_direction );

  return;
}

void trainArrives( uint32_t train_id, enum TRAIN_DIRECTION train_direction ){
  fprintf( stdout, "Current time: %d MAV %d heading %s arrived at the intersection\n", 
           current_time, train_id, directionAsString[ train_direction ] );
  train_count[train_direction]++;
  printf("Train count %s: %d \n",
   directionAsString[train_direction], train_count[train_direction] );
  pthread_t tid;
  ts = malloc( sizeof( struct train_struct )) ;
  ts->id = train_id;
  ts->direction = train_direction;
  pthread_create(&tid, NULL, trainLogic, ts);
  return;
}

void mediate( ){
  // TODO: MAV monitor code
  //deadlock
   if(train_count[NORTH] && !train_count[SOUTH]
       && !train_count[EAST] && !train_count[WEST]){
       pthread_cond_signal(&cond[NORTH]);
       return;
   }
   else if(!train_count[NORTH] && train_count[SOUTH]
       && !train_count[EAST] && !train_count[WEST]){
       pthread_cond_signal(&cond[SOUTH]);
       return;
   }
   else if(!train_count[NORTH] && !train_count[SOUTH]
       && train_count[EAST] && !train_count[WEST]){
       pthread_cond_signal(&cond[EAST]);
       return;
   }
   else if(!train_count[NORTH] && !train_count[SOUTH]
       && !train_count[EAST] && train_count[WEST]){
       pthread_cond_signal(&cond[WEST]);
       return;
   }
   else if(train_count[NORTH] && train_count[SOUTH]
       && !train_count[EAST] && !train_count[WEST]){
       pthread_cond_signal(&cond[NORTH]);
       return;
   }
   else if(!train_count[NORTH] && !train_count[SOUTH]
       && train_count[EAST] && train_count[WEST]){
       pthread_cond_signal(&cond[EAST]);
       return;
   }
   else if(train_count[NORTH] && train_count[SOUTH]
       && train_count[EAST] && train_count[WEST]){
       pthread_cond_signal(&cond[NORTH]);
       return;
   }

  if(train_count[NORTH]>0 && train_count[WEST]==0 ){
       pthread_cond_signal(&cond[NORTH]);
       return;
   }
  if(train_count[EAST]>0 && train_count[NORTH]==0 ){
       pthread_cond_signal(&cond[EAST]);
       return;
   }
  if(train_count[SOUTH]>0 && train_count[WEST]==0 ){
       pthread_cond_signal(&cond[SOUTH]);
       return;
   }
  if(train_count[WEST]>0 && train_count[SOUTH]==0 ){
       pthread_cond_signal(&cond[WEST]);
       return;
   } 



    
}
// initialize our cond var. here
// initialize mutex here
void init(){
  int i, ret;
  for(int i = 0;i<NUM_DIRECTIONS;i++)
    ret = pthread_cond_init(&cond[i], NULL);
   
    for(int i = 0;i<NUM_DIRECTIONS;i++)
     pthread_mutex_init(&direction_mutex[i], NULL) ;

  for(int i = 0;i<NUM_DIRECTIONS;i++)
    train_count[i]=0;
}

/*
 *
 *
 *  DO NOT MODIFY CODE BELOW THIS LINE
 *
 *
 *
 */

int process( )
{
  // If there are no more scheduled train arrivals
  // then return and exit
  if(scheduleEmpty()) return 0;

  // If we're done with a day's worth of schedule then
  // we're done.
  if( current_time > SECONDS_IN_A_DAY ) return 0;
  // Check for deadlocks
  mediate( );

  // While we still have scheduled train arrivals and it's time
  // to handle an event
  while( !scheduleEmpty() && current_time >= scheduleFront().arrival_time ) 
  {

#ifdef DEBUG
    fprintf( stdout, "Dispatching schedule event: time: %d train: %d direction: %s\n",
                      scheduleFront().arrival_time, scheduleFront().train_id, 
                      directionAsString[ scheduleFront().train_direction ] );
#endif  

    trainArrives( scheduleFront().train_id, scheduleFront().train_direction );

    // Remove the event from the schedule since it's done
    schedulePop( );

  }

  // Sleep for a simulated second. Depending on clock_tick this
  // may equate to 1 real world second down to 1 microsecond
  usleep( 1 * 1000000 / clock_tick );

  current_time ++;

  return 1;
}

int main ( int argc, char * argv[] )
{

  // Initialize time of day to be midnight
  current_time = 0;
  clock_tick   = 1;

  // Verify the user provided a data file name.  If not then
  // print an error and exit the program
  if( argc < 2 )
  {
    fprintf( stderr, "ERROR: You must provide a train schedule data file.\n");
    exit(EXIT_FAILURE);
  }

  // See if there's a second parameter which specifies the clock
  // tick rate.  
  if( argc == 3 )
  {
    int32_t tick = atoi( argv[2] );
    
    if( tick <= 0 )
    {
      fprintf( stderr, "ERROR: tick rate must be positive.\n");
      exit(EXIT_FAILURE);
    }
    else
    {
      clock_tick = tick;
    }
  }

  buildTrainSchedule( argv[1] );

  // Initialize the intersection to be empty
  in_intersection = INTERSECTION_EMPTY;

  // Call user specific initialization
  init( );

  // Start running the MAV manager
  while( process() );
 
  return 0;
}
