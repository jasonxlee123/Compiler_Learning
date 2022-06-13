/***
* Parser
* 5/27/2022
* Jason. Lee 2019011284
* Input: Production file
* Output: First set, Follow set, APF.
***/

#include <stdio.h>
#include <stdlib.h>

#define STACK_INIT_SIZE 100 //stack initial size 
#define STACKINCREMENT 10   //stack increment per time

typedef char SElemType;

typedef struct{  //stack structure
    SElemType *base;
    SElemType *top;
    int stacksize;
} SqStack;

int len_gram = 0;    //program productions nums
int len_ter = 0;     //length of terminator vector
int len_non_ter = 0; //length of nonterminator vector
int len_first = 0;   //first set nums
int len_follow = 0;  //follow set nums 
int flag_first = 1;  //first sets end
int flag_follow = 1; //follow sets end
char prod_old_set[200][200]; //original productions list

//variables defination
char ter_symbol[200];      //terminator
char non_ter[200];        //nonterminator
char all_symbol[400];      //all kinds of symbols
char first_set[100][100];  //first set on right of production
char follow_set[100][100]; //follow set on left of production
int M[200][200];          //analysis form ,address on vector

//init stack
int init_stack(SqStack *S){
    S->base = (SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));
    if (!S->base){
        printf("Can not init stack!\n");
        exit(0);
    }
    S->top = S->base;
    S->stacksize = STACKINCREMENT;
    return 1;
}

//get top elememt in the stack
char get_top_ele(SqStack *S){
    if (S->top == S->base)
        return ' ';
    return *(S->top - 1);
}

//put an elememt into stack
int Push(SqStack *S, SElemType e){
    if (S->top - S->base >= STACK_INIT_SIZE){
        S->base = (SElemType *)realloc(S->base, (S->stacksize + STACKINCREMENT) * sizeof(SElemType));
        if (!S->base){
            printf("Can not expand stack.\n");
            exit(0);
        }

        S->top = S->base + S->stacksize;
        S->stacksize += STACKINCREMENT;
    }
    *S->top++ = e;
    return 1;
}

//pop top elememt of stack
char Pop(SqStack *S){
    if (S->top == S->base)
        return ' ';
    return *--S->top;
}

//get length of expression
int length_expression(char str[]){
    int i = 0;
    while (str[i] != '\0'){
        i++;
    }
    return i;
}

//distinguish terminator or nor nonterminator
int check(char c){
    if (c >= 'A' && c <= 'Z')
        return 1; //nonterminator
    else if (c == '@' || c == '|' || c == ' ' || c == '\n' || c == '\r')
        return 0; //other factors
    else
        return 2; //terminator
}

//judge if the vec has certain char
int have_char(char str[], char c, int len){
    for (int i = 0; i < len; i++){	//traverse by char
        if (str[i] == c)	//exists the char 
            return 1;
    }
    return 0;	//do not exist
}

