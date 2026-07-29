/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:35:44 by etchipoq          #+#    #+#             */
/*   Updated: 2026/07/28 21:44:21 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Returns non-zero when the string contains only decimal digits.
 */
int	is_numeric_string(char *num)
{
	int	i;

	i = 0;
	while (num[i])
	{
		if (!((num[i] >= '0') && num[i] <= '9'))
			return (0);
		i++;
	}
	return (2048);
}

/**
 * Returns non-zero when the scheduler name is FIFO.
 */
int	is_fifo_scheduler(char *word)
{
	if ((strcmp(word, "fifo") != 0))
		return (0);
	else
		return (1);
}

/**
 * Destroys allocated resources and synchronization primitives.
 */
int	destroy_simulation(t_sim *sim)
{
	int	n_coders;
	int	i;

	n_coders = sim->args.number_of_coders;
	i = -1;
	while (++i < n_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].state);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
	}
	free(sim->coders);
	free(sim->dongles);
	free(sim->queue);
	pthread_mutex_destroy(&sim->queue_mutex);
	pthread_cond_destroy(&sim->cond);
	free(sim);
	printf("all clearr\n");
	return (1);
}
/**
 * Returns the current time in milliseconds.
 */
long	get_time_ms(void)
{
	struct timeval	tv;
	long			seconds_today;
	long			ms_today;

	gettimeofday(&tv, NULL);
	seconds_today = tv.tv_sec % 86400;
	ms_today = (seconds_today * 1000) + (tv.tv_usec / 1000);
	return (ms_today);
}

/**
 * Releases both dongles, updates the queue, then waits for cooldown.
 */
void	release_dongles_and_requeue(t_coder *coder, long cooldown)
{
	t_dongle	*right;
	t_dongle	*left;
	long	time;

	right = coder->right_dongle;
	left = coder->left_dongle;
	time = get_time_ms() - coder->sim->start_time;
	if (right == left)
	{
		right->available_at = (long) get_time_ms + cooldown;
		pthread_mutex_unlock(&right->mutex);
	}
	else
	{
		right->available_at = (long) get_time_ms + cooldown;
		left->available_at = (long) get_time_ms + cooldown;
		pthread_mutex_unlock(&right->mutex);
		pthread_mutex_unlock(&left->mutex);
	}
	if (coder->sim->args.scheduler)
		remove_coder_from_queue(coder->id, coder->sim);
	else
		wait_for_edf_turn(-1, coder->sim);
	pthread_mutex_lock(&coder->sim->queue_mutex);
	pthread_cond_broadcast(&coder->sim->cond);
	pthread_mutex_unlock(&coder->sim->queue_mutex);
	printf("%ld %d released dongles n (%d, %d)\n", time, coder->id, right->id,
		left->id);
}
