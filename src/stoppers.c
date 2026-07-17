#include "../codexion.h"

int check_compiles_count(t_sim *sim)
{
    long i;
    long required_compiles;
    t_args args;

    i = -1;
    args = sim->args;
    required_compiles = args.number_of_compiles_required;
    while (++i < args.number_of_coders)
    {
        //printf("here\n\n");
        if (required_compiles > sim->coders[i].compile_count)
            return 0;
    }

    sim->stop = 777;
    return 1;
}

int check_burnout(t_sim *sim, long last_compile, int id)
{
    long clock;
    
    if (last_compile == 0)
        clock = gettimelog() - sim->start_time;
    else
        clock = gettimelog() - last_compile;

    if (clock >= sim->args.time_to_burnout)
    {
        printf("\033[1;31mCoder %d has burnout\033[0m\n", id);
        sim->stop = 777;
        return 1;
    }
    return 0;
}

int check_stoppers(t_coder *coder)
{
    int stop;
    
    pthread_mutex_lock(&coder->state);
    
    if (check_burnout(coder->sim, coder->last_compile_start, coder->id) == 1)
        stop = 1;
    else if (check_compiles_count(coder->sim) == 1)
        stop = 1;
    else
        stop = 0;
    pthread_mutex_unlock(&coder->state);

    return stop;
    
}
