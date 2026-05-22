#include "../inc/txtfile.h"

#ifndef BUFFER_SIZE // размер буфера для чтения из файла
# define BUFFER_SIZE 32
#endif

char	*get_next_line(int fd)

{
	static char	*buffer;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);

	buffer = read_to_buffer(fd, buffer);
	if (!buffer || !buffer[0])
	{
		free(buffer);
		buffer = NULL;
		return (NULL);
	}

	line = extract_line(buffer);
	buffer = update_buffer(buffer);

	return (line);
}