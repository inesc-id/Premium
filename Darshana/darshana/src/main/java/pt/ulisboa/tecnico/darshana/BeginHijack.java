package pt.ulisboa.tecnico.darshana;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;

import pt.ulisboa.tecnico.darshana.structures.Structures.Latencies;
import pt.ulisboa.tecnico.darshana.utils.LogManager;
import pt.ulisboa.tecnico.darshana.utils.Utils;
import pt.ulisboa.tecnico.darshana.structures.Structures.Route;

public class BeginHijack {

	private static MoniTools tc = new MoniTools();
	private static List<String> time = new ArrayList<String>();
	private static String _myIP = "";
	private static String _destinationIp = "";
	private static String _destinationPort = "";
	public static int HIJACKER_PORT = 9091;

	private static void tracerouteServer(String address) throws UnknownHostException, IOException, InterruptedException{
		
//		InetAddress inetAddress = InetAddress.getByName(address);

		@SuppressWarnings("resource")
		ServerSocket listener = new ServerSocket(HIJACKER_PORT);
//		ServerSocket listener = new ServerSocket(HIJACKER_PORT, 0, inetAddress);

		while (true) {
			
			Socket sock = listener.accept();
			
			PrintWriter out = new PrintWriter(sock.getOutputStream(), true);

			// Getting Lat, Hop and Path values from hijacker -> destination
			Route hijackedRoute = tc.runTraceroute(_destinationIp, 0);
			Latencies hijackedPing = tc.runPing(_myIP, _destinationIp, _destinationPort);
			
			String hijackedPingStr = hijackedPing.getRtt().toString() + "-" + tc.getHopCount().toString();

			// Sending to Client/Source the Lat and Hop values from hijacker -> destination
			out.println(hijackedPingStr);

			// Sending to Client/Source the Path values from hijacker -> destination
			for (String c : hijackedRoute.getASes()) {
				out.println(c);
			}

			LogManager.print("Finished Sending Lat, Hop and Path values from hijacker -> destination...");
			
			sock.close();
		}
	}


	public static void main(String[] args){
	
		String addressToUse = args[0];  // address to use when creating a socket
		String clientAddress = args[1]; // the client/source to be hijacked

		Socket s;

		try {

			if (Utils.isPublicIp(addressToUse)) {
				_myIP = Utils.runCommand(Utils.GET_PUBLIC_IP_COMMAND).get(0);
			} else {
				_myIP = addressToUse;
			}

			s = new Socket(clientAddress, SimHijack.HIJACKER_SIMULATOR_PORT);

			// Receive from Client/Source the destination's IP and port
			BufferedReader input = new BufferedReader(new InputStreamReader(s.getInputStream()));
			_destinationIp = input.readLine();
			_destinationPort = input.readLine();

			LogManager.print("Got destination IP   : " + _destinationIp);
			LogManager.print("Got destination port : " + _destinationPort);

			PrintWriter out = new PrintWriter(s.getOutputStream(), true);
			out.println(_myIP);
			
			s.close();
			tracerouteServer(_myIP);

		} catch(Exception e) {
			System.out.println(e.getMessage());
			e.printStackTrace();
		}
		
	}
}
