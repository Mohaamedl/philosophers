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
**   1. Lock write_lock mutex for exclusive output access
**   2. Check if simulation has ended (no output after death)
**   3. Get current timestamp with elapsed_time()
**   4. Print formatted message: "timestamp id message"
**   5. Unlock write_lock mutex
** 
** Format: [timestamp_in_ms] [philosopher_id] [message]
** Example: 142 3 has taken a fork
*/
void	safe_print(t_philo *philo, char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&philo->table->write_lock);
	if (!philo->table->simulation_end)
	{
		timestamp = elapsed_time(philo->table->start_time);
		printf("%ld %d %s\n", timestamp, philo->id, msg);
	}
	pthread_mutex_unlock(&philo->table->write_lock);
}

/*
** @brief: Thread-safe death announcement
** @param: philo - philosopher who died
** @return: void
** 
** Implementation:
**   1. Lock write_lock mutex for exclusive output access
**   2. Print death message with timestamp
**   3. Set simulation_end flag to true
**   4. Unlock write_lock mutex
** 
** This ensures death message is printed exactly once and
** no other messages are printed after death.
*/
void	announce_death(t_philo *philo)
{
	long	timestamp;

	pthread_mutex_lock(&philo->table->write_lock);
	timestamp = elapsed_time(philo->table->start_time);
	printf("%ld %d died\n", timestamp, philo->id);
	philo->table->simulation_end = true;
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
