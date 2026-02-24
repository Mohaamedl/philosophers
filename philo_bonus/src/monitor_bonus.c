/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

/*
** @brief: Check if philosopher has died
** @param: philo - pointer to philosopher to check
** @return: true if dead, false otherwise
** 
** Implementation:
**   1. Get current time
**   2. Calculate time since last meal
**   3. Compare with time_to_die
** 
** No mutex needed - each process monitors only itself
** last_meal_time is local to this process
*/
bool	is_philosopher_dead(t_philo *philo)
{
	long	current_time;
	long	time_since_meal;

	pthread_mutex_lock(&philo->meal_lock);
	current_time = get_time_ms();
	time_since_meal = current_time - philo->last_meal_time;
	pthread_mutex_unlock(&philo->meal_lock);
	return (time_since_meal >= philo->table->time_to_die);
}

/*
** @brief: Monitor thread routine (runs inside each process)
** @param: arg - pointer to philosopher (void* cast)
** @return: NULL
** 
** Implementation:
**   1. Cast arg to t_philo*
**   2. Loop: check if philosopher is dead
**   3. If dead: announce death and exit process
**   4. If all meals eaten: exit process successfully
**   5. Sleep ~2ms before next check
** 
** Each process has its own monitor thread
** Monitors only the local philosopher
*/
void	*monitor_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while (1)
	{
		usleep(2000);
		if (is_philosopher_dead(philo))
		{
			announce_death(philo);
			exit(1);
		}
	}
	return (NULL);
}

/*
** @brief: Start monitor thread for this process
** @param: philo - pointer to philosopher
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Create detached thread
**   2. Thread runs monitor_routine
** 
** Thread is detached - no need to join
*/
int	start_monitor(t_philo *philo)
{
	pthread_t	monitor;

	if (pthread_create(&monitor, NULL, monitor_routine, philo) != 0)
	{
		printf("Error: Failed to create monitor thread\n");
		return (1);
	}
	pthread_detach(monitor);
	return (0);
}
