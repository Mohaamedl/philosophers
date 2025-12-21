/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/time.h>
# include <pthread.h>
# include <semaphore.h>
# include <signal.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <stdbool.h>

/*
** Bonus part uses:
** - Processes (fork()) instead of threads
** - Semaphores (sem_open, sem_wait, sem_post) instead of mutexes
** - Each philosopher is a separate process
** - Each process has its own monitor thread
** - Main process waits for first death or completion
*/

typedef struct s_table	t_table;

typedef struct s_philo
{
	int					id;
	int					meals_count;
	long				last_meal_time;
	pid_t				pid;
	pthread_t			monitor;
	t_table				*table;
}	t_philo;

typedef struct s_table
{
	int					philo_count;
	long				time_to_die;
	long				time_to_eat;
	long				time_to_sleep;
	int					must_eat_count;
	long				start_time;
	sem_t				*forks;
	sem_t				*write_sem;
	sem_t				*dead_sem;
	t_philo				*philos;
}	t_table;

/* ************************************************************************** */
/*                           PARSING FUNCTIONS                                */
/* ************************************************************************** */
int		validate_args(int argc, char **argv);
int		ft_atoi_positive(const char *str);
int		parse_arguments(t_table *table, int argc, char **argv);

/* ************************************************************************** */
/*                            TIME FUNCTIONS                                  */
/* ************************************************************************** */
long	get_time_ms(void);
long	elapsed_time(long start_time);
void	precise_sleep(long duration);
void	smart_sleep(long duration);

/* ************************************************************************** */
/*                    INITIALIZATION & CLEANUP FUNCTIONS                      */
/* ************************************************************************** */
int		init_semaphores(t_table *table);
int		init_philosophers(t_table *table);
int		init_table(t_table *table);
void	cleanup_table(t_table *table);
void	cleanup_semaphores(t_table *table);

/* ************************************************************************** */
/*                      SYNCHRONIZATION FUNCTIONS                             */
/* ************************************************************************** */
void	safe_print(t_philo *philo, char *msg);
void	announce_death(t_philo *philo);

/* ************************************************************************** */
/*                       PHILOSOPHER ACTIONS                                  */
/* ************************************************************************** */
void	take_forks(t_philo *philo);
void	eat_action(t_philo *philo);
void	drop_forks(t_philo *philo);
void	sleep_action(t_philo *philo);
void	think_action(t_philo *philo);

/* ************************************************************************** */
/*                       PROCESS ROUTINE                                      */
/* ************************************************************************** */
void	philosopher_process(t_philo *philo);
int		create_processes(t_table *table);
void	wait_processes(t_table *table);
void	kill_all_processes(t_table *table);

/* ************************************************************************** */
/*                         MONITOR FUNCTIONS                                  */
/* ************************************************************************** */
bool	is_philosopher_dead(t_philo *philo);
void	*monitor_routine(void *arg);
int		start_monitor(t_philo *philo);

#endif
