#ifndef CODEXION_H
#define CODEXION_H

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>


typedef struct s_args
{
	int    number_of_coders;
    long    time_to_burnout;
    long    time_to_compile;
    long    time_to_debug;
    long    time_to_refactor;
    int     number_of_compiles_required;
    long    dongle_cooldown;
    int     scheduler; // 0 = fifo, 1 = edf
} t_args;

typedef struct s_dongle
{
	int	id;
	pthread_mutex_t	mutex;
	// pthread_cond_t	cond;
}	t_dongle;

typedef struct s_coder
{
	int	id;
	pthread_t	thread;
	int	compile_count;
	long	last_compile_start;
	t_dongle *left_dongle;
	t_dongle *right_dongle;
	struct s_sim *sim;
	pthread_mutex_t state;
}	t_coder;

typedef struct s_sim
{
	t_args	args;
	t_coder	*coders;
	t_dongle	*dongles;
	int	stop;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	pthread_t monitor;
	long	start_time;
}	t_sim;

int	ft_isnumber(char* c);
int	scheduler(char* word);
int destroy_all(t_sim *sim);
int validate(int argc, char **args);
int iniciate(char **args, t_sim *simulator);
void *engine(void* arg);
long gettimelog();
void release(t_coder *coder, long cooldown);
void create_thread(t_sim *simualtor);
int check_stoppers(t_coder *coder);
void create_monitor(t_sim* sim);

#endif
