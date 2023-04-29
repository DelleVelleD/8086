#include <assert.h>
#include <stdio.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
((byte) & 0x80 ? '1' : '0'), \
((byte) & 0x40 ? '1' : '0'), \
((byte) & 0x20 ? '1' : '0'), \
((byte) & 0x10 ? '1' : '0'), \
((byte) & 0x08 ? '1' : '0'), \
((byte) & 0x04 ? '1' : '0'), \
((byte) & 0x02 ? '1' : '0'), \
((byte) & 0x01 ? '1' : '0') 

const char* reg_strings[] = {
	"al", "ax",
	"cl", "cx",
	"dl", "dx",
	"bl", "bx",
	"ah", "sp",
	"ch", "bp",
	"dh", "si",
	"bh", "di",
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
	int byte;
	unsigned char instruction[6] = {0};
	while(1){
#if 0
		printf("`%u`", ftell(file));
#endif
		
		byte = fgetc(file);
		if(byte == EOF) return 0;
		instruction[0] = (unsigned char)byte;
		
		unsigned char op_3 = instruction[0] & 0b11100000; //bit 5-7
		unsigned char op_4 = instruction[0] & 0b11110000; //bit 4-7
		unsigned char op_6 = instruction[0] & 0b11111100; //bit 2-7
		
#if 0
		printf("`0b"BYTE_TO_BINARY_PATTERN"`", BYTE_TO_BINARY(instruction[0]));
#endif
		
		//PUSH/POP segment register
		if(op_3 == 0b00000000){
			if(instruction[0] & 0b000000111 == 0b00000110){ //PUSH
				//TODO
				continue;
			}else if(instruction[0] & 0b000000111 == 0b00000111){ //POP
				//TODO
				continue;
			}
		}
		
		//MOV immediate to register
		if(op_4 == 0b10110000){
			unsigned char w   = instruction[0] & 0b00001000; //bit 3
			unsigned char reg = instruction[0] & 0b00000111; //bit 0-2
			
			byte = fgetc(file);
			if(byte == EOF) return -1;
			instruction[1] = (unsigned char)byte;
			
			if(w){
				byte = fgetc(file);
				if(byte == EOF) return -1;
				instruction[2] = (unsigned char)byte;
				
				unsigned short word = *(unsigned short*)(instruction+1);
				
				printf("mov %s, %u\n", reg_strings[2*reg + 1], (unsigned int)word);
			}else{
				printf("mov %s, %u\n", reg_strings[2*reg + 0], (unsigned int)instruction[1]);
			}
			continue;
		}
		
		switch(op_6){
			default:{
				printf("\nunrecognized opcode `0b"BYTE_TO_BINARY_PATTERN"` at `%u`\n", BYTE_TO_BINARY(instruction[0]), ftell(file));
				return -1;
			}break;
			
			//MOV register/memory to/from register
			case 0b10001000:{
				printf("mov ");
				
				byte = fgetc(file);
				if(byte == EOF) return -1;
				instruction[1] = (unsigned char)byte;
				
				unsigned char w = instruction[0] & 0b00000001; //bit 0
				unsigned char d = instruction[0] & 0b00000010; //bit 1
				
				unsigned char mod =  instruction[1]               >> 6; //bit 6-7
				unsigned char reg = (instruction[1] & 0b00111000) >> 3; //bit 3-5
				unsigned char r_m =  instruction[1] & 0b00000111;       //bit 0-2
				
				const char* reg_string = reg_strings[2*reg + w];
				switch(reg){
					case 0:{
						if(w){
						}else{
						}
					}break;
					case 1:{
						if(w){
						}else{
						}
					}break;
					case 2:{
						if(w){
						}else{
						}
					}break;
					case 3:{
						if(w){
						}else{
						}
					}break;
					case 4:{
						if(w){
						}else{
						}
					}break;
					case 5:{
						if(w){
						}else{
						}
					}break;
					case 6:{
						if(w){
						}else{
						}
					}break;
					case 7:{
						if(w){
						}else{
						}
					}break;
				}
				
				char string_buffer[32];
				const char* r_m_string = "";
				switch(mod){
					case 0:{
						switch(r_m){
							case 0:{
								r_m_string = "[bx + si]";
							}break;
							case 1:{
								r_m_string = "[bx + di]";
							}break;
							case 2:{
								r_m_string = "[bp + si]";
							}break;
							case 3:{
								r_m_string = "[bp + di]";
							}break;
							case 4:{
								r_m_string = "[si]";
							}break;
							case 5:{
								r_m_string = "[di]";
							}break;
							case 6:{ //direct address
								byte = fgetc(file);
								if(byte == EOF) return -1;
								instruction[2] = (unsigned char)byte;
								
								byte = fgetc(file);
								if(byte == EOF) return -1;
								instruction[3] = (unsigned char)byte;
								
								unsigned short word = *(unsigned short*)(instruction+2);
								
								sprintf(string_buffer, "[%u]", (unsigned int)word);
								r_m_string = string_buffer;
							}break;
							case 7:{
								r_m_string = "[bx]";
							}break;
						}
					}break;
					case 1:{
						byte = fgetc(file);
						if(byte == EOF) return -1;
						instruction[2] = (unsigned char)byte;
						
						const char* eac_string;
						switch(r_m){
							case 0:{
								eac_string = "bx + si";
							}break;
							case 1:{
								eac_string = "bx + di";
							}break;
							case 2:{
								eac_string = "bp + si";
							}break;
							case 3:{
								eac_string = "bp + di";
							}break;
							case 4:{
								eac_string = "si";
							}break;
							case 5:{
								eac_string = "di";
							}break;
							case 6:{
								eac_string = "bp";
							}break;
							case 7:{
								eac_string = "bx";
							}break;
						}
						
						if(instruction[2]){
							if(instruction[2] & 0b10000000){
								sprintf(string_buffer, "[%s - %u]", eac_string, (unsigned int)(0x100 - instruction[2]));
							}else{
								sprintf(string_buffer, "[%s + %u]", eac_string, (unsigned int)instruction[2]);
							}
						}else{
							sprintf(string_buffer, "[%s]", eac_string);
						}
						r_m_string = string_buffer;
					}break;
					case 2:{
						byte = fgetc(file);
						if(byte == EOF) return -1;
						instruction[2] = (unsigned char)byte;
						
						byte = fgetc(file);
						if(byte == EOF) return -1;
						instruction[3] = (unsigned char)byte;
						
						unsigned short word = *(unsigned short*)(instruction+2);
						
						const char* eac_string;
						switch(r_m){
							case 0:{
								eac_string = "bx + si";
							}break;
							case 1:{
								eac_string = "bx + di";
							}break;
							case 2:{
								eac_string = "bp + si";
							}break;
							case 3:{
								eac_string = "bp + di";
							}break;
							case 4:{
								eac_string = "si";
							}break;
							case 5:{
								eac_string = "di";
							}break;
							case 6:{
								eac_string = "bp";
							}break;
							case 7:{
								eac_string = "bx";
							}break;
						}
						
						if(word){
							if(word & 0b1000000000000000){
								sprintf(string_buffer, "[%s - %u]", eac_string, (unsigned int)(0x10000 - word));
							}else{
								sprintf(string_buffer, "[%s + %u]", eac_string, (unsigned int)word);
							}
						}else{
							sprintf(string_buffer, "[%s]", eac_string);
						}
						r_m_string = string_buffer;
					}break;
					case 3:{
						switch(r_m){
							case 0:{
								
							}break;
							case 1:{
								
							}break;
							case 2:{
								
							}break;
							case 3:{
								
							}break;
							case 4:{
								
							}break;
							case 5:{
								
							}break;
							case 6:{
								
							}break;
							case 7:{
								
							}break;
						}
						
						if(w){
						}else{
						}
						
						r_m_string = reg_strings[2*r_m + w];
					}break;
				}
				
				if(d){
					printf("%s, %s\n", reg_string, r_m_string);
				}else{
					printf("%s, %s\n", r_m_string, reg_string);
				}
			}break;
			
			case 0b11000100:{
				//MOV immediate to register/memory
				if(instruction[0] & 0b00000010){
					printf("mov ");
					
					byte = fgetc(file);
					if(byte == EOF) return -1;
					instruction[1] = (unsigned char)byte;
					
					unsigned char w = instruction[0] & 0b00000001; //bit 0
					
					unsigned char mod =  instruction[1]               >> 6; //bit 6-7
					unsigned char r_m =  instruction[1] & 0b00000111;       //bit 0-2
					
					switch(mod){
						case 0:{
							switch(r_m){
								case 0:{
									printf("[bx + si], ");
								}break;
								case 1:{
									printf("[bx + di], ");
								}break;
								case 2:{
									printf("[bp + si], ");
								}break;
								case 3:{
									printf("[bp + di], ");
								}break;
								case 4:{
									printf("[si], ");
								}break;
								case 5:{
									printf("[di], ");
								}break;
								case 6:{ //direct address
									byte = fgetc(file);
									if(byte == EOF) return -1;
									instruction[2] = (unsigned char)byte;
									
									byte = fgetc(file);
									if(byte == EOF) return -1;
									instruction[3] = (unsigned char)byte;
									
									unsigned short word = *(unsigned short*)(instruction+2);
									
									printf("[%u], ", (unsigned int)word);
								}break;
								case 7:{
									printf("[bx], ");
								}break;
							}
						}break;
						case 1:{
							//byte 3
							byte = fgetc(file);
							if(byte == EOF) return -1;
							instruction[2] = (unsigned char)byte;
							
							const char* eac_string;
							switch(r_m){
								case 0:{
									eac_string = "bx + si";
								}break;
								case 1:{
									eac_string = "bx + di";
								}break;
								case 2:{
									eac_string = "bp + si";
								}break;
								case 3:{
									eac_string = "bp + di";
								}break;
								case 4:{
									eac_string = "si";
								}break;
								case 5:{
									eac_string = "di";
								}break;
								case 6:{
									eac_string = "bp";
								}break;
								case 7:{
									eac_string = "bx";
								}break;
							}
							
							if(instruction[2]){
								if(instruction[2] & 0b10000000){
									printf("[%s - %u], ", eac_string, (unsigned int)(0x100 - instruction[2]));
								}else{
									printf("[%s + %u], ", eac_string, (unsigned int)instruction[2]);
								}
							}else{
								printf("[%s], ", eac_string);
							}
						}break;
						case 2:{
							//byte 3
							byte = fgetc(file);
							if(byte == EOF) return -1;
							instruction[2] = (unsigned char)byte;
							
							//byte 4
							byte = fgetc(file);
							if(byte == EOF) return -1;
							instruction[3] = (unsigned char)byte;
							
							unsigned short word = *(unsigned short*)(instruction+2);
							
							const char* eac_string;
							switch(r_m){
								case 0:{
									eac_string = "bx + si";
								}break;
								case 1:{
									eac_string = "bx + di";
								}break;
								case 2:{
									eac_string = "bp + si";
								}break;
								case 3:{
									eac_string = "bp + di";
								}break;
								case 4:{
									eac_string = "si";
								}break;
								case 5:{
									eac_string = "di";
								}break;
								case 6:{
									eac_string = "bp";
								}break;
								case 7:{
									eac_string = "bx";
								}break;
							}
							
							if(word){
								if(word & 0b1000000000000000){
									printf("[%s - %u], ", eac_string, (unsigned int)(0x10000 - word));
								}else{
									printf("[%s + %u], ", eac_string, (unsigned int)word);
								}
							}else{
								printf("[%s], ", eac_string);
							}
						}break;
						case 3:{
							switch(r_m){
								case 0:{
									
								}break;
								case 1:{
									
								}break;
								case 2:{
									
								}break;
								case 3:{
									
								}break;
								case 4:{
									
								}break;
								case 5:{
									
								}break;
								case 6:{
									
								}break;
								case 7:{
									
								}break;
							}
							
							if(w){
							}else{
							}
						}break;
					}
					
					byte = fgetc(file);
					if(byte == EOF) return -1;
					instruction[4] = (unsigned char)byte;
					
					if(w){
						byte = fgetc(file);
						if(byte == EOF) return -1;
						instruction[5] = (unsigned char)byte;
						
						unsigned short word = *(unsigned short*)(instruction+4);
						
						printf("word %u\n", (unsigned int)word);
					}else{
						printf("byte %u\n", (unsigned int)instruction[4]);
					}
				}else{
					//TODO LES/LDS
				}
			}break;
			
			//MOV memory/accumulator to accumulator/memory
			case 0b10100000:{
				unsigned char w = instruction[0] & 0b00000001; //bit 0
				unsigned char d = instruction[0] & 0b00000010; //bit 1
				
				byte = fgetc(file);
				if(byte == EOF) return -1;
				instruction[1] = (unsigned char)byte;
				
				unsigned short word = instruction[1];
				
				if(w){
					byte = fgetc(file);
					if(byte == EOF) return -1;
					instruction[2] = (unsigned char)byte;
					
					word = *(unsigned short*)(instruction+1);
				}
				
				if(d){ //accumulator to memory
					printf("mov [%u], ax\n", word);
				}else{ //memory to accumulator
					printf("mov ax, [%u]\n", word);
				}
			}break;
		}
	}
}