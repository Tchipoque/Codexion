#include "../codexion.h"

int validate(int argc, char **args)
{
    int i;
    char* parameters[] = {
        "number_of_coders", "time_to_burnout", "time_to_compile",
        "time_to_debug", "time_to_refactor", "number_of_compiles_required",
        "dongle_cooldown", "scheduler"
    };

    i = 1;
    if (argc != 9) {
        printf("ERROR, program is expecting 8 arguments\n");
        exit (1);
    }

    while(args[i]) {
        if (i == 8) {
            if ((strcmp(args[i], "fifo") != 0) && (strcmp(args[i], "edf") != 0))
            {
                printf("ERROR, %s has to be fifo or edf\n", parameters[i - 1]);
                exit (1);
            }
        }
        else if (!(ft_isnumber(args[i])) || atoi(args[i]) <= 0) {
            printf("ERROR, %s has to be a positive integer and 0 < \n", parameters[i - 1]);
            exit (1);
        }
        i++;
    }
    return 1;
}

t_coder assign_coder(int id, t_sim* sim, int d_c)
{
	t_coder coder;

	coder.id = id;
	coder.compile_count = 0;
	coder.last_compile_start = 0;
	coder.left_dongle = &sim->dongles[(id - 1 + d_c) % d_c];
	coder.right_dongle = &sim->dongles[id];
	coder.sim = sim;

	return coder;
}

t_dongle assign_dongles(int id)
{
	t_dongle dongle;

    dongle.id = id;
    pthread_mutex_init(&dongle.mutex, NULL);
	dongle.available = 0;
	dongle.avaible_at = 0;

	return dongle;
}

void create_thread(t_sim *simualtor)
{
	int	i;
	t_coder *coders;

	coders = simualtor->coders;
	i = 0;
	while(i < simualtor->args.number_of_coders)
	{
		pthread_create(&coders[i].thread, NULL, engine, &coders[i]);
		i++;
	}
}
int iniciate(char **args, t_sim *simulator)
{
	t_args	parameters;
	int	i;
	int	c;

	c = -1;
	parameters = (t_args){
		atoi(args[1]), atol(args[2]), atol(args[3]),
		atol(args[4]), atol(args[5]), atoi(args[6]),
		atol(args[7]), scheduler(args[8])
	};
	simulator->args = parameters;
	simulator->start_time = gettimelog();
	simulator->dongles = malloc(sizeof(t_dongle) * parameters.number_of_coders);
	if (!simulator->dongles)
		return 0;
	while(++c < parameters.number_of_coders)
		simulator->dongles[c] = assign_dongles(c);

    i = -1;
    simulator->coders = malloc(sizeof(t_coder) * parameters.number_of_coders);
	if (!simulator->coders)
		return free(simulator->dongles), 0;
	while(++i < parameters.number_of_coders)
		simulator->coders[i] = assign_coder(i, simulator, c);

    return 1;
}