//get productions form file, decomposite them, distinguish (non)terminator
int production_read(char s[]){
    FILE *fp;	//file pointer
    fp = fopen(s, "r");	//open file by reading
    char temp_gram[200];	//save production
    int index_temp_gram = 3;
    int index_gram = 0;
    if (fp != NULL){
        printf("All productions in file: \n");
        while (fgets(temp_gram, 200, fp) != NULL){
            printf("%s", temp_gram);
            char gram_right = temp_gram[0]; //save left char of production for expanding
            index_temp_gram = 3;	//
            index_gram = 0;
            prod_old_set[len_gram][0] = gram_right;
            prod_old_set[len_gram][1] = '-';
            prod_old_set[len_gram][2] = '>';
            index_gram = 3;
            while (temp_gram[index_temp_gram] != '\0'){
                if (temp_gram[index_temp_gram] == '\n' || temp_gram[index_temp_gram] == '\r'){
                    prod_old_set[len_gram][index_gram] = '\0'; //production tail as '\0' for traverse 
                    len_gram++;
                    break;
                }
                else if (temp_gram[index_temp_gram] == '|'){ //'|' then split
                    index_temp_gram++;
                    prod_old_set[len_gram][index_gram] = '\0';
                    len_gram++;
                    prod_old_set[len_gram][0] = gram_right;
                    prod_old_set[len_gram][1] = '-';
                    prod_old_set[len_gram][2] = '>';
                    index_gram = 3;
                }

                prod_old_set[len_gram][index_gram] = temp_gram[index_temp_gram];
                index_gram++;
                index_temp_gram++;
            }
        }
        prod_old_set[len_gram][index_gram] = '\0'; //tail of last production as '\0'
        len_gram++;
    }
    else{
        printf("Can not open file! \n");
    }
    //print processed productions
    printf("\nprocessed productions: \n");
    for (int i = 0; i < len_gram; i++){
        printf("%s\n", prod_old_set[i]);
    }
    //distinguish (non)terminator
    for (int i = 0; i < len_gram; i++){
        for (int j = 3; prod_old_set[i][j] != '\0'; j++){
            char temp = prod_old_set[i][j];
            int value_return = check(temp);
            if (value_return == 1){
                if (!have_char(non_ter, temp, len_non_ter)){
                    non_ter[len_non_ter] = temp;
                    len_non_ter++;
                }
            }
            else if (value_return == 2){
                if (!have_char(ter_symbol, temp, len_ter)){
                    ter_symbol[len_ter] = temp;
                    len_ter++;
                }
            }
            else{
                continue;
            }
        }
    }
    //print (non)terminator
    printf("terminator:\n");
    for (int i = 0; i < len_ter; i++){
        printf("%c ", ter_symbol[i]);
    }
    printf("\n");
    printf("nonterminator:\n");
    for (int i = 0; i < len_non_ter; i++){
        printf("%c ", non_ter[i]);
    }
    printf("\n");

    return 0;
}

//initialize first sets 
void init_first(){
    int temp = 0;
    while (temp < len_non_ter){
        first_set[len_first][0] = non_ter[temp];
        first_set[len_first][1] = '\0'; //tail as \0 for traverse 
        temp++;
        len_first++;
    }
    temp = 0;
    while (temp < len_ter){
        first_set[len_first][0] = ter_symbol[temp];
        first_set[len_first][1] = ter_symbol[temp];
        first_set[len_first][2] = '\0';
        temp++;
        len_first++;
    }
}

//initialize follow sets 
void init_follow(){
    int temp = 0;
    while (temp < len_non_ter){
        follow_set[len_follow][0] = non_ter[temp];
        follow_set[len_follow][1] = '\0';
        temp++;
        len_follow++;
    }
}

//print first sets and follow sets 
void show(){
    printf("\nfirst sets:\n");
    for (int i = 0; i < len_first; i++){
        printf("first(%c)= ", first_set[i][0]);
        int j = 1;
        while (first_set[i][j] != '\0'){
            printf("%c", first_set[i][j]);
            printf("  ");
            j++;
        }
        printf("\n\n");
    }
    printf("\nfollow sets:\n");
    for (int i = 0; i < len_follow; i++){
        printf("follow(%c)= ", follow_set[i][0]);
        int j = 1;
        while (follow_set[i][j] != '\0'){
            printf("%c", follow_set[i][j]);
            printf("  ");
            j++;
        }
        printf("\n\n");
    }
}

//locate position of first sets and follow sets 
int find_position(char c, char str[][100], int length){
    for (int i = 0; i < length; i++){
        if (str[i][0] == c)
            return i;
    }
    return -1; //do not find 
}

//judge if the array is empty
int is_empty(char str[]){
    if (str[1] == '\0')
        return 1;
    return 0;
}

//check for duplicates then add symbol to array
void add_element(char str[], char c){
    int i = 0;
    while (str[i] != '\0'){
        if (str[i] == c)
            return;
        i++;
    }
    str[i] = c;
    i++;
    flag_first = 1;  //first set is expanding
    flag_follow = 1; //follow set is expanding
    str[i] = '\0';
}

//judge if terminator or empty word 
int is_ter(char c, char str[], int len){
    for (int i = 0; i < len; i++){
        if (str[i] == c || c == '@')
            return 1;
    }
    return 0;
}

//merge two sets 
void merge_set(char str1[], char str2[]){
    int i = 1;
    while (str2[i] != '\0'){
        char t = str2[i];
        add_element(str1, t); //check for duplicates 
        i++;
    }
}

