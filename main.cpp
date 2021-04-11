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
    bool syntaxError=false;
    int tmpCount=0;

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
        if(found!=string::npos){ //ASSIGNMENT STATEMENT
            string sag=line.substr(found+1); //assignmentin sag kısmı
            string sol=line.substr(0,found); //assignmentın sol kısmı

            while(sag[0]==' '){  //bastaki boslukları sil
                sag=sag.substr(1);
            }
            while(sag[sag.size()-1]==' '){  //sondaki boslukları sil
                sag=sag.substr(0,sag.size()-1);
            }
            while(sol[0]==' '){  //bastaki boslukları sil
                sol=sol.substr(1);
            }
            while(sol[sol.size()-1]==' '){  //sondaki boslukları sil
                sol=sol.substr(0,sol.size()-1);
            }

            if(sol.find(" ")!=-1){
                syntaxError=true;
                cout << "ERROR";
            }

            if(sag.find("+")==-1 &&sag.find("-")==-1&&sag.find("*")==-1&&sag.find("/")==-1){ //toplama vs yoksa
                if(sag.find(" ")!=-1){ //islem yoksa sag tarafta sadece bi sey vardir. sondaki ve bastaki boslukları sildiğimiz için bosluk varsa error
                    syntaxError=true;
                    cout << "ERROR";
                }

                for(int i=0; i<sag.length();i++){
                    if(sag.at(i)<48 || sag.at(i)>57 ){ // [0-9] değilse çıkıyo (ascii tablo şeyi)
                        a=false;
                        break;
                    }
                }
                if(a==true){ //[0-9] ve toplama vs yok
                    outfile<<"store i32 "<<sag<<", i32* %"<<sol<<endl;
                }
            }
            //ASIL KABUS BURADA BASLIYOR :(((



        }

    }




    return 0;
}