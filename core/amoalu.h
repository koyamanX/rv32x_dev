#ifndef AMOALU_H
#define AMOALU_H
declare amoalu {
	input funct5[5];
	input rs1_data[32];
	input rs2_data[32];
	output q[32];
	func_in exe(rs1_data, rs2_data, funct5) : q;
}
#endif
