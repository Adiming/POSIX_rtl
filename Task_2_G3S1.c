// Order of argv is : period, load, scheduling policy, priority
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include "errno.h"
#include <stdlib.h>
#include <string.h>
#include <sched.h>

# define CLOCKID CLOCK_REALTIME

void new_action_funct(){
printf("Missed deadline \n");
}


void p_task(long period_ms, int load, timer_t timer, struct itimerspec time_left){
for(int i = 0; i < load*1000; i++){}

timer_gettime(timer, &time_left);
float task_duration = (time_left.it_value.tv_sec*1000000000 + time_left.it_value.tv_nsec)/1000000.0;

printf("Time used for execution in ms is %1.5f \n", period_ms - task_duration);
}


int main(int argc, char** argv){

sigset_t signal_set;
timer_t timer;
struct itimerspec timer_spec, time_left;
//struct sigaction task;



// Create a timer
timer_create(CLOCK_REALTIME, NULL, &timer);

// Specify the timer’s period.
long period_ms = atoi(argv[1]);
int load = atoi(argv[2]);
printf("Started with a period in ms of:%ld\n", period_ms);
timer_spec.it_value.tv_sec = 0;
timer_spec.it_value.tv_nsec = period_ms*1000000; 
timer_spec.it_interval.tv_sec = 0;
timer_spec.it_interval.tv_nsec = period_ms*1000000;
printf("My pid:%d\n", getpid());

//New action
struct sigaction new_action;
memset(&new_action, 0, sizeof(new_action));
new_action.sa_handler = new_action_funct; 
sigemptyset( &new_action.sa_mask );
new_action.sa_flags = 0;
sigaction(SIGALRM, &new_action, NULL);
//printf("Defined a new handler for SIGALRM \n");

if (atoi(argv[3]) == 1){
	struct sched_param spar;
	    spar.sched_priority = sched_get_priority_min(SCHED_FIFO) + atoi(argv[4]);
		printf("Priority of task: %d \n", spar.sched_priority);	
	    if(sched_setscheduler(0, SCHED_FIFO, &spar) < 0)
	    {
		printf("Unable to set SCHED_FIFO. %d\n", errno);
		exit(EXIT_FAILURE);
	    }
}
else if(atoi(argv[3]) == 0){
	struct sched_param spar;
	    spar.sched_priority = sched_get_priority_max(SCHED_RR);
	    if(sched_setscheduler(0, SCHED_RR, &spar) < 0)
	    {
		printf("Unable to set SCHED_RR. %d\n", errno);
		exit(EXIT_FAILURE);
	    }
}
// Setup a signal set for sigwait() to wait for SIGALRM
sigemptyset(&signal_set);
sigaddset(&signal_set, SIGALRM);
//sigprocmask(SIG_BLOCK, &signal_set, NULL);
timer_settime(timer, 0, &timer_spec, NULL);
int signum;
while(1){
	//sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
	//printf("Time used for execution is %ld \n", period - timer_gettime(timer, &timer_spec));
	p_task(period_ms, load, timer, time_left);	
	sigwait(&signal_set, &signum);
	
	}
}
