/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:35:08 by etchipoq          #+#    #+#             */
/*   Updated: 2026/08/04 23:30:22 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Builds the EDF priority snapshot from the current coder state.
 */
static void	build_wait_time_priorities(t_priority priority_list[], t_sim *sim)
{
	int			i;
	long		now;
	t_coder		*coders;
	t_priority	buf;

	i = 0;
	coders = sim->coders;
	now = get_time_ms();
	while (++i <= sim->args.number_of_coders)
	{
		sim->queue[i] = i;
		buf.id = i;
		if (coders[i].last_compile_start == 0)
			buf.burnout = INT_MAX;
		else
			buf.burnout = now - coders[i].last_compile_start;
		priority_list[i] = buf;
	}
}

/**

	* Sorts the queue by descending waiting time.
 */
static void	sort_queue_by_wait_time_desc(t_priority priority_list[], t_sim *sim)
{
	int	i;
	int	b;
	int	id_1;
	int	id_2;
	int	temp;

	i = 0;
	while (++i < sim->args.number_of_coders)
	{
		b = i;
		while (++b <= sim->args.number_of_coders)
		{
			id_1 = sim->queue[i];
			id_2 = sim->queue[b];
			if (priority_list[id_1].burnout < priority_list[id_2].burnout
				|| priority_list[id_1].burnout == 0)
			{
				temp = id_1;
				sim->queue[i] = id_2;
				sim->queue[b] = temp;
			}
		}
	}
}

/**
 * Waits until the caller reaches the front of the EDF queue.
 */
void	wait_for_edf_turn(int id, t_sim *sim)
{
	t_priority	*priority_list;
	int			n;

	n = sim->args.number_of_coders + 1;
	priority_list = malloc(sizeof(t_priority) * n);
	if (!priority_list)
		return ;
	pthread_mutex_lock(&sim->queue_mutex);
	build_wait_time_priorities(priority_list, sim);
	sort_queue_by_wait_time_desc(priority_list, sim);
	allowed_coders(sim);
	while (id != -1 && !sim->stop)
	{
		if (sim->queue[1] == id || sim->coders[id].go)
			break ;
		pthread_cond_wait(&sim->cond, &sim->queue_mutex);
	}
	pthread_mutex_unlock(&sim->queue_mutex);
	free(priority_list);
}

/**
 * Waits until the caller reaches the front of the FIFO queue.
 */
void	wait_for_fifo_turn(int id, t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->queue_mutex);
	while (!sim->stop)
	{
		if (!queue_contains_id(id, sim))
		{
			i = 0;
			while (++i <= sim->args.number_of_coders)
			{
				if (sim->queue[i] == -1)
				{
					sim->queue[i] = id;
					break ;
				}
			}
		}
		allowed_coders(sim);
		if (sim->queue[1] == id || sim->coders[id].go)
			break ;
		pthread_cond_wait(&sim->cond, &sim->queue_mutex);
	}
	pthread_mutex_unlock(&sim->queue_mutex);
}
