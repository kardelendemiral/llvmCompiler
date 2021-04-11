#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]) {

    string infileName=argv[1];
    string outfileName=argv[2];

    ifstream infile;
    ofstream outfile;

    infile.open(infileName);
    outfile.open(outfileName);

    vector<string> vars; //variable'lar bu vectorde hep

    outfile << "; ModuleID = 'mylang2ir'\n"
               "declare i32 @printf(i8*, ...)\n"
               "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl << endl;

    string line;

    while(getline(infile,line)){
        int found=line.find("=");
        if(found != string::npos){
            string s=line.substr(0,found);

            int n =s.length();
            char ar[n+1]; // token şeyini yapmam için stringi char arrayine çevirmem gerekti başka yolunu bulamadım
            strcpy(ar,s.c_str()); //stringi char arrayine kopyalıyo
            char* token = strtok(ar," "); // whitespace e göre ayırdı tokenları

            if(!count(vars.begin(), vars.end(), token)){ //variable vectorde yoksa vektore ekleyip allocate ediyoz
                vars.push_back(token);
                outfile << "%" << token <<" = alloca i32" << endl;

            }

        }

    }

    outfile << endl;

    for(int i=0;i<vars.size();i++){
        outfile << "store i32 0, i32* %" << vars[i] << endl;
    }

    outfile << endl;

    infile.clear(); //burda file'ı okuduk bitti tekrar başlamak istiyoz o yüzden bu satırları yazmam gerekti
    infile.seekg(0, infile.beg);

    //BISMILLAHIRRAHMANIRRAHIM ALLAH CC HELP US IF YOU EXIST
    //dunyanın en basıc kodunu gormeye hazır ol <3
    while(getline(infile,line)){
        int found=line.find("=");
        bool a=true;
        if(found!=string::npos){
            string s=line.substr(found+1);
            if(s.find("+")==-1 &&s.find("-")==-1&&s.find("*")==-1&&s.find("/")==-1){ //toplama vs yoksa
                for(int i=0; i<s.length();i++){
                    if(s.at(i)==32){ //whitespace ise geçiyo
                        continue;
                    }
                    if(s.at(i)<48 || s.at(i)>57){ // [0-9] değilse çıkıyo (ascii tablo şeyi)
                        a=false;
                        break;
                    }
                }
                if(a==true){ //[0-9] ve toplama vs yok
                    //bu kısım biraz inefficient oldu daha mantıklı bişey bulabiliriz
                    string bef=line.substr(0,found); //variable nameini alıyom bu satırlarda+
                    int n =bef.length();
                    char a[n+1];
                    strcpy(a,bef.c_str());
                    char* name = strtok(a," ");//+
                    int k =s.length();
                    char a2[n+1];
                    strcpy(a2,s.c_str());
                    char* value = strtok(a2," ");//bu da valuesu
                    outfile<<"store i32 "<<value<<", i32* %"<<name<<endl;
                    //NOTLARRR:
                    /* burda toplama vs yoksa = den sonra sadece 1 sayının olduğunu assume ettim eğer syntax
                     * hatası falan verirlerse sıçmak mesela k=   verirse olmuyo aynı şekilde a= 3   2  da olmaz
                     * ama  boşlukları falan hallediyo
                     * strtok yerine bişey bulsak iyi olcak :(
                    */

                }
            }

        }

    }




    return 0;
}