//add str2 array without empty word to str1
void merge_first_set_null(char str1[], char str2[]){
    int i = 1;
    while (str2[i] != '\0'){
        char t = str2[i];
        if (t != '@'){
            add_element(str1, t);
        }
        i++;
    }
}

//judge whether exists empty word (@)
int have_null(char str[]){
    int i = 0;
    while (str[i] != '\0'){
        if (str[i] == '@')
            return 1;
        i++;
    }
    return 0;
}

//create first set
void creat_first(){
    init_first();	//initialize first set 
    while (flag_first){ //circle till flag == 0
        flag_first = 0; //set 0 before traverse
        for (int i = 0; i < len_gram; i++){
            char right_gram = prod_old_set[i][0];     //get symbols on the left of production
            int index_first_right = find_position(right_gram, first_set, len_first); //get the position of the left symbol of production in the first set
            int j = 3;  //point to the first symbol on right of production
            while (prod_old_set[i][j] != '\0'){  //traverse right part of the production
                char left_gram = prod_old_set[i][j];
                if (is_ter(left_gram, ter_symbol, len_ter)){ //first symbol is terminator
                    //add to first set then end traverse
                    add_element(first_set[index_first_right], left_gram);
                    break;
                }
                else{
                    int index_first_left = find_position(left_gram, first_set, len_first); //nonterminator, get its position in first set
                    if (is_empty(first_set[index_first_left])){   //if first is empty then end traverse turn to next production
                        break;
                    }
                    else{ //first set is not empty
                    
                        if (have_null(first_set[index_first_left])){ //judege whether first set is empty                      
                            //if empty word exists add it into follow set then continue traverse next symbol
                            merge_first_set_null(first_set[index_first_right], first_set[index_first_left]);
                        }
                        else{ //if not then add its first set into first set of left part of production                       
                            merge_set(first_set[index_first_right], first_set[index_first_left]);
                            break;  //then exit
                        }
                    }
                }
                j++;  //point to next symbol
            }
            if (prod_old_set[i][j] == '\0'){ //right part of production is nonterminator with empty word in follow set           
                add_element(first_set[index_first_right], '@');
            }
        }
    }
}

//get length of production
int length_prod(char str[]){
    int i = 0;
    while (str[i] != '\0'){
        i++;
    }
    return i;
}

//create follow set
void creat_follow(){
    init_follow();   //initialize followset
    add_element(follow_set[find_position(prod_old_set[0][0], follow_set, len_follow)], '#'); //start as #
    while (flag_follow){ //circle till follow set doesnt expand    
        flag_follow = 0; //expanding flag start as 0
        for (int i = 0; i < len_gram; i++){
            char right_gram = prod_old_set[i][0];  //get a symbol on left production
            int index_first_set = find_position(right_gram, first_set, len_first);  //get the position in the first set of the symbol
            int index_follow_set_right = find_position(right_gram, follow_set, len_follow); //get the position in the follow set of the symbol
            int length = length_prod(prod_old_set[i]);   //get the length od the production
            for (int point_aim = 3; point_aim < length; point_aim++){  //begin at first symbol on the right of production
                char head = prod_old_set[i][point_aim];   //calculate head follow set 
                int index_follow_set = find_position(head, follow_set, len_follow);  //get head position in follow set 
                if (index_follow_set != -1){  //head is a nonterminator
                    if (point_aim == length - 1){  //if head is tail then add the symbol follow set into head follow set
                        merge_set(follow_set[index_follow_set], follow_set[index_follow_set_right]);
                    }
                    else{  //if head is not last element
                        int next = point_aim + 1;  //point to next symbol
                        int temp_flag = 0;
                        while (next < length){
                            //get position in first set 
                            int index_first_set_temp = find_position(prod_old_set[i][next], first_set, len_first);
                            while (have_null(first_set[index_first_set_temp])){ //traverse till first set has no empty word
                                merge_first_set_null(follow_set[index_follow_set], first_set[index_first_set_temp]); //add next without ew to head follow set 
                                next++;  //move to next symbol
                                if (next == length){ //if tail symbol then add next fs to head fs
                                    int index_first_set_temp = find_position(prod_old_set[i][next - 1], follow_set, len_follow);
                                    merge_set(follow_set[index_follow_set], follow_set[index_first_set_temp]);
                                    temp_flag = 1; //end of production, flag = 1
                                    break;
                                }
                                index_first_set_temp = find_position(prod_old_set[i][next], first_set, len_first);
                            }
                            if (temp_flag){ //last symbol then end circle
                                break;
                            }
                            merge_first_set_null(follow_set[index_follow_set], first_set[index_first_set_temp]); //former circle ends -> next has no  ew in fs then add to head fs
                            break;
                        }
                    }
                }
            }
        }
    }
}

