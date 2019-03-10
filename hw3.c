//goulias ioannis aem:1785 bistarakis aleksandros aem:1829

/*this is a program that reads various expressions containing wildcards ("?*") if the expressions are valid the program reads various names and checks if the names match the expressions.
 * If the names match,a replacement for each name is printed for each specific wildcard */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaxNameLen  32 //max length of names without '/0' being counted

struct nameInfo {
	char* name;
	char* replacement;
};

typedef struct nameInfo nameInfoT;
char* findMatch(char* str, char* expression);
void findAllReplacements(nameInfoT* names, size_t count, char* expression);
void printAllReplacements(nameInfoT* names, size_t count);
void clearAllReplacements(nameInfoT* names, size_t count);
int checkExpression(char* expression);
void freeNames(nameInfoT* names, size_t count);

nameInfoT* readNames(size_t* array_size);

int main(int argc, char* argv[]) {
	
	size_t names_count = 0;
	nameInfoT* names = readNames(&names_count); //call readNames to allocate memory
	char formatstring[13];
	char buffer[MaxNameLen + 1];
	
	while (1) {
		sprintf(formatstring,"%%%ds",MaxNameLen);
		scanf(formatstring, buffer);

		if (!strcmp(buffer, "END")) break;
		
		if (checkExpression(buffer)) printf("Skipping %s\n", buffer); //call checkExpression , if checkExpression returns 1 expression is not valid else the expression is valid and the code continues
		else {
			printf("%s\n", buffer);
			findAllReplacements(names, names_count, buffer); //call findAllReplacements
			printAllReplacements(names, names_count); //call printAllReplacements
			clearAllReplacements(names, names_count); //call clearAllReplacements
		}
	}

	freeNames(names, names_count); //call freeNames
	
	return(0);
}

nameInfoT* readNames(size_t* array_size) {//readNames prototype

	char buffer[MaxNameLen + 1];
	char formatstring[13];
	nameInfoT* tempPtr;
	nameInfoT* array = (nameInfoT*)malloc(sizeof(nameInfoT));  //allocate memory for the nameInfoT array
	
	if (array == NULL) {
		
		printf("Allocation failure.\nProgram will be terminated.\n"); /*if allocation fails , error message is printed and the program terminates because
																		if a memory failure happens during runtime it is better to terminate the program rather than continue with the rest inputs. 
																		If the program continued to the next names the output would differ from the stdoutput*/
		exit(1);
	}
	
	size_t max_size = 1;
	size_t size = 0;

	while (1) {
		
		sprintf(formatstring,"%%%ds",MaxNameLen);
		scanf(formatstring, buffer);

		if (!strcmp(buffer, "END")) break;

		if (size == max_size) { //every time the array is filled with names, it's size is doubled

			max_size *= 2;
			tempPtr = (nameInfoT*)realloc(array, max_size * sizeof(nameInfoT)); //tempPtr to use in reallocation
			
			if (tempPtr == NULL) {
				printf("Reallocation failure.\nThe program will be terminated.\n");/*if reallocation fails , error message is printed and the program terminates because
																					if a memory failure happens during runtime it is better to terminate the program rather than continue with the rest inputs. 
																					If the program continued to the next names the output would differ from the stdoutput*/
				exit(1);
			}
			array = tempPtr; //if reallocation is succesfull array  becomes tempPtr thus it's size is doubled
		}

		size_t len = strlen(buffer) + 1;

		array[size].name = (char*)malloc(len * sizeof(char));//allocate memory for names in the array
		if (array[size].name == NULL) {
			printf("Reallocation failure.\nThe program will be terminated.\n");
			freeNames(array, size); //if reallocation fails the array and the names are freed
			exit(1);
		}
		array[size].replacement = NULL; //replacement points to NULL in this stage

		memcpy(array[size].name, buffer, len * sizeof(char));//save the current name in the array of names

		size++; //size escalates by 1
	}
	
	array = (nameInfoT*)realloc(array, size * sizeof(nameInfoT));//array of nameInfoT gets it's final memory allocation
	*array_size = size; //size of the array is returned by reference 

	return(array);
}

