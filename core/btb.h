#ifndef BTB_H
#define BTB_H
declare btb {
	input pc[32];
	func_in predict(pc);
	func_out predict_as_taken();
	func_out predict_as_not_taken();
	/* if predic_as_taken, predicted_new_pc points branch target address
		registered during execution.
		if predict_as_not_taken, predicted_new_pc points pc
		so, when using as next pc on predict_as_not_taken, predicted_new_pc must be added
		by two or four depending on instruction length of next instructiion.
	*/
	output predicted_new_pc[32];

	/* register branch instruction' address and target address */
	input update_branch_instruction_address[32];
	input update_branch_target_address[32];
	func_in update(update_branch_instruction_address, update_branch_target_address);
	/* actual branch result */
	func_in taken();
	func_in not_taken();
	/* flush btb contents and branch prediction(takes one cycle to appear result), 
		`regist` on same cycle is just ignored  */
	func_in flush();
}
#endif
