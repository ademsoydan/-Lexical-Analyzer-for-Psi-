#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
char readNextChar(FILE *);
int DetermineCharType(char*);
int ControlForKeywordOrIdentifier(FILE *, FILE *);
int is_keyword(char*);
int is_identifier(char*);
int is_operator(char*);
char* two_char_to_str(char,char);
char* get_bracket_name(char);

const int maxIdentifierSize = 20;
 
enum MyCharTypes{digit, letter, op, space, underscore, endofline,bracket, quote};
enum MyTokenTypes{keyword, identifier, opt, stringConst , intConst, undefined};
enum MyErrorTypes{oversize = 5, NotStartsAlpha};



// DRIVER FUNCTION
int main()
{
	char* str_temp = NULL;

    FILE* ptr;
    FILE* f_out_ptr;
    char ch;
    int type;
 
    // Opening file in reading mode
    ptr = fopen("code.psi", "r");
    f_out_ptr = fopen("code.lex", "w");
 
    if (NULL == ptr) {
        printf("file can't be opened \n");
    }
   
    
    while((ch  = readNextChar(ptr)) != EOF ){
    	ungetc(ch, ptr);
    	int charType = DetermineCharType(ch);
    	
    	if(charType != op && charType != space && charType != endofline && charType != bracket){
    	
    		ControlForKeywordOrIdentifier(ptr,f_out_ptr);
		}
	    else{
	    	
	    	ControlForOthers(ptr,f_out_ptr);
		}	
	}
	fclose(ptr);
    return 0;
    	
}


char readNextChar(FILE *stream){
    char c;
    c = fgetc(stream);
    return c;
}

int DetermineCharType(char* c){

	if(isspace(c)){
		
		return space;
	}
	else if(isdigit(c)){
		return digit;
	}
	else if(isalpha(c)){
		return letter;
	}
	else if(c == '+' || c == '-' || c == ':'|| c == '*'|| c == '/'|| c == '-' || c == '=' ){
		return op;
	}
	else if(c == '_'){
		return underscore;
	}
	else if(c ==';'){
	
		return endofline;
	}
	else if('(',')','{','}','[',']'){
		return bracket;
	}
	else if('"'){
		return quote;
	}
	return;
	
}

int ControlForOthers(FILE *stream, FILE *output){
	char c;
	char* str_temp = (char*) malloc(100);
	int i = 0;
    str_temp[1] = '\0';
    char c2;
	while(true){
		
		c  = readNextChar(stream);
		
		int charType = DetermineCharType(c);
		
		if(charType == space){
			break;
		}
		else if(c == '"'){
			char c3 = 'y';
			int i = 0;
			while(c3 != '"' && c3 != EOF){
				c3 = readNextChar(stream);
				str_temp[i] = c3;
				str_temp[i +1] = '\0';
				
				i++;
			}
			if(c3 == EOF){
					fprintf(output, "%s\n", "String Constant has not been created");
					break;
				}
			
			str_temp[i-1] = '\0';
			fprintf(output, "String Constant(%s)\n", str_temp);
			
			break;
			
		}
		else if(charType == bracket || c == '*'){
			if(c == '('){
				
				c2  = readNextChar(stream);
			
				str_temp = two_char_to_str(c,c2);
				
				if(strcmp("(*",str_temp) == 0){
				
					char c3 = 'y';
					while(c3 != '*'){
						c3 = readNextChar(stream);
						if(c3 == EOF){
							break;
						}
					}
					
					char c4 = readNextChar(stream);
					str_temp = two_char_to_str(c3,c4);
					if(strcmp("*)",str_temp)==0){
						break;
					}
					else{
						fprintf(output, "%s\n", "Comment line has not been closed");
							break;
						
					}
					
					
					
					
				}
				else{
					ungetc(c2, stream);
					fprintf(output, "%s\n", "LeftPar");
				}
			}
			else if(charType == bracket){
				char* bracketName = get_bracket_name(c);
				fprintf(output, "%s\n", bracketName);
			}
			
		}
		
		if(charType == op){
			c2  = readNextChar(stream);
   	 	 	str_temp = two_char_to_str(c,c2);
   	 	 	if(is_operator(str_temp)){
   	 	 		fprintf(output, "Operator(%s)\n", str_temp);
				break;
			}
			else{
				fprintf(output, "Operator(%c)\n", c);
					ungetc(c2, stream);
					break;
			}
		}
		else if(charType == endofline){
			fprintf(output, "EndOfLine\n");
					break;
		}
		
		else{
			
			break;
		}
		i++;
		
	}
		
}

