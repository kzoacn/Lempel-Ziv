#include <cstdio>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

const int MAX_SIZE = 64 * 1024;
FILE *fin, *fout;

unsigned char buff[MAX_SIZE];

map<string, unsigned int> dict_enc;
map<unsigned int, string> dict_dec;

void encode()
{
    int len;
    unsigned int id = 0;
    string previous, current;

    for (; id < 256; id++)
    {
        dict_enc[to_string(id)] = id;
    }

    while ((len = fread(buff, 1, MAX_SIZE, fin)) > 0)
    {
        int cur = 0;

        while (cur < len)
        {
            for (int i = cur; i < len; i++)
            {
                current = to_string(buff[i]);
                if (dict_enc.count(previous + current))
                {
                    previous = previous + current;
                }
                else
                {
                    fwrite(&dict_enc[previous], 1, 4, fout);
                    dict_enc[previous + current] = id;
                    id++;
                    previous = current;
                }
            }
        }
    }
    fwrite(&dict_enc[previous], 1, 4, fout);
}

void decode()
{
    int len;
    unsigned int id = 0;
    string previous, current;
    vector<string> all;

    for (; id < 256; id++)
    {
        dict_enc[to_string(id)] = id;
        dict_dec[id] = to_string(id);
    }

    while ((len = fread(buff, 1, MAX_SIZE / 4 * 4, fin)) > 0)
    {
        int cur = 0;

        for (int i = 0; i < len; i += 4)
        {
            unsigned int u = *(unsigned int *)(buff + i);
            if (dict_dec.count(u))
            {
                current = dict_dec[u];
                all.push_back(current);
                previous = current;

                if (dict_enc.count(previous + current) == 0)
                {
                    dict_enc[previous + current] = id;
                    dict_dec[id] = previous + current;
                    id++;
                }
            }
            else
            {
                printf("error\n");
            }
        }
    }
    fwrite(all.data(), 1, all.size(), fout);
}