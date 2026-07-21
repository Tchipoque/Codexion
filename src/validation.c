/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etchipoq <etchipoq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 23:35:37 by etchipoq          #+#    #+#             */
/*   Updated: 2026/07/22 00:38:26 by etchipoq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/**
 * Validates a single command-line argument.
 */
static int	validate_argument(int i, char *arg, char *parameters[])
{
	if (i == 8)
	{
		if ((strcmp(arg, "fifo") != 0) && (strcmp(arg, "edf") != 0))
		{
			printf("ERROR, %s has to be fifo or edf\n", parameters[i - 1]);
			return (0);
		}
	}
	else if (!(is_numeric_string(arg)) || atoi(arg) <= 0)
	{
		printf("ERROR, %s has to be a positive integer and 0 < \n", parameters[i
			- 1]);
		return (0);
	}
	return (1);
}
/**
 * Validates the full command-line argument list.
 */
int	validate_arguments(int argc, char **args)
{
	int		i;
	char	**parameters;

	i = 0;
	parameters = (char *[]){"number_of_coders", "time_to_burnout",
		"time_to_compile", "time_to_debug", "time_to_refactor",
		"number_of_compiles_required", "dongle_cooldown", "scheduler"};
	if (argc != 9)
	{
		printf("ERROR, program is expecting 8 arguments\n");
		exit(1);
	}
	while (args[++i])
	{
		if (!validate_argument(i, args[i], parameters))
			exit(1);
	}
	return (1);
}
