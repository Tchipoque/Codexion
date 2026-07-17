#include "../codexion.h"


int	ft_isnumber(char* num)
{
	int i;

	i = 0;
	while(num[i])
	{
		if (!((num[i] >= '0') && num[i] <= '9'))
			return (0);
		i++;
	}

	return (2048);
}

int	scheduler(char* word)
{
	if ((strcmp(word, "fifo") != 0))
		return 0;
	else
		return 1;
}

int destroy_all(t_sim *sim)
{
    int n_coders;
    int i;

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
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->queue_mutex);
    free(sim);
    printf("all clearr\n");
    return 1;
}
long gettimelog()
{
	struct timeval tv;
	long seconds_today;
	long ms_today;

	gettimeofday(&tv, NULL);

	seconds_today = tv.tv_sec % 86400;
	ms_today = (seconds_today * 1000) + (tv.tv_usec / 1000);

	return ms_today;
}

void release(t_coder *coder, long cooldown)
{
	t_dongle *right;
	t_dongle *left;

	right = coder->right_dongle;
	left = coder->left_dongle;

	usleep(1000 * cooldown);
	/* If both pointers are the same (1 coder), unlock only once */
	if (right == left)
		pthread_mutex_unlock(&right->mutex);
	else
	{
		pthread_mutex_unlock(&right->mutex);
		pthread_mutex_unlock(&left->mutex);
	}
	remove_id(coder->id, coder->sim);
	printf("Coder %d released dongles n (%d, %d)\n", coder->id, right->id, left->id);
}

