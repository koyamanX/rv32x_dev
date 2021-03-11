/* 
	Parameterize TLB(fully-associative, pseudo-random-replace)
	Generates `tlb_TLB_SIZE` module.
	@TLB_SIZE: NUMBER OF TLB ENTRIES (default 32)
*/

#ifndef TLB_SIZE
#define TLB_SIZE		32
#endif

#define LOG2_TLB_SIZE	_int(_log10(TLB_SIZE.0)/_log10(2.0))
#define TLB_TAG_SIZE	20
#define TLB_PTE_SIZE	32
#define TLB_LEVELS_SIZE 2

#define VALID	1'b1
#define INVALID	1'b0

declare tlb_%TLB_SIZE% {
	input access_tag[TLB_TAG_SIZE];
	func_in access_tlb(access_tag);
	output levels[TLB_LEVELS_SIZE];
	output pte[TLB_PTE_SIZE];
	func_out tlb_hit(levels, pte);
	input refill_tag[TLB_TAG_SIZE];
	input refill_pte[TLB_PTE_SIZE];
	input refill_levels[TLB_LEVELS_SIZE];
	func_in refill(refill_tag, refill_levels, refill_pte);
	func_in flush();
}

module tlb_%TLB_SIZE% {
	mem tlb_data[TLB_SIZE][TLB_PTE_SIZE] = {0};
	mem tlb_levels[TLB_SIZE][TLB_LEVELS_SIZE] = {0};
	mem tlb_tag[TLB_SIZE][TLB_TAG_SIZE] = {0};
	mem tlb_valid[TLB_SIZE] = {0};
	reg tlb_random_replace_pos[LOG2_TLB_SIZE] = 0;
	integer i_;

	tlb_random_replace_pos++;

	func access_tlb {
		generate(i_ = 0; i_ < TLB_SIZE; i_++) {
			if((tlb_valid[i_] == VALID) && (tlb_tag[i_] == access_tag)) {
				tlb_hit(tlb_levels[i_], tlb_data[i_]);
			}
		}
	}
	func refill {
		tlb_data[tlb_random_replace_pos] := refill_pte;
		tlb_levels[tlb_random_replace_pos] := refill_levels;
		tlb_tag[tlb_random_replace_pos] := refill_tag;
		tlb_valid[tlb_random_replace_pos] := VALID;
	}
	func flush {
		generate(i_ = 0; i_ < TLB_SIZE; i_++) {
			tlb_valid[i_] := INVALID;
		}
	}
}

#undef TLB_SIZE
#undef LOG2_TLB_SIZE

/* Not parameterize values below for now */
#if 0
#undef TLB_TAG_SIZE
#undef TLB_PTE_SIZE
#endif
