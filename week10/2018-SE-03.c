#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>


int main(int argc,char* argv[]){
	if(argc != 3 && argc != 5){
		errx(1,"argument count 2 or 4 needed");
	}
	
	int a = -1;
	int b = -1;
	int flag = 0; //-c
	char fieldsep = ' ';
	
	if(argc == 3){
	
		if(strcmp(argv[1],"-c") != 0){
			errx(2,"expected -c");
		}
		
		if(strlen(argv[2]) == 1){
			a = atoi(argv[2]);
			b = a;
		}
		
		else if(strlen(argv[2]) == 3 && argv[2][1] == '-' ){
			a =	atoi(argv[2]);
			b = atoi(argv[2]+2); //pointer arithmetics !!!
		
			if (a > b){
				errx(5,"range invalid");
			}
		}
		
		else {
			errx(3,"format expected <number> or <number-number>");
		}
	
	}
	else if	(argc == 5){
		
		flag = 1; // -d
		
		if(strcmp(argv[1],"-d") != 0){
			errx(4,"expected -d");
		}
		
		fieldsep = argv[2][0];
		if(strcmp(argv[3],"-f") != 0){
			errx(4,"expected -f");
		}
		
		if(strlen(argv[4]) == 1){
			a = atoi(argv[4]);
			b = a;
		}
		
		else if(strlen(argv[4]) == 3 && argv[4][1] == '-' ){
			a =	atoi(argv[4]);
			b = atoi(argv[4]+2);
			
			if (a > b){
				errx(5,"range invalid");
			}
		}
		
		else {
			errx(3,"format expected <number> or <number-number>");
		}
	}

	int length = 1;
	char byte;
	int readbytes;
	int currfield = 1;
	
	while ((readbytes = read(0,&byte,sizeof(byte))) == sizeof(byte)){
	
		if(flag == 0){
		
			if (byte == '\n'){
				length = 1;
				if (write(1,&byte,sizeof(byte)) != sizeof(byte)){
					err(6,"err writing byte");
				}
				continue;
			}
			//dprintf(1, "length: %d, a: %d",length,a);

			if(length >= a && length <= b){
			
				if (write(1,&byte,sizeof(byte)) != sizeof(byte)){
					err(6,"err writing byte");
				}
				//		dprintf(1, "length: %d, a: %d",length,a);
			}
			length++;
		}
		
		else {
			if (byte == '\n'){
				currfield = 1;
			
				if (write(1,&byte,sizeof(byte)) != sizeof(byte)){
					err(6,"err writing byte");
				}
				continue;
			}

			if (byte == fieldsep){
				currfield++;
				if (currfield >= a + 1 && currfield <= b){
					if (write(1,&byte,sizeof(byte)) != sizeof(byte)){
						err(6,"err writing byte");
					}
				}
				continue;
			}
		
			if(currfield >= a && currfield <= b){			
				if (write(1,&byte,sizeof(byte)) != sizeof(byte)){
					err(6,"err writing byte");
				}
			}
			//dprintf(1,"currfield: %d, a:, %d",currfield,a);
		}
	}
	
	if (readbytes < 0){
		err(7,"err reading from stdin");
	}

	return 0;
}