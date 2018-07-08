package pt.inescid.mpmadapter;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.List;

import org.junit.After;
import org.junit.BeforeClass;
import org.junit.Test;

import depspace.client.DepSpaceAccessor;
import depspace.general.DepSpaceException;
import depspace.general.DepTuple;

public class DepSpaceAdapterTest {

	private static final String MPM_ADAPTER_HOST = "127.0.0.1";
	private static final int MPM_ADAPTER_PORT = 7892;

	private static final String CONFIG_DIR = "/config/";

	private static DepSpaceAccessor depSpaceAccessor;

	private static Socket clientSocket;
	private static BufferedReader in;
	private static PrintWriter out;
	
	private Object[][] tuples = new Object[][] {{ "device","192.168.1.1"}, {"device","127.0.2.1"},  {"device","10.0.2.4"}};

	
	/*
	@BeforeClass
	public static void setup() throws UnknownHostException, IOException, InterruptedException {
		System.out.println("\n\n\nSetting up...\n\n\n");

		
		//-------------------- Booting the DepSpace Adapter -------------------
		DepSpaceAdapter depSpaceAdapter = new DepSpaceAdapter(MPM_ADAPTER_PORT, CONFIG_DIR);
		Thread depSpaceAdapterThread = new Thread(depSpaceAdapter);
		depSpaceAdapterThread.start();
		System.out.println("DepSpaceAdapter initialized");
		
		
		
		
		//-------------- Starting the Socket to the adapter -------------------
		//TODO: To be fixed
		Thread.sleep(3000);

		// the socket to test the adapter
		clientSocket = new Socket(MPM_ADAPTER_HOST, MPM_ADAPTER_PORT);
		
		OutputStream adapterOutputStream = clientSocket.getOutputStream();
		InputStream adapterInputStream = clientSocket.getInputStream();
		in = new BufferedReader(new InputStreamReader(adapterInputStream));
		out = new PrintWriter(adapterOutputStream);
		
		
		// 5 seconds timout for the read operation.
		//If it takes longer than that to response is because something is wrong.
//		clientSocket.setSoTimeout(5 * 1000);

		// DepSpace client to test
		depSpaceAccessor = DepSpaceAdapter.getDepSpaceAccessor(CONFIG_DIR, 5);
	}

	@After
	public void cleanUp() throws DepSpaceException {
		System.out.println("\n\n\nCleaning tuples...\n\n\n");
		DepTuple depTuple = null;

		// remove every tuple
		for (Object tuple : tuples) {
			depTuple = DepTuple.createTuple(tuple);
			depSpaceAccessor.inp(depTuple);

		}

	}

	@Test
	public void rdpTest() throws IOException, DepSpaceException {
		System.out.println("\n\n\nTestRdp...\n\n\n");
		String command = "rdp " + tuples[0][0] + "," + tuples[0][1];

		// inserting a test tuple before reading it
		DepTuple tuple = DepTuple.createTuple(tuples[0]);
		depSpaceAccessor.out(tuple);

		// Now let's read it using our adapter
		out.println(command);

		String response = in.readLine();
		assertEquals("OK", response);
		
		response = in.readLine();
		assertEquals(response, tuples[0]);
	}
	
	
	

	@Test
	public void outTest() throws IOException, DepSpaceException {
		System.out.println("\n\n\nOutTest...\n\n\n");
		String command = "out " + tuples[0][0] + "," + tuples[0][1];
		
		System.out.println("Sending '" + command + "' to the Adapter");
		//executing command in our adapter
		out.println(command);
		System.out.println("Sent '" + command + "' to the Adapter");
		
		String response = in.readLine();
		System.out.println("OutTestResult " + response);
		assertEquals("OK", response);
		
		//Now let's test if it was successfully executed
		DepTuple tupleWritten = DepTuple.createTuple(tuples[0]);
		DepTuple tuppleRead = depSpaceAccessor.rd(tupleWritten);
		assertEquals(tupleWritten, tuppleRead);
		
	}
	
	
	
	

	@Test
	public void rdAllTest() throws DepSpaceException, IOException {
		System.out.println("\n\n\nrdAllTest...\n\n\n");
		String template = "device,*";
		String command = "rdall " + template;
		
		//Lets insert all the tupples
		DepTuple depTuple = null;
		for (Object tupleString : tuples) {
			depTuple = DepTuple.createTuple(tupleString);
			depSpaceAccessor.out(depTuple);
		}
		
		
		//Now let's read all of them using the template
		out.println(command);

		String response = in.readLine();
		assertEquals("OK", response);
		
		List<Object[]> tuplesList = Arrays.asList(tuples);
			
		while( !(response = in.readLine()).equals("") ){
			 
			System.out.println("Got this tuple: " + response);
			boolean exists = false;
			for (Object[] tuple : tuplesList) {
				String tupleString = tuple[0] + "," + tuple[1];  
				if(tupleString.equals(response)){
					tuplesList.remove(tuple);
					exists = true;
					break;
				}
			}
			assertTrue(exists);
			
		}
		
		assertTrue(tuplesList.isEmpty());
		
	}

//	@Test
//	public void exitTes() {
//		fail("Not yet implemented");
//	}
*/
}
