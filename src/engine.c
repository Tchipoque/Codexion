/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:34:27 by etchipoq          #+#    #+#             */
/*   Updated: 2026/08/06 21:25:11 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Performs the refactoring step.
 */
static void	perform_refactoring(t_coder *coder, t_args args)
{
	long	time;

	if (coder->sim->stop)
		return ;
	time = get_time_ms() - coder->sim->start_time;
	pthread_mutex_lock(&coder->sim->log);
	printf("%ld %d is refactoring \n", time, coder->id);
	pthread_mutex_unlock(&coder->sim->log);

	usleep(1000 * args.time_to_refactor);
}

/**
 * Performs the debugging step.
 */
static void	perform_debugging(t_coder *coder, t_args args)
{
	long	time;

	if (coder->sim->stop)
		return ;
	time = get_time_ms() - coder->sim->start_time;
	pthread_mutex_lock(&coder->sim->log);
	printf("%ld %d is debugging \n", time, coder->id);
	pthread_mutex_unlock(&coder->sim->log);

	usleep(1000 * args.time_to_debug);
}

/**
 * Performs the compile step and records the last compile start time.
 */
static void	perform_compile(t_coder *coder, t_args args)
{
	long	time;

	if (coder->sim->stop)
		return ;
	pthread_mutex_lock(&coder->state);
	coder->last_compile_start = get_time_ms();
	coder->compile_count += 1;
	pthread_mutex_unlock(&coder->state);
	time = get_time_ms() - coder->sim->start_time;
	pthread_mutex_lock(&coder->sim->log);
	printf("%ld %d is compiling \n", time, coder->id);
	pthread_mutex_unlock(&coder->sim->log);

	usleep(1000 * args.time_to_compile);
}

/**
 * Runs the coder loop until the simulation stops.
 */
void	*run_coder_cycle(void *arg)
{
	t_coder		*coder;
	t_args		args;
	t_dongle	*first;
	t_dongle	*second;

	coder = (t_coder *)arg;
	coder->last_compile_start = coder->sim->start_time;
	args = coder->sim->args;
	while (!coder->sim->stop)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
		while (first == second && !coder->sim->stop)
			usleep(1000);
		acquire_dongles(coder, first, second);
		if (coder->sim->stop)
			break ;
		perform_compile(coder, args);
		release_dongles_and_requeue(coder, args.dongle_cooldown);
		perform_debugging(coder, args);
		perform_refactoring(coder, args);
	}
	return (NULL);
}
