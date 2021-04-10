#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

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


    return 0;
}
