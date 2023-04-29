REG = [
	"al", "ax",
	"cl", "cx",
	"dl", "dx",
	"bl", "bx",
	"ah", "sp",
	"ch", "bp",
	"dh", "si",
	"bh", "di",
];

R_M = [
	"bx + si",
	"bx + di",
	"bp + si",
	"bp + di",
	"si",
	"di",
	"bp",
	"bx",
];

if ARGV.size != 1
	exit(false);
end

$file = File.open(ARGV[0], "rb");
print("; ",ARGV[0],":\n");
print("bits 16\n\n");

$instruction = Array.new(6);
def next_byte(offset)
	return if $instruction[offset] = $file.getbyte();
	exit(offset == 0);
end

def make_word(offset) #make a 16bit integer from two 8bit integers
	return ($instruction[offset+1] << 8) | $instruction[offset];
end

while true
	next_byte(0);
	#printf("[%2d] ", $file.pos);
	#printf("[%2d] 0b%08b ", $file.pos, $instruction[0]);
	
	op3 = $instruction[0] & 0b11100000; #bit 5..7
	op4 = $instruction[0] & 0b11110000; #bit 4..7
	op6 = $instruction[0] & 0b11111100; #bit 2..7
	
	#PUSH/POP segment register
	if op3 == 0b00000000
		if    $instruction[0] & 0b00000111 == 0b00000110 #PUSH
			#TODO
			next; #next instruction
		elsif $instruction[0] & 0b00000111 == 0b00000111 #POP
			#TODO
			next; #next instruction
		end
	end
	
	#MOV immediate to register
	if op4 == 0b10110000
		w   = $instruction[0] & 0b00001000; #bit 3
		reg = $instruction[0] & 0b00000111; #bit 0..2
		
		next_byte(1);
		if w != 0b00000000
			next_byte(2);
			word = make_word(1);
			
			printf("mov %s, %u\n", REG[2*reg+1], word);
		else
			printf("mov %s, %u\n", REG[2*reg+0], $instruction[1]);
		end
		next; #next instruction
	end
	
	case op6
		#MOV register/memory to/from register
		when 0b10001000 then
			printf("mov ");
			
			w = $instruction[0] & 0b00000001; #bit 0
			d = $instruction[0] & 0b00000010; #bit 1
			next_byte(1);

			mod =  $instruction[1]               >> 6; #bit 6..7
			reg = ($instruction[1] & 0b00111000) >> 3; #bit 3..5
			r_m =  $instruction[1] & 0b00000111;       #bit 0..2
			
			reg_string = REG[2*reg+w];
			r_m_string = "ERR";
			case mod
				when 0 then
					case r_m
						when 0..5 then
							r_m_string = "["+R_M[r_m]+"]";
						when 6 then
							next_byte(2); next_byte(3);
							r_m_string = "["+make_word(2).to_s+"]";
						when 7 then
							r_m_string = "["+R_M[r_m]+"]";
					end
				when 1 then
					next_byte(2);
					if $instruction[2] != 0b00000000
						if $instruction[2] & 0b10000000 != 0b00000000
							r_m_string = "["+R_M[r_m]+" - "+(0x100-$instruction[2]).to_s+"]";
						else
							r_m_string = "["+R_M[r_m]+" + "+$instruction[2].to_s+"]";
						end
					else
						r_m_string = "["+R_M[r_m]+"]";
					end
				when 2 then
					next_byte(2); next_byte(3);
					word = make_word(2);
					if word != 0b00000000_00000000
						if word & 0b10000000_00000000 != 0b00000000_00000000
							r_m_string = "["+R_M[r_m]+" - "+(0x10000-word).to_s+"]";
						else
							r_m_string = "["+R_M[r_m]+" + "+word.to_s+"]";
						end
					else
						r_m_string = "["+R_M[r_m]+"]";
					end
				when 3 then
					r_m_string = REG[2*r_m+w];
			end
			
			if d != 0b00000000
				printf("%s, %s\n", reg_string, r_m_string);
			else
				printf("%s, %s\n", r_m_string, reg_string);
			end
		when 0b11000100 then
			#MOV immediate to register/memory
			if $instruction[0] & 0b00000010
				printf("mov ");
				
				w = $instruction[0] & 0b00000001; #bit 0
				next_byte(1);
				
				mod = $instruction[1]               >> 6; #bit 6..7
				r_m = $instruction[1] & 0b00000111;       #bit 0..2
				
				case mod
					when 0 then
						case r_m
							when 0..5 then printf("["+R_M[r_m]+"], ");
							when 6 then
								next_byte(2); next_byte(3);
								printf("["+make_word(2)+"], ");
							when 7 then printf("["+R_M[r_m]+"], ");
						end
					when 1 then
						next_byte(2);
						if instruction[2] != 0b00000000
							if $instruction[2] & 0b10000000 != 0b00000000
								printf("["+R_M[r_m]+" - "+(0x100-$instruction[2]).to_s+"], ");
							else
								printf("["+R_M[r_m]+" + "+$instruction[2].to_s+"], ");
							end
						else
							printf("["+R_M[r_m]+"], ");
						end
					when 2 then
						next_byte(2); next_byte(3);
						word = make_word(2);
						if word != 0b00000000_00000000
							if word & 0b10000000_00000000 != 0b00000000_00000000
								printf("["+R_M[r_m]+" - "+(0x10000-word).to_s+"], ");
							else
								printf("["+R_M[r_m]+" + "+word.to_s+"], ");
							end
						else
							printf("["+R_M[r_m]+"], ");
						end
					when 3 then
						printf("%s, ", REG[2*r_m+w]);
				end
				
				next_byte(4);
				if w != 0b00000000
					next_byte(5);
					printf("word %d\n", make_word(4));
				else
					printf("byte %d\n", $instruction[4]);
				end
			else
				#TODO LES/LDS
			end
		#MOV memory/accumulator to accumulator/memory
		when 0b10100000 then
			w = $instruction[0] & 0b00000001; #bit 0
			d = $instruction[0] & 0b00000010; #bit 1
			next_byte(1);
			
			word = $instruction[1];
			
			if w != 0b00000000
				next_byte(2);
				word = make_word(1);
			end
			
			printf(d != 0b00000000 ? "mov [%u], ax\n" : "mov ax, [%u]\n", word);
		else
			printf("\nerror: unrecognized opcode `0b%b` at [%d]\n", $instruction[0], $file.pos);
			exit(false);
	end
end
