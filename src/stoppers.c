/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stoppers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:35:14 by etchipoq          #+#    #+#             */
/*   Updated: 2026/07/28 21:26:51 by etchipoq         ###   ########.fr       */
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

	i = -1;
	args = sim->args;
	required_compiles = args.number_of_compiles_required;
	while (++i < args.number_of_coders)
	{
		// printf("here\n\n");
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
	int	burnt;
	long	time;

	burnt = 0;
	clock = get_time_ms() - last_compile;
	time = get_time_ms() - sim->start_time;
	if (clock >= sim->args.time_to_burnout)
	{
		printf("\033[1;31m%ld %d has burnout\033[0m\n",time,  id);
		sim->stop = 7;
		sim->burned_out = 9;
		burnt = 1;
		// destroy_simulation(sim);
		// exit(1);
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
