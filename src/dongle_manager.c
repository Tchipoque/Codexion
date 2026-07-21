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


void request_dongle(t_coder* coder, t_dongle* dongle)
{
    if (coder->sim->args.scheduler)
        request_fifo(coder->id, coder->sim);
    else
        request_edf(coder->id, coder->sim);

    take_dongle(coder, dongle);

}
