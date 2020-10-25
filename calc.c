/*
    compile with: gcc -g -Wall -pedantic calc.c -o calc.x -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void calc(char *inp);

int main(int argc, char *argv[]){

    if(argv[1] == NULL){
        printf("Missing arguments, ex: 'calc.x 1+1' or 'calc.x help'\n");
        return 0;
    }
    if(strcmp("help", argv[1]) == 0){
        printf("Allowed operators: + - * / ^\nWorks with parenthesises\nErrors may occur with huge numbers\n");
        return 0;
    }
    
    //allows spaces
    int tot_length = strlen(argv[1]);
    for(int i = 2; i < argc; i++){
        tot_length += strlen(argv[i]);
    }
    char inp[tot_length];
    int inpI = 0;
    for(int i = 1; i < argc; i++){
        int size = strlen(argv[i]);
        memcpy(&inp[inpI], argv[i], sizeof(char) * size);
        inpI += size;
    } 

    calc(inp);
    return 0;
}

void calc(char *inp){
    char * tmpChrPnt;
    char tmpChr;
    int length = strlen(inp);
    double after;
    double res;
    char resStr[50];
    int resStrLength;
    char *prod;

 //   fprintf(stderr, "start: %s\n", inp);                                                       //default check

    //finds ) || end of string
    int parntEndAt;
    tmpChrPnt = strchr(inp,')');
    if(tmpChrPnt == NULL){
        parntEndAt = length;
    }
    else{
        parntEndAt = tmpChrPnt- inp;
    }

    //parntAt = right ( parntendat = right )
    int parntAt;
    tmpChrPnt = &inp[0];
    int tstParntAt = 0;
    while (tstParntAt < parntEndAt)
    {
        parntAt = tstParntAt;
        tmpChrPnt=strchr(tmpChrPnt+1,'(');
        if(tmpChrPnt == NULL){
            break;
        }
        tstParntAt = tmpChrPnt - inp;
    }

    //finds right operator
    const char *parntAtChr = &inp[parntAt];
    char *operator[6] = {
        strchr(parntAtChr, '-'),
        strchr(parntAtChr, '+'),
        strchr(parntAtChr, '%'),
        strchr(parntAtChr, '/'),
        strchr(parntAtChr, '*'),
        strchr(parntAtChr, '^')
    };
    tmpChrPnt = NULL;
    int first = length;
    for(int i = 0; i < 6; i++){
        if(i == 2 || i == 5){first = length;}//prioritizes
        char * op = operator[i];
        if (op != NULL && op - inp < first && op - inp < parntEndAt){ // found && best one yet && within ()
            tmpChrPnt = op;
            first = i;
        }
    }

    if(tmpChrPnt != NULL){tmpChr = *(tmpChrPnt - 1);} // safety
    if(tmpChrPnt == NULL || (*tmpChrPnt == '-' && (tmpChr == NULL || tmpChr == '('))){ //didn't find operator in prefered ()
        if(parntEndAt == length){//no parenthesis
            //done
            printf("%s\n", inp);
            exit(0);
        }
        else{
            //removes () if only num inside
            int numLength = parntEndAt - parntAt - 1;

            prod = malloc(length - 2 * sizeof(char));
            memcpy(prod, inp, sizeof(char) * (parntAt));
            int prodI = parntAt;
            memcpy(&prod[prodI], &inp[parntAt + 1], sizeof(char) * numLength);
            prodI += parntEndAt - parntAt - 1;
            memcpy(&prod[prodI], &inp[parntEndAt + 1], sizeof(char) * (length - parntEndAt - 1));
            length -=2;
            tmpChr = prod[parntAt-2];

            //functions
            if(isalpha(tmpChr)){
                after = atof(&prod[parntAt]);

                if(prod[parntAt-4] == 'a'){
                    switch(tmpChr){
                        case 'o': //acos
                            res = acos(after); break;

                        case 'i': //asin
                            res = asin(after); break;

                        case 'a': //atan
                            res = atan(after); break;
                    }

                    res *= 180 / M_PI;
                    numLength += 4;
                    parntAt -= 4;
                }
                else{
                    after *= M_PI / 180;
                    switch(tmpChr){
                        case 'o': //cos
                            res = cos(after); break;

                        case 'i': //sin
                            res = sin(after); break;

                        case 'a': //tan
                            res = tan(after); break;
                    }
                    numLength += 3;
                    parntAt -= 3;
                }

                //to string
                char resStr[50];
                snprintf(resStr, 50, "%f", res);
                resStrLength = strlen(resStr);

                //remove function from ecuation
                prod = malloc((length - numLength + resStrLength) * sizeof(char));
                memcpy(prod, inp, sizeof(char) * (parntAt));
                memcpy(&prod[parntAt], resStr, sizeof(char) * (resStrLength));
                memcpy(&prod[parntAt + resStrLength], &inp[parntEndAt+1], sizeof(char) * (length - parntEndAt + 1));

            }
            calc(prod);
            exit(0);
        }
    }


    if(!isdigit(*(tmpChrPnt - 1))){
        printf("Invalid character before operator: %c\n", *(tmpChrPnt-1));
        exit(0);
    }

    //finds nums
    double before;
    int beforeI;
    for(beforeI= tmpChrPnt - inp - 1; (isdigit(inp[beforeI]) || inp[beforeI] == '.') && beforeI != -1; beforeI--){}
    if((inp[beforeI] == '-' && (inp[beforeI - 1] > '\'' && inp[beforeI - 1] < '.'))||beforeI == 0){
        beforeI--;
    }
    before = atof(&inp[beforeI+1]);

    int afterNum;
    for(afterNum= tmpChrPnt - inp +2; (isdigit(inp[afterNum]) || inp[afterNum] == '.') && afterNum < length; afterNum++){}
    after = atof(tmpChrPnt + 1);
    if(!isdigit(*(tmpChrPnt + 1))){
        printf("Invalid character after operator: %c\n", *(tmpChrPnt+1));
        exit(0);
    }


    //calculates
    switch(*tmpChrPnt){
        case '^': res = pow(before, after); break;
        case '*': res = before * after; break;
        case '%': res = fmod(before, after); break;
        case '/': res = before / after; break;
        case '+': res = before + after; break;
        case '-': res = before - after; break;
    }


    //to string
    snprintf(resStr, 50, "%f", res);
    resStrLength = strlen(resStr);

    //replaces equation with answer
    int prodlength = beforeI + length - 1 + afterNum + resStrLength;
    prod = malloc(prodlength * sizeof(char));
    memcpy(prod, inp, sizeof(char) * (beforeI+1));
    memcpy(&prod[beforeI+1], resStr, sizeof(char) * (resStrLength));
    memcpy(&prod[beforeI + 1 + resStrLength], &inp[afterNum], sizeof(char) * (length - afterNum));

    //recursion
    calc(prod);
}