#include "Dh.h"

int myrand(void *rng_state, unsigned char *output, size_t len)
{
	mt19937 rnd;
	rnd.seed(time(0));
	size_t i;
	
	if (rng_state != NULL)
		rng_state = NULL;
	
	for (i = 0; i < len; ++i)
		output[i] = rnd() % 256;
	
	return(0);
}

Dh::Dh()
{
	x = new mpi();
	mpi_init(x);
	mpi_fill_random(x, PRIVATE_KEY_SIZE, myrand, NULL);
}

Dh::Dh(mpi *g, mpi *p) : Dh()
{
	Dh::g = g;
	Dh::p = p;
}

Dh::Dh(string g, string p) : Dh()
{
	Dh::g = new mpi();
	Dh::p = new mpi();
	mpi_init(Dh::g);
	mpi_init(Dh::p);
	
	mpi_read_string(Dh::g, RADIX, g.c_str());
	mpi_read_string(Dh::p, RADIX, p.c_str());
}

Dh::~Dh()
{
	mpi_free(g);
	mpi_free(p);
	mpi_free(x);
	delete g, p, x;
}

Dh Dh::create()
{
	mpi     *g = new mpi(),
			*p = new mpi();
	
	mpi_init(g);
	mpi_init(p);
	
	mpi_lset(g, 3);
	mpi_gen_prime(p, PUBLIC_KEY_SIZE, 1, myrand, NULL);
	
	return Dh(g, p);
}

mpi *Dh::get_public_key_mpi()
{
	mpi *pub_key = new mpi();
	mpi_init(pub_key);
	
	mpi_exp_mod(pub_key, g, x, p, NULL);
	
	return pub_key;
}

mpi *Dh::get_shared_key(mpi *y)
{
	mpi *sh_key = new mpi();
	mpi_init(sh_key);
	
	mpi_exp_mod(sh_key, y, x, p, NULL);
	
	return sh_key;
}

string Dh::get_public_key()
{
	char str[STRING_SIZE];
	size_t len;
	
	mpi *pub_key = get_public_key_mpi();
	mpi_write_string(pub_key, RADIX, str, &len);
	
	string result = str;
	
	mpi_free(pub_key);
	delete pub_key;
	
	return result;
}

string Dh::get_shared_key(string y)
{
	char str[STRING_SIZE];
	size_t len;
	
	mpi *m_y = new mpi();
	mpi_init(m_y);
	mpi_read_string(m_y, RADIX, y.c_str());
	
	mpi *sh_key = get_shared_key(m_y);
	mpi_write_string(sh_key, RADIX, str, &len);
	
	string result = str;
	
	mpi_free(m_y);
	mpi_free(sh_key);
	delete m_y, sh_key;
	
	return result;
}

string Dh::get_base()
{
	char str[STRING_SIZE];
	size_t len;
	
	mpi_write_string(g, RADIX, str, &len);
	
	string result = str;
	return result;
}

string Dh::get_modulo()
{
	char str[STRING_SIZE];
	size_t len;
	
	mpi_write_string(p, RADIX, str, &len);
	
	string result = str;
	return result;
}
