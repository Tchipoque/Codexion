/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stoppers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:35:14 by etchipoq          #+#    #+#             */
/*   Updated: 2026/08/11 22:39:08 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Returns non-zero when every coder reached the required compile count.
 */
int	check_compile_goal(t_sim *sim)
{
	long	i;
	long	required_compiles;
	t_args	args;

	i = 0;
	args = sim->args;
	required_compiles = args.number_of_compiles_required;
	while (++i <= args.number_of_coders)
	{
		if (required_compiles > sim->coders[i].compile_count)
			return (0);
	}
	sim->stop = 18;
	return (1);
}

/**
 * Returns non-zero when a coder exceeded the burnout deadline.
 */
int	check_burnout_timeout(t_sim *sim, long last_compile, int id)
{
	long	clock;
	int		burnt;
	long	time;

	burnt = 0;
	clock = get_time_ms() - last_compile;
	if (clock >= sim->args.time_to_burnout)
	{
		sim->stop = 1;
		pthread_mutex_lock(&sim->queue_mutex);
		pthread_cond_broadcast(&sim->cond);
		pthread_mutex_unlock(&sim->queue_mutex);
		time = get_time_ms() - sim->start_time;
		printf("\033[1;31m%ld %d burned out\033[0m\n", time, id);
		sim->burned_out = 1;
		burnt = 1;
	}
	return (burnt);
}

/**
 * Evaluates all simulation stop conditions for one coder.
 */
int	check_stop_conditions(t_coder *coder)
{
	int	stop;

	pthread_mutex_lock(&coder->state);
	if (check_burnout_timeout(coder->sim, coder->last_compile_start,
			coder->id) == 1)
		stop = 1;
	else if (check_compile_goal(coder->sim) == 1)
		stop = 1;
	else
		stop = 0;
	pthread_mutex_unlock(&coder->state);
	return (stop);
}

/**
 * Returns non-zero when the shared queue already contains the coder id.
 */
int	queue_contains_id(int id, t_sim *sim)
{
	int	found;
	int	i;

	found = 0;
	i = 1;
	while (i <= sim->args.number_of_coders)
	{
		if (sim->queue[i] == id)
		{
			found = 1;
			break ;
		}
		i++;
	}
	return (found);
}