char* findMatch(char* str, char* expression) { //findMatch prototype
	char* WildCards = "?*";
	char* wildcard = strpbrk(expression, WildCards); //wildcard takes the value of either '?' or '*'
	int len_str, len_exp;
	len_exp = strlen(expression);
	len_str = strlen(str);
	int len = wildcard - expression; //len indicates where the wildcard character is in the expression
	char *s = NULL; //pointer needed for pointer arithmetic afterwards
	
	// if there are characters before the wildcard,
	// check whether they are same for str and expression...
	// if not, return NULL
	if (len > 0 && strncmp(str, expression, len)) return (NULL);

	if (*wildcard == '?') { //if the expression contains '?'
		
		// check if all characters after the wildcard match,
		// if not, return NULL
		if (strcmp(str + len + 1, expression + len + 1)) return (NULL);
		
		else {
			
			char* ret = (char*)malloc(sizeof(char)* 2);// allocate a new string to contain the matched character as well as null terminating char
			
			if (ret == NULL) {
				
				printf("Allocation failure.\nThe program will continue to the next replacement.\n"); //if allocation fails the program will continue with the next replacement (that is prefered because names are already given)
				
				return (NULL);
			}
			
			ret[0] = str[len];// the ret[0] has the value of the str in the position that len indicates(the position that the wildcard appeared in the expression
			ret[1] = '\0'; //'\0' entered manually
			
			if (ret[0] == '\0') { //if the wildcard expression consists only of '?' then nothing will be printed and the ret string is freed

				free (ret);
				return (NULL);
			}
			return (ret);
		}
	}



	else if (*wildcard == '*') { //if the expression contains '*'
		
		if (len_exp == 1) {//if the expression consists only of '*' all the names are going to be printed with replacements being the same strings as the names

			char* ret = (char*)malloc((len_str+1) * sizeof(char));// allocate a new string to contain the matched character as well as null terminating char
			
			if (ret == NULL) {
				
				printf("Allocation failure.\nThe program will continue to the next replacement.\n");//if allocation fails the program will continue with the next replacement (that is prefered because names are already given)
				
				return (NULL);
			}
			
			memcpy(ret,str, (len_str) * sizeof(char));
			
			ret[len_str+1] = '\0';//'\0' entered manually
			
			return (ret);
		}
		
		if(len +1 == len_exp) { //if '*' at the end of the expression
			
			if (!strncmp(str, expression, len - 1)) { //if a previous character of the str matches a previous character of the expression
				
				char* ret = (char*)malloc((len_str - len + 2)*sizeof(char));// allocate a new string to contain the matched character as well as null terminating char
				
				if (ret==NULL) {
					
					printf("Allocation failure.\nThe program will continue to the next replacement.\n");//if allocation fails the program will continue with the next replacement (that is prefered because names are already given)
					return (NULL);
					
				}
				
				strcpy(ret, &str[len]); //replacement is the remaining str after the position of the wildcard in the expression
				
				return (ret);
			}
			return (NULL);
		}
		
		
		// try to match the remainder of the expression,
		// if it cant be matched, move to next character of str and try to match from there
		// search stops when terminating char is found
		int len2 = 0;//int that indicates the length of the replacement
		for (s = str+len; *s; s++, len2++) {// figure out how many characters were skipped until match was found

			if (!strcmp(s, expression + len + 1)) {		
				
				char* ret = (char*)malloc(len2 * sizeof(char));// allocate a new string to contain the matched character as well as null terminating char
				if (ret == NULL) {
					printf("Allocation failure.\nThe program will continue to the next replacement.\n");//if allocation fails the program will continue with the next replacement (that is prefered because names are already given)
					return (NULL);
				}
				memcpy(ret, str + len, (len2) * sizeof(char));
				ret[len2] = '\0';
				return (ret);
			}
		}
	}
	return(NULL);
}

void findAllReplacements(nameInfoT* names, size_t count, char* expression) { //findAllReplacements prototype

	size_t i = 0;
	
	for (i = 0; i < count; i++) names[i].replacement = findMatch(names[i].name, expression); //for all the names that were counted in readNames if a replacement is found, it is stored in the array

}

void printAllReplacements(nameInfoT* names, size_t count) { //printAllReplacements prototype

	size_t i = 0;
	
	for (i = 0; i < count; i++) if (names[i].replacement) printf("\tword: %s, replacement: %s\n", names[i].name, names[i].replacement); //if the replacement of the specific name exists then the name and the replacement are printed

}

void clearAllReplacements(nameInfoT* names, size_t count) { //clearAllReplacements prototype

	size_t i = 0;
	
	for (i = 0; i < count; i++) {
		
		if (names[i].replacement) free(names[i].replacement); //replacements are freed and pointed to NULL

		names[i].replacement = NULL;
		
	}
}

// Returns 0 if expression is valid else 1 is returned
int checkExpression(char* expression) { //checkExpression prototype

	int c = 0;
	int i = 0;
	
	for (i = 0; expression[i] != '\0'; i++) {
		
		if (expression[i] == '?' || expression[i] == '*') {
			
			c++;
		}
	}
	
	if (c == 1) return 0;
	
	return (1);
	
}

void freeNames(nameInfoT* names, size_t count) { //freeNames prototype

	size_t i = 0;
	
	for (i = 0; i < count; i++){
		
		if (names[i].name) free(names[i].name); //names are freed

		names[i].name = NULL;
		
	}

	free(names); //the array of nameInfoT is freed
}