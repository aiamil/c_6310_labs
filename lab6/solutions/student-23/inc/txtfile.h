#ifndef TXTFILE_H
# define TXTFILE_H

# define BUFFER_SIZE 42  

# include "../libft/inc/libft.h"
# include <unistd.h>
# include <fcntl.h>

// Основные функции
char	**read_file(const char *filename);
void	free_lines(char **lines);
void	print_lines(char **lines);

// Функции замены (вариант 3)
char	**replace_substr(char **lines, const char *old, const char *new_str);
int		count_lines_array(char **lines);
char	*replace_in_line(const char *line, const char *old, const char *new_str);

// Функции get_next_line
char	*get_next_line(int fd);
char	*read_to_buffer(int fd, char *buffer);
char	*extract_line(char *buffer);
char	*update_buffer(char *buffer);
int		find_newline(char *buffer);

#endif