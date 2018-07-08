#ifndef PREMIUM_SPECS_H_
#define PREMIUM_SPECS_H_

typedef enum {
	// 1. closes communication socket
	// 2. teardown machete
	SHUTDOWN_COMMUNICATION,

	// 1. closes communication socket
	// 2. teardown path
	// 3. setup path
	// 4. starts communication socket
	SHUTDOWN_COMMUNICATION_AND_STARTS_AGAIN_WOUT_BAD_PATH,

	// 1. shuts down compromised path
	// 2. continues communication using the
	//    other established paths 
	REUSE_PATHS_LEFT, 

	// 1. shuts down compromised path
	// 2. continues the sending the data 
	//    from the compromised path, through a new uncompromised path
	CREATE_NEW_PATH, 

	// 1. Does nothing just let it slide
	DO_NOTHING_AND_IGNORE_ALERTS
	
} machete_reaction;

#endif /* PREMIUM_SPECS_H_ */