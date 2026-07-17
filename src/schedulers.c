#include "../codexion.h"


void remove_id(int id, t_sim* sim)
{
    int i;
    int removed;

    i = -1;
    removed = 0;
    (void) id;
    pthread_mutex_lock(&sim->queue_mutex);
    while(++i < sim->args.number_of_coders - 1)
        sim->queue[i] = sim->queue[1 + i];
    
    sim->queue[i] = -1;

    pthread_mutex_unlock(&sim->queue_mutex);

}
int find_id(int id, t_sim* sim)
{
    int found;
    int i;

    found = 0;
    i = 0;
    while (i < sim->args.number_of_coders)
    {
        if (sim->queue[i] == id)
        {
            found = 1;
            break;
        }
        i++;
    }
    return found;
}

void fifo_dongle(int id, t_sim* sim)
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
    pthread_mutex_unlock(&sim->queue_mutex);

    while (!sim->stop)
    {
        if (sim->queue[0] == id)
            return 1;
        usleep(10);
    }
    return 0;
}

void edf_dongle(int id, t_sim *sim)
{
}

void request_dongle(t_coder* coder, t_dongle* dongle)
{
    if (coder->sim->args.scheduler)
        fifo_dongle(coder->id, coder->sim);
    else
        printf("EDF \n ");
    
    take_dongle(coder, dongle);

}