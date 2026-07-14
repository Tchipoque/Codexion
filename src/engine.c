
#include "../codexion.h"

void refactoring(t_coder* coder, t_args args)
{
    printf("Coder %d refactoring\n", coder->id);
	usleep(1000 * args.time_to_refactor);

}

void debugging(t_coder* coder, t_args args)
{
    printf("Coder %d debugging\n", coder->id);
	usleep(1000 * args.time_to_debug);

}

int compile(t_coder* coder, t_args args)
{
    if (check_stoppers(coder))
        return 0;
    coder->last_compile_start = gettimelog();
    printf("Coder %d compiling\n", coder->id);
    coder->compile_count += 1;
	usleep(1000 * args.time_to_compile);
    return 1;
}

int take_dongle(t_coder *coder, t_dongle *dongle)
{
    char *type;

    if (check_stoppers(coder))
        return 0;
    if (coder->id == dongle->id)
        type = "right";
    else
        type = "left";

    pthread_mutex_lock(&dongle->mutex);
    printf("Coder %d took the %s dongle n %d\n", coder->id, type, dongle->id);
    return 1;
}
void *engine(void* arg)
{
    t_coder* coder;
    t_args args;

    coder = (t_coder *) arg;
    args = coder->sim->args;

    while (!coder->sim->stop)
    {
        if (!take_dongle(coder, coder->right_dongle)
        || !take_dongle(coder, coder->left_dongle))
            break;
        if (!compile(coder, args))
            break;
        release(coder, args.dongle_cooldown);
        debugging(coder, args);
        refactoring(coder, args);
    }
    return NULL;
}
