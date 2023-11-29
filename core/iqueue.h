declare iqueue
{
    input pc[32];
    input insts[128];
    output inst[32];
    output addr[32];
    func_in read(pc);
    func_out ready(inst);
    func_out fetch(addr);
    func_in mem_ready(insts);
}