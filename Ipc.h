// Класс, реализующий работу именованных каналов

#ifndef LAB4_IPC_H
#define LAB4_IPC_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <exception>

#define BUFFER_SIZE 512

using namespace std;

class Ipc
{
private:
	int fifo;
	char buffer[BUFFER_SIZE];
	
public:
	Ipc(string pipe_name, bool server);
	~Ipc();
	
	void read(string &s);
	void write(string &s);
};

class Exception : public exception
{
	const char *msg;
public:
	Exception(const char *message)
	{
		msg = message;
	}
	
	virtual const char *what() const noexcept
	{
		return msg;
	}
};


#endif //LAB4_IPC_H
