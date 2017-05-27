#include "Ipc.h"

Ipc::Ipc(string pipe_name, bool server)
{
	if (server)
	{
		unlink(pipe_name.c_str());
		umask(0);
		
		if (mknod(pipe_name.c_str(), S_IFIFO | 0666, 0) < 0)
			throw new Exception("Failed to create pipe");
	}
	
	fifo = open(pipe_name.c_str(), O_RDWR);
	
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
	ssize_t len = ::read(fifo, buffer, BUFFER_SIZE);
	if (len < 0)
		throw new Exception("Failed to read from the pipe");
	
	s = buffer;
	if (s.length() < len - 1)
	{
		string buf = buffer + s.length() + 1;
		write(buf);
	}
}

void Ipc::write(string &s)
{
	if (::write(fifo, s.c_str(), s.size() + 1) < 0)
		throw new Exception("Failed to write into the pipe");
}
