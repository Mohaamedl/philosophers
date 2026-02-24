/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

/*
** @brief: Philosopher takes two forks using semaphore
** @param: philo - pointer to philosopher
** @return: void
** 
** Implementation:
**   1. Wait on forks semaphore (decrements count)
**   2. Print "has taken a fork"
**   3. Wait on forks semaphore again (for second fork)
**   4. Print "has taken a fork" again
** 
** Semaphore ensures only N forks can be held at once
** Single philosopher will deadlock (by design, as in mandatory part)
*/
void	take_forks(t_philo *philo)
{
	sem_wait(philo->table->forks);
	safe_print(philo, "has taken a fork");
	sem_wait(philo->table->forks);
	safe_print(philo, "has taken a fork");
}

/*
** @brief: Philosopher eating action
** @param: philo - pointer to philosopher
** @return: void
** 
** Implementation:
**   1. Print "is eating" message
**   2. Update last_meal_time to current time
**   3. Increment meals_count
**   4. Sleep for time_to_eat duration
** 
** Note: last_meal_time is local to process, no mutex needed
*/
void	eat_action(t_philo *philo)
{
	safe_print(philo, "is eating");
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = get_time_ms();
	philo->meals_count++;
	pthread_mutex_unlock(&philo->meal_lock);
	smart_sleep(philo->table->time_to_eat);
}

/*
** @brief: Philosopher releases both forks
** @param: philo - pointer to philosopher
** @return: void
** 
** Implementation:
**   1. Post forks semaphore (increments count)
**   2. Post forks semaphore again (for second fork)
** 
** Makes forks available for other processes
*/
void	drop_forks(t_philo *philo)
{
	sem_post(philo->table->forks);
	sem_post(philo->table->forks);
}

/*
** @brief: Philosopher sleeping action
** @param: philo - pointer to philosopher
** @return: void
** 
** Implementation:
**   1. Print "is sleeping" message
**   2. Sleep for time_to_sleep duration
*/
void	sleep_action(t_philo *philo)
{
	safe_print(philo, "is sleeping");
	smart_sleep(philo->table->time_to_sleep);
}

/*
** @brief: Philosopher thinking action
** @param: philo - pointer to philosopher
** @return: void
** 
** Implementation:
**   1. Print "is thinking" message
**   2. Add adaptive thinking time for odd counts
** 
** Prevents starvation with odd philosopher counts
*/
void	think_action(t_philo *philo)
{
	long	think_time;

	safe_print(philo, "is thinking");
	if (philo->table->philo_count % 2 != 0)
	{
		think_time = (philo->table->time_to_eat * 2)
			- philo->table->time_to_sleep;
		if (think_time < 0)
			think_time = 0;
		if (think_time > 600)
			think_time = 200;
		if (think_time > 0)
			usleep(think_time * 1000);
	}
}
