/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/10/27 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

/*
** @brief: Gets current timestamp in milliseconds
** @param: void
** @return: timestamp in ms since epoch
** 
** Implementation:
**   1. Call gettimeofday() to get current time
**   2. Convert tv_sec (seconds) to milliseconds (* 1000)
**   3. Add tv_usec (microseconds) converted to milliseconds (/ 1000)
**   4. Return total as long
*/
long	get_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/*
** @brief: Calculates elapsed time since start
** @param: start_time - reference timestamp in milliseconds
** @return: elapsed time in milliseconds
** 
** Implementation:
**   1. Get current time with get_time_ms()
**   2. Subtract start_time
**   3. Return difference
*/
long	elapsed_time(long start_time)
{
	return (get_time_ms() - start_time);
}

/*
** @brief: Precise sleep function using usleep
** @param: duration - sleep time in milliseconds
** @return: void
** 
** Implementation:
**   1. Convert ms to microseconds (ms * 1000)
**   2. Use usleep() for the duration
**   3. Handle potential usleep interruptions with loop
**   4. Ensure minimum precision for timing
*/
void	precise_sleep(long duration)
{
	long	start;
	long	elapsed;

	start = get_time_ms();
	while (1)
	{
		elapsed = get_time_ms() - start;
		if (elapsed >= duration)
			break ;
		usleep(100);
	}
}

/*
** @brief: Enhanced sleep with busy-wait for precision
** @param: duration - sleep time in milliseconds
** @return: void
** 
** Implementation:
**   1. Get start time
**   2. Use usleep for 90% of duration (bulk sleep)
**   3. Busy-wait for remaining 10% for precision
**   4. Check elapsed time continuously in busy-wait
** 
** This approach reduces CPU usage while maintaining precision:
** - usleep saves CPU during most of the wait
** - busy-wait ensures we don't oversleep
*/
void	smart_sleep(long duration)
{
	long	start;
	long	elapsed;
	long	bulk_sleep;

	start = get_time_ms();
	bulk_sleep = duration * 9 / 10;
	if (bulk_sleep > 0)
		usleep(bulk_sleep * 1000);
	while (1)
	{
		elapsed = get_time_ms() - start;
		if (elapsed >= duration)
			break ;
		usleep(50);
	}
}
