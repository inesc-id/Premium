# imports
import sys, string, os
from log_file_constants import *
from utils import *

# Global Variables


# Functions

def convert_exp_to_csv(is_short, filename, max_experiments, output_csv_file):
    csv_line = ""
    current_exp = 1

    log_file_lines = read_file_into_lines(filename)

    for line in log_file_lines:

        if is_short:
            csv_line, current_exp = do_short_extraction(csv_line, current_exp, line, output_csv_file)
        else:
            csv_line, current_exp = do_long_extraction(csv_line, current_exp, line, output_csv_file)

        if current_exp > max_experiments:
            break

            # testing this program
            # csv_line = line
            # print csv_line
            # break

    return


def do_short_extraction(csv_line, current_exp, line, output_csv_file):
    if INIT_EXPERIMENT in line:
        csv_line += str(current_exp) + ","
    if TOTAL_SETUP_TIME in line:
        csv_line += extract_time_value_from_line(line) + ","
    if TOTAL_CONNECTION_TIME in line:
        csv_line += extract_time_value_from_line(line) + ","
    if TOTAL_TEARDOWN_TIME in line:
        csv_line += extract_time_value_from_line(line)
    if END_EXPERIMENT in line:
        output_csv_file.write(csv_line + "\n")
        current_exp += 1
        csv_line = ""
    return csv_line, current_exp


def do_long_extraction(csv_line, current_exp, line, output_csv_file):
    # is setup or is teardown
    # is_setup = True

    if INIT_EXPERIMENT in line:
        csv_line += str(current_exp) + ","

    # Setup .......................................................

    setup_str_list = [GET_NODES_FROM_MANAGER_TIME, PROBE_NODES_TIME, GET_RECEIVER_INFO_TIME,
                      TOTAL_SETUP_ALL_FLOWS_TIME, TOTAL_SETUP_TIME]
    if any(str_marker in line for str_marker in setup_str_list):
        csv_line += extract_time_value_from_line(line) + ","

    if SETUP_FOR_NODE_IP in line:
        csv_line += extract_ip_value_from_line(line) + ","

    setup_node_str_list = [SETUP_SENDER_FORWARD_RULES, SETUP_NODE_FORWARD_RULES, SETUP_NODE_DARSHANA_TIME,
                           SETUP_NODE_TIME, SETUP_SENDER_PM_TIME]
    if any(str_marker in line for str_marker in setup_node_str_list):
        csv_line += extract_time_value_from_line(line) + ","

    # Connection ..................................................

    if TOTAL_CONNECTION_TIME in line:
        csv_line += extract_time_value_from_line(line) + ","

    # Teardown ....................................................

    teardown_str_list = [TOTAL_TEARDOWN_NODES_TIME, TOTAL_TEARDOWN_DEVICE_TIME]
    if any(str_marker in line for str_marker in teardown_str_list):
        csv_line += extract_time_value_from_line(line) + ","
    if TOTAL_TEARDOWN_TIME in line:
        csv_line += extract_time_value_from_line(line) # Last value of logging experiment

    if END_EXPERIMENT in line:
        output_csv_file.write(csv_line + "\n")
        current_exp += 1
        csv_line = ""
    return csv_line, current_exp



def main():
    # Verifying number of arguments
    if len(sys.argv) < 3:
        print "-> Ending execution ..."
        return

    # Number of successful experiments
    number_of_experiments = sys.argv[1]

    # Number of Overlay Nodes
    number_of_overlay_nodes = sys.argv[2]

    # Log filename
    log_filename = sys.argv[3]

    # Check if file exists
    if not (os.path.isfile(log_filename)):
        print "-> File " + log_filename + " does not exist!"
        print "-> Ending execution ..."
        return

    # Real Main --------------------------
    is_short = True
    csv_short_file = create_csv_file(is_short, number_of_overlay_nodes)
    convert_exp_to_csv(is_short, log_filename, number_of_experiments, csv_short_file)
    csv_short_file.close()
    print "-> Done generating short csv file ..."

    is_long = False
    csv_long_file = create_csv_file(is_long, number_of_overlay_nodes)
    convert_exp_to_csv(is_long, log_filename, number_of_experiments, csv_long_file)
    csv_long_file.close()
    print "-> Done generating long csv file ..."


# Main runs
main()
