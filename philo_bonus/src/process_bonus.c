/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

/*
** @brief: Main philosopher process routine
** @param: philo - pointer to philosopher
** @return: void (exits process)
** 
** Implementation:
**   1. Initialize last_meal_time
**   2. Start monitor thread for death detection
**   3. Main loop: take_forks -> eat -> drop -> sleep -> think
**   4. Exit when monitor detects death or meal completion
** 
** This function runs in a child process
** Each philosopher is completely independent
*/
void	philosopher_process(t_philo *philo)
{
	if (pthread_mutex_init(&philo->meal_lock, NULL) != 0)
		exit(1);
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = philo->table->start_time;
	pthread_mutex_unlock(&philo->meal_lock);
	if (start_monitor(philo) != 0)
		exit(1);
	if (philo->table->philo_count == 1)
	{
		safe_print(philo, "has taken a fork");
		while (1)
			usleep(100000);
	}
	if (philo->id % 2 == 0)
		usleep(15000);
	while (1)
	{
		take_forks(philo);
		eat_action(philo);
		drop_forks(philo);
		if (philo->table->must_eat_count > 0
			&& philo->meals_count >= philo->table->must_eat_count)
			exit(0);
		sleep_action(philo);
		think_action(philo);
	}
}

/*
** @brief: Fork all philosopher processes
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Loop through all philosophers
**   2. Fork a new process for each
**   3. Child: run philosopher_process()
**   4. Parent: store child PID and continue
** 
** Error handling: kill all processes if fork fails
*/
int	create_processes(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->philo_count)
	{
		table->philos[i].pid = fork();
		if (table->philos[i].pid < 0)
		{
			printf("Error: Failed to fork process %d\n", i + 1);
			kill_all_processes(table);
			return (1);
		}
		if (table->philos[i].pid == 0)
			philosopher_process(&table->philos[i]);
		i++;
	}
	return (0);
}

/*
** @brief: Wait for all processes or first death
** @param: table - pointer to table structure
** @return: void
** 
** Implementation:
**   1. Wait for any child process to exit
**   2. If exit status indicates death, kill all others
**   3. Reap all remaining processes
** 
** Uses waitpid to detect process exits
*/
void	wait_processes(t_table *table)
{
	int	i;
	int	status;
	int	exited_count;

	exited_count = 0;
	while (exited_count < table->philo_count)
	{
		waitpid(-1, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		{
			i = 0;
			while (i < table->philo_count)
			{
				kill(table->philos[i].pid, SIGKILL);
				i++;
			}
			break ;
		}
		exited_count++;
	}
	while (waitpid(-1, NULL, 0) > 0)
		;
}

/*
** @brief: Kill all philosopher processes
** @param: table - pointer to table structure
** @return: void
** 
** Used when fork fails or cleanup needed
*/
void	kill_all_processes(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->philo_count)
	{
		if (table->philos[i].pid > 0)
			kill(table->philos[i].pid, SIGKILL);
		i++;
	}
}
