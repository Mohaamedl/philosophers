/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/10/30 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

/*
** @brief: Helper to destroy all table mutexes
** @param: table - pointer to table structure
** @return: void
*/
static void	destroy_table_mutexes(t_table *table)
{
	pthread_mutex_destroy(&table->write_lock);
	pthread_mutex_destroy(&table->meal_lock);
	pthread_mutex_destroy(&table->sim_lock);
}

/*
** @brief: Initializes fork mutexes
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
*/
static int	init_fork_mutexes(t_table *table)
{
	int	i;

	table->forks = malloc(sizeof(pthread_mutex_t) * table->philo_count);
	if (!table->forks)
	{
		printf("Error: Failed to allocate fork mutexes\n");
		return (1);
	}
	i = 0;
	while (i < table->philo_count)
	{
		if (pthread_mutex_init(&table->forks[i], NULL) != 0)
		{
			printf("Error: Failed to initialize fork mutex %d\n", i);
			while (--i >= 0)
				pthread_mutex_destroy(&table->forks[i]);
			free(table->forks);
			return (1);
		}
		i++;
	}
	return (0);
}

/*
** @brief: Initializes all mutexes in the table
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
*/
int	init_mutexes(t_table *table)
{
	if (pthread_mutex_init(&table->write_lock, NULL) != 0)
		return (printf("Error: Failed to initialize write_lock\n"), 1);
	if (pthread_mutex_init(&table->meal_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&table->write_lock);
		return (printf("Error: Failed to initialize meal_lock\n"), 1);
	}
	if (pthread_mutex_init(&table->sim_lock, NULL) != 0)
	{
		destroy_table_mutexes(table);
		return (printf("Error: Failed to initialize sim_lock\n"), 1);
	}
	if (init_fork_mutexes(table) != 0)
	{
		destroy_table_mutexes(table);
		return (1);
	}
	return (0);
}

/*
** @brief: Allocates and initializes philosopher array
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Allocate memory for philos array
**   2. Initialize each philosopher's basic data (id, meals_count)
**   3. Assign left and right fork pointers (circular pattern)
**   4. Set table reference for each philosopher
**   5. Initialize last_meal_time to 0 (will be set at simulation start)
**   6. Handle memory allocation failures
** 
** Fork assignment strategy:
**   - Left fork: philosopher's own fork (index = id - 1)
**   - Right fork: next philosopher's fork (index = id % philo_count)
**   - This creates a circular arrangement
*/
int	init_philosophers(t_table *table)
{
	int	i;

	table->philos = malloc(sizeof(t_philo) * table->philo_count);
	if (!table->philos)
	{
		printf("Error: Failed to allocate philosophers array\n");
		return (1);
	}
	i = 0;
	while (i < table->philo_count)
	{
		table->philos[i].id = i + 1;
		table->philos[i].meals_count = 0;
		table->philos[i].last_meal_time = table->start_time;
		table->philos[i].left_fork = &table->forks[i];
		table->philos[i].right_fork = &table->forks[(i + 1)
			% table->philo_count];
		table->philos[i].table = table;
		i++;
	}
	return (0);
}

/*
** @brief: Main initialization function
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Set simulation_end to false (simulation starts running)
**   2. Record start_time with get_time_ms() for timestamp calculation
**   3. Call init_mutexes() to set up all mutex locks
**   4. Call init_philosophers() to create philosopher array
**   5. Handle any initialization failures with proper cleanup
** 
** Note: This assumes parse_arguments() has already been called
** and table contains valid philo_count and timing values.
*/
int	init_table(t_table *table)
{
	table->simulation_end = false;
	table->start_time = get_time_ms();
	if (table->start_time == -1)
	{
		printf("Error: Failed to get start time\n");
		return (1);
	}
	if (init_mutexes(table) != 0)
		return (1);
	if (init_philosophers(table) != 0)
	{
		cleanup_table(table);
		return (1);
	}
	return (0);
}
