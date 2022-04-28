/***
* Lexer
* 17/4/2022
* Jason. Lee 2019011284
* Input: C Source File
* Output: Lexer Bigram List And TXT File
***/
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

/* biagram lexer token structure */
struct _2tup{
    string token;	//word string value
    int id;	//word code
};

/* judge blank one or four */
bool is_blank(char ch){
    return ch == ' ' || ch == '    ';
}

/* forward scan */
bool gofor(char& ch, string::size_type& pos, const string& prog){
    ++pos;	//next char 
    if (pos >= prog.size())	//end of file 
    {
        return false;
    }
    else	//still have char 
    {
        ch = prog[pos];	//set new char as ch value 
        return true;
    }
}

_2tup scanner(const string& prog, string::size_type& pos, const map<string, int>& keys, int& row){
    /* recognization order:
    if
        identifier/keyword
    else if
        number
    else
        other symbol
    */
    _2tup ret;	//token unit 
    string token;	//token value 
    int id = 0;	//token id 

    char ch;	//single char for operation
    ch = prog[pos];	//init begining char 

    while(is_blank(ch))	{	//blank then next char 
        ++pos;	//pointer to next position
        ch = prog[pos];	//get net char 
    }
	
    //identify keyword and symbol 
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {	
	//begingning status
        while((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {	
		//jump to next status
            token += ch;	//add to value 
            if (!gofor(ch, pos, prog)) {	//move to next status
                break;	//end then stop
            }
        }
		
        //default code is normal symbol:81
        id = keys.size();

        //identify keyword 
        map<string, int>::const_iterator cit = keys.find(token);
        if (cit != keys.end()) {	//not nomal symbol
            id = cit->second;	//set id as relative keyword 
        }
    }
    //identify constant number
    else if ((ch >= '0' && ch <= '9') || ch == '.') {	//begingning status 
        while (ch >= '0' && ch <= '9' || ch == '.') {
            token += ch;	//add to value 
            if (!gofor(ch, pos, prog)) {	//move to next status
                break;	//end then stop
            }
        }
        id = keys.size() - 1;	//constant code = 80
        int dot_num = 0;	//check illeagal dot 
        for (string::size_type i = 0; i != token.size(); ++i) {
            if (token[i] == '.') {	//calculate dot number 
                ++dot_num;
            }
        }
        if (dot_num > 1) {	//constant with one more dot is illeagal
            id = -1;
        }
    }
	//operator
    else {
        map<string, int>::const_iterator cit;
        switch (ch) {
        case '-': // - operator
            token += ch;	//add to value 
            if (gofor(ch, pos, prog)) {
                if (ch == '-' || ch == '=' || ch == '>') { // -- operator
                    token += ch;
                    gofor(ch, pos, prog);
                }
            }
            cit = keys.find(token);	//match token code 
            if (cit != keys.end()) {
                id = cit->second;
            }            
            break;
        case '!': 	// ! operator        
        case '%': 	// % operator
        case '*':	// * operator
        case '^':	// ^ operator
        case '=':	// = operator
            token += ch;
            if (gofor(ch, pos, prog)) {
                if (ch == '=') { // !% or %= operator
                    token += ch;
                    gofor(ch, pos, prog);
                }
            }
            cit = keys.find(token);	//match token code
            if (cit != keys.end()) {
                id = cit->second;
            }
            break;
        case '/': 	// / operator
            token += ch;
            if (gofor(ch, pos, prog)) {
                if (ch == '=') { // /= operator
                    token += ch;
                    gofor(ch, pos, prog);
                }
                else if (ch == '/') { // single-line annotation
                    token += ch;
                    ++pos;
                    while (pos < prog.size()) {	//scan till end of line 
                        ch = prog[pos];
                        if (ch == '\n') {
                            break;
                        }
                        token += ch;
                        ++pos;
                    }
                    if (pos >= prog.size()) {
                        ;
                    }
                    else {
                        ;
                    }
                    id = keys.size() - 2;	//annotation code = 79
                    break;
                }
                else if (ch == '*') { // multi-line annotation                                 
                    // annotation with error notice
                    token += ch;
                    if (!gofor(ch, pos, prog)) {	//without tail is illeagal
                        token += "\nErrorAnnotation!!!";
                        id = -10;
                        break;
                    }
                    if (pos + 1 >= prog.size()) {
                        token += ch;
                        token += "\nErrorAnnotation!!!";
                        id = -10;
                        break;
                    }
                    char xh = prog[pos + 1];	//forward scan second char 
                    while (ch != '*' || xh != '/') {
                        token += ch;
                        if (ch == '\n') {	//next line should refresh row value 
                            ++row;
                        }
                        if (!gofor(ch, pos, prog)) {
                            token += "\nErrorAnnotation!!!";
                            id = -10;
                            ret.token = token;
                            ret.id    = id;
                            return ret;
                        }
                        if (pos + 1 >= prog.size()) {
                            token += ch;
                            token += "\nErrorAnnotation!!!";
                            id = -10;
                            ret.token = token;
                            ret.id    = id;
                            return ret;
                        }
                        xh = prog[pos + 1];
                    }
                    token += ch;	//add tail to value 
                    token += xh;
                    pos += 2;	//refresh pointer
                    ch = prog[pos];
                    id = keys.size() - 2;	//annotation code = 79
                    break;
                }
            }
			//only /
            cit = keys.find(token);	//match token code
            if (cit != keys.end()) {
                id = cit->second;
            }
            break;
        case '&':
            token += ch;
            if (gofor(ch, pos, prog)) {
                if (ch == '&' || ch == '=') {
                    token += ch;
                    gofor(ch, pos, prog);
                }
            }
            cit = keys.find(token);	//match token code
            if (cit != keys.end()) {
                id = cit->second;
            }
            break;
        case '|':
            token += ch;
            if (gofor(ch, pos, prog)) {
                if (ch == '|' || ch == '=') {
                    token += ch;
                    gofor(ch, pos, prog);
                }
            }
            cit = keys.find(token);	//match token code
            if (cit != keys.end()) {
                id = cit->second;
            }
            break;
        case '+':
            token += ch;
            if (gofor(ch, pos, prog)) {
                if (ch == '+' || ch == '=') {
                    token += ch;
                    gofor(ch, pos, prog);
                }
            }
            cit = keys.find(token);	//match token code
            if (cit != keys.end()) {
                id = cit->second;
            }
            break;
        case '<':
            token += ch;
            if (gofor(ch, pos, prog)) {
                if (ch == '<') {
                    token += ch;
                    if (gofor(ch, pos, prog)) {
                        if (ch == '=') {	//operator <<=
                            token += ch;
                            gofor(ch, pos, prog);
                        }
                    }
                }
                else if (ch == '=') {	//operator <=
                    token += ch;
                    gofor(ch, pos, prog);
                }
            }
            cit = keys.find(token);	//match token code
            if (cit != keys.end()) {
                id = cit->second;
            }
            break;
        case '>':
            token += ch;
            if (gofor(ch, pos, prog)) {
                if (ch == '>') {
                    token += ch;
                    if (gofor(ch, pos, prog)) {
                        if (ch == '=') {	//operator >>=
                            token += ch;
                            gofor(ch, pos, prog);
                        }
                    }
                }
                else if (ch == '=') {	//operator >=
                    token += ch;
                    gofor(ch, pos, prog);
                }
            }
            cit = keys.find(token);	//match token code
            if (cit != keys.end()) {
                id = cit->second;
            }
            break;
        case '(':
        case ')':
        case ',':
        case '.':
        case ':':
        case ';':
        case '?':
        case '[':
        case ']':
        case '{':
        case '}':
        case '~':
        case '"':
            token += ch;
            gofor(ch, pos, prog);
            cit = keys.find(token);	//match token code
            if (cit != keys.end()) {
                id = cit->second;
            }
            break;
        case '\n':	//newline -> ignore then continue scan 
            token += "Newline";
            ++pos;
            ch = prog[pos];
            id = -2;
            break;
        default:	//illeagal status
            token += "Error";
            ++pos;
            ch = prog[pos];
            id = -1;
            break;
        }
    }
    ret.token = token;	//set token unit
    ret.id    = id;

    return ret;
}

//initialize keyword map
void init_keys(const string& file, map<string, int>& keys) {
    ifstream fin(file.c_str());
    if (!fin) {	//file does not exist
        cerr << file << " doesn't exist!" << endl;
        exit(1);
    }
    keys.clear();	//clear map
    string line;
    string key;
    int id;
    while (getline(fin, line)) {
        istringstream sin(line);	//construct inputstream
        sin >> key >> id;	//context to varibles
        keys[key] = id;	//identifier/keyword <--> id
    }
    /*
    cout << "Identifier code list:" << endl;
    for (map<string, int>::const_iterator i = keys.begin(); i != keys.end(); ++i) {
        std::cout << i->first << " : " << i->second << std::endl;
    }
    */
}

//initialize source file 
void read_prog(const string& file, string& prog) {
    ifstream fin(file.c_str());	//only C file
    if (!fin) {
        cerr << file << " error!" << endl;
        exit(2);
    }
    prog.clear();	//prog clear 
    string line;
    while (getline(fin, line)) {
        prog += line + '\n';	//format line context
    }
}

int main() {
	//initialize code list 
    map<string, int> keys;
    init_keys("c_keys.txt", keys);
	
	//initialize source file
    string prog;
    read_prog("prog1.txt", prog);
	
	//initialize vector to save token 
    vector< _2tup > tups;
    string token, id;
	
    string::size_type pos = 0;	//init pointer
    int row  = 1;	//init row/line number
    
    _2tup tu;	//token unit

    cout << "Source file:" << endl << prog << endl << endl;

    int no = 0;	//token count
    
    cout << "Token list:" << endl;

    do {
        tu = scanner(prog, pos, keys, row);	//get each token 

        switch (tu.id) {
        case -1:	//illeagal context
            ++no;
            cout << no << ": ";
            cout << "Error in row" << row << "!" << '<' << tu.token<< "," << tu.id << '>' << endl;
            tups.push_back(tu);	//add to vector
            break;
        case -2:	//end of a line(\n)
            ++row;
            // cout << '<' << tu.token<< "," << tu.id << '>' << endl;
            break;
        default:	//leagal token print
            ++no;
            cout << no << ": ";
            cout << '<' << tu.token<< "," << tu.id << '>' << endl;
            tups.push_back(tu);	//add to vector
            break;
        }        
    } while (tu.id != 0 && pos < prog.size());	//scan till last char of file
    
    cout << endl << "Total number of tokens: " << tups.size() << endl;
    return 0;
}
