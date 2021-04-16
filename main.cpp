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

bool isInt(string s){
    s=whitespace(s);
    for(int i=0;i<s.length();i++){
        if(s.at(i)<48 || s.at(i)>57){
            return false;
        }
    }
    return true;
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
stack<string> infixToPostfix(string str){
    stack<string> output;
    stack<char> tmp;

    string s="";

    for(int i=0;i<str.length();i++){
        if(str[i]!=' '){
            s=s+str[i];
        }
    }
    // cout << str;

    for(int i=0;i<s.length();i++){
        //cout << s[i] << endl;
        bool integer=false;
        int length=0;
        int j=i;
        while(s[j]!='('&& s[j]!=')'&& s[j]!='*'&&s[j]!='+'&&s[j]!='/'&&s[j]!='-'&&j<s.length()){
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
    //cout <<"çıktım "<<endl;
    return output;
}

void operation(string x1,string x2, string op,int& tempno,vector<string> var,ofstream& outfile){
    bool xb=false;  //{x2 vectordeyse true oluyo ki tekrar yazmasın
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


string muko(string expr,ofstream& outfile,int& tempno,vector<string> vars){

    if(expr.find("+")==-1 &&expr.find("-")==-1&&expr.find("*")==-1&&expr.find("/")==-1){ //toplama vs yoksa
        if(!isInt(expr)){ //  t=f0 falan burda
            outfile<<"%t"<<tempno<<" = load i32* %"<<expr<<endl;
            tempno++;
            return "%t"+to_string(tempno-1);
        } else { //t=5 falan
            return expr;
        }
    } else {// t=t-1 vs
        stack<string> s=tepetaklak(infixToPostfix(expr)); //stack i ters çevirmem gerekti doğru sırayla poplamak için
        stack<string> t; //temporary bir stack bu operator gelene kadar popladıklarımızı burda tutuyoz operator bulunca geri 2 tane popluyoz
        //printStack(s);

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

                if(!s.empty()){
                    string n="%t"+to_string(tempno); //buralar hep yazdırma kısmı
                    s.push(n); //vectore yeni variable ımızı pushladık
                    tempno++;
                }
            }else{ // operator değilse temp e pushluyoruz
                t.push(s.top());
                s.pop();

            }
        }
        tempno++;
        return "%t"+to_string(tempno-1);
    }

}
bool errorCatchForExpressions(string line){
    //bu finksiyonu asağıdaki fonksiyonun içinde çağırcaz
    //orn print st ise icindeki expressionı buraya yollicak 5++ 3 -4//? falan diye saçmalamış mı diye bakcaz
}
bool errorCatch(string line){
    int comment=line.find("#");
    if(comment!=-1){
        line=line.substr(0,comment); //commentli kısmı kestik attık
    }
    //valid lines are 1- asignment 2- empty line 3- printSt 4- if st 5- while st 6- }
    //BUNLARDAN BIRININ SYNTAXINA UYUYO MU DIYE BAKCAZ AKSI HALDE FALSE DONCEK
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
    outfile<< endl;

    string line;

    while(getline(infile,line)){

        int found=line.find("=");

        if(found != string::npos ){
            string s=line.substr(0,found);
            s=whitespace(s);
            if(!count(vars.begin(), vars.end(), s)){ //variable vectorde yoksa vektore ekleyip allocate ediyoz
                vars.push_back(s);
                outfile << "%" << s <<" = alloca i32" << endl;
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

    bool inWhile=false;
    bool inIf=false;

    //BISMILLAHIRRAHMANIRRAHIM ALLAH CC HELP US IF YOU EXIST
    //dunyanın en basıc kodunu gormeye hazır ol <3
    while(getline(infile,line)){
        int found=line.find("=");
        bool whil=false;
        bool ifSt=false;
        bool printSt=false;
        bool assignment=false;

        if(found!=string::npos){
            assignment=true;
        }

        if(whitespace(line)=="}"&&inWhile){ //while'ın içindeysek ve while bittiyse
            outfile << "br label %whcond" << endl;
            outfile << endl;
            outfile << "whend:" << endl << endl;
            inWhile=false;
        }

        if(whitespace(line)=="}"&&inIf){ //if'in içindeysek ve if bittiyse
            outfile << "ifend:" << endl;
            inIf=false;
        }

        if(line.find("while")!=-1){//while ise
            whil=true;
            inWhile=true;
            outfile<<endl;
            outfile << "br label %whcond" << endl;
            outfile<<"whcond:"<<endl;
        }
        if(line.find("print")!=-1){ //print ise
            printSt=true;
        }

        if(line.find("if")!=-1){ //print ise
            outfile << "br label %ifcond" << endl;
            outfile << "ifcond:" << endl;
            ifSt=true;
            inIf=true;
        }

        while(line.find("choose")!=-1){ //choose varsa içindekileri alcaz choose(  3,n+1 ,2,f0 )
            int acpar=line.find("(");
            int kappar=line.find_last_of(")");
            string incho=line.substr(acpar+1,kappar-acpar-1);
            char v=',';
            int vi1=incho.find(v);    //virgul yerleri
            int vi2=incho.find(v,vi1+1);
            int vi3=incho.find(v,vi2+1);
            string exp1=incho.substr(0,vi1);  //expressionları tek tek aldım
            string exp2=incho.substr(vi1+1,vi2-vi1-1);
            string exp3=incho.substr(vi2+1,vi3-vi2-1);
            string exp4=incho.substr(vi3+1,kappar-vi3-1);
            exp1=whitespace(exp1);   //buna gerek yok heralde ama yine de yapim dedim
            exp2=whitespace(exp2);
            exp3=whitespace(exp3);
            exp4=whitespace(exp4);
            string res1=muko(exp1,outfile,tempno,vars);
        }

        if(assignment || whil || printSt || ifSt){
            string s;
            string expr;
            string sol;
            if(whil || printSt || ifSt){
                int acpar=line.find("(");
                int kappar=line.find_last_of(")");
                expr=line.substr(acpar+1,kappar-acpar-1); //parantezin içini aldım
                expr=whitespace(expr);
                // cout << expr << endl;

            } else {
                expr=line.substr(found+1); //assignmentin expr kısmı
                sol=line.substr(0,found); //assignmentın sol kısmı
                expr=whitespace(expr);
                sol=whitespace(sol);
            }
            string res=muko(expr,outfile,tempno,vars);

            if(whil){ //while ise yazılan şeyler
                outfile<<"%t"<<tempno<<" = icmp ne i32 "<<res<<", 0"<<endl; //buraya tam ne yazcağımızı anlamadım tekrar bakmak lazım
                outfile<<"br i1 %t"<<tempno<<", label %whbody, label %whend"<<endl; //"    "    " "    "       "     "
                outfile<<endl;
                outfile<<"whbody:"<<endl;
                tempno++;
            } else if(printSt){
                outfile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " << res<<" )"<<endl;
            } else if(ifSt){
                outfile<<"%t"<<tempno<<" = icmp ne i32 "<<res<<", 0"<<endl;
                outfile<<"br i1 %t"<<tempno<<", label %ifbody, label %ifend"<<endl;
                outfile << endl;
                outfile << "ifbody:" << endl;
                tempno++;
            } else if(assignment){
                outfile<<"store i32 "<<res<<", i32* %";
                //tempno++;
                string sol=line.substr(0,found); // buralar hep yazdırma kısmı
                sol=whitespace(sol);
                outfile<<sol<<endl;
            }
        }
    }

    outfile << "ret i32 0" << endl;
    outfile << "}";

    return 0;
}