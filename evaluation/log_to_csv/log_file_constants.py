
# CSV Header
csv_header_short = "Exp, Setup, Connection, Teardown"

csv_header_long_init = 'Exp, Get Nodes From Manager, Probe Nodes, Get Receiver Info,'
csv_header_long_middle = 'Setup all flows, Setup, Connection,'
csv_header_long_end = 'Teardown Nodes, Teardown Device, Teardown'

# CSV Filename
short_csv_name = "eval_short"
long_csv_name = "eval_long"

# Log file specific markers

INIT_EXPERIMENT = "Begin logging PREMIUM"
END_EXPERIMENT = "Ending logging PREMIUM"

# Node specific log markers
"""
-> Setup sender forward rules time = 4 ms
-> Setup node's forward rules time = 1 ms
-> Setup node's darshana time = 352 ms
-> Setup nodes time = 354 ms
-> Setup sender path monitoring time = 1 ms
"""
SETUP_FOR_NODE_IP = "Setup for node "

SETUP_SENDER_FORWARD_RULES = "Setup sender forward rules time"
SETUP_NODE_FORWARD_RULES = "Setup node's forward rules time"
SETUP_NODE_DARSHANA_TIME = "Setup node's darshana time"
SETUP_NODE_TIME = "Setup nodes time"
SETUP_SENDER_PM_TIME = "Setup sender path monitoring time"


# Separators
LOG_VALUE_SEPARATOR = " = "
LOG_VALUE_UNIT = " ms"
LOG_LINE_BREAK = "\n"


# Setup Constants

SETUP_PHASE = "Setup Phase"
TOTAL_SETUP_TIME = "Setup time"
TOTAL_SETUP_ALL_FLOWS_TIME = "Setup all flows time"

## Init Setup

"""
-> Getting Overlay Nodes from Manager time = 181 ms
-> ### Number of hops returned from Manager = 2
-> Probing Overlay Nodes time = 1001 ms
-> ### Number of responsive hops = 2
-> Getting Receiver's info time = 59 ms
"""

GET_NODES_FROM_MANAGER_TIME = "Getting Overlay Nodes from Manager time"
PROBE_NODES_TIME = "Probing Overlay Nodes time"
GET_RECEIVER_INFO_TIME = "Getting Receiver's info time"


# Connection Constants

CONNECTION_PHASE = "Connection Phase"
TOTAL_CONNECTION_TIME = "Connection duration time"

# Teardown Constants

TEARDOWN_PHASE = "Teardown Phase"
TOTAL_TEARDOWN_NODES_TIME = "Teardown nodes time"
TOTAL_TEARDOWN_DEVICE_TIME = "Teardown device time"
TOTAL_TEARDOWN_TIME = "Teardown time"

