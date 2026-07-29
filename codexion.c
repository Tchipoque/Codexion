/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 22:46:05 by etchipoq          #+#    #+#             */
/*   Updated: 2026/07/28 21:52:46 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>

/**
 * Prints the final simulation summary.
 */
static void	print_simulation_summary(t_sim *sim)
{
	long	i;
	long	c;

	i = 0;
	printf("-----------------------------------\n");
	printf("RESUME:\n");
	printf("N of coders: %d\n", sim->args.number_of_coders);
	printf("N of compiles: %d\n", sim->args.number_of_compiles_required);
	printf("-----------------------------------\n");
	while (i < sim->args.number_of_coders)
	{
		c = i + 1;
		printf("Coder %ld : %d compiles\n", c, sim->coders[i].compile_count);
		i++;
	}
	i = (get_time_ms() - sim->start_time) / 1000;
	printf("-----------------------------------\n");
	if (sim->burned_out)
		printf("Unsuccessfully finished cycles in %ld seconds.\n", i);
	else
		printf("Successfully finished cycles in %ld seconds.\n", i);
	printf("-----------------------------------\n");
}

/**
 * Program entry point.
 */
int	main(int argc, char **argv)
{
	t_sim	*sim;
	int		i;

	i = 0;
	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (0);
	if (!validate_arguments(argc, argv))
		return (0);
	initialize_simulation(argv, sim);
	printf("Simulation initialized\n");
	printf("-----------------------------------\n");
	create_coder_threads(sim);
	start_monitor_thread(sim);
	while (i < sim->args.number_of_coders)
		pthread_join(sim->coders[i++].thread, NULL);
	pthread_join(sim->monitor, NULL);
	print_simulation_summary(sim);
	destroy_simulation(sim);
	return (0);
}
