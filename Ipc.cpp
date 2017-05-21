#include "Ipc.h"

Ipc::Ipc()
{
	#ifdef SERVER
	
	unlink(PIPE_NAME);
	umask(0);
	
	if (mknod(PIPE_NAME, S_IFIFO|0666, 0) < 0)
		throw new Exception("Failed to create pipe");
	
	#endif
	
	fifo = open(PIPE_NAME, O_RDWR);
	
	if(fifo < 0)
		throw new Exception("Failed to open pipe");
}

Ipc::~Ipc()
{
	if (fifo < 0)
		return;;
	
	close(fifo);
}

void Ipc::read(string &s)
{
	if (::read(fifo, buffer, BUFFER_SIZE) < 0)
		throw new Exception("Failed to read from the pipe");
	
	s = buffer;
}

void Ipc::write(string &s)
{
	if (::write(fifo, s.c_str(), s.size() + 1) < 0)
		throw new Exception("Failed to write into the pipe");
}
