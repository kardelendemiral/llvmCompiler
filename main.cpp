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

string choose(int& chooseno,string line,ofstream& outfile, vector<string> &vars,int &tempno);

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
        bool notOperation=false;
        int length=0;
        int j=i;
        while(s[j]!='('&& s[j]!=')'&& s[j]!='*'&&s[j]!='+'&&s[j]!='/'&&s[j]!='-'&&j<s.length()){
            notOperation=true;
            length++;
            j++;
        }
        if(notOperation){
            string operand;
            operand=s.substr(i,length);
            if(operand=="choose"){
                string a=s.substr(i);
                int ilk=a.find("(");
                int chooselength=length;
                int count=1;
                int k=ilk+1;
                //cout <<"ilk "<< ilk <<endl;
                while(k<a.length()&&count!=0){
                    if(a[k]=='('){
                        count++;
                    } else if(a[k]==')'){
                        count--;
                    }
                    chooselength++;
                    k++;
                }
                operand=s.substr(i,chooselength+1);
                //cout << chooselength<< " " << length << " "<<operand <<endl;
                i=i+chooselength;
            } else {
                i=j-1;
            }
            output.push(operand);
            //cout << operand<< " "<< i<< " " <<j << endl;
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
        op="sdiv";
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


string muko(string expr,ofstream& outfile,int& tempno,vector<string> &vars,int chooseno){

    expr=whitespace(expr);


    stack<string> s=tepetaklak(infixToPostfix(expr)); //stack i ters çevirmem gerekti doğru sırayla poplamak için
    stack<string> t; //temporary bir stack bu operator gelene kadar popladıklarımızı burda tutuyoz operator bulunca geri 2 tane popluyoz
    //printStack(s);

    if(s.size()==1){ //stackte sadece 1 şey varsa expression sadece 1 elemanlıdır
        string str=s.top();
        if(expr.substr(0,6)=="choose"&&expr[expr.length()-1]==')'){ //sagda sadece choose var
            chooseno++;
            return choose(chooseno,expr,outfile,vars,tempno);
        }
        if(isInt(str)){ //sayı ya da tempse kendini dönüyoz
            return str;
        }

        if(find(vars.begin(),vars.end(),str)==vars.end()){  //variablelarda yok allocate etcez
            outfile << "%" << str <<" = alloca i32" << endl;
            outfile << "store i32 0, i32* %" << str << endl;
            vars.push_back(str);
        }
        outfile<<"%t"<<tempno<<" = load i32* %"<<expr<<endl; //variable olduğu için load ediyoz
        tempno++;
        return "%t"+to_string(tempno-1);

    }

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

            if(x1.substr(0,6)=="choose"){
                chooseno++;
                x1=choose(chooseno,x1,outfile,vars,tempno);
            } else if(find(vars.begin(),vars.end(),x1)==vars.end()&&!isInt(x1)&&x1[0]!='%'){ //variablelarda yok allocate etcez
                outfile << "%" << x1 <<" = alloca i32" << endl;
                outfile << "store i32 0, i32* %" << x1 << endl;
                vars.push_back(x1);
            }

            if(x2.substr(0,6)=="choose"){
                chooseno++;
                x2=choose(chooseno,x2,outfile,vars,tempno);
            } else if(find(vars.begin(),vars.end(),x2)==vars.end()&&!isInt(x2)&&x2[0]!='%'){ //variablelarda yok allocate etcez
                outfile << "%" << x2 <<" = alloca i32" << endl;
                outfile << "store i32 0, i32* %" << x2 << endl;
                vars.push_back(x2);
            }

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

string choose(int& chooseno,string line,ofstream& outfile, vector<string> &vars,int &tempno){ //her türlü choose1 temporary döndercek
    int acpar=line.find("(");
    int kappar=line.find_last_of(")");
    string incho=line.substr(acpar+1,kappar-acpar-1);
    char v=',';
    vector<int> virguller(3);
    bool parantez=false;
    int count=0;
    for(int i=0;i<incho.length();i++){
        if(incho[i]==','&&!parantez){
            virguller[count]=i;
            count++;
        } else if(incho[i]=='('){
            parantez=true;
        } else if(incho[i]==')'){
            parantez=false;
        }
    }

    string exp1=incho.substr(0,virguller[0]);  //expressionları tek tek aldım
    string exp2=incho.substr(virguller[0]+1,virguller[1]-virguller[0]-1);
    string exp3=incho.substr(virguller[1]+1,virguller[2]-virguller[1]-1);
    string exp4=incho.substr(virguller[2]+1,kappar-virguller[2]-1);
    exp1=whitespace(exp1);   //buna gerek yok heralde ama yine de yapim dedim
    exp2=whitespace(exp2);   //0
    exp3=whitespace(exp3);   //+
    exp4=whitespace(exp4);   //-
    //cout <<exp1 <<" "<<exp2 <<" "<<exp3 <<" "<<exp4 <<endl;
    string res1=muko(exp1,outfile,tempno,vars,chooseno);

    string res2=muko(exp2,outfile,tempno,vars,chooseno);
    string res3=muko(exp3,outfile,tempno,vars,chooseno);

    outfile << "%t" << tempno <<" = icmp eq i32 "<< res1 <<", 0" <<endl; //0 mı?
    tempno++;

    outfile << "%t" << tempno << " = select i1 "<< "%t" <<tempno-1<< ", i32 " << res2 <<", i32 "<<res3 <<endl; //0'sa res2 değilse res3
    string a="%t"+to_string(tempno);
    tempno++;

    string res4=muko(exp4,outfile,tempno,vars,chooseno);

    outfile << "%t" << tempno <<" = icmp slt i32 "<< res1 <<", 0" <<endl; //negatif mi?
    tempno++;

    outfile << "%t" << tempno << " = select i1 "<< "%t" <<tempno-1<< ", i32 " << res4 <<", i32 "<<a <<endl; //negatifse res4 değilse önceki
    tempno++;

    return "%t"+to_string(tempno-1);

    /*
    outfile << "br i1 %t" <<tempno<<", label %exp2"<<chooseno<<", label %exp2"<<chooseno<<"end" << endl;
    tempno++;
    outfile << "exp2"<<chooseno <<":"<<endl;
    string res2=muko(exp2,outfile,tempno,vars,chooseno);
    outfile<<"%choose"<<chooseno<<" = select i1 true, i32 "<<res2<<",i32 0"<<endl;
    outfile<<"br label %choose"<<chooseno<<"end"<<endl;
    outfile << "exp2"<<chooseno<<"end:"<<endl;
    outfile << "%t" <<tempno << " = icmp ugt i32 " << res1 << ", 0" <<endl; //pozitifse doğru
    outfile << "br i1 %t" <<tempno<<", label %exp3"<<chooseno<<", label %exp3"<<chooseno<<"end"<< endl;
    tempno++;
    outfile << "exp3"<<chooseno<<":" <<endl;
    string res3=muko(exp3,outfile,tempno,vars,chooseno);
    outfile<<"%choose"<<chooseno<<" = select i1 true, i32 "<<res3<<",i32 0"<<endl;
    outfile<<"br label %choose"<<chooseno<<"end"<<endl;
    outfile << "exp3"<<chooseno<<"end:"<<endl;
    outfile << "%t" <<tempno << " = icmp ult i32 " << res1 << ", 0" <<endl; //pozitifse doğru
    outfile << "br i1 %t" <<tempno<<", label %exp4"<<chooseno<<", label %exp4"<<chooseno<<"end"<< endl;
    tempno++;
    outfile << "exp4"<<chooseno <<":"<<endl;
    string res4=muko(exp4,outfile,tempno,vars,chooseno);
    outfile<<"%choose"<<chooseno<<" = select i1 true, i32 "<<res4<<",i32 0"<<endl;
    outfile<<"br label %choose"<<chooseno<<"end"<<endl;
    outfile << "exp4"<<chooseno<<"end:"<< endl;
    outfile<<"choose"<<chooseno<<"end:"<<endl;
    //chooseno++;
    return "choose"+to_string(chooseno);*/

}


bool errorCatchForExpressions(string s){
    if(s.find("=")!=-1 || s.find("print")!=-1 || s.find("while")!=-1 || s.find("if")!=-1){ //bunlar varsa error
        cout<<"false";
        return false;
    }
    s=whitespace(s);
    if((s[0]<48||s[0]>57)&&(s[0]<65||s[0]>90)&&(s[0]<97||s[0]>122) && s[0]!='('){ //bunlardan biriyle başlamıyosa error
        cout<<"false";
        return false;
    }
    if(s.find("(")!=-1 || s.find(")")!=-1){ //parantez checki
        stack<char> st;
        for(int i=0;i<s.length();i++){
            if(s[i]=='('){
                st.push('(');
            }else if(s[i]==')'){
                if(st.empty() || st.top()!='('){
                    cout<<"false";
                    return false;
                }
                st.pop();
            }
        }
        if(st.size()!=0){
            cout<< "false";
            return false;
        }
    }


    for(int i=0;i<s.length();i++){
        //cout << s[i] << endl;
        if(s[i]==' '){
            continue;
        }
        bool notOperation=false;
        int length=0;
        int j=i;
        while(s[j]!='('&& s[j]!=')'&& s[j]!='*'&&s[j]!='+'&&s[j]!='/'&&s[j]!='-'&&s[j]!=' ' &&j<s.length()){ //variableları alıyo
            notOperation=true;
            length++;
            j++;
        }
        if(notOperation){
            string operand;
            operand=s.substr(i,length);
            if(operand=="choose"){
                string a=s.substr(i);
                int ilk=a.find("(");
                int chooselength=length;
                int count=1;
                int k=ilk+1;
                //cout <<"ilk "<< ilk <<endl;
                while(k<a.length()&&count!=0){
                    if(a[k]=='('){
                        count++;
                    } else if(a[k]==')'){
                        count--;
                    }
                    chooselength++;
                    k++;
                }
                operand=s.substr(i,chooselength+1);
                //cout << chooselength<< " " << length << " "<<operand <<endl;
                i=i+chooselength;
            } else {
                i=j-1;
            }
            bool f=false;
            for(int k=i+1;k<s.length();k++){ //burda i+1 mi j+1 mi olacak emin değilim
                if(s[k]!=' '){//
                    f=true;
                }
                if(s[k]!='+' && s[k]!='-' &&s[k]!='*' && s[k]!='/'&& s[k]!=')' &&s[k]!=' ' ){ //variabledan sonra bunlardan biri yoksa error
                    cout<<"false";
                    return false;
                }
                if(f){
                    break;
                }
            }

        } else{ //operatorlerden sonra bunlar yoksa error
            bool m=false;
            for(int q=j+1;q<s.length();q++){ //burda i+1 mi j+1 mi olacak emin değilim
                if(s[q]!=' '){
                    m=true;
                }
                if( (s[q]<48||s[q]>57)&&(s[q]<65||s[q]>90)&&(s[q]<97||s[q]>122) && s[q]!='(' && s[q]!=' '){
                    cout<<"false";
                    return false;
                }
                if(m){
                    break;
                }
            }


        }

    }
    //bu finksiyonu asağıdaki fonksiyonun içinde çağırcaz
    //orn print st ise icindeki expressionı buraya yollicak 5++ 3 -4//? falan diye saçmalamış mı diye bakcaz
}
bool errorCatch(string line, bool inWhile ,bool inIf){
    int comment=line.find("#");
    if(comment!=-1){
        line=line.substr(0,comment); //commentli kısmı kestik attık
    }
    line=whitespace(line);
    if(line==""){ //bos line sa true
        return true;
    }
    if(line=="}" ){
        if(!inIf && !inWhile){
            return false;
        } else {
            return true;
        }
    }
    if(line.substr(0,5)=="while"){
        if(inWhile || inIf){
            return false;
        }
        int firstpr=5;
        while(line[firstpr]==' '&&firstpr<line.length()){
            firstpr++;
        }
        int lastpr=line.length()-2; // } dan bi önceki
        while(line[lastpr]==' '&&lastpr>=0){
            lastpr--;
        }
        if(line[firstpr]=='(' && line[lastpr]==')' && line[line.length()-1]=='}'){
            return errorCatchForExpressions(line.substr(firstpr+1,lastpr-firstpr-1));
        } else {
            return false;
        }
    }
    if(line.substr(0,5)=="print"){
        if(line[5]=='(' && line[line.length()-1]==')'){
            return errorCatchForExpressions(line.substr(6,line.length()-7));
        } else {
            return false;
        }

    }
    if(line.substr(0,2)=="if"){
        if(inWhile || inIf){
            return false;
        }
        int firstpr=2;
        while(line[firstpr]==' '&&firstpr<line.length()){
            firstpr++;
        }
        int lastpr=line.length()-2; // } dan bi önceki
        while(line[lastpr]==' '&&lastpr>=0){
            lastpr--;
        }
        if(line[firstpr]=='(' && line[lastpr]==')' && line[line.length()-1]=='}'){
            return errorCatchForExpressions(line.substr(firstpr+1,lastpr-firstpr-1));
        } else {
            return false;
        }
    }
    int eq=line.find("=");
    if(eq!=-1){
        string left=line.substr(0,eq);
        string right=line.substr(eq+1);
        bool leftBool=errorCatchForExpressions(left);
        bool rightBool=errorCatchForExpressions(right);
        if(leftBool&&rightBool){
            return true;
        } else {
            return false;
        }
    }
    return false;
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
    int chooseno=1;

    outfile << "; ModuleID = 'mylang2ir'\n"
               "declare i32 @printf(i8*, ...)\n"
               "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl << endl;
    outfile<<"define i32 @main() {"<<endl;
    outfile<< endl;

    string line;
    errorCatchForExpressions("  345  + ab -c + choose(1+choose(2+n,15,6,9),2,3+choose(1,2,3,4),4) -12 ");


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

    /* int a=1;
     choose(a,"choose( n+1 ,  9,   8, choose(o*h ,1,2,3) )",outfile,vars,tempno);*/

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
            outfile<<"br label %ifend"<<endl;
            outfile << "ifend:" <<endl<< endl;
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

        if(line.find("if")!=-1){ //if ise
            outfile << "br label %ifcond" << endl;
            outfile << "ifcond:" << endl;
            ifSt=true;
            inIf=true;
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
            string res=muko(expr,outfile,tempno,vars,chooseno);

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