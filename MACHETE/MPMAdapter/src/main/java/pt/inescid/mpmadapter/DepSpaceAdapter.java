package pt.inescid.mpmadapter;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Collection;
import java.util.Properties;

import depspace.client.DepSpaceAccessor;
import depspace.client.DepSpaceAdmin;
import depspace.general.DepSpaceConfiguration;
import depspace.general.DepSpaceException;
import depspace.general.DepSpaceProperties;
import depspace.general.DepTuple;

//enum Command {
//	rdp, cas, inp, rdall, inall, out, replace, exit;
//}

public class DepSpaceAdapter implements Runnable {

	private int port = 7892;
	private String configDir = "/config/";
	private int clientID = 6;
	private String tupleSpaceName = "Demo Space";

	private ServerSocket serverSocket;

	public DepSpaceAdapter() {
		// use default values
	}

	public DepSpaceAdapter(int proxyPort, String configDir, int clientID, String tupleSpaceName) {
		this.port = proxyPort;
		this.configDir = configDir;
		this.clientID = clientID;
		this.tupleSpaceName = tupleSpaceName;
	}

	public void run() {

		try {
			serverSocket = new ServerSocket(port);
		} catch (IOException ioe) {
			MPMAdapter.logSevere(ioe.getMessage());
			System.exit(-1);
		}
		MPMAdapter.logInfo("MPM Adapter socket listening...");

		DepSpaceAccessor accessor = getDepSpaceAccessor(configDir, clientID);

		BufferedReader in;
		PrintWriter out;

		try {

			while (true) {
				String commandLine, commandArgs;
				// Command command;
				DepTuple template, tuple;
				Socket clientSocket = serverSocket.accept();
				MPMAdapter.logInfo("Accepted connection from: " + clientSocket.getRemoteSocketAddress().toString());
				in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream(), StandardCharsets.UTF_8));
				out = new PrintWriter(new OutputStreamWriter(clientSocket.getOutputStream(), StandardCharsets.UTF_8),
						true);

				while (!clientSocket.isClosed()) {

					MPMAdapter.logInfo("Waiting for a command...");
					commandLine = in.readLine();
					MPMAdapter.logInfo("Received command line: " + commandLine);

					if (commandLine == null) {
						// if null, the end of the stream has been reached
						// i.e. the socket should be closed
						clientSocket.close();
						break;
					}
					String[] commandParts = commandLine.split(" ");
					String command = "";
					try {
						command = commandParts[0].toLowerCase().trim();
						// MPMAdapter.logInfo("This is the command: '"+commandParts[0]+"'");

						// command = Command.valueOf(commandParts[0]);
					} catch (java.lang.IllegalArgumentException e) {
						MPMAdapter.logSevere("Error: " + e.getMessage());
						out.println("Error: " + e.getMessage());
						continue;
					}

					if (commandParts.length > 1) {
						commandArgs = commandLine.split(" ")[1];
					} else {
						commandArgs = null;
					}

					if (command.equals("rdp")) {
						// command to read one tuple
						MPMAdapter.logInfo("Executing a rdp...");
						template = DepTuple.createTuple((Object[]) commandArgs.split(","));
						DepTuple dt;
						try {
							dt = accessor.rdp(template);
						} catch (DepSpaceException e) {
							MPMAdapter.logSevere(e.getMessage());
							out.println("ERROR: " + e.getMessage());
							continue;
						}
						out.println("OK");
						if (dt == null) {
							out.println();
						} else {
							MPMAdapter.logInfo(dt.toStringTuple());
							out.println(dt.toStringTuple());
						}
					} else if (command.equals("out")) {
						// command to write one tuple
						MPMAdapter.logInfo("Executing an out...");
						if (commandArgs == null || commandArgs.trim().isEmpty()) {
							MPMAdapter.logSevere("Invalid number of args. ");
							out.println("ERROR: Invalid number of args");

							continue;
						}
						tuple = DepTuple.createTuple((Object[]) commandArgs.split(","));
						System.out.println("[OUT] tuple=" + tuple);

						try {
							MPMAdapter.logInfo("Sending to the accessor");
							accessor.out(tuple);
							MPMAdapter.logInfo("Sent to the accessor");
						} catch (DepSpaceException e) {
							MPMAdapter.logSevere(e.getMessage());
							out.println("ERROR: " + e.getMessage());

							continue;
						}
						out.println("OK");


					} else if (command.equals("inp")) {
						// command to write one tuple
						MPMAdapter.logInfo("Executing an inp...");
						if (commandArgs == null || commandArgs.trim().isEmpty()) {
							MPMAdapter.logSevere("Invalid number of args. ");
							out.println("ERROR: Invalid number of args");

							continue;
						}
						tuple = DepTuple.createTuple((Object[]) commandArgs.split(","));
						System.out.println("[INP] tuple=" + tuple);

						try {
							MPMAdapter.logInfo("Sending to the accessor");
							accessor.inp(tuple);
							MPMAdapter.logInfo("Sent to the accessor");
						} catch (DepSpaceException e) {
							MPMAdapter.logSevere(e.getMessage());
							out.println("ERROR: " + e.getMessage());

							continue;
						}
						out.println("OK");

					} else if (command.equals("rdall")) {
						// command to read a set of tuples
						MPMAdapter.logInfo("Executing a rdall...");
						Collection<DepTuple> list = null;
						if (commandArgs == null || commandArgs.trim().isEmpty()) {
							MPMAdapter.logInfo("Invalid number of args.");
							out.println("ERROR: Invalid number of args.");
							continue;

						}
						template = DepTuple.createTuple((Object[]) commandArgs.split(","));
						try {
							list = accessor.rdAll(template, 0);
						} catch (DepSpaceException e) {
							MPMAdapter.logSevere(e.getMessage());
							out.println("ERROR: " + e.getMessage());
							continue;
						}

						out.println("OK");
						for (DepTuple tup : list) {
							MPMAdapter.logInfo(tup.toStringTuple());
							out.println(tup.toStringTuple());
						}
						out.println();
					} else if (command.equals("exit")) {
						// command to close the connection
						MPMAdapter.logInfo("Received an exit command. Closing connection...");
						clientSocket.close();
					} else {

						// unknown command, do nothing
						break;

					}
				}
			}
		} catch (

		IOException ioe) {
			MPMAdapter.logSevere("Unexpected IO error " + ioe.getMessage());
			ioe.printStackTrace();
			System.exit(-1);
		}

	}

	public DepSpaceAccessor getDepSpaceAccessor(String configDir, int clientID) {
		boolean create = false;

		String currentDir = System.getProperty("user.dir");

		configDir = currentDir + configDir;
		MPMAdapter.logInfo("Config dir " + configDir);

		DepSpaceConfiguration.init(configDir);

		Properties prop = DepSpaceProperties.createDefaultProperties(this.tupleSpaceName);

		DepSpaceAccessor accessor = null;
		if (create) {
			try {
				accessor = new DepSpaceAdmin(clientID).createSpace(prop);
			} catch (DepSpaceException e) {
				MPMAdapter.logSevere(e.getMessage());
				System.exit(-1);
			}
		} else {
			try {
				accessor = new DepSpaceAdmin(clientID).createAccessor(prop, create);
			} catch (DepSpaceException e) {
				MPMAdapter.logSevere(e.getMessage());
				System.exit(-1);
			}
		}

		MPMAdapter.logInfo("Passed depspace access");

		return accessor;
	}

	public void stop() {
		if (this.serverSocket == null) {
			return;
		}
		try {
			this.serverSocket.close();
		} catch (IOException e) {
			// do nothing...
		}

	}

}
