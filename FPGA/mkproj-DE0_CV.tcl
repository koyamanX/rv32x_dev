#!quartus_sh -t
#
# mkproj-DE0
#
# 2013-07-10 Created for DE0 board

package require cmdline
set options {
	{ "project.arg" "" "project name" }
	{ "script.arg" "" "script file" }
}
set usage "\[options\] \[files...\]"
array set opt [::cmdline::getoptions ::argv $options $usage]

if {$opt(project) == ""} {
	return -code error "Usage: $argv0 $usage"
}

########

project_new $opt(project) -overwrite

foreach arg $argv {
	set_global_assignment -name VERILOG_FILE $arg
}

if {$opt(script) != ""} {
	set_global_assignment -name PRE_FLOW_SCRIPT_FILE "quartus_sh:$opt(script)"
}

########
	
set_global_assignment -name NUM_PARALLEL_PROCESSORS 12
set_global_assignment -name FMAX_REQUIREMENT 50MHz
create_base_clock -fmax 50MHz -duty_cycle 50 m_clock

set_global_assignment -name TOP_LEVEL_ENTITY top_bram	

#============================================================
# Build by Terasic System Builder V1.0.0
#============================================================
set_global_assignment -name USE_GENERATED_PHYSICAL_CONSTRAINTS OFF -section_id eda_palace
set_global_assignment -name NOMINAL_CORE_SUPPLY_VOLTAGE 1.2V
set_global_assignment -name LL_ROOT_REGION ON -section_id "Root Region"
set_global_assignment -name LL_MEMBER_STATE LOCKED -section_id "Root Region"
#============================================================
# Build by Terasic V1.0.0
#============================================================


set_global_assignment -name FAMILY "Cyclone V"
set_global_assignment -name DEVICE 5CEBA4F23C7

set_global_assignment -name PARTITION_NETLIST_TYPE SOURCE -section_id Top
set_global_assignment -name PARTITION_FITTER_PRESERVATION_LEVEL PLACEMENT_AND_ROUTING -section_id Top
set_global_assignment -name PARTITION_COLOR 16764057 -section_id Top
set_global_assignment -name STRATIX_DEVICE_IO_STANDARD "2.5 V"

set_global_assignment -name OPTIMIZATION_TECHNIQUE SPEED
set_global_assignment -name SYNTH_TIMING_DRIVEN_SYNTHESIS ON
set_global_assignment -name OPTIMIZE_HOLD_TIMING "ALL PATHS"
set_global_assignment -name OPTIMIZE_MULTI_CORNER_TIMING ON
set_global_assignment -name FITTER_EFFORT "STANDARD FIT"
set_global_assignment -name STRATIXV_CONFIGURATION_SCHEME "ACTIVE SERIAL X1"
set_global_assignment -name USE_CONFIGURATION_DEVICE ON
set_global_assignment -name STRATIXII_CONFIGURATION_DEVICE EPCS64
set_global_assignment -name CRC_ERROR_OPEN_DRAIN ON
set_global_assignment -name OUTPUT_IO_TIMING_NEAR_END_VMEAS "HALF VCCIO" -rise
set_global_assignment -name OUTPUT_IO_TIMING_NEAR_END_VMEAS "HALF VCCIO" -fall
set_global_assignment -name OUTPUT_IO_TIMING_FAR_END_VMEAS "HALF SIGNAL SWING" -rise
set_global_assignment -name OUTPUT_IO_TIMING_FAR_END_VMEAS "HALF SIGNAL SWING" -fall
set_global_assignment -name ACTIVE_SERIAL_CLOCK FREQ_100MHZ
set_instance_assignment -name PARTITION_HIERARCHY root_partition -to | -section_id Top

######## Pin assignemnt

set_global_assignment -name ENABLE_ADVANCED_IO_TIMING ON
set_global_assignment -name FORCE_CONFIGURATION_VCCIO ON
set_global_assignment -name CYCLONEII_RESERVE_NCEO_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name CYCLONEII_RESERVE_NCEO_AFTER_CONFIGURATION "USE AS REGULAR IO"

set_location_assignment PIN_P22 -to p_reset
set_location_assignment PIN_M9 -to m_clock
set_location_assignment PIN_H11 -to SCLK
set_location_assignment PIN_B11 -to MOSI
set_location_assignment PIN_K9 -to MISO
set_location_assignment PIN_C11 -to CS

