package pt.inescid.mpmadapter;

import java.io.File;
import java.util.logging.Logger;

public class MPMAdapter {

	private static Logger logger = Logger.getLogger(MPMAdapter.class.getName());
	public static final String MPM_ADAPTER_IP = "127.0.0.2";

	/**
	 * 
	 * @param args
	 *            <mpmAdapterPort> <configDir> <clientID> <tupleSpaceName>
	 */
	public static void main(String[] args) {
		checkArgs(args);
		int port = Integer.parseInt(args[0]);
		String configDir = args[1];
		int clientID = Integer.parseInt(args[2]);
		String tupleSpaceName = args[3];

		DepSpaceAdapter depSpaceAdapter = new DepSpaceAdapter(port, configDir, clientID, tupleSpaceName);
		Thread depSpaceAdapterThread = new Thread(depSpaceAdapter);
		depSpaceAdapterThread.run();
	}

	private static void checkArgs(String[] args) {
		if (args.length < 2) {
			logger.severe("Usage: MPMAdapter <mpmAdapterPort> <configDir>");
			System.exit(-1);
		}

		try {
			Integer.parseInt(args[0]);
		} catch (Exception e) {
			logger.severe("Port number invalid (" + args[0] + ")");
			System.exit(-1);
		}

		String currentDir = System.getProperty("user.dir");
		String configDir = args[1];
		if (!new File(currentDir + configDir).exists()) {
			logger.severe("Config dir (" + currentDir + configDir + ") does not exist");
			System.exit(-1);
		}

		if (!new File(currentDir + configDir).isDirectory()) {
			logger.severe("Config dir (" + currentDir + configDir + ") is not a directory");
			System.exit(-1);
		}

	}

	public static void logInfo(String message) {
		logger.info("[MPM Adapter] " + message);
	}

	public static void logSevere(String message) {
		logger.severe("[MPM Adapter] " + message);
	}
}
