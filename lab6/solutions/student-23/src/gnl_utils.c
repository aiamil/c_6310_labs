#include "../inc/txtfile.h"

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

char	*extract_line(char *buffer)
{
	int		i;
	char	*line;

	i = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		i++;

	line = ft_substr(buffer, 0, i);
	return (line);
}

char	*update_buffer(char *buffer)
{
	int		i;
	char	*new_buffer;

	i = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;

	if (!buffer[i])
	{
		free(buffer);
		return (NULL);
	}

	new_buffer = ft_strdup(buffer + i + 1);
	free(buffer);
	return (new_buffer);
}

int	find_newline(char *buffer)
{
	int	i;

	if (!buffer)
		return (0);
	i = 0;
	while (buffer[i])
	{
		if (buffer[i] == '\n')
			return (1);
		i++;
	}
	return (0);
}

char	*read_to_buffer(int fd, char *buffer)
{
	char	*tmp;
	char	*read_buf;
	int		bytes;

	read_buf = malloc(BUFFER_SIZE + 1);
	if (!read_buf)
		return (NULL);

	bytes = 1;
	while (!find_newline(buffer) && bytes > 0)
	{
		bytes = read(fd, read_buf, BUFFER_SIZE);
		if (bytes < 0)
		{
			free(read_buf);
			return (NULL);
		}
		read_buf[bytes] = '\0';
		tmp = ft_strjoin(buffer, read_buf);
		free(buffer);
		buffer = tmp;
	}

	free(read_buf);
	return (buffer);
}