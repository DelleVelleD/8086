#![allow(unused_parens,unused_variables,unused_assignments,unused_mut,dead_code)]

const REG: &'static[&'static str] = &[
	"al", "ax",
	"cl", "cx",
	"dl", "dx",
	"bl", "bx",
	"ah", "sp",
	"ch", "bp",
	"dh", "si",
	"bh", "di",
];

const R_M: &'static[&'static str] = &[
	"bx + si",
	"bx + di",
	"bp + si",
	"bp + di",
	"si",
	"di",
	"bp",
	"bx",
];

fn main(){
	let args: Vec<String> = std::env::args().collect();
	if(args.len() != 2){
		print!("USAGE: 8086.exe <test_path>\n");
		return();
	}
	
	let file: Vec<u8> = std::fs::read(&args[1]).expect("");
	let mut offset: usize = 0;
	
	print!("; {}:\n", args[1]);
	print!("bits 16\n\n");
	
	let mut instruction: [u8; 6] = [0; 6];
	while(offset < file.len()){
		instruction[0] = file[{let tmp = offset; offset += 1; tmp}];
		//print!("[{}] {:#08b} ", offset, instruction[0]);
		
		let op3 = instruction[0] & 0b11100000; //bit 5-7
		let op4 = instruction[0] & 0b11110000; //bit 4-7
		let op6 = instruction[0] & 0b11111100; //bit 2-7
		
		//PUSH/POP segment register
		//TODO this
		if(op3 == 0b00000000){
			if      (instruction[0] & 0b00000111 == 0b00000110){ //PUSH
				print!("\n"); continue; //next instruction
			}else if(instruction[0] & 0b00000111 == 0b00000111){ //POP
				print!("\n"); continue; //next instruction
			}
		}
		
		//MOV immediate to register
		if(op4 == 0b10110000){
			let w   = instruction[0] & 0b00001000; //bit 3
			let reg = instruction[0] & 0b00000111; //bit 0-2
			
			instruction[1] = file[{let tmp = offset; offset += 1; tmp}];
			if(w != 0){
				instruction[2] = file[{let tmp = offset; offset += 1; tmp}];
				let word = (u16::from(instruction[2]) << 8) | u16::from(instruction[1]);
				
				print!("mov {}, {}", REG[usize::from(2*reg+1)], word);
			}else{
				print!("mov {}, {}", REG[usize::from(2*reg+0)], instruction[1]);
			}
			print!("\n"); continue; //next instruction
		}
		
		match(op6){
			//MOV register/memory to/from register
			0b10001000 => {
				print!("mov ");
				
				let w = instruction[0] & 0b00000001; //bit 0
				let d = instruction[0] & 0b00000010; //bit 1
				instruction[1] = file[{let tmp = offset; offset += 1; tmp}];
				
				let _mod = instruction[1]               >> 6; //bit 6-7
				let reg = (instruction[1] & 0b00111000) >> 3; //bit 3-5
				let r_m =  instruction[1] & 0b00000111;       //bit 0-2
				
				let reg_string = REG[usize::from(2*reg+w)];
				let r_m_string = match(_mod){
					0 => {
						match(r_m){
							0..=5 | 7 => {
								format!("[{}]", R_M[usize::from(r_m)])
							},
							6 => {
								instruction[2] = file[{let tmp = offset; offset += 1; tmp}];
								instruction[3] = file[{let tmp = offset; offset += 1; tmp}];
								let word = (u16::from(instruction[3]) << 8) | u16::from(instruction[2]);
								
								format!("[{}]", word)
							},
							_ => String::from("ERR")
						}
					},
					1 => {
						instruction[2] = file[{let tmp = offset; offset += 1; tmp}];
						
						if(instruction[2] != 0){
							if(instruction[2] & 0b10000000 != 0){
								format!("[{} - {}]", R_M[usize::from(r_m)], 0x100 - u16::from(instruction[2]))
							}else{
								format!("[{} + {}]", R_M[usize::from(r_m)], instruction[2])
							}
						}else{
							format!("[{}]", R_M[usize::from(r_m)])
						}
					},
					2 => {
						instruction[2] = file[{let tmp = offset; offset += 1; tmp}];
						instruction[3] = file[{let tmp = offset; offset += 1; tmp}];
						let word = (u16::from(instruction[3]) << 8) | u16::from(instruction[2]);
						
						if(word != 0){
							if(word & 0b10000000_00000000 != 0){
								format!("[{} - {}]", R_M[usize::from(r_m)], 0x10000 - u32::from(word))
							}else{
								format!("[{} + {}]", R_M[usize::from(r_m)], word)
							}
						}else{
							format!("[{}]", R_M[usize::from(r_m)])
						}
					},
					3 => {
						String::from(REG[usize::from(2*r_m+w)])
					}
					_ => String::from("ERR")
				};
				
				if(d != 0){
					print!("{}, {}", reg_string, r_m_string);
				}else{
					print!("{}, {}", r_m_string, reg_string);
				}
			},
			
			0b11000100 => {
				//MOV immediate to register/memory
				if(instruction[0] & 0b00000010 != 0){
					print!("mov ");
					
					let w = instruction[0] & 0b00000001; //bit 0
					instruction[1] = file[{let tmp = offset; offset += 1; tmp}];
					
					let _mod = instruction[1]               >> 6; //bit 6-7
					let r_m =  instruction[1] & 0b00000111;       //bit 0-2
					
					match(_mod){
						0 => {
							match(r_m){
								0..=5 | 7 => {
									print!("[{}], ", R_M[usize::from(r_m)]);
								},
								6 => {
									instruction[2] = file[{let tmp = offset; offset += 1; tmp}];
									instruction[3] = file[{let tmp = offset; offset += 1; tmp}];
									let word = (u16::from(instruction[3]) << 8) | u16::from(instruction[2]);
									
									print!("[{}], ", word);
								},
								_ => {}
							}
						},
						1 => {
							instruction[2] = file[{let tmp = offset; offset += 1; tmp}];
							
							if(instruction[2] != 0){
								if(instruction[2] & 0b10000000 != 0){
									print!("[{} - {}], ", R_M[usize::from(r_m)], 0x100 - u16::from(instruction[2]));
								}else{
									print!("[{} + {}], ", R_M[usize::from(r_m)], instruction[2]);
								}
							}else{
								print!("[{}], ", R_M[usize::from(r_m)]);
							}
						},
						2 => {
							instruction[2] = file[{let tmp = offset; offset += 1; tmp}];
							instruction[3] = file[{let tmp = offset; offset += 1; tmp}];
							let word = (u16::from(instruction[3]) << 8) | u16::from(instruction[2]);
							
							if(word != 0){
								if(word & 0b10000000_00000000 != 0){
									print!("[{} - {}], ", R_M[usize::from(r_m)], 0x10000 - u32::from(word));
								}else{
									print!("[{} + {}], ", R_M[usize::from(r_m)], word);
								}
							}else{
								print!("[{}], ", R_M[usize::from(r_m)]);
							}
						},
						3 => {
							print!("[{}], ", REG[usize::from(2*r_m+w)]);
						},
						_ => {
							print!("ERR, ");
						}
					}

					instruction[4] = file[{let tmp = offset; offset += 1; tmp}];
					if(w != 0){
						instruction[5] = file[{let tmp = offset; offset += 1; tmp}];
						let word = (u16::from(instruction[5]) << 8) | u16::from(instruction[4]);
						
						print!("word {}", word);
					}else{
						print!("byte {}", instruction[4]);
					}
				}else{
					//TODO LES/LDS
				}
			},

			//MOV memory/accumulator to accumulator/memory
			0b10100000 => {
				let w = instruction[0] & 0b00000001; //bit 0
				let d = instruction[0] & 0b00000010; //bit 1
				instruction[1] = file[{let tmp = offset; offset += 1; tmp}];
				
				let mut word = u16::from(instruction[1]);
				if(w != 0){
					instruction[2] = file[{let tmp = offset; offset += 1; tmp}];

					word = (u16::from(instruction[2]) << 8) | u16::from(instruction[1]);
				}

				if(d != 0){
					print!("mov [{}], ax", word);
				}else{
					print!("mov ax, [{}]", word);
				}
			},
			
			_ => {
				print!("error: unrecognized opcode `{:#08b}` at [{}]\n", instruction[0], offset);
				return();
			}
		}
		
		print!("\n");
	}
}
