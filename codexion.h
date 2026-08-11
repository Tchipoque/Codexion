/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:36:10 by etchipoq          #+#    #+#             */
/*   Updated: 2026/08/03 21:47:40 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# define _XOPEN_SOURCE 500
# include <limits.h>
# include <pthread.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

/**
 * Priority snapshot used by the EDF scheduler.
 */
typedef struct s_priority
{
	int				id;
	long			burnout;
}					t_priority;

/**
 * Parsed command-line arguments for the simulation.
 */
typedef struct s_args
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				number_of_compiles_required;
	long			dongle_cooldown;
	int				scheduler;
}					t_args;

/**
 * A dongle resource protected by a mutex.
 */
typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	long			available_at;
}					t_dongle;

/**
 * Runtime state for one coder thread.
 */
typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	int				compile_count;
	long			last_compile_start;
	long			deadline;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	int				go;
	struct s_sim	*sim;
	pthread_mutex_t	state;
}					t_coder;

/**
 * Shared simulation state.
 */
typedef struct s_sim
{
	t_args			args;
	t_coder			*coders;
	t_dongle		*dongles;
	int				stop;
	pthread_t		monitor;
	int				burned_out;
	int				*queue;
	pthread_mutex_t	queue_mutex;
	pthread_mutex_t	log;
	pthread_cond_t	cond;
	long			start_time;
}					t_sim;

/**
 * Returns non-zero when the string contains only decimal digits.
 */
int					is_numeric_string(char *text);

/**
 * Returns non-zero when the scheduler name is "fifo".
 */
int					is_fifo_scheduler(char *word);

/**
 * Releases every allocated resource and destroys synchronization objects.
 */
int					destroy_simulation(t_sim *sim);

/**
 * Validates the full command-line argument list.
 */
int					validate_arguments(int argc, char **args);

/**
 * Allocates and initializes the simulation state.
 */
int					initialize_simulation(char **args, t_sim *simulator);

/**
 * Runs one coder thread through the simulation loop.
 */
void				*run_coder_cycle(void *arg);

/**
 * Returns the current time in milliseconds.
 */
long				get_time_ms(void);

/**
 * Waits for the cooldown, releases both dongles, and updates the queue.
 */
void				release_dongles_and_requeue(t_coder *coder, long cooldown);

/**
 * Starts one thread per coder.
 */
void				create_coder_threads(t_sim *simulator);

/**
 * Checks whether the simulation should stop.
 */
int					check_stop_conditions(t_coder *coder);

/**
 * Starts the monitor thread.
 */
void				start_monitor_thread(t_sim *sim);

/**
 * Removes a coder id from the shared queue.
 */
void				remove_coder_from_queue(int id, t_sim *sim);

/**
 * Requests both dongles after scheduler approval.
 */
void				acquire_dongles(t_coder *coder, t_dongle *dongle1,
						t_dongle *dongle2);

/**
 * Waits for the caller's turn in the FIFO queue.
 */
void				wait_for_fifo_turn(int id, t_sim *sim);

/**
 * Waits for the caller's turn in the EDF queue.
 */
void				wait_for_edf_turn(int id, t_sim *sim);

/**
 * Returns non-zero when the queue already contains the coder id.
 */
int					queue_contains_id(int id, t_sim *sim);

void				allowed_coders(t_sim *sim);

#endif
