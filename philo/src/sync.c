/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sync.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

/*
** @brief: Thread-safe output function
** @param: philo - philosopher performing action, msg - message to print
** @return: void
** 
** Implementation:
**   1. Lock sim_lock to check simulation_end flag
**   2. If simulation ended, unlock and return (no output after death)
**   3. Unlock sim_lock
**   4. Lock write_lock mutex for exclusive output access
**   5. Get current timestamp and print message
**   6. Unlock write_lock mutex
** 
** Format: [timestamp_in_ms] [philosopher_id] [message]
*/
void	safe_print(t_philo *philo, char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&philo->table->sim_lock);
	if (philo->table->simulation_end)
	{
		pthread_mutex_unlock(&philo->table->sim_lock);
		return ;
	}
	pthread_mutex_unlock(&philo->table->sim_lock);
	pthread_mutex_lock(&philo->table->write_lock);
	timestamp = elapsed_time(philo->table->start_time);
	printf("%ld %d %s\n", timestamp, philo->id, msg);
	pthread_mutex_unlock(&philo->table->write_lock);
}

/*
** @brief: Thread-safe death announcement
** @param: philo - philosopher who died
** @return: void
** 
** Implementation:
**   1. Lock sim_lock to set simulation_end flag atomically
**   2. Set simulation_end to true
**   3. Unlock sim_lock
**   4. Lock write_lock for exclusive output
**   5. Print death message with timestamp
**   6. Unlock write_lock
** 
** Order matters: set flag first, then print to prevent race.
*/
void	announce_death(t_philo *philo)
{
	long	timestamp;

	pthread_mutex_lock(&philo->table->sim_lock);
	philo->table->simulation_end = true;
	pthread_mutex_unlock(&philo->table->sim_lock);
	pthread_mutex_lock(&philo->table->write_lock);
	timestamp = elapsed_time(philo->table->start_time);
	printf("%ld %d died\n", timestamp, philo->id);
	pthread_mutex_unlock(&philo->table->write_lock);
}

/*
** @brief: Checks if simulation should end
** @param: table - pointer to table structure
** @return: 1 if should end, 0 if continue
** 
** Implementation:
**   1. Lock sim_lock mutex for safe flag reading
**   2. Read simulation_end flag
**   3. Unlock sim_lock mutex
**   4. Return flag value
** 
** This provides thread-safe access to the simulation state.
*/
int	should_end_simulation(t_table *table)
{
	int	result;

	pthread_mutex_lock(&table->sim_lock);
	result = table->simulation_end;
	pthread_mutex_unlock(&table->sim_lock);
	return (result);
}

/*
** @brief: Sets simulation end flag safely
** @param: table - pointer to table structure
** @return: void
** 
** Implementation:
**   1. Lock sim_lock mutex for safe flag writing
**   2. Set simulation_end to true
**   3. Unlock sim_lock mutex
** 
** Used by monitor when stopping conditions are met.
*/
void	end_simulation(t_table *table)
{
	pthread_mutex_lock(&table->sim_lock);
	table->simulation_end = true;
	pthread_mutex_unlock(&table->sim_lock);
}
