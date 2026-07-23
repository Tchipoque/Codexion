/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:34:27 by etchipoq          #+#    #+#             */
/*   Updated: 2026/07/23 22:28:15 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Performs the refactoring step.
 */
static void	perform_refactoring(t_coder *coder, t_args args)
{
	printf("Coder %d refactoring\n", coder->id);
	usleep(1000 * args.time_to_refactor);
}

/**
 * Performs the debugging step.
 */
static void	perform_debugging(t_coder *coder, t_args args)
{
	printf("Coder %d debugging\n", coder->id);
	usleep(1000 * args.time_to_debug);
}

/**
 * Performs the compile step and records the last compile start time.
 */
static void	perform_compile(t_coder *coder, t_args args)
{
	pthread_mutex_lock(&coder->state);
	coder->last_compile_start = get_time_ms();
	coder->compile_count += 1;
	pthread_mutex_unlock(&coder->state);
	printf("Coder %d compiling\n", coder->id);
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
	args = coder->sim->args;
	while (!coder->sim->stop)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
		acquire_dongles(coder, first, second);
		perform_compile(coder, args);
		release_dongles_and_requeue(coder, args.dongle_cooldown);
		perform_debugging(coder, args);
		perform_refactoring(coder, args);
	}
	return (NULL);
}
