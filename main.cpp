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

string whitespace(string x){ //whitespaceleri siliyo sağdan ve soldan, çok gerekcek diye methoda geçirdim
    while(x[0]==' '){  //bastaki boslukları sil
        x=x.substr(1);
    }
    while(x[x.size()-1]==' '){  //sondaki boslukları sil
        x=x.substr(0,x.size()-1);
    }
    return x;
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

    s=whitespace(s);

    if(s[0]=='-'){
        s="0-"+s.substr(1);
    }
    char last =s[0];
    for(int i=1;i<s.length()-1;i++){
        if(s[i]==' '){
            continue;
        }
        if(last=='*' || last=='-'|| last=='+'|| last=='/'|| last=='('){
            if(s[i]=='-'){
                int length=0;
                int j=i+1;
                while(j<s.length()&&s[j]>=48 && s[j]<=57){
                    length++;
                    j++;
                }
                string sol=s.substr(0,i);
                string sag=s.substr(j);
                string sayi=s.substr(i+1,length);
                s=sol+"(0-"+sayi+")"+sag;
                i=i+3;
            }

        }
        last=s[i];
    }

    //cout << "string: " <<s << endl;


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

void operation(string x1,string x2, string op,int& tempno,vector<string> var,ofstream& outfile){
    bool xb=false;  //x2 vectordeyse true oluyo ki tekrar yazmasın
    bool xi=false;  //x1    "          "       "    "       "

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
    //cout<<"x1: "<<x1<<"x2: "<<x2<<endl;
    if(find(var.begin(),var.end(),x2)!=var.end()){ //eğer vectorde varsa int değil
        outfile<<"%t"<<ini<<" = load i32* %"<<x2<<endl;
        xi=true;
        ini++;
    }
    if(count(var.begin(),var.end(),x1)){ //eğer vectorde varsa int değil demektir başında % olacak
        outfile<<"%t"<<ini<<" = load i32* %"<<x1<<endl;
        xb=true;
        ini++;
    }


    outfile<<"%t"<<ini<<" = "<< op <<" i32 "; //add sub vs yazılıyo
    if(xi==true){  //bunların hepsi int ise % yazmaması gerektiği için yazıldı
        outfile<<"%t"<<tempno<<", ";  //çok uzun saçma bi kod oldu ben mal mıyım :(
        tempno++;
    }else{
        outfile<<x2<<", ";
    }
    if(xb==true){
        outfile<<"%t"<<tempno<<endl;
        tempno++;
    }else{
        outfile<<x1<<endl;
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
    int tempno=1;

    outfile << "; ModuleID = 'mylang2ir'\n"
               "declare i32 @printf(i8*, ...)\n"
               "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl << endl;
    outfile<<"define i32 @main() {"<<endl;

    string line;

    while(getline(infile,line)){

        int found=line.find("=");

        if(found != string::npos ){

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
        int wh=line.find("while");
        bool whil=false;

        if(wh!=-1){//while ise
            whil=true;
            outfile<<endl;
            outfile<<"whcond:"<<endl;

        }
        if(found!=string::npos|| whil== true ){ //ASSIGNMENT STATEMENT
            string s;
            if(whil==true){
                int acpar=line.find("(");
                int kappar=line.find(")");
                s=line.substr(acpar+1,kappar-acpar-1); //parantezin içini aldım
                s=whitespace(s);

            }
            string sag=line.substr(found+1); //assignmentin sag kısmı
            string sol=line.substr(0,found); //assignmentın sol kısmı

            sag=whitespace(sag);
            sol=whitespace(sol);
            if(whil==true){
                sag=s; //assingmentta sagdaki işlemlerin aynısını yapacağımız için sag a eşitledim
                cout<<sag<<endl;
            }

            if(sol.find(" ")!=-1 || sol.length()==0 || sag.length()==0 ){ // hiçbişey yazmıyosa veya boşluk varsa error
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
                        // t=f0 falan burda

                        a=false;
                        outfile<<"%t"<<tempno<<" = load i32* %"<<sag<<endl;
                        if(whil==false){ //whileda bunu yazdırmıyoz
                            outfile<<"store i32 %t"<<tempno<<", i32* %"<<sol<<endl;
                        }
                        tempno++;
                    }
                }
                if(a==true){ //[0-9] ve toplama vs yok
                    // t= 1 vs
                    outfile<<"store i32 "<<sag<<", i32* %"<<sol<<endl;

                }
            }

            else{  // t=t-1 vs
                stack<string> s=tepetaklak(infixToPostfix(sag)); //stack i ters çevirmem gerekti doğru sırayla poplamak için
                stack<string> t; //temporary bir stack bu operator gelene kadar popladıklarımızı burda tutuyoz operator bulunca geri 2 tane popluyoz

                while(!s.empty()){
                    if(s.top()=="+" || s.top()=="*" || s.top()=="/" || s.top()=="-"){ //eğer operator bulursa tempteki 2 taneyi poplayacak
                        string op=s.top();
                        s.pop();
                        string x1=t.top();
                        t.pop();
                        string x2=t.top();
                        t.pop();
                        x1=whitespace(x1); //bunlarsız boku yiyoruz
                        x2=whitespace(x2); //    "      "      "

                        operation(x1,x2,op,tempno,vars,outfile);//bunu yukarda açıklıyom add falan yazdırılan kısım bu

                        if(s.empty() && whil== false){//eğer stack boşaldıysa sağ tarafta bir şey kalmamış demektir ve sol tarafa store ediyoruz
                            //whileda yapmıyoz bu kısmı da
                            outfile<<"store i32 %t"<<tempno<<", i32* %";
                            tempno++;
                            string sol=line.substr(0,found); // buralar hep yazdırma kısmı
                            sol=whitespace(sol);
                            outfile<<sol<<endl;
                        }else{ //daha stack boşalmadığı için devam

                            string n="%t"+to_string(tempno); //buralar hep yazdırma kısmı
                            s.push(n); //vectore yeni variable ımızı pushladık
                            tempno++;

                        }

                    }else{ // operator değilse temp e pushluyoruz
                        t.push(s.top());
                        s.pop();

                    }
                }
            }
            //ASIL KABUS BURADA BASLIYOR :(((

            //KABUS YARİLANDİ GİBİ SUPHANALLAH İLK DEFA GORENLER BEGENSİN
            if(whil==true){ //while ise yazılan şeyler
                outfile<<"%t"<<tempno<<" = icmp ne i32 %t"<<tempno-1<<", 0"<<endl; //buraya tam ne yazcağımızı anlamadım tekrar bakmak lazım
                outfile<<"br i1 %t"<<tempno<<", label %whbody, label %whend"<<endl; //"    "    " "    "       "     "
                outfile<<endl;
                outfile<<"whbody:"<<endl;
                tempno++;

            }


        }






    }




    return 0;
}