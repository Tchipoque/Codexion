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

    free(sim->coders);

    while (++i < n_coders)
        pthread_mutex_destroy(&sim->dongles[i].mutex);

    free(sim->dongles);
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
