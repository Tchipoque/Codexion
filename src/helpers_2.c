#include "../codexion.h"

void check_compiles_count(t_sim *sim)
{
    long i;
    long compile_count;
    long required_compiles;
    t_args args;

    i = -1;
    compile_count = 0;
    required_compiles = args.number_of_coders * args.number_of_compiles_required;
    args = sim->args;
    while (++i < args.number_of_coders)
        compile_count += sim->coders[i].compile_count;

    if (compile_count >= required_compiles)
    {
        printf("THe system has concluided\n");
        sim->stop = 777;
    }
}

void check_burnout(t_sim *sim, long last_compile)
{
    long clock;

    if (last_compile == 0)
        clock = gettimelog() - sim->start_time;
    else
        clock = gettimelog() - last_compile;

    if (clock >= sim->args.time_to_burnout)
    {
        printf("THe system has burnout\n");
        sim->stop = 777;
    }
}