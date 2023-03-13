#include <assert.h>
#include <stdio.h>

enum{
	MOV = 0x88
};

int main(int args_count, char** args){
	if(args_count != 2){
		return -1;
	}
	
	//open file
	char* filename = args[1];
	FILE* file = fopen(filename, "rb");
	if(!file){
		return -1;
	}
	
	printf("; %s:\n", filename);
	printf("bits 16\n");
	printf("\n");
	
	//decode machine instructions
	int c;
	unsigned char instruction[6] = {0};
	while(1){
		//byte 1
		c = fgetc(file);
		if(c == EOF) return 0;
		instruction[0] = (unsigned char)c;
		
		unsigned char w      = instruction[0] & 0x01; //bit 0 (from right)
		unsigned char d      = instruction[0] & 0x02; //bit 1
		unsigned char opcode = instruction[0] & 0xfc; //bit 2-7
		
		switch(opcode){
			case MOV:{
				printf("mov ");
				
				//byte 2
				c = fgetc(file);
				if(c == EOF) return -1;
				instruction[1] = (unsigned char)c;
				
				unsigned char mod =  instruction[1]         >> 6; //bit 6-7
				unsigned char reg = (instruction[1] & 0x3f) >> 3; //bit 3-5
				unsigned char r_m =  instruction[1] & 0x07;       //bit 0-2
				
				const char* reg_string = "";
				switch(reg){
					case 0:{
						if(w){
							reg_string = "ax";
						}else{
							reg_string = "al";
						}
					}break;
					case 1:{
						if(w){
							reg_string = "cx";
						}else{
							reg_string = "cl";
						}
					}break;
					case 2:{
						if(w){
							reg_string = "dx";
						}else{
							reg_string = "dl";
						}
					}break;
					case 3:{
						if(w){
							reg_string = "bx";
						}else{
							reg_string = "bl";
						}
					}break;
					case 4:{
						if(w){
							reg_string = "sp";
						}else{
							reg_string = "ah";
						}
					}break;
					case 5:{
						if(w){
							reg_string = "bp";
						}else{
							reg_string = "ch";
						}
					}break;
					case 6:{
						if(w){
							reg_string = "si";
						}else{
							reg_string = "dh";
						}
					}break;
					case 7:{
						if(w){
							reg_string = "di";
						}else{
							reg_string = "bh";
						}
					}break;
				}
				
				const char* r_m_string = "";
				switch(r_m){
					case 0:{
						switch(mod){
							case 0:{
								
							}break;
							case 1:{
								
							}break;
							case 2:{
								
							}break;
							case 3:{
								if(w){
									r_m_string = "ax";
								}else{
									r_m_string = "al";
								}
							}break;
						}
					}break;
					case 1:{
						switch(mod){
							case 0:{
								
							}break;
							case 1:{
								
							}break;
							case 2:{
								
							}break;
							case 3:{
								if(w){
									r_m_string = "cx";
								}else{
									r_m_string = "cl";
								}
							}break;
						}
					}break;
					case 2:{
						switch(mod){
							case 0:{
								
							}break;
							case 1:{
								
							}break;
							case 2:{
								
							}break;
							case 3:{
								if(w){
									r_m_string = "dx";
								}else{
									r_m_string = "dl";
								}
							}break;
						}
					}break;
					case 3:{
						switch(mod){
							case 0:{
								
							}break;
							case 1:{
								
							}break;
							case 2:{
								
							}break;
							case 3:{
								if(w){
									r_m_string = "bx";
								}else{
									r_m_string = "bl";
								}
							}break;
						}
					}break;
					case 4:{
						switch(mod){
							case 0:{
								
							}break;
							case 1:{
								
							}break;
							case 2:{
								
							}break;
							case 3:{
								if(w){
									r_m_string = "sp";
								}else{
									r_m_string = "ah";
								}
							}break;
						}
					}break;
					case 5:{
						switch(mod){
							case 0:{
								
							}break;
							case 1:{
								
							}break;
							case 2:{
								
							}break;
							case 3:{
								if(w){
									r_m_string = "bp";
								}else{
									r_m_string = "ch";
								}
							}break;
						}
					}break;
					case 6:{
						switch(mod){
							case 0:{
								
							}break;
							case 1:{
								
							}break;
							case 2:{
								
							}break;
							case 3:{
								if(w){
									r_m_string = "si";
								}else{
									r_m_string = "dh";
								}
							}break;
						}
					}break;
					case 7:{
						switch(mod){
							case 0:{
								
							}break;
							case 1:{
								
							}break;
							case 2:{
								
							}break;
							case 3:{
								if(w){
									r_m_string = "di";
								}else{
									r_m_string = "bh";
								}
							}break;
						}
					}break;
				}
				
				if(d){
					printf("%s, %s\n", reg_string, r_m_string);
				}else{
					printf("%s, %s\n", r_m_string, reg_string);
				}
			}break;
		}
	}
}