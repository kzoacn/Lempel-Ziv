#include <cstdio>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

const int MAX_SIZE = 64 * 1024;
const int MAX_CODE_LEN = 8;

FILE* fin, * fout;

unsigned char buff[MAX_SIZE];

typedef uint16_t enc_symbol;

map<string, enc_symbol> dict_enc;
map<enc_symbol, string> dict_dec;

void encode()
{
	int len;
	enc_symbol id;
	unsigned char ch;
	string previous, current;

	for (id = 0, ch = 0; id < 256; id++, ch++)
	{
		current = ch;
		dict_enc[current] = id;
		dict_dec[id] = current;
	}

	while ((len = fread(buff, sizeof(unsigned char), MAX_SIZE, fin)) > 0)
	{
		for (int i = 0; i < len; i++)
		{
			ch = buff[i];
			current = ch;
			if (dict_enc.count(previous + current))
			{
				previous = previous + current;
			}
			else
			{
				::fwrite(&dict_enc[previous], sizeof(enc_symbol), 1, fout);
				if (previous.length() < MAX_CODE_LEN)
				{
					if (dict_dec.count(id) == 0)
					{
						dict_dec[id] = previous + current;
						dict_enc[previous + current] = id;
						id++;
					}
				}
				previous = current;
			}
		}
	}
	::fwrite(&dict_enc[previous], sizeof(enc_symbol), 1, fout);
}

void decode()
{
	int len;
	unsigned char ch;
	enc_symbol id = 0, pW = 0, cW = 0;
	string previous, current;

	for (id = 0, ch = 0; id < 256; id++, ch++)
	{
		current = ch;
		dict_enc[current] = id;
		dict_dec[id] = current;
	}

	// first symbol
	len = fread(buff, sizeof(enc_symbol), 1, fin);
	cW = *(enc_symbol*)(buff);
	if (len != 1 || dict_dec.count(cW) != 1)
	{
		printf("\nerror1\n");
		return;
	}

	current = dict_dec[cW];
	//::fwrite(&current, 1, current.size(), fout);
	for (size_t j = 0; j < current.size(); j++)
	{
		::fwrite(&current[j], sizeof(unsigned char), 1, fout);
	}

	// other symbol
	while ((len = fread(buff, 1, MAX_SIZE / sizeof(enc_symbol) * sizeof(enc_symbol), fin)) > 0)
	{
		for (int i = 0; i < len; i += sizeof(enc_symbol))
		{
			pW = cW;

			cW = *(enc_symbol*)(buff + i);

			if (dict_dec.count(cW))
			{
				current = dict_dec[cW];
				//::fwrite(&current, 1, current.size(), fout);
				for (size_t j = 0; j < current.size(); j++)
				{
					::fwrite(&current[j], sizeof(unsigned char), 1, fout);
				}

				previous = dict_dec[pW];
				current = dict_dec[cW][0];

				if (previous.length() < MAX_CODE_LEN)
				{
					if (dict_enc.count(previous + current) == 0)
					{
						while (dict_dec.count(id) == 1)
						{
							id++;
						}

						dict_enc[previous + current] = id;
						dict_dec[id] = previous + current;
						id++;
					}
				}
			}
			else
			{
				previous = dict_dec[pW];
				current = dict_dec[pW][0];

				if (dict_enc.count(previous + previous) == 0)
				{
					if (dict_dec.count(cW) == 0)
					{
						dict_enc[previous + previous] = cW;
						dict_dec[cW] = previous + previous;
					}
					else
					{
						printf("\nerror2\n");
						return;
					}
				}
				string temp = previous + previous[0];
				//::fwrite(&temp, 1, temp.size(), fout);
				for (size_t j = 0; j < temp.size(); j++)
				{
					::fwrite(&temp[j], sizeof(unsigned char), 1, fout);
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	// if (argc != 4)
	// {
	//     puts("./lz78w encode/decode <in-file> <out-file>");
	//     return 0;
	// }
	// fin = fopen(argv[2], "rb");
	// fout = fopen(argv[3], "wb");

	// if (argv[1][0] == 'e')
	// {
	//     encode();
	// }
	// else
	// {
	//     decode();
	// }

	//fopen_s(&fin, "./src.txt", "rb");
	//fopen_s(&fout, "./lz78w.out", "wb");

	//encode();

	fopen_s(&fin, "./lz78w.out", "rb");
	fopen_s(&fout, "./lz78w_dec.txt", "wb");

	decode();

	fclose(fin);
	fclose(fout);

	return 0;
}