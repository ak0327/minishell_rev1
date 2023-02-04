/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update_shlvl.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takira <takira@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/03 21:56:24 by takira            #+#    #+#             */
/*   Updated: 2023/02/04 09:17:50 by takira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_new_shlvl(int current_shlvl_num, bool is_atoi_success);

/* return value		: SUCCESS or FAILURE of the process. */
/* new SHLVL string	: update *now_shlvl_str passed in the argument **now_shlvl_str. */
int update_shlvl(char **current_shlvl_str)
{
	int		current_shlvl_num;
	bool	is_atoi_success;

	if (!current_shlvl_str)
		return (FAILURE);
	current_shlvl_num = ft_atoi(*current_shlvl_str, &is_atoi_success);
	*current_shlvl_str = free_1d_alloc(*current_shlvl_str);
	if (is_atoi_success && current_shlvl_num == 999)
		*current_shlvl_str = ft_strdup("");
	else
		*current_shlvl_str = get_new_shlvl(current_shlvl_num, is_atoi_success);;
	if (!*current_shlvl_str)
		return (FAILURE);
	ft_dprintf(STDERR_FILENO, "[#DEBUG shlvl] now:%d, newnum:%s\n", current_shlvl_num, *current_shlvl_str);
	return (SUCCESS);
}

static char	*get_new_shlvl(int current_shlvl_num, bool is_atoi_success)
{
	int 	new_shlvl_num;
	char	*new_shlvl_str;

	if (!is_atoi_success || current_shlvl_num <= 0)
		new_shlvl_num = 1;
	else if (current_shlvl_num == INT_MAX)
		new_shlvl_num = 0;
	else if (1000 < current_shlvl_num + 1)
	{
		ft_dprintf(STDERR_FILENO, "minishell: warning: shell level (%d) too high, resetting to 1\n", current_shlvl_num + 1);
		new_shlvl_num = 1;
	}
	else
		new_shlvl_num = current_shlvl_num + 1;
	new_shlvl_str = ft_itoa(new_shlvl_num);
	if (!new_shlvl_str)
		return (perror_ret_nullptr("malloc"));
	return (new_shlvl_str);
}

int	add_initial_shlvl(t_list **env_list_head)
{
	t_env_elem		*elem;

	if (!env_list_head)
		return (FAILURE);
	errno = 0;
	elem = (t_env_elem *)malloc(sizeof(t_env_elem));
	if (!elem)
		return (perror_ret_int("malloc", FAILURE));
	elem->key = ft_strdup("SHLVL");
	elem->value = ft_strdup("1");
	elem->not_print = 0;
	if (!elem->key || !elem->value)
	{
		free_env_elem(elem);
		elem = NULL;
		return (perror_ret_int("malloc", FAILURE));
	}
	return (SUCCESS);
}
