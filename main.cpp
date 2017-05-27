#include <iostream>
#include <csignal>
#include <boost/algorithm/string.hpp>

#include "Ipc.h"
#include "Dh.h"
#include "blowfish.h"

using namespace std;
using namespace boost;

#define DEBUG
#define PIPE_NAME "lab4"

#ifdef CLIENT
static string secret_values[] = { "white flag", "green box", "first floor" };
#endif

#ifdef MITM
static string replace_values[] = { "red", "black", "second" };
#endif

// Обработчик сигналов ОС
void signalHandler(int signum)
{
	cout << "\nSignal (" << signum << ") received" << endl;
	
	if (signum == SIGPIPE)
		cout << "Pipe-in write error: no listeners found" << endl;
}

string key_exchange(Ipc *ipc, bool server)
{
	string cmd;
	vector<string> args;
	
	if (server)
	{
		do
		{
			ipc->read(cmd);
			split(args, cmd, is_any_of(" "));
		} while (args[0] != "CON");
		cout << endl << "Incoming connection:" << endl << cmd << endl;
		
		Dh dh = Dh(args[1], args[2]);
		cmd = "ACK " + dh.get_public_key();
		ipc->write(cmd);
		cout << endl << "Sent:" << endl << cmd << endl;
		
		return dh.get_shared_key(args[3]);
	}
	else
	{
		cout << "Generating DH params..." << endl;
		Dh dh = Dh::create();
		cmd = "CON " + dh.get_base() + " " + dh.get_modulo() + " " + dh.get_public_key();
		ipc->write(cmd);
		cout << endl << "Sent:" << endl << cmd << endl;
		
		do
		{
			ipc->read(cmd);
			split(args, cmd, is_any_of(" "));
		} while (args[0] != "ACK");
		cout << endl << "Received:" << endl << cmd << endl;
		
		return dh.get_shared_key(args[1]);
	}
}

int main()
{
	// Регистрируем обработчик сигналов ОС
	signal(SIGPIPE, signalHandler);
	srand(time(0));
	
	cout <<
	
#ifdef SERVER
	     "Server" <<
#endif

#ifdef CLIENT
		 "Client" <<
#endif

#ifdef MITM
		 "MitM" <<
#endif
	     " started" << endl;
	
	try
	{
		string chan_name;
		
#ifdef MITM
		Ipc *ipc = new Ipc(PIPE_NAME, false);
		string keyServer = key_exchange(ipc, false);
		
		ipc->read(chan_name);
		Ipc *chanServer, *chanClient;
		chanServer = new Ipc(chan_name, false);
		string client_chan_name;
		cout << "Server channel: " << chan_name << endl;
		
		do
			client_chan_name = "chan" + to_string(rand() % 10);
		while (chan_name == client_chan_name);
		
		chanClient = new Ipc(client_chan_name, true);
		string keyClient = key_exchange(ipc, true);
		ipc->write(client_chan_name);
		cout << "Client channel: " << client_chan_name << endl;
#endif

#ifdef SERVER
		Ipc *ipc = new Ipc(PIPE_NAME, true);
		string key = key_exchange(ipc, true);
#endif

#ifdef CLIENT
		Ipc *ipc = new Ipc(PIPE_NAME, false);
		string key = key_exchange(ipc, false);
#endif

#ifdef MITM
		cout << "Server session key: " << keyServer << endl
			 << "Client session key: " << keyClient << endl
		     << "Starting MitM attack..." << endl;
		
		Blowfish bfServer(keyServer), bfClient(keyClient);
		string str;
		size_t i = 0;
		
		do
		{
			chanClient->read(str);
#ifdef DEBUG
			cout << "<MitM> [Debug] " << str << endl;
#endif
			if (str == "END")
				break;
			
			if (str.substr(0, 3) == "MSG")
			{
				str = bfClient.Decrypt(str.substr(4));
				
				cout << "<MitM> Received value: " << str.substr(4) << endl;
				str = str.replace(4, 5, replace_values[i++]);
				cout << "<MitM> Spoofed: " << str.substr(4) << endl;
				
				str = "MSG " + bfServer.Encrypt(str);
				chanServer->write(str);
#ifdef DEBUG
				cout << "<MitM> [Debug] " << str << endl;
#endif
			}
		}
		while (true);
		delete chanServer, chanClient;
#else
		cout << "Session key: " << key << endl << "Starting secure data exchange..." << endl;
		
		Blowfish bf(key);
		string str;
		Ipc *chan;
#endif

#ifdef SERVER
		chan_name = "chan" + to_string(rand() % 10);
		chan = new Ipc(chan_name, true);
		ipc->write(chan_name);
		
		cout << "Switched to channel: " << chan_name << endl;
		do
		{
			chan->read(str);
			
#ifdef DEBUG
			cout << "[Debug] " << str << endl;
#endif
			if (str == "END")
				break;
				
			if (str.substr(0, 3) == "MSG")
			{
				str = bf.Decrypt(str.substr(4));
				cout << "Received value: " << str.substr(4) << endl;
			}
		}
		while (true);
#endif

#ifdef CLIENT
		ipc->read(chan_name);
		
		chan = new Ipc(chan_name, false);
		cout << "Switched to channel: " << chan_name << endl;
		
		for (string value: secret_values)
		{
			str = "VAL " + value;
			str = "MSG " + bf.Encrypt(str);
			chan->write(str);
#ifdef DEBUG
			cout << "[Debug] " << str << endl;
#endif
			cout << "Sent value: " << value << endl;
		}
		str = "END";
		chan->write(str);
#endif
		cout << "Session closed" << endl;
		return 0;
	}
	catch (Exception *ex)
	{
		cout << "Error " << errno << ": " << ex->what() << endl;
		delete ex;
		return 1;
	}
}