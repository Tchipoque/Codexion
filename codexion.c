/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 22:46:05 by etchipoq          #+#    #+#             */
/*   Updated: 2026/07/16 22:51:56 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>

void print_briefing(t_sim* sim)
{
    int i;

    i = 0;
    printf("-----------------------------------\n");
    printf("RESUME:\n");
    printf("N of coders: %d\n", sim->args.number_of_coders);
    printf("N of compiles: %d\n", sim->args.number_of_compiles_required);
    printf("-----------------------------------\n");
    while(i < sim->args.number_of_coders)
    {
        printf("Coder %d : %d compiles\n", i, sim->coders[i].compile_count);
        i++;
    }
    printf("-----------------------------------\n");
    printf("Finished in %ld seconds\n", (gettimelog() - sim->start_time) / 1000);
    printf("-----------------------------------\n");

}

int main(int argc, char** argv)
{
    t_sim *sim;
    int i;

    i = 0;
    sim = malloc(sizeof(t_sim));
    if (!sim)
        return 0;

    if (!validate(argc, argv))
        return 0;

    iniciate(argv, sim);


    printf("Simulation initialized\n");
    printf("-----------------------------------\n");

    create_thread(sim);
    create_monitor(sim);

    while(i < sim->args.number_of_coders)
        pthread_join(sim->coders[i++].thread, NULL);

    pthread_join(sim->monitor, NULL);

    print_briefing(sim);
    destroy_all(sim);
    return(0);

}
