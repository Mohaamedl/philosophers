/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

/*
** @brief: Entry point of bonus program
** @param: argc - argument count, argv - argument array
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Parse and validate arguments
**   2. Initialize table and semaphores
**   3. Fork all philosopher processes
**   4. Wait for death or completion
**   5. Clean up resources
** 
** Bonus part uses processes instead of threads
** Each philosopher runs in separate process
** Main process waits for first death
*/
int	main(int argc, char **argv)
{
	t_table	table;

	memset(&table, 0, sizeof(t_table));
	if (parse_arguments(&table, argc, argv) != 0)
		return (1);
	if (init_table(&table) != 0)
		return (1);
	if (create_processes(&table) != 0)
	{
		cleanup_table(&table);
		return (1);
	}
	wait_processes(&table);
	cleanup_table(&table);
	return (0);
}
