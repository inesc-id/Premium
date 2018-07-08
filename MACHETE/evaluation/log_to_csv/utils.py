from log_file_constants import *

# parsers

def extract_time_value_from_line(line):
    return line.split(LOG_VALUE_SEPARATOR)[1].split(LOG_VALUE_UNIT)[0]


def extract_ip_value_from_line(line):
    return line.split(SETUP_FOR_NODE_IP)[1].split(LOG_LINE_BREAK)[0]


def create_csv_file(is_short, num_overlay_nodes):
    if is_short:
        filename = build_filename(short_csv_name, num_overlay_nodes)
        header = build_short_csv_header()
    else:
        filename = build_filename(long_csv_name, num_overlay_nodes)
        header = build_long_csv_header(num_overlay_nodes)

    csv_file = open(filename, "w+")
    csv_file.write(header + "\n")

    csv_extraction_type = "short" if is_short else "long"
    print "-> Created " + csv_extraction_type + " csv file ..."
    return csv_file


def build_filename(init_filename, n_nodes):
    return init_filename + "_" + n_nodes + "_nodes" + ".csv"


def read_file_into_lines(filename):
    f = open(filename, 'r')
    data = f.readlines()
    f.close()
    return data


def build_long_csv_header(num_nodes):

    setup_nodes_sub_header = ""
    teardown_nodes_sub_header = ""
    number_overlay_nodes = int(num_nodes)

    for i in range(number_overlay_nodes):
        setup_nodes_sub_header += "Node IP, Sender Fwd rules, Node Fwd rules, Node Darshana rules, Setup Node, Sender Path Monitor, "

    for i in range(number_overlay_nodes):
        teardown_nodes_sub_header += "Node Fwd rules, Node Darshana rules, "

    return csv_header_long_init + setup_nodes_sub_header + csv_header_long_middle + teardown_nodes_sub_header + csv_header_long_end


def build_short_csv_header():
    return csv_header_short