//initialize predictive analysis form
void init_apf(){
    for (int i = 0; i <= len_non_ter; i++){
        for (int j = 0; j <= len_ter; j++){
            M[i][j] = -1;
        }
    }
}

//print predictive analysis form
void show_form(){
    printf("\npredictive analysis form: \n");
    for (int i = 0; i <= len_non_ter; i++){
        for (int j = 0; j <= len_ter; j++){
            if (i == 0){
                if (j == 0){
                    printf("0\t");
                }
                else
                    printf("%c\t", ter_symbol[j - 1]);
            }
            else if (j == 0){
                if (i == 0){
                    printf("0\t");
                }
                else{
                    printf("%c\t", non_ter[i - 1]);
				}
            }
            else{
                printf("%s\t", prod_old_set[M[i][j]]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

//find position of symbol in array
int find_position_char(char c, char str[], int len){
    for (int i = 0; i < len; i++){
        if (c == str[i]){
            return i;
        }
    }
    return -1;
}

//create analysis form
void create_apf(){
    //add # into terminator array
    ter_symbol[len_ter] = '#';
    len_ter++;
    init_apf();
    //traverse all productions
    for (int i = 0; i < len_gram; i++){
        //get location of symbol on left production
        char left_gram = prod_old_set[i][0];
        int index_non = find_position_char(left_gram, non_ter, len_non_ter);
        index_non++;
        //construct first set of right production
        char temp_first[100];
        temp_first[0] = '\0';  //tail as \0 sign ending
        int j = 3;    //point to first symbol on right of production 
        while (prod_old_set[i][j] != '\0'){  //traverse right production
            char left_gram = prod_old_set[i][j];
            if (is_ter(left_gram, ter_symbol, len_ter)){  //first symbol is terminator
                //add into first set then stop traverse
                add_element(temp_first, left_gram);
                break;
            }
            else{ //nonterminator
                int index_first_left = find_position(left_gram, first_set, len_first); //get its position in first set 
                if (is_empty(first_set[index_first_left])){  //if first set is empty then stop traverse to next production
                    break;
                }
                else{  //if first set is not empty 
                    if (have_null(first_set[index_first_left])){  //any ew in fs?
                        //has ew then add to fs then continue traverse next symbol
                        merge_first_set_null(temp_first, first_set[index_first_left]);
                    }
                    else{ //has no ew then add fs into fs of left production
                        merge_set(temp_first, first_set[index_first_left]);
                        break;
                    }
                }
            }
            j++;
        }
        if (prod_old_set[i][j] == '\0'){  //no break in former circle, right of production is nonterminator with fs having ew
            add_element(temp_first, '@');
        }
        //judege whether first alpha has ew
        int k = 0;
        while (temp_first[k] != '\0'){
            if (temp_first[k] != '@'){ //ignore ew
                int index_ter = find_position_char(temp_first[k], ter_symbol, len_ter);
                index_ter++;
                M[index_non][index_ter] = i;
            }
            k++;
        }
        if (have_null(temp_first)){  //ew exists 
            int index_follow_set = find_position(left_gram, follow_set, len_follow);
            int t = 1;
            char c = follow_set[index_follow_set][t];
            while (c != '\0'){
                int index_ter = find_position_char(c, ter_symbol, len_ter);
                index_ter++;
                M[index_non][index_ter] = i;
                t++;
                c = follow_set[index_follow_set][t];
            }
        }
    }
}

//string analizing
void control(char str[]){
    printf("string analizing...\n");
    SqStack stack;
    init_stack(&stack);
    //push '#' into stack 
    Push(&stack, '#');
    //push 1st symbol into stack 
    Push(&stack, prod_old_set[0][0]);
    int pointer = 0; //pointer to array
    int flag = 1;
    int step = 1;
    while (flag){
        printf("\n\nstep:%d\n", step);
        //get top element
        char top = get_top_ele(&stack);
        printf("top elememt: %c\n", top);
        //top elememt is terminator and not #
        if (top != '#' && is_ter(top, ter_symbol, len_ter)){
            if (top == str[pointer]){ //if top ele is same as outer ele
                printf("top ele %c is same as outer ele %c, pop %c, point to next symbol.\n", top, str[pointer], top);
                Pop(&stack);
                pointer++;
            }
            else{  //error
                printf("ERROR 1\n");
                break;
            }
        }
        else if (top == '#'){  //end
            if (top == str[pointer]){
                printf("top ele %c is same as outer ele %c.\n", top, str[pointer]);
                printf("receive successfully.\n");
                flag = 0;
            }
            else{  //error
                printf("ERROR 2\n");
                break;
            }
        }
        else{
            int index_non = find_position_char(top, non_ter, len_non_ter);
            index_non++;
            int index_ter = find_position_char(str[pointer], ter_symbol, len_ter);
            index_ter++;
            int index_gram = M[index_non][index_ter];
            //judge if M has expression
            if (index_gram != -1){
                //yes, pop then push into stack in t-order
                printf("M(%c,%c) production: %s, put %s into stack.\n", top, str[pointer], prod_old_set[index_gram], prod_old_set[index_gram]);
                Pop(&stack);
                printf("pop top element %c.\n", top);
                if (prod_old_set[index_gram][3] != '@'){
                    int length = length_expression(prod_old_set[index_gram]);
                    for (int i = length - 1; i > 2; i--){
                        printf("put %c into stack.\n", prod_old_set[index_gram][i]);
                        Push(&stack, prod_old_set[index_gram][i]);
                    }
                }
            }
            else{//no, error
                printf("ERROR 3\n");
                break;
            }
        }
        step++;
    }
    if (!flag){
        printf("success!\n");
    }
}

int main(){
	printf("Program begins...\n");
    //read productions from file and preprocess
	production_read("source.txt"); 
    //create first set 
	creat_first();                 
    //create follow set 
	creat_follow();                
    //print first and follow set 
	show();                        
    //create apf 
	create_apf();         
    //print apf
	show_form();
	//analize an expression
	control("(i+i)*i#");
	printf("\nProgram ends...\n");
    return 0;
}



//NULLABLE SET
NULLABLE={};
while(NULLABLE is still changing){
	foreach(production p:x->β){
		if(β==ε){
			NULLABLE∪={x};}
		if(β=γ1...γn){
			if(γ1∈NULLABLE && ... && γn∈NULLABLE){
				NULLABLE∪={x};}}}}


//FIRST SET
foreach(nonterminator N)
	FIRST(N) = {};
while(some set is still changing){
	foreach(production P: N->β1...βn){
		foreach(βi from β1 upto βn){
			if(βi==a...){
				FIRST(N)∪={a};
				break;}
			if(βi==M...){
				FIRST(N)∪=FIRST(M);
				if(M is not in NULLABLE){
					break;}}}}}

//FOLLOW SET					
foreach(nonterminator N)
	FOLLOW(N) = {};
while(some set is still changing){
	foreach(production P: N->β1...βn){
		temp=FOLLOW(N);
		foreach(βi from β1 downto βn){
			if(βi==a...){
				temp∪={a};}
			if(βi==M...){
				FOLLOW(M)∪=temp;
				if(M is not in NULLABLE){
					temp=FIRST(M);}
				else{					
					temp∪=FIRST(M);}}}}}

//LL(1)
tokens[];
i=0;
stack=[S]; //S is beginning symbol
while(stack!=[]){
		if(stack[top] is a terminator t){
			if(t==tokens[i++]){
				pop();}
			else{
				error();}}
		else if(stack[top] is a nonterminator T){
			pop();
			push(table[T, tokens[i]]);}}


//FIRST_alpha
foreach(production p)
	FIRST_alpha(p) = {};
calculate_FIRST_alpha(production P: N->β1...βn){
	foreach(production P: N->β1...βn){
		foreach(βi from β1 upto βn){
			if(βi==a...){
				FIRST_alpha(p)∪={a};
				return;}
			if(βi==M...){
				FIRST_alpha(p)∪=FIRST(M);
				if(M is not in NULLABLE){
					return;}}}}
	FIRST_alpha(p)∪=FIRST(N);}