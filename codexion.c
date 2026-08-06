/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 22:46:05 by etchipoq          #+#    #+#             */
/*   Updated: 2026/08/03 21:21:04 by etchipoq         ###   ########.fr       */
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

	i = 1;
	if (sim->burned_out)
		return ;
	printf("-----------------------------------\n");
	printf("RESUME:\n");
	printf("N of coders: %d\n", sim->args.number_of_coders);
	printf("N of compiles: %d\n", sim->args.number_of_compiles_required);
	printf("-----------------------------------\n");
	while (i <= sim->args.number_of_coders)
	{
		printf("Coder %ld : %d compiles\n", i, sim->coders[i].compile_count);
		i++;
	}
	i = (get_time_ms() - sim->start_time) / 1000;
	printf("-----------------------------------\n");
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

	i = 1;
	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (1);
	if (!validate_arguments(argc, argv))
		return (free(sim), 1);
	if (!initialize_simulation(argv, sim))
		return (free(sim), 1);
	printf("Simulation initialized\n");
	printf("-----------------------------------\n");
	create_coder_threads(sim);
	start_monitor_thread(sim);
	while (i <= sim->args.number_of_coders)
		pthread_join(sim->coders[i++].thread, NULL);
	pthread_join(sim->monitor, NULL);
	print_simulation_summary(sim);
	destroy_simulation(sim);
	return (0);
}
