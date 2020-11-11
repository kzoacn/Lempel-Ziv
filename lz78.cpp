#include <cstdio>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
using namespace std;
const int MAX_SIZE = 64 * 1024;
FILE *fin, *fout;

unsigned char buff[MAX_SIZE];

vector<map<unsigned char, unsigned int>> trie;
vector<unsigned int> fa;
vector<unsigned char> val;

void encode()
{
    int len;
    val.push_back(0);
    fa.push_back(0);
    trie.push_back(map<unsigned char, unsigned int>());
    while ((len = fread(buff, 1, MAX_SIZE, fin)) > 0)
    {

        int cur = 0;
        while (cur < len)
        {
            int u = 0;
            bool ok = true;
            int last;
            unsigned char buf;
            for (int i = cur; i < len; i++)
            {
                if (trie[u].count(buff[i]))
                {
                    ok = true;
                    last = u;
                    buf = buff[i];
                    u = trie[u][buff[i]];
                }
                else
                {
                    ok = false;
                    fwrite(&u, 1, 4, fout);
                    fwrite(&buff[i], 1, 1, fout);
                    cur = i + 1;
                    trie[u][buff[i]] = trie.size();
                    trie.push_back(map<unsigned char, unsigned int>());
                    fa.push_back(u);
                    val.push_back(buff[i]);
                    break;
                }
            }
            if (ok)
            {
                fwrite(&last, 1, 4, fout);
                fwrite(&buf, 1, 1, fout);
                break;
            }
        }
    }
}

void decode()
{
    int len;
    val.push_back(0);
    fa.push_back(0);
    trie.push_back(map<unsigned char, unsigned int>());
    vector<unsigned char> all;
    while ((len = fread(buff, 1, MAX_SIZE / 5 * 5, fin)) > 0)
    {

        for (int i = 0; i < len; i += 5)
        {
            int u = *(int *)(buff + i);
            unsigned char c = buff[i + 4];
            vector<unsigned char> vec;
            int v = u;
            while (v)
            {
                vec.push_back(val[v]);
                v = fa[v];
            }
            reverse(vec.begin(), vec.end());
            for (auto x : vec)
            {
                all.push_back(x);
            }
            all.push_back(c);
            if (trie[u].count(c))
                continue;
            trie[u][c] = trie.size();
            trie.push_back(map<unsigned char, unsigned int>());
            fa.push_back(u);
            val.push_back(c);
        }
    }
    fwrite(all.data(), 1, all.size(), fout);
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        puts("./lz78 encode/decode <in-file> <out-file>");
        return 0;
    }
    fin = fopen(argv[2], "rb");
    fout = fopen(argv[3], "wb");

    if (argv[1][0] == 'e')
    {
        encode();
    }
    else
    {
        decode();
    }

    fclose(fin);
    fclose(fout);

    return 0;
}