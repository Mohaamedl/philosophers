/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 22:21:45 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/09/24 22:24:55 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/time.h>
# include <pthread.h>

/*
** Allowed functions: memset, printf, malloc, free, write, usleep, gettimeofday
** pthread_create, pthread_detach, pthread_join, pthread_mutex_init,
** pthread_mutex_destroy, pthread_mutex_lock, pthread_mutex_unlock
*/

typedef struct s_philo	t_philo;
typedef struct s_table	t_table;

typedef struct s_philo
{
	int								id;
	int								meals_count;
	long							last_meal_time;
	pthread_t					thread;
	pthread_mutex_t		*left_fork;
	pthread_mutex_t		*right_fork;
	t_table						*table;
}	t_philo;

typedef struct s_table
{
	int								philo_count;
	long							time_to_die;
	long							time_to_eat;
	long							time_to_sleep;
	int								must_eat_count;
	long							start_time;
	bool							simulation_end;
	pthread_mutex_t		*forks;
	pthread_mutex_t		write_lock;
	pthread_mutex_t		meal_lock;
	pthread_mutex_t		sim_lock;
	pthread_t					monitor;
	t_philo						*philos;
}	t_table;

#endif
