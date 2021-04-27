
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stack>

using namespace std;

string choose(int& chooseno,string line,ofstream& outfile, vector<string> &vars,int &tempno);

stack<string> tepetaklak(stack<string> s){ //reverses the output of the infixtopostfix function to use it in operations in the correct order
    stack<string> t;
    while(!s.empty()){
        t.push(s.top());
        s.pop();
    }
    return t;
}
string whitespace(string x){ //erases the white spaces in a string from its beginning and the end
    if(x.length()==0){
        return x;
    }
    while(isspace(x[0])){  //bastaki boslukları sil
        x=x.substr(1);
        if(x.length()==0){
        return x;
     }
    }
    while(isspace(x[x.size()-1])){  //sondaki boslukları sil
        x=x.substr(0,x.size()-1);
        if(x.length()==0){
        return x;
    }
    }
    return x;
}

bool isInt(string s){ //determines if the parameter string is an integer
    s=whitespace(s);
    for(int i=0;i<s.length();i++){
        if(s.at(i)<'0' || s.at(i)>'9'){
            return false;
        }
    }
    return true;
}


int precedence ( char a ){ //this function is used in the infixtopostfix function to determine precedences of the operations
    if(a == '+' || a =='-'){
        return 1;
    }

    if(a == '*' || a =='/'){
        return 2;
    }
    return 0;
}
stack<string> infixToPostfix(string str){ //converts an infix notation to postfix notation. it also recognizes choose functions as whole then push them to the stack just like the integers and variables
    stack<string> output;
    stack<char> tmp;

    string s="";

    for(int i=0;i<str.length();i++){
        if(!isspace(str[i])){
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
                int ilk=a.find('(');
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

void operation(string x1,string x2, string op,int& tempno,vector<string> var,ofstream& outfile){  //this function takes two operands and an operator, then writes the neccessary llvm codes to the output file using the operation and operand types
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
        op="sdiv";
    }
    //cout<<"x1: "<<x1<<"x2: "<<x2<<endl;
    if(count(var.begin(),var.end(),x2)){ //is the operand a variable?
        outfile<<"%t"<<ini<<" = load i32* %"<<x2<<endl;
        xi=true;
        ini++;
    }
    if(count(var.begin(),var.end(),x1)){ 
        outfile<<"%t"<<ini<<" = load i32* %"<<x1<<endl;
        xb=true;
        ini++;
    }


    outfile<<"%t"<<ini<<" = "<< op <<" i32 "; 
    if(xi){ 
        outfile<<"%t"<<tempno<<", ";  
        tempno++;
    }else{
        outfile<<x2<<", ";
    }
    if(xb){
        outfile<<"%t"<<tempno<<endl;
        tempno++;
    }else{
        outfile<<x1<<endl;
    }
}


string muko(string expr,ofstream& outfile,int& tempno,vector<string> &vars,int chooseno){ //this is the most inportant function in the program. it takes expressions, analyze them and do the necessary operations callling the other functions

    expr=whitespace(expr);


    stack<string> s=tepetaklak(infixToPostfix(expr)); 
    stack<string> t; 
    //printStack(s);

    if(s.size()==1){ //if there's only one element in the stack then the expression is integer, variable or the choose function
        string str=s.top();
        if(expr.substr(0,6)=="choose"&&expr[expr.length()-1]==')'){ //choose
            chooseno++;
            return choose(chooseno,expr,outfile,vars,tempno);
        }
        if(find(vars.begin(),vars.end(),expr)!=vars.end()){ //variable
            outfile<<"%t"<<tempno<<" = load i32* %"<<expr<<endl; 
            tempno++;
            return "%t"+to_string(tempno-1);
        }
        return expr; //integer

    }

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

            if(x1.substr(0,6)=="choose"){
                chooseno++;
                x1=choose(chooseno,x1,outfile,vars,tempno);
            }

            if(x2.substr(0,6)=="choose"){
                chooseno++;
                x2=choose(chooseno,x2,outfile,vars,tempno);
            }

            operation(x1,x2,op,tempno,vars,outfile); 

            if(!s.empty()){
                string n="%t"+to_string(tempno);
                s.push(n);
                tempno++;
            }
        }else{ 
            t.push(s.top());
            s.pop();

        }
    }
    tempno++;
    return "%t"+to_string(tempno-1);

}