char* two_char_to_str(char chr1, char chr2){
    char* temp = (char*) malloc(sizeof(char) * 3);
    temp[0] = chr1;
    temp[1] = chr2;
    temp[2] = '\0';
    return temp;
}



int ControlForKeywordOrIdentifier(FILE *stream, FILE *output){
   char* str_temp = (char*) malloc(100);
   int stringType = undefined;
   int charType;
   char c;
   int i = 0;
   str_temp[1] = '\0';
   
   int haveUnderscore = 0;
   int haveLetter= 0;
   int haveDigit = 0;
   int haveOperator = 0;
   
   while(true) {
   	 c  = readNextChar(stream);
   	 
   	 charType = DetermineCharType(c);
   	 
   	 if(charType != space && charType != op && charType != endofline && charType != bracket ){
		
	
	   	 if(charType == letter){
	   	 	haveLetter = 1;
	     }
	     else if(charType == digit){
	     	haveDigit = 1;
		 }
		 else if(charType == underscore){
		 	haveUnderscore = 1;
		 }
		
	    if(haveLetter == 1 && haveDigit == 0 && haveUnderscore == 0 && haveOperator == 0 ){
		 		stringType = keyword;
		 }
		 else if(haveDigit == 1 && haveLetter == 0 && haveUnderscore == 0 && haveOperator == 0){
		 	stringType = intConst;
		 }
		 else if(haveOperator == 0){
		 	stringType = identifier;
		 }
		 str_temp[i] = c;
   	 	 str_temp[i+1] = '\0';
		
	}

	else{
	 	
	 	str_temp[i] = '\0';
	 	ungetc(c, stream);
	 	// for keyword
	 	if(stringType == keyword){
	 		if(is_keyword(str_temp) == 1){
				fprintf(output, "Keyword(%s)\n", str_temp);
			}
			else{
				stringType = identifier;
			}		
		}
		// for identifier
		if(stringType ==identifier){
			
			int a  = is_identifier(str_temp);
			if(a==1){
				
				fprintf(output, "Identifier(%s)\n", str_temp);
			}
			else if(a == -1){
                fprintf(output, "ERROR: <%s> is not starting with alphabetic character\n", str_temp);
			}
			else if(a == 0){
				fprintf(output, "ERROR: Identifier <%s> is too long\n", str_temp);
			}
			break;
		}
		else if(stringType == intConst){
				//printf("  INT CONST (%s)  ", str_temp);
				if(strlen(str_temp) <= 10){
					fprintf(output, "IntConst(%s)\n", str_temp);
				}
				else{
					fprintf(output, "ERROR: INTEGER CONSTANT is longer than 10 characters. (%s)\n", str_temp);
				}
				break;
		}
		
		break;
	 }
	 	
	
	 i++;
    		
	}
}

int is_identifier(char* str){

    // returns 1 if str is an identifier else a negative number
    char* p = NULL;
    // must be shorter than 20 characters
    if( strlen(str) > maxIdentifierSize){
        return 0;
    }
    // make the str all uppercase
    str = strupr(str);
    // must start with alphabetic chars
    if( str[0] > 'Z' || str[0] < 'A'){
        return -1;
    }
    str = strlwr(str);
    return 1;
}


int is_keyword(char* str){
    char keywords[18][10] = {"break","case","char","const","continue","do","else","enum",
                            "float","for","goto","if","int","long","record","return","static","while"};
    int i, flag = 0;
	str = strlwr(str);
    for(i = 0; i < 18; ++i){
        if(strcmp(keywords[i], str) == 0){
            flag = 1;
            break;
        }
    }
    return flag;
}

int is_operator(char* str){
     char operators[7][3] = {"+","-","*","/","++","--",":="};
    int i, flag = 0;

    for(i = 0; i < 7; ++i){
        if(strcmp(operators[i], str) == 0){
            flag = 1;
            break;
        }
    }
    return flag;
}

char* get_bracket_name(char ch){
    if(ch == '('){
        return "LeftPar";
    }else if(ch == ')'){
        return "RightPar";
    }else if(ch == '{'){
        return "LeftCurlyBracket";
    }else if(ch == '}'){
        return "RightCurlyBracket";
    }else if(ch =='['){
    	return "LeftSquareBracket";
	}else if(ch ==']'){
    	return "RightSquareBracket";
	}
	else{
        return NULL;
    }
}
