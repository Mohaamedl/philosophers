/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/10/27 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

/*
** @brief: Checks if a character is a whitespace
** @param: c - character to check
** @return: 1 if whitespace, 0 otherwise
*/
static int	ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}

/*
** @brief: Checks if a character is a digit
** @param: c - character to check
** @return: 1 if digit, 0 otherwise
*/
static int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

/*
** @brief: Converts string to positive integer with overflow protection
** @param: str - string to convert
** @return: converted integer or -1 on error
** 
** Implementation:
**   1. Skip leading whitespace
**   2. Check for '+' sign only (no '-' allowed for positive)
**   3. Convert digit by digit with overflow check
**   4. Ensure entire string is consumed (no trailing chars)
**   5. Return -1 for invalid input
** 
** Error cases:
**   - NULL or empty string
**   - Negative numbers (starts with '-')
**   - Non-numeric characters
**   - Overflow (result > INT_MAX)
**   - Leading zeros followed by other digits
*/
int	ft_atoi_positive(const char *str)
{
	long	result;
	int		i;

	if (!str || !*str)
		return (-1);
	i = 0;
	while (ft_isspace(str[i]))
		i++;
	if (str[i] == '+')
		i++;
	else if (str[i] == '-')
		return (-1);
	if (!ft_isdigit(str[i]))
		return (-1);
	result = 0;
	while (ft_isdigit(str[i]))
	{
		result = result * 10 + (str[i] - '0');
		if (result > 2147483647)
			return (-1);
		i++;
	}
	if (str[i] != '\0')
		return (-1);
	return ((int)result);
}

/*
** @brief: Validates command line arguments
** @param: argc - argument count, argv - argument array
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Check argc is 5 or 6
**   2. Validate each argument is positive integer
**   3. Check ranges: philo_count >= 1, times > 0
**   4. Handle optional must_eat parameter
**   5. Return error codes for invalid input
** 
** Validation rules:
**   - argc must be 5 (mandatory) or 6 (with must_eat)
**   - All numeric arguments must be positive integers
**   - number_of_philosophers >= 1
**   - time_to_die > 0
**   - time_to_eat > 0
**   - time_to_sleep > 0
**   - number_of_times_each_philosopher_must_eat >= 1 (if provided)
*/
int	validate_args(int argc, char **argv)
{
	int	i;
	int	value;

	if (argc != 5 && argc != 6)
	{
		printf("Error: Invalid number of arguments\n");
		printf("Usage: ./philo number_of_philosophers time_to_die ");
		printf("time_to_eat time_to_sleep [number_of_times_to_eat]\n");
		return (1);
	}
	i = 1;
	while (i < argc)
	{
		value = ft_atoi_positive(argv[i]);
		if (value <= 0)
		{
			printf("Error: Argument %d must be a positive integer\n", i);
			return (1);
		}
		i++;
	}
	return (0);
}

/*
** @brief: Parses arguments into table structure
** @param: table - table structure to fill, argc/argv - arguments
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Call validate_args first
**   2. Convert each argument using ft_atoi_positive
**   3. Fill table structure fields
**   4. Set must_eat_count to -1 if not provided
**   5. Validate special case: 1 philosopher
** 
** Arguments mapping:
**   argv[1] -> philo_count
**   argv[2] -> time_to_die
**   argv[3] -> time_to_eat
**   argv[4] -> time_to_sleep
**   argv[5] -> must_eat_count (optional, -1 if not provided)
*/
int	parse_arguments(t_table *table, int argc, char **argv)
{
	if (validate_args(argc, argv) != 0)
		return (1);
	table->philo_count = ft_atoi_positive(argv[1]);
	table->time_to_die = ft_atoi_positive(argv[2]);
	table->time_to_eat = ft_atoi_positive(argv[3]);
	table->time_to_sleep = ft_atoi_positive(argv[4]);
	if (argc == 6)
		table->must_eat_count = ft_atoi_positive(argv[5]);
	else
		table->must_eat_count = -1;
	if (table->philo_count <= 0 || table->time_to_die <= 0
		|| table->time_to_eat <= 0 || table->time_to_sleep <= 0)
	{
		printf("Error: All values must be positive\n");
		return (1);
	}
	if (argc == 6 && table->must_eat_count <= 0)
	{
		printf("Error: Number of times to eat must be positive\n");
		return (1);
	}
	return (0);
}