string choose(int& chooseno,string line,ofstream& outfile, vector<string> &vars,int &tempno){ //this function writes the necessary operations to the output file for the choose function
    int acpar=line.find('(');
    int kappar=line.find_last_of(')');
    string incho=line.substr(acpar+1,kappar-acpar-1);
    char v=',';
    vector<int> virguller(3);
    bool parantez=false;
    int count=0;
    int vco=0;
    stack<char>vi;

    for(int p=0; p<incho.length();p++){ //here we determine the locations of the commas in the inner expression of the choose function analyzing the parantheses
        if(incho[p]=='('){
            vi.push('(');    
        }
        if(incho[p]==')'){
            if(!vi.empty()){
                vi.pop();
            }
        }
        if(incho[p]==',' && vi.empty()){
            virguller[vco]=p;
            //cout<<p<<endl;
            vco++;
        }
    }

    string exp1=incho.substr(0,virguller[0]); //we seperate the for parameters for the choose function
    string exp2=incho.substr(virguller[0]+1,virguller[1]-virguller[0]-1);
    string exp3=incho.substr(virguller[1]+1,virguller[2]-virguller[1]-1);
    string exp4=incho.substr(virguller[2]+1,kappar-virguller[2]-1);
    exp1=whitespace(exp1);   //first expression
    exp2=whitespace(exp2);   //0
    exp3=whitespace(exp3);   //+
    exp4=whitespace(exp4);   //-
    //cout <<exp1 <<" "<<exp2 <<" "<<exp3 <<" "<<exp4 <<endl;
    string res1=muko(exp1,outfile,tempno,vars,chooseno); //expressions are sent to the expression handler function

    string res2=muko(exp2,outfile,tempno,vars,chooseno);
    string res3=muko(exp3,outfile,tempno,vars,chooseno);

    outfile << "%t" << tempno <<" = icmp eq i32 "<< res1 <<", 0" <<endl; //is it 0?
    tempno++;

    outfile << "%t" << tempno << " = select i1 "<< "%t" <<tempno-1<< ", i32 " << res2 <<", i32 "<<res3 <<endl; //if 0 then res2 if not res3
    string a="%t"+to_string(tempno);
    tempno++;

    string res4=muko(exp4,outfile,tempno,vars,chooseno);

    outfile << "%t" << tempno <<" = icmp slt i32 "<< res1 <<", 0" <<endl; //is it negative?
    tempno++;

    outfile << "%t" << tempno << " = select i1 "<< "%t" <<tempno-1<< ", i32 " << res4 <<", i32 "<<a <<endl; //ig negative then res4 if not the one we select earlier
    tempno++;

    return "%t"+to_string(tempno-1);


}
bool isValidVariableName(string str){ //this function is used in the error handler function to determine if a token have a valid variable name
    str=whitespace(str);
    if(str==""){
        return false;
    }
    if(!(str[0]>='a'&&str[0]<='z')&&!(str[0]>='A'&&str[0]<='Z')){
        return false;
    }
    for(int i=1;i<str.length();i++){
        if(!(str[i]>='0'&&str[i]<='9')&&!(str[i]>='a'&&str[i]<='z')&&!(str[i]>='A'&&str[i]<='Z')&&str[i]!='_'){
            return false;
        }
    }
    return true;
}

