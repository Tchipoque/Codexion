#include "../codexion.h"


void* monitoring(void *arg) 
{
    t_sim* sim;
    int i;

    sim = (t_sim*) arg;

    while (!sim->stop)
    {
        i = 0;
        while(i < sim->args.number_of_coders)
        {
            if (check_stoppers(&sim->coders[i++]))
                break;
        }
    }
    return NULL;
}

void create_monitor(t_sim* sim)
{
	pthread_create(&sim->monitor, NULL, monitoring, sim);
}