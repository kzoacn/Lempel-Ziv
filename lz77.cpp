#include<cstdio>
#include<vector>
#include<iostream>
#include<algorithm>
using namespace std;
const int MAX_SIZE=64*1024;
const int SEARCH_SIZE=64;
const int LOOK_SIZE=64;
FILE *fin,*fout;

unsigned char buff[MAX_SIZE];

struct triple{
    unsigned char pre,size;
    unsigned char c;
    triple(int p,int s,int cc){
        pre=p;
        size=s;
        c=cc;
    }
};


void encode(){
    int len;
    while((len=fread(buff,1,MAX_SIZE,fin))>0){
        int cur=0;
        while(cur<len){//TODO improve
            triple tp(0,0,buff[cur]);
            for(int pre=1;pre<SEARCH_SIZE&&cur-pre>=0;pre++){
                int size=0;
                for(int l=0;l<LOOK_SIZE;l++){
                    if(buff[cur-pre+l]==buff[cur+l]){
                        size++;
                    }else{
                        break;
                    }
                }
                if(size>tp.size){
                    if(cur+size>=MAX_SIZE){
                        tp.pre=pre;
                        tp.size=size-1;
                        tp.c=buff[cur+size-1];
                    }else{
                        tp.pre=pre;
                        tp.size=size;
                        tp.c=buff[cur+size];
                    }
                }
            }
            cur=cur+tp.size+1;
            fwrite(&tp,1,3,fout);
            //printf("%d %d %d\n",tp.pre,tp.size,tp.c);
        }
    }

}

void decode(){
    int len;
    vector<unsigned char>all;
    while((len=fread(buff,1,MAX_SIZE/3*3,fin))>0){
        for(int i=0;i<len;i+=3){
            triple tp(buff[i],buff[i+1],buff[i+2]);
            //printf("%d %d %d\n",tp.pre,tp.size,tp.c);
            int cur=all.size();
            for(int j=0;j<tp.size;j++){
                all.push_back(all[cur-tp.pre+j]);
            }
            all.push_back(tp.c);
        }
    }
    fwrite(all.data(),1,all.size(),fout);
}
int main(int argc,char **argv){
    if(argc!=4){
        puts("./lz77 encode/decode <in-file> <out-file>");
        return 0;
    }
    fin=fopen(argv[2],"rb");
    fout=fopen(argv[3],"wb");

    if(argv[1][0]=='e'){
        encode();
    }else{
        decode();
    }


    fclose(fin);
    fclose(fout);

    return 0;
}