bool errorCatchForExpressions(string s){ //error checker function for expressions.

    s=whitespace(s);
    if(s.find('=')!=-1 ){
        return false;
    }
    if(s.length()==0){
        return false;
    }
    if((s[0]<48||s[0]>57)&&(s[0]<65||s[0]>90)&&(s[0]<97||s[0]>122) && s[0]!='('){ 

        return false;
    }
    if(s.find('(')!=-1 || s.find(')')!=-1){ //check if the parantheses are balanced
        stack<char> st;
        for(int i=0;i<s.length();i++){
            if(s[i]=='('){
                st.push('(');
            }else if(s[i]==')'){
                if(st.empty() || st.top()!='('){
                    //cout<<"false";
                    return false;
                }
                st.pop();
            }
        }
        if(st.size()!=0){

            return false;
        }
    }


    for(int i=0;i<s.length();i++){
        //cout << s[i] << endl;
        if(isspace(s[i])){
            continue;
        }
        bool notOperation=false;
        int length=0;
        int j=i;
        while(s[j]!='('&& s[j]!=')'&& s[j]!='*'&&s[j]!='+'&&s[j]!='/'&&s[j]!='-'&&!isspace(s[j]) &&j<s.length()){ //take the variables
            notOperation=true;
            length++;
            j++;
        }
        if(notOperation){
            string operand="";
            operand=s.substr(i,length);
            isValidVariableName(operand);

            if(operand=="if"|| operand=="while"|| operand=="print" ){ //they cannot be variables

                return false;
            }
            if(operand=="choose"){ //if it is choose function, chech if it has syntax error
                string a=s.substr(i);
                int ilk=a.find('(');
                int chooselength=length;
                int e=a.find('e');
                for(int w=e+1; w<ilk; w++){
                    if(isspace(a[w])){
                        chooselength++;
                    }else{
                        return false;
                    }
                }
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
                //  cout<<operand<<endl;
                i=i+chooselength;
                int acpar=operand.find('(');
                int kappar=operand.find_last_of(')');
                string incho=operand.substr(acpar+1,kappar-acpar-1);
                //cout <<incho << endl;
                char v=',';
                vector<int> virguller(3);
                bool parantez=false;
                int countt=0;
                int vco=0;
                stack<char> vi;
                for(int p=0; p<incho.length();p++){ 
                    if(incho[p]=='('){
                        vi.push('(');   
                    }
                    if(incho[p]==')'){
                        if(!vi.empty()){
                            vi.pop();
                        }
                    }
                    if(incho[p]==',' && vi.empty()){
                        virguller[vco]=p;
                        //cout<<p<<endl;
                        vco++;
                    }
                }
                if(vco!=3){

                    return false;
                }
                /*for(int y=0;y<incho.length();y++){
                    if(incho[y]==','&&!parantez){
                        virguller[countt]=y;
                        countt++;
                    } else if(incho[y]=='('){
                        parantez=true;
                    } else if(incho[y]==')'){
                        parantez=false;
                    }
                }*/
                string exp1=incho.substr(0,virguller[0]);  //expressionları tek tek aldım
                string exp2=incho.substr(virguller[0]+1,virguller[1]-virguller[0]-1);
                string exp3=incho.substr(virguller[1]+1,virguller[2]-virguller[1]-1);
                string exp4=incho.substr(virguller[2]+1,kappar-virguller[2]-1);
                // cout<<exp1<<" "<<exp2<<" "<<exp3<<" "<<exp4;
                if(!errorCatchForExpressions(exp1) || !errorCatchForExpressions(exp2)||!errorCatchForExpressions(exp3)||!errorCatchForExpressions(exp4) ){

                    return false;
                }
            } else {
                i=j-1;
            }
            bool f=false;
            for(int k=i+1;k<s.length();k++){ //burda i+1 mi j+1 mi olacak emin değilim
                if(!isspace(s[k])){//
                    f=true;
                }
                if(s[k]!='+' && s[k]!='-' &&s[k]!='*' && s[k]!='/'&& s[k]!=')' &&!isspace(s[k])){ //variabledan sonra bunlardan biri yoksa error

                    return false;
                }
                if(f){
                    break;
                }
            }

        } else{ //operatorlerden sonra bunlar yoksa error
            bool m=false;  //(3+6) -9
            for(int q=j+1;q<s.length();q++){ //burda i+1 mi j+1 mi olacak emin değilim
                if(!isspace(s[q])){ //boşluk değilse buldu bir sonrakini
                    m=true;
                } else {
                    continue;
                }
                if(s[j]==')'){
                    if(s[q]!='+' && s[q]!='-' &&s[q]!='*' && s[q]!='/'&& s[q]!=')' &&!isspace(s[q])){
                        return false;
                    }

                } else if( (s[q]<48||s[q]>57)&&(s[q]<65||s[q]>90)&&(s[q]<97||s[q]>122) && s[q]!='(' && !isspace(s[q])){//+-*/(
                    return false;
                }
                if(m){
                    break;
                }
            }


        }

    }
    return true;
    //bu finksiyonu asağıdaki fonksiyonun içinde çağırcaz
    //orn print st ise icindeki expressionı buraya yollicak 5++ 3 -4//? falan diye saçmalamış mı diye bakcaz
}
bool errorCatch(string line, bool inWhile ,bool inIf){
    int comment=line.find('#');
    if(comment!=-1){
        line=line.substr(0,comment); //commentli kısmı kestik attık
    }
    line=whitespace(line);
    if(line.empty()){ //bos line sa true
        return true;
    }
    if(line=="}" ){
        if(!inIf && !inWhile){
            return false;
        } else {
            return true;
        }
    }
    if(line.substr(0,6)=="while "||line.substr(0,6)=="while(" || line.substr(0,6)=="while\t"){
        if(inWhile || inIf){
            return false;
        }
        int firstpr=5;
        while(isspace(line[firstpr])&&firstpr<line.length()){
            firstpr++;
        }
        int lastpr=line.length()-2; // } dan bi önceki
        while(isspace(line[lastpr])&&lastpr>=0){
            lastpr--;
        }
        if(line[firstpr]=='(' && line[lastpr]==')' && line[line.length()-1]=='{'){
            return errorCatchForExpressions(line.substr(firstpr+1,lastpr-firstpr-1));
        } else {
            return false;
        }
    }
    if(line.substr(0,6)=="print "||line.substr(0,6)=="print("|| line.substr(0,6)=="print\t"){
        int firstpr=5;
        while(isspace(line[firstpr])&&firstpr<line.length()){
            firstpr++;
        }
        int lastpr=line.length()-1;
        while(isspace(line[lastpr])&&lastpr>=0){
            lastpr--;
        }
        if(line[firstpr]=='(' && line[lastpr]==')'){
            return errorCatchForExpressions(line.substr(6,line.length()-7));
        } else {
            return false;
        }

    }
    if(line.substr(0,3)=="if "||line.substr(0,3)=="if(" || line.substr(0,3)=="if\t"){
        if(inWhile || inIf){
            return false;
        }
        int firstpr=2;
        while(isspace(line[firstpr])&&firstpr<line.length()){  //while      m   (  c)
            firstpr++;
        }
        int lastpr=line.length()-2; // } dan bi önceki
        while(isspace(line[lastpr])&&lastpr>=0){
            lastpr--;
        }
        if(line[firstpr]=='(' && line[lastpr]==')' && line[line.length()-1]=='{'){
            return errorCatchForExpressions(line.substr(firstpr+1,lastpr-firstpr-1));
        } else {
            return false;
        }
    }
    int eq=line.find('=');
    if(eq!=-1){
        string left=line.substr(0,eq);
        string right=line.substr(eq+1);
        bool leftBool=isValidVariableName(left);
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
    bool inWhile=false;
    bool inIf=false;
    int lineNum=0;

    while(getline(infile,line)){
        line = whitespace(line);
        if(line.find('#')!=-1){
            line=line.substr(0,line.find('#'));
        }
        if(line.length()==0){
            lineNum++;
            continue;
        }
       if(!errorCatch(line,inWhile,inIf)){
            cout << "Line "<<lineNum<<": syntax error" << endl;
            outfile << "ret i32 0" << endl;
            outfile << "}";
            return 0;
        }

        if(line=="}"){
            if(inIf){
                inIf=false;
            } else if(inWhile){
                inWhile=false;
            }
        }
        if(line.substr(0,6)=="while " || line.substr(0,6)=="while(" ||line.substr(0,6)=="while\t"){
            inWhile=true;
            line=line.substr(line.find('(')+1);
        } else if (line.substr(0,3)=="if " || line.substr(0,3)=="if(" || line.substr(0,3)=="if\t"){
            inIf=true;
            line=line.substr(line.find('(')+1);
        } else if(line.substr(0,6)=="print " || line.substr(0,6)=="print(" || line.substr(0,6)=="print\t"){
            line=line.substr(line.find('(')+1);
        }

        for(int i=0;i<line.length();i++){
            bool isOperand=false;
            int length=0;
            int j=i;
            while(!isspace(line[j])&&line[j]!='('&& line[j]!=')'&& line[j]!='*'&&line[j]!='+'&&line[j]!='/'&&line[j]!='-'&&line[j]!=','&&line[j]!='}'&&line[j]!='{'&&line[j]!='='&&j<line.length()){
                isOperand=true;
                length++;
                j++;
            }
            string operand=line.substr(i,length);
            operand=whitespace(operand);
            if(operand=="choose"){
                i=j-1;
                continue;
            }
            if(isValidVariableName(operand)){
                i=j-1;
                if(find(vars.begin(),vars.end(),operand)==vars.end()){
                    vars.push_back(operand);
                }
            }
        }

        lineNum++;

    }

    /*for(int i=0;i<vars.size();i++){
        cout << vars[i]<<endl;
    }*/

   if(inWhile||inIf){
        cout << "Line "<<lineNum-1<<": syntax error" << endl;
        outfile << "ret i32 0" << endl;
        outfile << "}";
        return 0;
    }
    outfile << endl;


    for(int i=0;i<vars.size();i++){
        outfile << "%" << vars[i] <<" = alloca i32" << endl;
        outfile << "store i32 0, i32* %" << vars[i] << endl;

    }

    outfile << endl;

    infile.clear(); //burda file'ı okuduk bitti tekrar başlamak istiyoz o yüzden bu satırları yazmam gerekti
    infile.seekg(0, infile.beg);

    /* int a=1;
     choose(a,"choose( n+1 ,  9,   8, choose(o*h ,1,2,3) )",outfile,vars,tempno);*/

    inWhile=false;
    inIf=false;
    int ifNo=0;
    int whileNo=0;
    //int lineNum=1;
    //cout<<errorCatch(" a=choose(4,3,2,1)",inWhile,inIf);

    //BISMILLAHIRRAHMANIRRAHIM ALLAH CC HELP US IF YOU EXIST
    //dunyanın en basıc kodunu gormeye hazır ol <3
    while(getline(infile,line)){
        line=whitespace(line);
        int found=line.find('=');
        bool whil=false;
        bool ifSt=false;
        bool printSt=false;
        bool assignment=false;

        if(line.find('#')!=-1){
            line=line.substr(0,line.find('#'));
        }

        line=whitespace(line);

        if(line==""){
            continue;
        }

        if(found!=string::npos){
            assignment=true;
        }

        if(whitespace(line)=="}"&&inWhile){ //while'ın içindeysek ve while bittiyse
            outfile << "br label %whcond"<<whileNo << endl;
            outfile << endl;
            outfile << "whend"<<whileNo<<":" << endl << endl;
            inWhile=false;
            whileNo++;
        }

        if(whitespace(line)=="}"&&inIf){ //if'in içindeysek ve if bittiyse
            outfile<<"br label %ifend"<<ifNo<<endl;
            outfile << "ifend"<<ifNo<<":" <<endl<< endl;
            inIf=false;
            ifNo++;
        }

        if(line.substr(0,6)=="while " || line.substr(0,6)=="while(" ||line.substr(0,6)=="while\t"){//while ise
            whil=true;
            inWhile=true;
            outfile<<endl;
            outfile << "br label %whcond"<<whileNo << endl;
            outfile<<"whcond"<<whileNo<<":"<<endl;
        }
        if(line.substr(0,6)=="print " || line.substr(0,6)=="print(" || line.substr(0,6)=="print\t"){ //print ise
            printSt=true;
        }

        if(line.substr(0,3)=="if " || line.substr(0,3)=="if(" || line.substr(0,3)=="if\t"){ //if ise
            outfile << "br label %ifcond"<<ifNo<< endl;
            outfile << "ifcond"<<ifNo<<":" << endl;
            ifSt=true;
            inIf=true;
        }

        if(assignment || whil || printSt || ifSt){
            string s;
            string expr;
            string sol;
            if(whil || printSt || ifSt){
                int acpar=line.find('(');
                int kappar=line.find_last_of(')');
                expr=line.substr(acpar+1,kappar-acpar-1); //parantezin içini aldım
                expr=whitespace(expr);
                // cout << expr << endl;

            } else {
                expr=line.substr(found+1); //assignmentin expr kısmı
                sol=line.substr(0,found); //assignmentın sol kısmı
                expr=whitespace(expr);
                sol=whitespace(sol);
                /*if(find(vars.begin(),vars.end(),sol)==vars.end()){
                    outfile << "%" << sol <<" = alloca i32" << endl;
                    outfile << "store i32 0, i32* %" << sol << endl;
                    vars.push_back(sol);
                }*/
            }
            string res=muko(expr,outfile,tempno,vars,chooseno);

            if(whil){ //while ise yazılan şeyler
                outfile<<"%t"<<tempno<<" = icmp ne i32 "<<res<<", 0"<<endl; //buraya tam ne yazcağımızı anlamadım tekrar bakmak lazım
                outfile<<"br i1 %t"<<tempno<<", label %whbody"<<whileNo<<", label %whend"<<whileNo<<endl; //"    "    " "    "       "     "
                outfile<<endl;
                outfile<<"whbody"<<whileNo<<":"<<endl;
                tempno++;
            } else if(printSt){
                outfile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " << res<<" )"<<endl;
            } else if(ifSt){
                outfile<<"%t"<<tempno<<" = icmp ne i32 "<<res<<", 0"<<endl;
                outfile<<"br i1 %t"<<tempno<<", label %ifbody"<<ifNo<<", label %ifend"<<ifNo<<endl;
                outfile << endl;
                outfile << "ifbody"<<ifNo<<":" << endl;
                tempno++;
            } else if(assignment){
                outfile<<"store i32 "<<res<<", i32* %";
                //tempno++;
                string sl=line.substr(0,found); // buralar hep yazdırma kısmı
                sl=whitespace(sl);
                outfile<<sl<<endl;
            }
        }
    }

    outfile << "ret i32 0" << endl;
    outfile << "}";

    return 0;
}
