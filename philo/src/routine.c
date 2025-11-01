/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

/*
** @brief: Main philosopher thread routine
** @param: arg - pointer to philosopher (void* cast)
** @return: NULL
** 
** Implementation:
**   1. Cast arg to t_philo*
**   2. Initialize last_meal_time to simulation start
**   3. Handle single philosopher special case
**   4. Main loop: check simulation_end flag
**   5. Execute cycle: take_forks -> eat -> drop_forks -> sleep -> think
**   6. Break loop if simulation ends
** 
** Algorithm: Resource Hierarchy (Dijkstra)
**   - No artificial delays needed (unlike even/odd approach)
**   - Fork ordering in take_forks() prevents deadlock
**   - All philosophers start immediately for better performance
*/
void	*philosopher_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(&philo->table->meal_lock);
	philo->last_meal_time = philo->table->start_time;
	pthread_mutex_unlock(&philo->table->meal_lock);
	if (philo->table->philo_count == 1)
	{
		take_forks(philo);
		while (!should_end_simulation(philo->table))
			usleep(100);
		return (NULL);
	}
	while (!should_end_simulation(philo->table))
	{
		take_forks(philo);
		eat_action(philo);
		drop_forks(philo);
		sleep_action(philo);
		think_action(philo);
	}
	return (NULL);
}

/*
** @brief: Creates all philosopher threads
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Loop through all philosophers
**   2. Create thread for each with pthread_create
**   3. Pass philosopher pointer as argument
**   4. Handle thread creation failures
**   5. Store thread ID in philosopher structure
** 
** Error handling:
**   If thread creation fails, set simulation_end flag
**   and return error code.
*/
int	create_threads(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->philo_count)
	{
		if (pthread_create(&table->philos[i].thread, NULL,
				philosopher_routine, &table->philos[i]) != 0)
		{
			printf("Error: Failed to create thread for philosopher %d\n",
				i + 1);
			end_simulation(table);
			return (1);
		}
		i++;
	}
	return (0);
}

/*
** @brief: Waits for all philosopher threads to complete
** @param: table - pointer to table structure
** @return: void
** 
** Implementation:
**   1. Loop through all philosophers
**   2. Call pthread_join for each thread
**   3. Handle join failures appropriately
** 
** Note: This blocks until all philosopher threads have finished
** their execution (after simulation_end is set).
*/
void	join_threads(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->philo_count)
	{
		pthread_join(table->philos[i].thread, NULL);
		i++;
	}
}

/*
** @brief: Waits for monitor thread to complete
** @param: table - pointer to table structure
** @return: void
** 
** Note: Called after monitor detects death or completion.
*/
void	join_monitor(t_table *table)
{
	pthread_join(table->monitor, NULL);
}
