/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

/*
** @brief: Checks if a philosopher has died (starvation)
** @param: philo - pointer to philosopher to check
** @param: current_time - current timestamp in milliseconds
** @return: true if philosopher is dead, false otherwise
** 
** Thread-safety: Protected by meal_lock mutex
** Performance: Called every ~1ms by monitor thread
*/
bool	is_philosopher_dead(t_philo *philo, long current_time)
{
	long	time_since_meal;
	bool	is_dead;

	pthread_mutex_lock(&philo->table->meal_lock);
	time_since_meal = current_time - philo->last_meal_time;
	is_dead = (time_since_meal >= philo->table->time_to_die);
	pthread_mutex_unlock(&philo->table->meal_lock);
	return (is_dead);
}

/*
** @brief: Checks if all philosophers have eaten required meals
** @param: table - pointer to table structure
** @return: true if all satisfied, false otherwise
** 
** Thread-safety: Protected by meal_lock mutex
*/
bool	all_philosophers_satisfied(t_table *table)
{
	int		i;
	bool	all_satisfied;

	if (table->must_eat_count <= 0)
		return (false);
	pthread_mutex_lock(&table->meal_lock);
	all_satisfied = true;
	i = 0;
	while (i < table->philo_count)
	{
		if (table->philos[i].meals_count < table->must_eat_count)
		{
			all_satisfied = false;
			break ;
		}
		i++;
	}
	pthread_mutex_unlock(&table->meal_lock);
	return (all_satisfied);
}

/*
** @brief: Check all philosophers for death
** @param: table - pointer to table structure
** @param: current_time - current timestamp
** @return: true if death detected, false otherwise
*/
static bool	check_deaths(t_table *table, long current_time)
{
	int	i;

	i = 0;
	while (i < table->philo_count)
	{
		if (is_philosopher_dead(&table->philos[i], current_time))
		{
			announce_death(&table->philos[i]);
			return (true);
		}
		i++;
	}
	return (false);
}

/*
** @brief: Main monitor thread routine
** @param: arg - pointer to table structure (void* cast)
** @return: NULL
** 
** Performance: ~1ms sleep = <10ms death detection, ~1-2% CPU
*/
void	*monitor_routine(void *arg)
{
	t_table	*table;

	table = (t_table *)arg;
	while (!should_end_simulation(table))
	{
		if (check_deaths(table, get_time_ms()))
			return (NULL);
		if (all_philosophers_satisfied(table))
		{
			end_simulation(table);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

/*
** @brief: Starts the monitor thread
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Create monitor thread with pthread_create
**   2. Pass table pointer as argument to monitor_routine
**   3. Handle thread creation failure
**   4. Store thread ID in table->monitor
** 
** Error handling:
**   If thread creation fails, print error and return 1
** 
** Note: Monitor thread runs concurrently with philosopher threads
** and continuously checks for death/completion conditions.
*/
int	start_monitor(t_table *table)
{
	if (pthread_create(&table->monitor, NULL, monitor_routine, table) != 0)
	{
		printf("Error: Failed to create monitor thread\n");
		end_simulation(table);
		return (1);
	}
	return (0);
}
