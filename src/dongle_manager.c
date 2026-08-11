/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_manager.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:34:24 by etchipoq          #+#    #+#             */
/*   Updated: 2026/08/11 22:45:01 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Removes a coder from the shared queue and wakes waiting threads.
 */
void	remove_coder_from_queue(int id, t_sim *sim)
{
	int	i;
	int	pos;

	i = 0;
	pos = -1;
	pthread_mutex_lock(&sim->queue_mutex);
	while (++i <= sim->args.number_of_coders)
	{
		if (sim->queue[i] == id)
		{
			pos = i;
			break ;
		}
	}
	if (pos != -1)
	{
		i = pos - 1;
		while (++i < sim->args.number_of_coders)
			sim->queue[i] = sim->queue[i + 1];
		sim->queue[sim->args.number_of_coders] = -1;
	}
	pthread_cond_broadcast(&sim->cond);
	pthread_mutex_unlock(&sim->queue_mutex);
}

/**
 * Locks one dongle and logs the acquisition.
 */
static void	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	long	time;

	time = get_time_ms() - coder->sim->start_time;
	while (!coder->sim->stop && get_time_ms() < dongle->available_at)
		usleep(100);
	if (coder->sim->stop)
		return ;
	pthread_mutex_lock(&dongle->mutex);
	if (coder->sim->stop)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return ;
	}
	pthread_mutex_lock(&coder->sim->log);
	printf("%ld %d has taken dongle n %d\n", time, coder->id, dongle->id);
	pthread_mutex_unlock(&coder->sim->log);
}

/**
 * Waits for scheduler approval and then locks both requested dongles.
 */
void	acquire_dongles(t_coder *coder, t_dongle *dongle1, t_dongle *dongle2)
{
	long	now;

	if (coder->sim->stop)
		return ;
	if (coder->sim->args.scheduler)
		wait_for_fifo_turn(coder->id, coder->sim);
	else
		wait_for_edf_turn(coder->id, coder->sim);
	if (coder->sim->stop)
		return ;
	while (!coder->sim->stop)
	{
		now = get_time_ms();
		if (now >= dongle1->available_at && now >= dongle2->available_at)
			break ;
		usleep(100);
	}
	acquire_dongle(coder, dongle1);
	while (dongle1 == dongle2 && !coder->sim->stop)
		usleep(1000);
	acquire_dongle(coder, dongle2);
}

static void	set_allowed_coders(t_sim *sim, int front_parity)
{
	int	i;
	int	id;
	int	n;

	n = sim->args.number_of_coders;
	i = 1;
	while (i <= n)
	{
		id = sim->queue[i];
		if (id != -1 && (id % 2) == front_parity)
		{
			if (!(n % 2 == 1 && ((id == 1 && sim->coders[n].go) || (id == n
							&& sim->coders[1].go))))
				sim->coders[id].go = 1;
		}
		i++;
	}
}

void	allowed_coders(t_sim *sim)
{
	int	i;
	int	n;

	if (sim->queue[1] == -1)
		return ;
	n = sim->args.number_of_coders;
	i = 1;
	while (i <= n)
		sim->coders[i++].go = 0;
	set_allowed_coders(sim, sim->queue[1] % 2);
	pthread_cond_broadcast(&sim->cond);
}
