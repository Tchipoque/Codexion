#include "../codexion.h"

void initialize_priority_list(t_priority priority_list[], t_sim *sim)
{
    int i;
    long now;
    t_coder *coders;
    t_priority buf;

    i = -1;
    coders = sim->coders;
    now = gettimelog();
    while (++i < sim->args.number_of_coders)
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

void sort_queue_by_burnout(t_priority priority_list[], t_sim *sim)
{
    int i;
    int b;
    int id_1;
    int id_2;
    long temp;

    i = -1;
    while (++i < sim->args.number_of_coders - 1)
    {
        b = i;
        while (++b < sim->args.number_of_coders)
        {
            id_1 = sim->queue[i];
            id_2 = sim->queue[b];
            if (priority_list[id_1].burnout < priority_list[id_2].burnout || priority_list[id_1].burnout == 0)
            {
                temp = id_1;
                sim->queue[i] = id_2;
                sim->queue[b] = temp;
            }
        }
    }
}

void request_edf(int id, t_sim *sim)
{
    t_priority priority_list[sim->args.number_of_coders];

    pthread_mutex_lock(&sim->queue_mutex);
    initialize_priority_list(priority_list, sim);
    sort_queue_by_burnout(priority_list, sim);


    while (id != -1 && sim->queue[0] != id)
        pthread_cond_wait(&sim->cond, &sim->queue_mutex);

    pthread_mutex_unlock(&sim->queue_mutex);
}

void request_fifo(int id, t_sim* sim)
{
    int i;

    i = -1;
    pthread_mutex_lock(&sim->queue_mutex);
    while(++i < sim->args.number_of_coders)
    {
        if (find_id(id, sim))
            break;
        if (sim->queue[i] == -1)
        {
            sim->queue[i] = id;
            break;
        }
    }

    while (sim->queue[0] != id)
        pthread_cond_wait(&sim->cond, &sim->queue_mutex);

    pthread_mutex_unlock(&sim->queue_mutex);

}
