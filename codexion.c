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


void refactoring(int id)
{
    printf("Coder %d refactoring\n", id);
	usleep(1000000);

}


void debugging(int id)
{
    printf("Coder %d debugging\n", id);
	usleep(1000000);
    refactoring(id);

}

void compiling(int id)
{
    printf("Coder %d compiling\n", id);
	usleep(1000000);
    debugging(id);
}

void take_rdongle(int id)
{
    printf("Coder %d took right dongle\n", id);
	usleep(1000000);
    compiling(id);

}

void take_ldongle(t_coder *coder)
{
    if(coder->left_dongle->available)
    {

    }
	usleep(1000000);
    take_rdongle(id);

}
void *increment_counter(void* arg)
{
	t_coder *coder;
	coder = (t_coder *) arg;

	printf("CREATED C%d\n", coder->id);
	usleep(2000000);
    take_ldongle(coder->id);
    return NULL;
}


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

    int i = 0;


    for (int i = 0; i < sim->args.number_of_coders; i++)
        pthread_join(sim->coders[i].thread, NULL);

    destroy_all(sim);
    return(0);


}
