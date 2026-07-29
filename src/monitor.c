/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:34:40 by etchipoq          #+#    #+#             */
/*   Updated: 2026/07/28 14:28:13 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Periodically checks whether the simulation should stop.
 */
void	*monitor_simulation(void *arg)
{
	t_sim	*sim;
	int		i;

	sim = (t_sim *)arg;
	while (!sim->stop)
	{
		i = 0;
		while (i < sim->args.number_of_coders)
		{
			if (check_stop_conditions(&sim->coders[i++]))
				break ;	
		}
	}
	return (NULL);
}

/**
 * Starts the monitor thread.
 */
void	start_monitor_thread(t_sim *sim)
{
	pthread_create(&sim->monitor, NULL, monitor_simulation, sim);
}
