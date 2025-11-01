/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

/*
** @brief: Entry point of the program
** @param: argc - argument count, argv - argument array
** @return: 0 on success, 1 on error
** 
** Implementation:
**   1. Validate argument count (argc == 5 or 6)
**   2. Declare and initialize table structure
**   3. Call parse_arguments()
**   4. Call init_table()
**   5. Call create_threads()
**   6. Call start_monitor() (Phase 4)
**   7. Call join_monitor()
**   8. Call join_threads()
**   9. Call cleanup_table()
**   10. Return appropriate exit code
** 
** Note: Monitor thread runs concurrently with philosopher threads.
** It detects deaths and meal completion, then signals simulation end.
*/
int	main(int argc, char **argv)
{
	t_table	table;

	memset(&table, 0, sizeof(t_table));
	if (parse_arguments(&table, argc, argv) != 0)
		return (1);
	if (init_table(&table) != 0)
		return (1);
	if (create_threads(&table) != 0)
	{
		cleanup_table(&table);
		return (1);
	}
	if (start_monitor(&table) != 0)
	{
		cleanup_table(&table);
		return (1);
	}
	join_monitor(&table);
	join_threads(&table);
	cleanup_table(&table);
	return (0);
}
