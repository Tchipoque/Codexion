/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 22:46:05 by etchipoq          #+#    #+#             */
/*   Updated: 2026/07/10 18:04:13 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>


int main(int argc, char** argv)
{
    t_sim *sim;

    sim = malloc(sizeof(t_sim));
    if (!sim)
        return 0;

    if (!validate(argc, argv))
        return 0;

    iniciate(argv, sim);


    printf("Simulation initialized\n");
    printf("Number of coders: %s \n", argv[1]);
    printf("-----------------------------------\n");

    for (int i = 0; i < sim->args.number_of_coders; i++)
        pthread_join(sim->coders[i].thread, NULL);

    destroy_all(sim);
    return(0);


}
