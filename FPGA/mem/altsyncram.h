declare altsyncram interface {
    param_int
        widthad_a,
        width_a,
        width_byteena_a,
        numwords_a;
    param_str
        clock_enable_input_a,
        clock_enable_output_a,
        intended_device_family,
        lpm_hint,
        lpm_type,
        init_file,
        operation_mode,
        outdata_aclr_a,
        outdata_reg_a,
        power_up_uninitialized,
        read_during_write_mode_port_a;
    input address_a[13];
    input clock0;
    input data_a[8];
    input wren_a;
    output q_a[8];
    input aclr0;
    input aclr1;
    input address_b[14];
    input addressstall_a;
    input addressstall_b;
    input byteena_a;
    input byteena_b;
    input clock1;
    input clocken0;
    input clocken1;
    input clocken2;
    input clocken3;
    input data_b[8];
    output eccstatus;
    output q_b[8];
    input rden_a;
    input rden_b;
    input wren_b;
}
