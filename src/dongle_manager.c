/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_manager.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:34:24 by etchipoq          #+#    #+#             */
/*   Updated: 2026/07/22 00:38:26 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Removes a coder from the shared queue and wakes waiting threads.
 */
void	remove_coder_from_queue(int id, t_sim *sim)
{
	int	i;

	i = -1;
	(void)id;
	pthread_mutex_lock(&sim->queue_mutex);
	while (++i < sim->args.number_of_coders - 1)
		sim->queue[i] = sim->queue[1 + i];
	sim->queue[i] = -1;
	pthread_cond_broadcast(&sim->cond);
	pthread_mutex_unlock(&sim->queue_mutex);
}
/**
 * Returns non-zero when the shared queue already contains the coder id.
 */
int	queue_contains_id(int id, t_sim *sim)
{
	int	found;
	int	i;

	found = 0;
	i = 0;
	while (i < sim->args.number_of_coders)
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

/**
 * Locks one dongle and logs the acquisition.
 */
static void	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	char	*type;

	if (coder->id == dongle->id)
		type = "right";
	else
		type = "left";
	pthread_mutex_lock(&dongle->mutex);
	printf("Coder %d took the %s dongle n %d\n", coder->id, type, dongle->id);
}

/**
 * Waits for scheduler approval and then locks both requested dongles.
 */
void	acquire_dongles(t_coder *coder, t_dongle *dongle1, t_dongle *dongle2)
{
	t_dongle	*tmp;

	if (coder->sim->args.scheduler)
		wait_for_fifo_turn(coder->id, coder->sim);
	else
		wait_for_edf_turn(coder->id, coder->sim);
	if (dongle1->id > dongle2->id)
	{
		tmp = dongle1;
		dongle1 = dongle2;
		dongle2 = tmp;
	}
	if (dongle1->id == dongle2->id)
		acquire_dongle(coder, dongle1);
	else
	{
		acquire_dongle(coder, dongle1);
		acquire_dongle(coder, dongle2);
	}
}
