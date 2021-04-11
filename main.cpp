#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <stack>
#include <queue>

using namespace std;
stack<string> tepetaklak(stack<string> s){
    stack<string> t;
    while(!s.empty()){
        t.push(s.top());
        s.pop();
    }
    return t;
}
void printStack(stack<string> s){
    stack<string> tmp;
    while(!s.empty()){
        tmp.push(s.top());
        s.pop();
    }
    while(!tmp.empty()){
        cout << tmp.top() <<" ";
        tmp.pop();
    }
}
void printStack(stack<char> s){
    stack<char> tmp;
    while(!s.empty()){
        tmp.push(s.top());
        s.pop();
    }
    while(!tmp.empty()){
        cout << tmp.top() <<" ";
        tmp.pop();
    }
}

int precedence ( char a ){
    if(a == '+' || a =='-'){
        return 1;
    }

    if(a == '*' || a =='/'){
        return 2;
    }
    return 0;
}
stack<string> infixToPostfix(string s){
    stack<string> output;
    stack<char> tmp;

    for(int i=0;i<s.length();i++){
        //cout << s[i] << endl;
        bool integer=false;
        int length=0;
        int j=i;
        while(s[j]!='('&& s[j]!=')'&& s[j]!='*'&&s[j]!='+'&&s[j]!='/'&&s[j]!='-'){
            integer=true;
            length++;
            j++;
        }
        if(integer){
            string operand;
            operand=s.substr(i,length);
            output.push(operand);
            //cout << operand<< " "<< i<< " " <<j << endl;
            i=j-1;
        } else if(s[i]=='('){
            tmp.push('(');
        } else if(s[i]==')'){
            while(!tmp.empty() && tmp.top()!='('){
                string a(1, tmp.top());
                //cout<< "outputa koydum: " << a<< endl;
                output.push(a);
                tmp.pop();
            }
            tmp.pop();
        } else {
            if(tmp.empty()){
                tmp.push(s[i]);
            } else {
                while(!tmp.empty()&& precedence(s[i])<=precedence(tmp.top())){
                    string a(1,tmp.top());
                    output.push(a);
                    tmp.pop();
                }
                tmp.push(s[i]);
            }
        }
        /*cout << i << ": " << "tmp: ";
        printStack(tmp);
        cout << "output: ";
        printStack(output);
        cout << endl;*/
    }
    while(!tmp.empty()){
        string a(1,tmp.top());
        output.push(a);
        tmp.pop();
    }
    return output;
}

string whitespace(string x){ //whitespaceleri siliyo sağdan ve soldan, çok gerekcek diye methoda geçirdim
    while(x[0]==' '){  //bastaki boslukları sil
        x=x.substr(1);
    }
    while(x[x.size()-1]==' '){  //sondaki boslukları sil
        x=x.substr(0,x.size()-1);
    }
    return x;
}

void operation(string x1,string x2, string op,int& tempno,vector<string> var){  //outfile yapamadım ondan cout suan
        bool xb=false;
        bool xi=false;

        int ini=tempno;
        if(op=="+"){
            op="add";
        }if(op=="-"){
        op="sub";
        }if(op=="*"){
        op="mul";
        }if(op=="/"){
        op="div";
        }
        //cout<<"x1:"<<x1<<"x2:"<<x2<<endl;
        if(count(var.begin(),var.end(),x1)){ //eğer vectorde varsa int değil demektir başında % olacak
            cout<<"%t"<<ini<<" = load i32* %"<<x1<<endl;
            xb=true;
            ini++;
        }
        if(find(var.begin(),var.end(),x2)!=var.end()){
            cout<<"%t"<<ini<<" = load i32* %"<<x2<<endl;
            xi=true;
            ini++;
        }

        cout<<"%t"<<ini<<" = "<< op <<" i32 ";
        if(xi==true){  //bunların hepsi int ise % yazmaması gerektiği için yazıldı
            cout<<"%t"<<tempno<<", ";  //çok uzun saçma bi kod oldu ben mal mıyım :(
            tempno++;
        }else{
            cout<<x2<<", ";
        }
        if(xb==true){
            cout<<"%t"<<tempno<<endl;
            tempno++;
        }else{
            cout<<x1<<endl;
        }
}

int main(int argc, char* argv[]) {

    string infileName=argv[1];
    string outfileName=argv[2];

    ifstream infile;
    ofstream outfile;

    infile.open(infileName);
    outfile.open(outfileName);

    vector<string> vars; //variable'lar bu vectorde hep
    bool syntaxError=false;
    int tempno=0;

    outfile << "; ModuleID = 'mylang2ir'\n"
               "declare i32 @printf(i8*, ...)\n"
               "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl << endl;
    outfile<<"define i32 @main() {"<<endl;

    string line;

    while(getline(infile,line)){

        int found=line.find("=");
        if(found != string::npos){
            string s=line.substr(0,found);
            s=whitespace(s);
            if(s.find(" ")!=-1 || s.length()==0){  //namede bosluk varsa veya name yoksa error
                cout<<"ERROR"<<endl;
                continue;
            }else{

                if(!count(vars.begin(), vars.end(), s)){ //variable vectorde yoksa vektore ekleyip allocate ediyoz
                    vars.push_back(s);

                    outfile << "%" << s <<" = alloca i32" << endl;

                }

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

            sag=whitespace(sag);
            sol=whitespace(sol);

            if(sol.find(" ")!=-1 || sol.length()==0 || sag.length()==0 ){ // hiçbişey yazmıyosa veya boşluk varsa error
                syntaxError=true;
                cout << "ERROR";
                continue;
            }

            if(sag.find("+")==-1 &&sag.find("-")==-1&&sag.find("*")==-1&&sag.find("/")==-1){ //toplama vs yoksa
                if(sag.find(" ")!=-1){ //islem yoksa sag tarafta sadece bi sey vardir. sondaki ve bastaki boslukları sildiğimiz için bosluk varsa error
                    syntaxError=true;
                    cout << "ERROR";
                    continue;
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

            else{
                stack<string> s=tepetaklak(infixToPostfix(sag));
                stack<string> t;
               // cout<<"stack:";
               // printStack(s);
                //cout<<endl;
                while(!s.empty()){
                    if(s.top()=="+" || s.top()=="*" || s.top()=="/" || s.top()=="-"){
                        string op=s.top();
                        s.pop();
                        string x1=t.top();
                        t.pop();
                        string x2=t.top();
                        t.pop();
                        x1=whitespace(x1);
                        x2=whitespace(x2);

                        operation(x1,x2,op,tempno,vars);
                        cout<<"store i32 %t"<<tempno<<", i32* %";
                        tempno++;
                        if(s.empty()){
                            string sol=line.substr(0,found);
                            sol=whitespace(sol);
                            cout<<sol<<endl;
                        }else{
                            cout<<"t"<<tempno<<endl;
                            string n="%t"+to_string(tempno);
                            s.push(n);
                            tempno++;

                        }


                    }else{
                        t.push(s.top());
                        s.pop();

                    }
                }
            }
            //ASIL KABUS BURADA BASLIYOR :(((

            //KABUS YARİLANDİ GİBİ SUPHANALLAH İLK DEFA GORENLER BEGENSİN



        }


        if(line.find("while")!=-1){ //while
            //muthis bi ilerleme kaydettim fark ettiysen while bitmek üzere
        }

    }




    return 0;
}