/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

/*
** @brief: Initialize semaphores for the simulation
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Unlink any existing semaphores with same names
**   2. Create forks semaphore (value = philo_count)
**   3. Create write semaphore for output (value = 1)
**   4. Create dead semaphore for death signaling (value = 0)
** 
** Semaphore names must be unique and start with /
** Forks semaphore acts as counting semaphore (multiple forks available)
*/
int	init_semaphores(t_table *table)
{
	sem_unlink("/forks");
	sem_unlink("/write");
	sem_unlink("/dead");
	table->forks = sem_open("/forks", O_CREAT | O_EXCL, 0644,
			table->philo_count);
	if (table->forks == SEM_FAILED)
	{
		printf("Error: Failed to create forks semaphore\n");
		return (1);
	}
	table->write_sem = sem_open("/write", O_CREAT | O_EXCL, 0644, 1);
	if (table->write_sem == SEM_FAILED)
	{
		printf("Error: Failed to create write semaphore\n");
		sem_close(table->forks);
		sem_unlink("/forks");
		return (1);
	}
	table->dead_sem = sem_open("/dead", O_CREAT | O_EXCL, 0644, 0);
	if (table->dead_sem == SEM_FAILED)
	{
		printf("Error: Failed to create dead semaphore\n");
		return (1);
	}
	return (0);
}

/*
** @brief: Initialize philosopher structures
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Allocate array of philosophers
**   2. Initialize each philosopher's data
**   3. Set table reference in each philosopher
** 
** Note: PIDs will be set when processes are forked
*/
int	init_philosophers(t_table *table)
{
	int	i;

	table->philos = malloc(sizeof(t_philo) * table->philo_count);
	if (!table->philos)
	{
		printf("Error: Failed to allocate philosophers\n");
		return (1);
	}
	i = 0;
	while (i < table->philo_count)
	{
		table->philos[i].id = i + 1;
		table->philos[i].meals_count = 0;
		table->philos[i].last_meal_time = 0;
		table->philos[i].pid = 0;
		table->philos[i].table = table;
		i++;
	}
	return (0);
}

/*
** @brief: Initialize the table structure
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Record simulation start time
**   2. Initialize semaphores
**   3. Initialize philosopher structures
** 
** Cleanup on error to prevent resource leaks
*/
int	init_table(t_table *table)
{
	table->start_time = get_time_ms();
	if (init_semaphores(table) != 0)
		return (1);
	if (init_philosophers(table) != 0)
	{
		cleanup_semaphores(table);
		return (1);
	}
	return (0);
}
