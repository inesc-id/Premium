package pt.ulisboa.tecnico.darshana;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.List;
import java.util.ArrayList;
import pt.ulisboa.tecnico.darshana.utils.LogManager;
import pt.ulisboa.tecnico.darshana.utils.Utils;
import pt.ulisboa.tecnico.darshana.structures.Structures;

public class SimHijack implements Runnable {
	
	private String _sourceIp;
	private String _destinationIp;
	private String _destinationPort;
	private static String _hijackerIP;
	public static int HIJACKER_SIMULATOR_PORT = 9090;

	public SimHijack(String sourceIp, String destinationIp, String destinationPort){
		_sourceIp = sourceIp;
		_destinationIp = destinationIp;
		_destinationPort = destinationPort;
	}

	/**
	 * Getting ready to exchange data with receiver
	 * This data includes destination's IP and port,
	 * so that the Hijacker can measure Lat, Hop and Path to the destination
	 *
	 * @param sourceIp ip of the source
	 * @param destinationIp ip of the destination
	 * @param destinationPort port where the destination is listening for ping requests
	 */
	public void rcvConnect(String sourceIp, String destinationIp, String destinationPort){

		InetAddress inetAddress;

		try {

			inetAddress = InetAddress.getByName(sourceIp);

			ServerSocket listener = new ServerSocket(HIJACKER_SIMULATOR_PORT, 0, inetAddress);
			Socket s = listener.accept();

			// Sending destination's IP and port to the hijacker
			PrintWriter out = new PrintWriter(s.getOutputStream(), true);
			out.println(destinationIp);
			out.println(destinationPort);

			// Receiving hijacker's IP from the hijacker
			BufferedReader input = new BufferedReader(new InputStreamReader(s.getInputStream()));
			_hijackerIP = input.readLine();

			// Signal monitor of the simulated hijacker's presence
			RouteMon.hijacker = _hijackerIP;
			RouteMon.hijack = 1;
			
			listener.close();

		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}

	/**
	 * Receiving data from Hijacker
	 * This data includes hijacked measures from Lat, Hop and Path, measured from hijacker -> destination
	 *
	 * @throws IOException
	 */
	public static void rcvHijacked() throws IOException {
		
		List<String> hijackedMeasures = new ArrayList<String>();
		String measure;
		Double rtt = (double) 0;
		Double hopCount = (double) 0;
		
		Socket socket = new Socket(_hijackerIP, BeginHijack.HIJACKER_PORT);
		BufferedReader input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
		LogManager.print("Hey Im mr hijacker look at me !");

		// Receiving hijacked measures from hijacker
		while((measure = input.readLine()) != null){
        	hijackedMeasures.add(measure);
			LogManager.print(measure);
        }
		LogManager.print("Finished receiving");

		try	{
			// Getting and setting first measure which has format "Lat-Hop"
			String[] measures = hijackedMeasures.get(0).split("[-]");
			rtt = Double.parseDouble(measures[0]);
			hopCount = Double.parseDouble(measures[1]);

			// Remove first measure "Lat-Hop"
			hijackedMeasures.remove(0);
			// Remove first as from the hijacked route
	//        hijackedMeasures.remove(0);

			// Save hijacked Lat and Hop
			RouteMon.hijackedObj = new Structures.HijackedObj(rtt,hopCount);
			// Save hijacked Path
			RouteMon.hijackedPath = hijackedMeasures;

		} catch (IndexOutOfBoundsException e) {
			LogManager.print("I guess Hijacker stop responding...");
		}
        socket.close();
	}

	@Override
	public void run() {
		rcvConnect(_sourceIp, _destinationIp, _destinationPort);
	}
}