set_location_assignment PIN_AA2 -to LEDR[0]
set_location_assignment PIN_AA1 -to LEDR[1]
set_location_assignment PIN_W2 -to LEDR[2]
set_location_assignment PIN_Y3 -to LEDR[3]
set_location_assignment PIN_N2 -to LEDR[4]
set_location_assignment PIN_N1 -to LEDR[5]
set_location_assignment PIN_U2 -to LEDR[6]
set_location_assignment PIN_U1 -to LEDR[7]
set_location_assignment PIN_L2 -to LEDR[8]
set_location_assignment PIN_L1 -to LEDR[9]

set_location_assignment PIN_U21 -to HEX0[0]
set_location_assignment PIN_V21 -to HEX0[1]
set_location_assignment PIN_W22 -to HEX0[2]
set_location_assignment PIN_W21 -to HEX0[3]
set_location_assignment PIN_Y22 -to HEX0[4]
set_location_assignment PIN_Y21 -to HEX0[5]
set_location_assignment PIN_AA22 -to HEX0[6]
set_location_assignment PIN_AA20 -to HEX1[0]
set_location_assignment PIN_AB20 -to HEX1[1]
set_location_assignment PIN_AA19 -to HEX1[2]
set_location_assignment PIN_AA18 -to HEX1[3]
set_location_assignment PIN_AB18 -to HEX1[4]
set_location_assignment PIN_AA17 -to HEX1[5]
set_location_assignment PIN_U22 -to HEX1[6]
set_location_assignment PIN_Y19 -to HEX2[0]
set_location_assignment PIN_AB17 -to HEX2[1]
set_location_assignment PIN_AA10 -to HEX2[2]
set_location_assignment PIN_Y14 -to HEX2[3]
set_location_assignment PIN_V14 -to HEX2[4]
set_location_assignment PIN_AB22 -to HEX2[5]
set_location_assignment PIN_AB21 -to HEX2[6]
set_location_assignment PIN_Y16 -to HEX3[0]
set_location_assignment PIN_W16 -to HEX3[1]
set_location_assignment PIN_Y17 -to HEX3[2]
set_location_assignment PIN_V16 -to HEX3[3]
set_location_assignment PIN_U17 -to HEX3[4]
set_location_assignment PIN_V18 -to HEX3[5]
set_location_assignment PIN_V19 -to HEX3[6]
set_location_assignment PIN_U20 -to HEX4[0]
set_location_assignment PIN_Y20 -to HEX4[1]
set_location_assignment PIN_V20 -to HEX4[2]
set_location_assignment PIN_U16 -to HEX4[3]
set_location_assignment PIN_U15 -to HEX4[4]
set_location_assignment PIN_Y15 -to HEX4[5]
set_location_assignment PIN_P9 -to HEX4[6]
set_location_assignment PIN_N9 -to HEX5[0]
set_location_assignment PIN_M8 -to HEX5[1]
set_location_assignment PIN_T14 -to HEX5[2]
set_location_assignment PIN_P14 -to HEX5[3]
set_location_assignment PIN_C1 -to HEX5[4]
set_location_assignment PIN_C2 -to HEX5[5]
set_location_assignment PIN_W19 -to HEX5[6]

set_location_assignment PIN_U13 -to SW[0]
set_location_assignment PIN_V13 -to SW[1]
set_location_assignment PIN_T13 -to SW[2]
set_location_assignment PIN_T12 -to SW[3]
set_location_assignment PIN_AA15 -to SW[4]
set_location_assignment PIN_AB15 -to SW[5]
set_location_assignment PIN_AA14 -to SW[6]
set_location_assignment PIN_AA13 -to SW[7]
set_location_assignment PIN_AB13 -to SW[8]
set_location_assignment PIN_AB12 -to SW[9]
#============================================================

set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to m_clock
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to p_reset
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[7]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[8]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LEDR[9]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SCLK
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to MISO
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to MOSI
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to CS
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to MOSI
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to MISO
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to CS

#============================================================
# SW
#============================================================
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[7]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[8]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SW[9]

#============================================================
# HEX0
#============================================================
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX0[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX0[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX0[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX0[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX0[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX0[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX0[6]

#============================================================
# HEX1
#============================================================
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX1[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX1[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX1[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX1[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX1[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX1[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX1[6]

#============================================================
# HEX2
#============================================================
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX2[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX2[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX2[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX2[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX2[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX2[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX2[6]

#============================================================
# HEX3
#============================================================
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[6]

#============================================================
# HEX4
#============================================================
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[6]

#============================================================
# HEX5
#============================================================
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[6]

project_close
