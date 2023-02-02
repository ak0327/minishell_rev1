/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_matches_wildcard.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takira <takira@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 10:30:09 by takira            #+#    #+#             */
/*   Updated: 2023/02/02 21:53:41 by takira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expansion.h"

static int **get_dp_table(size_t row, size_t col);
static int	is_match(const char *wildcard_str, const char *target_str, int **dp, const int *valid_table);
static void	free_dp_array(int **dp, size_t row_size);

int	*get_long_valid_table(const int *valid_table, size_t len_wildcard)
{
	size_t			idx;
	const size_t	len = len_wildcard + 10;
	int 			*long_table;

	if (!valid_table)
		return (NULL);
	long_table = (int *)ft_calloc(sizeof(int), len);
	if (!long_table)
		return (perror_ret_nullptr("malloc"));
	idx = 0;
	while (idx < len_wildcard)
	{
		long_table[idx] = valid_table[idx];
		idx++;
	}
	debug_print_wildcard_valid_list(long_table, len);
	return (long_table);
}

/* return true if wildcard_str matches str */
/* dp[i][j] : wildcardのi文字目, targetのj文字目まで見て文字列が成立すれば1, 成立しなければ0*/
int	is_matches_wildcard_and_target_str(const char *wildcard_str, const char *target_str, const int *valid_table)
{
	const size_t	len_wildcard = ft_strlen_ns(wildcard_str);
	const size_t	len_target = ft_strlen_ns(target_str);
	int				**dp;
	int				ans;
	int				*valid_table_long;

	if (!target_str || !wildcard_str || !valid_table)
		return (false);
	dp = get_dp_table(len_wildcard + 1, len_target + 1);
	if (!dp)
		return (PROCESS_ERROR);
	valid_table_long = get_long_valid_table(valid_table, len_wildcard);
	if (!valid_table_long)
		return (PROCESS_ERROR);
	ans = is_match(wildcard_str, target_str, dp, valid_table_long);
	free_dp_array(dp, len_wildcard + 1);
	free(valid_table_long);
	return (ans);
}

//文字列"*" と * は一致しないと仮定
// wild   "hoge*"
//         ^   ^
// target "hoge*"
//         ^
static int	is_match(const char *wildcard_str, const char *target_str, int **dp, const int *valid_table)
{
	const size_t	len_wildcard = ft_strlen_ns(wildcard_str);
	const size_t	len_target = ft_strlen_ns(target_str);
	size_t			i;
	size_t			j;

	if (!dp || !*dp)
		return (false);
	dp[0][0] = 1;
	i = 0;
	printf("\n");
	printf("START!!!\n");
	printf("\n");

	while (i < len_wildcard + 1)
	{
		j = 0;
		while (j < len_target + 1)
		{
			if (i > 0 && valid_table[i - 1] == 1)
				dp[i][j] = max(dp[i][j - 1], dp[i - 1][j]);
			if (j > 0 && i > 0 && valid_table[i - 1] == 0 && wildcard_str[i - 1] == target_str[j - 1])
				dp[i][j] = max(dp[i][j], dp[i - 1][j - 1]);
			printf("j:%zu, len_target:%zu, targetstr:%s\n", j, len_target, target_str);
			j++;
		}
		printf("i:%zu, len_wild:%zu, wildstr:%s\n", i, len_wildcard, wildcard_str);
		i++;
	}
	printf("\n");
	printf("END!!!\n");
	printf("\n");

	if (dp[len_wildcard][len_target] > 0)
		return (true);
	return (false);
}

static int **get_dp_table(size_t row, size_t col)
{
	int		**dp;
	size_t	idx;

	dp = (int **)calloc(sizeof(int *), row + 1);
	if (!dp)
		return (perror_ret_nullptr("malloc"));
	idx = 0;
	while (idx < row)
	{
		dp[idx] = (int *)calloc(sizeof(int), col + 1);
		if (!dp[idx])
			return (perror_ret_nullptr("malloc"));
		idx++;
	}
	return (dp);
}

static void	free_dp_array(int **dp, size_t row_size)
{
	size_t	i;

	if (!dp || !*dp)
		return ;
	i = 0;
	while (i < row_size)
	{
		free_1d_alloc(dp[i]);
		i++;
	}
	free(dp);
}

void	print_dp_array(int **array, const char *wild, const char *target)
{
	size_t	row;
	size_t	col;
	size_t	wild_len = ft_strlen_ns(wild);
	size_t	str_len = ft_strlen_ns(target);

	if (!array)
		return ;
	row = 0;
	while (row < wild_len + 2)
	{
		col = 0;
		while (col < str_len + 2)
		{
			if ((row == 0 && col == 0) || (row == 1 && col == 0) || (row == 0 && col == 1))
				ft_printf("- ");
			else if (row == 0)
				ft_printf("%c ", target[col - 2]);
			else if (col == 0)
				ft_printf("%c ", wild[row - 2]);
			else
				ft_printf("%d ", array[row - 1][col - 1]);
			col++;
		}
		row++;
		ft_printf("\n");
	}
	ft_printf("\n");
}
