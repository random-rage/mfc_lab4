#include <iostream>
#include <csignal>
#include <boost/algorithm/string.hpp>

#include "Ipc.h"
#include "Dh.h"
#include "blowfish.h"

using namespace std;
using namespace boost;

#define DEBUG

#ifdef CLIENT
static string secret_values[] = { "white flag", "blue box", "first floor" };
#endif

// Обработчик сигналов ОС
void signalHandler(int signum)
{
	cout << "\nSignal (" << signum << ") received" << endl;
	
	if (signum == SIGPIPE)
		cout << "Pipe-in write error: pipe is closed" << endl;
}

string key_exchange(Ipc *ipc)
{
	string cmd;
	vector<string> args;
	
#ifdef SERVER
	do
	{
		ipc->read(cmd);
		split(args, cmd, is_any_of(" "));
	}
	while (args[0] != "CON");
	cout << endl << "Incoming connection:" << endl << cmd << endl;
	
	Dh dh = Dh(args[1], args[2]);
	cmd = "ACK " + dh.get_public_key();
	ipc->write(cmd);
	cout << endl << "Sent:" << endl << cmd << endl;
	
	return dh.get_shared_key(args[3]);
#else
	cout << "Generating DH params..." << endl;
	Dh dh = Dh::create();
	cmd = "CON " + dh.get_base() + " " + dh.get_modulo() + " " + dh.get_public_key();
	ipc->write(cmd);
	cout << endl << "Sent:" << endl << cmd << endl;
	
	do
	{
		ipc->read(cmd);
		split(args, cmd, is_any_of(" "));
	}
	while (args[0] != "ACK");
	cout << endl << "Received:" << endl << cmd << endl;
	
	return dh.get_shared_key(args[1]);
#endif
}

int main()
{
	// Регистрируем обработчик сигналов ОС
	signal(SIGPIPE, signalHandler);
	
	cout <<
#ifdef SERVER
	     "Server"
#else
		 "Client"
#endif
	     " started" << endl;
	
	try
	{
		Ipc *ipc = new Ipc();
		string key = key_exchange(ipc);
		cout << "Session key: " << key << endl << "Starting secure data exchange..." << endl;
		
		Blowfish bf(key);
		string str;
		
#ifdef SERVER
		do
		{
			ipc->read(str);
#ifdef DEBUG
			cout << "[Debug] Received: " << str << endl;
#endif
			if (str == "END")
				break;
				
			if (str.size() > 3)
			{
				str = bf.Decrypt(str);
				cout << "Received value: " << str.substr(4) << endl;
			}
		}
		while (true);
#else
		for (string value: secret_values)
		{
			str = "VAL " + value;
			str = bf.Encrypt(str);
			ipc->write(str);
#ifdef DEBUG
			cout << "[Debug] Sent: " << str << endl;
#endif
			cout << "Sent value: " << value << endl;
		}
		str = "END";
		ipc->write(str);
#endif
		cout << "Session closed" << endl;
		delete ipc;
		return 0;
	}
	catch (Exception *ex)
	{
		cout << "Error " << errno << ": " << ex->what() << endl;
		delete ex;
		return 1;
	}
}