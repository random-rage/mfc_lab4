// Класс, реализующий работу протокола Diffie-Hellman'а

#ifndef LAB4_DH_H
#define LAB4_DH_H

#include <random>
#include "bignum.h"

#define PRIVATE_KEY_SIZE 10
#define PUBLIC_KEY_SIZE 512
#define STRING_SIZE 1024
#define RADIX 16

using namespace std;

class Dh
{
private:
	mpi     *g, // Основание
			*p, // Модуль
			*x; // Закрытый ключ
	
	Dh();
	
public:
	Dh(mpi *g, mpi *p);
	Dh(string g, string p);
	~Dh();
	
	static Dh create();
	
	string get_base();
	string get_modulo();
	
	mpi *get_public_key_mpi();
	string get_public_key();
	
	mpi *get_shared_key(mpi *y);
	string get_shared_key(string y);
};


#endif //LAB4_DH_H
