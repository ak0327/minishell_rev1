/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execution.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takira <takira@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 15:15:10 by takira            #+#    #+#             */
/*   Updated: 2023/01/28 21:52:35 by takira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_EXECUTION_H
# define COMMAND_EXECUTION_H

# include "minishell.h"

/* fd */
# define READ			0
# define WRITE			1
# define R_FD_INFILE	0
# define R_FD_OUTFILE	1
# define R_FD_HEREDOC	2

/* exit_status */
# define CHDIR_FAILURE				1
# define EXIT_TOO_MANY_ARGS			1
# define FILE_OPEN_ERROR			1
# define CMD_NOT_FOUND				1
# define EXIT_SIGQUIT				131
# define EXIT_NUMERIC_ARGS_REQUIRED	255
# define SYNTAX_ERROR				258

/* string */
# define PATH	"PATH"
# define HEREDOC_TMP_FILE	".heredoc_tmp"



/* ************************** */
/*          typedef           */
/* ************************** */
typedef struct s_info			t_info;
typedef struct s_env_elem		t_env_elem;
typedef struct s_token_elem		t_token_elem;
typedef struct s_split_info		t_split_info;
typedef struct s_exec_list		t_exec_list;
typedef struct s_command_info	t_command_info;
typedef struct s_redirect_info	t_redirect_info;

typedef enum e_token_type		t_token_type;
typedef enum e_syntax_err		t_syntax_err;
typedef enum e_node_kind		t_node_kind;



/* ************************** */
/*           enum             */
/* ************************** */

/* ************************** */
/*          struct            */
/* ************************** */

/* ************************** */
/*         function           */
/* ************************** */

int		execute_execlist(t_info *info);
int		execute_pipeline(t_list_bdi *pipeline_cmds_head, t_list *envlist_head);

/* ft_exec.c */
int		ft_execve(t_command_info *command_info, char **minishell_envp, t_list *envlist);

/* judge_fork_process */
bool	is_child_process(pid_t pid);
bool	is_parent_process(pid_t pid);

/* pipe_helper.c */
void	init_pipefd(int prev_pipefd[2], int now_pipefd[2]);
void	copy_prevfd_to_newfd(int prev_pipefd[2], const int now_pipefd[2]);
int		dup2_fds(int now_fd[2], int prev_fd[2], t_list_bdi *next);
int		close_fds(int now_fd[2], int prev_fd[2], t_list_bdi *next);

/* create_envp.c */
char	**create_minishell_envp(t_list *envlist_head);


/* execute_heredoc */
int execute_heredoc(t_command_info *command_info);

#endif //COMMAND_EXECUTION_H
