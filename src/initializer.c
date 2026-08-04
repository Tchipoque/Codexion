/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initializer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:34:31 by etchipoq          #+#    #+#             */
/*   Updated: 2026/08/03 21:23:19 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Initializes one coder and links its adjacent dongles.
 */
static t_coder	initialize_coder(int id, t_sim *sim)
{
	t_coder	coder;
	int		left;

	left = id - 1;
	if (left < 1)
		left = sim->args.number_of_coders;
	coder.id = id;
	coder.compile_count = 0;
	coder.go = 0;
	coder.last_compile_start = get_time_ms();
	coder.left_dongle = &sim->dongles[left];
	coder.right_dongle = &sim->dongles[id];
	pthread_mutex_init(&coder.state, NULL);
	coder.sim = sim;
	return (coder);
}

/**
 * Initializes one dongle.
 */
static t_dongle	initialize_dongle(int id)
{
	t_dongle	dongle;

	dongle.id = id;
	dongle.available_at = get_time_ms();
	pthread_mutex_init(&dongle.mutex, NULL);
	return (dongle);
}

/**
 * Creates one worker thread per coder.
 */
void	create_coder_threads(t_sim *simualtor)
{
	int		i;
	t_coder	*coders;

	coders = simualtor->coders;
	i = 1;
	while (i <= simualtor->args.number_of_coders)
	{
		pthread_create(&coders[i].thread, NULL, run_coder_cycle, &coders[i]);
		i++;
	}
}

/**
 * Initializes the shared simulation state after allocations are ready.
 */
static int	initialize_simulation_state(t_sim *simulator)
{
	int	i;

	simulator->stop = 0;
	simulator->burned_out = 0;
	simulator->start_time = get_time_ms();
	pthread_mutex_init(&simulator->queue_mutex, NULL);
	pthread_cond_init(&simulator->cond, NULL);
	i = 0;
	while (++i <= simulator->args.number_of_coders)
		simulator->dongles[i] = initialize_dongle(i);
	i = 0;
	while (++i <= simulator->args.number_of_coders)
		simulator->coders[i] = initialize_coder(i, simulator);
	i = 0;
	while (++i <= simulator->args.number_of_coders)
		simulator->queue[i] = -1;
	return (1);
}

/**
 * Allocates and initializes the full simulation state.
 */
int	initialize_simulation(char **args, t_sim *simulator)
{
	t_args	parameters;
	size_t	n;
	size_t	n_plus;

	parameters = (t_args){atoi(args[1]), atol(args[2]), atol(args[3]),
		atol(args[4]), atol(args[5]), atoi(args[6]), atol(args[7]),
		is_fifo_scheduler(args[8])};
	simulator->args = parameters;
	n = (size_t)parameters.number_of_coders;
	n_plus = n + 1;
	if (n_plus > SIZE_MAX / sizeof(t_dongle))
		return (0);
	simulator->dongles = malloc(sizeof(t_dongle) * n_plus);
	if (!simulator->dongles)
		return (0);
	if (n_plus > SIZE_MAX / sizeof(t_coder))
		return (free(simulator->dongles), 0);
	simulator->coders = malloc(sizeof(t_coder) * n_plus);
	if (!simulator->coders)
		return (free(simulator->dongles), 0);
	simulator->queue = malloc(sizeof(int) * n_plus);
	if (!simulator->queue)
		return (free(simulator->dongles), free(simulator->coders), 0);
	return (initialize_simulation_state(simulator));
}
