/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sync_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

/*
** @brief: Thread-safe output function using semaphore
** @param: philo - philosopher performing action, msg - message to print
** @return: void
** 
** Implementation:
**   1. Wait on write semaphore for exclusive access
**   2. Get current timestamp
**   3. Print formatted message
**   4. Post write semaphore
** 
** No simulation_end check needed - processes exit on death
*/
void	safe_print(t_philo *philo, char *msg)
{
	long	timestamp;

	sem_wait(philo->table->write_sem);
	timestamp = elapsed_time(philo->table->start_time);
	printf("%ld %d %s\n", timestamp, philo->id, msg);
	sem_post(philo->table->write_sem);
}

/*
** @brief: Announce philosopher death
** @param: philo - philosopher who died
** @return: void
** 
** Implementation:
**   1. Wait on write semaphore
**   2. Print death message
**   3. Post write semaphore
**   4. Post dead semaphore to signal main process
** 
** After posting dead_sem, main process will kill all others
*/
void	announce_death(t_philo *philo)
{
	long	timestamp;

	sem_wait(philo->table->write_sem);
	timestamp = elapsed_time(philo->table->start_time);
	printf("%ld %d died\n", timestamp, philo->id);
	sem_post(philo->table->write_sem);
	sem_post(philo->table->dead_sem);
}
