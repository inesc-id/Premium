#ifndef COMMON_H_
#define COMMON_H_

// Inclue common includes here

#include <sys/time.h>

// options = [full, lat, hop, path, prop]
#define DARSHANA_MONITORING_MODE "lat"
// <lat> <hop> <path> <prop> <path-nones> <probe-period>
#define DARSHANA_METRICS_THRESHOLDS "1 1 0.8 1.5 0.3 1"
#define DARSHANA_RELATIVE_PATH "../../darshana/darshana/"
#define DARSHANA_SERVER_PORT "11130" // HARDCODED values

#define DAR_PORT 10101 // HARDCODED values

#define PORT_SIZE 10
#define RULE_SIZE 200
#define IP_ADDR_LENGTH 20

/* Maximum numebr of flows, this also limits:
 * - the maximum number of overlay nodes to use
 * - maximum number of Source and Destination IPs
 */
#define MAX_FLOWS 20

#endif /* COMMON_H_ */