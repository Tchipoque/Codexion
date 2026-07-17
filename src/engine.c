
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

void compile(t_coder* coder, t_args args)
{
    pthread_mutex_lock(&coder->state);
    coder->last_compile_start = gettimelog();
    coder->compile_count += 1;
    pthread_mutex_unlock(&coder->state);

    printf("Coder %d compiling\n", coder->id);
	usleep(1000 * args.time_to_compile);
}

void take_dongle(t_coder *coder, t_dongle *dongle)
{
    char *type;

    if (coder->id == dongle->id)
        type = "right";
    else
        type = "left";

    pthread_mutex_lock(&dongle->mutex);
    printf("Coder %d took the %s dongle n %d\n", coder->id, type, dongle->id);
}
void *engine(void* arg)
{
    t_coder* coder;
    t_args args;
    t_dongle *first;
    t_dongle *second;

    coder = (t_coder *) arg;
    args = coder->sim->args;

    while (!coder->sim->stop)
    {
        /* lock dongles in a consistent order to avoid circular wait */
        first = coder->left_dongle;
        second = coder->right_dongle;
        if (first->id > second->id)
        {
            t_dongle *tmp = first;
            first = second;
            second = tmp;
        }

        /* If both dongles are the same (single coder), only take once */
        if (first == second)
        {
            request_dongle(coder, first);
            compile(coder, args);
        }
        else
        {
            request_dongle(coder, first);
            request_dongle(coder, second);
            compile(coder, args);
        }
        release(coder, args.dongle_cooldown);
        debugging(coder, args);
        refactoring(coder, args);
    }
    return NULL;
}
