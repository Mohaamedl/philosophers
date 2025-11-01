/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/10/30 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

/*
** @brief: Cleans up all allocated resources
** @param: table - pointer to table structure
** @return: void
** 
** Implementation:
**   1. Destroy all fork mutexes if they exist
**   2. Destroy other mutexes (write, meal, sim) if initialized
**   3. Free philosophers array if allocated
**   4. Free forks array if allocated
**   5. Reset all pointers to NULL for safety
** 
** Note: This function should be safe to call even if
** initialization was only partially completed.
*/
void	cleanup_table(t_table *table)
{
	int	i;

	if (table->forks)
	{
		i = 0;
		while (i < table->philo_count)
		{
			pthread_mutex_destroy(&table->forks[i]);
			i++;
		}
		free(table->forks);
		table->forks = NULL;
	}
	pthread_mutex_destroy(&table->write_lock);
	pthread_mutex_destroy(&table->meal_lock);
	pthread_mutex_destroy(&table->sim_lock);
	if (table->philos)
	{
		free(table->philos);
		table->philos = NULL;
	}
}
