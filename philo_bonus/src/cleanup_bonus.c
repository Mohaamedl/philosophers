/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

/*
** @brief: Clean up and unlink all semaphores
** @param: table - pointer to table structure
** @return: void
** 
** Implementation:
**   1. Close semaphore file descriptors
**   2. Unlink semaphores from system
** 
** Must be called to prevent semaphore leaks
*/
void	cleanup_semaphores(t_table *table)
{
	if (table->forks)
	{
		sem_close(table->forks);
		sem_unlink("/forks");
	}
	if (table->write_sem)
	{
		sem_close(table->write_sem);
		sem_unlink("/write");
	}
	if (table->dead_sem)
	{
		sem_close(table->dead_sem);
		sem_unlink("/dead");
	}
}

/*
** @brief: Clean up all resources
** @param: table - pointer to table structure
** @return: void
** 
** Implementation:
**   1. Free philosopher array
**   2. Clean up semaphores
** 
** Called at program exit
*/
void	cleanup_table(t_table *table)
{
	if (table->philos)
		free(table->philos);
	cleanup_semaphores(table);
}
