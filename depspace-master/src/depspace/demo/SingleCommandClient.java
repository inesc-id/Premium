/*
 * ClientTest.java
 *
 * Created on 15 de Maio de 2006, 15:20
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package depspace.demo;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.Properties;
import java.util.Scanner;

import depspace.client.DepSpaceAccessor;
import depspace.client.DepSpaceAdmin;
import depspace.general.DepSpaceConfiguration;
import depspace.general.DepSpaceException;
import depspace.general.DepSpaceProperties;
import depspace.general.DepTuple;

import java.net.*;
import java.security.InvalidKeyException;
import java.io.*;

/**
 * 
 * @author edualchieri
 */
public class SingleCommandClient {

	private int executions;
	private static boolean DEBUG = true;
	private static int LENGTH = 512;

	public void run() {
		try {
			System.out.println("THE END!!!");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public DepTuple get(int i) {
		return DepTuple.createTuple(i, "confidentiality", "I'm the client",
				"BUMMMM!!!");
	}

	private void access(DepSpaceAccessor accessor) throws Exception {
		System.out.println("Using the tuple: " + get(0));
		for (int i = 0; i < executions; i++) {
			System.out.println("Sending " + i);
			DepTuple template = DepTuple.createTuple(i, "*", "*", "*");
			System.out.println("RDP: " + accessor.rdp(template));
		}
		synchronized (this) {
			try {
				this.wait(500);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	public static void main(String[] args) throws DepSpaceException, InvalidKeyException, FileNotFoundException, IllegalArgumentException {
		int exec = 1;
		boolean create = false;
		//int clientID = (int) (new Date().getTime()/1000);
		int clientID = 1030;
		String configDir = args[0];
		String name = "Demo Space";
		String currentDir = System.getProperty("user.dir");
		if(DEBUG) System.out.println("Current dir " + currentDir);
		configDir = currentDir + configDir;
		DepSpaceConfiguration.init(configDir);
		Properties prop = DepSpaceProperties.createDefaultProperties(name);
		//prop.put(DepSpaceProperties.DPS_CONFIDEALITY, "false");
		DepSpaceAccessor accessor = null;
		if (create) {
			accessor = new DepSpaceAdmin(clientID).createSpace(prop);
		} else {
			accessor = new DepSpaceAdmin(clientID).createAccessor(prop, create);
		}

		if(DEBUG)System.out.println("Passed depspace access");
		ServerSocket localSock;
		BufferedReader inFromC;
		DataOutputStream outFromDep;

		try {
			localSock = new ServerSocket(7892, LENGTH, InetAddress.getByName("127.0.0.2"));
			while(true){
				String strFromC = new String("UTF-8"), commStr = new String("UTF-8");
				Command command;
				DepTuple template, tuple;
				byte[] data;
				String response = new String("UTF-8");
				System.out.println("Waiting for another socket");
				Socket cSock = localSock.accept();
				inFromC = new BufferedReader(new InputStreamReader (cSock.getInputStream()));
				outFromDep = new DataOutputStream(cSock.getOutputStream());
				while(!cSock.isClosed()){
					System.out.println("Waiting to read");
					strFromC = inFromC.readLine();
					if(DEBUG) System.out.println("strFromC " + strFromC.split(" ")[0] + "-" + strFromC.split(" ")[1]);
					command = Command.valueOf(strFromC.split(" ")[0]);
					commStr = strFromC.split(" ")[1];
					switch(command){
					case rdp:
						template = DepTuple.createTuple(commStr.split(","));
						DepTuple dt = accessor.rdp(template);
						if(dt==null){
							data = "null\0".getBytes("UTF-8");
							outFromDep.write(data);
						}
						else{
							if(DEBUG)System.out.println(dt.toStringTuple());
							response = dt.toStringTuple() + "\0";
							data = response.getBytes("UTF-8");
							outFromDep.write(data);
						}
						if(!commStr.split(",")[0].equals("device")){
							cSock.close();
						}
						break;
					/*case cas:
						if(commStr.length()<3){
							if(DEBUG)System.out.println("Invalid number of args. ");
							continue;
						}
						template = DepTuple.createTuple(commStr.split(","));
						tuple = DepTuple.createTuple(strFromC.split(" ")[2].split(","));
						DepTuple res = accessor.cas(template, tuple);
						if(res==null){
							data = "null\0".getBytes("UTF-8");
							outFromDep.write(data);
						}
						else{
							if(DEBUG)System.out.println(res.toStringTuple());
							response = res.toStringTuple() + "\0";
							data = response.getBytes("UTF-8");
							outFromDep.write(data);
						}
						break;
					case inall:
						if(commStr.length()<2){
							if(DEBUG)System.out.println("Invalid number of args. ");
							continue;
						}
						template = DepTuple.createTuple(commStr.split(","));
						response = "";
						Collection<DepTuple> l = accessor.inAll(template);
						for(DepTuple t : l)
						{
							if(DEBUG)System.out.println(t.toStringTuple());
							response += t.toStringTuple() + " ";
						}
						response += "\0";
						data = response.getBytes("UTF-8");
						outFromDep.write(data);
						break;
					case inp:
						if(commStr.length()<2){
							if(DEBUG)System.out.println("Invalid number of args. ");
							continue;
						}
						template = DepTuple.createTuple(commStr.split(","));
						DepTuple t = accessor.inp(template);
						if(t==null){
							if(DEBUG)System.out.println("null");
							data = "null\0".getBytes("UTF-8");
							outFromDep.write(data);
						}
						else{
							if(DEBUG)System.out.println(t.toStringTuple());
							response = t.toStringTuple() + "\0";
							data = response.getBytes("UTF-8");
							outFromDep.write(data);
						}
						break;*/
					case out:
						if(commStr.length()<2){
							if(DEBUG)System.out.println("Invalid number of args. ");
							continue;
						}
						tuple = DepTuple.createTuple(commStr.split(","));
						accessor.out(tuple);
						if(!commStr.split(",")[0].equals("device")){
							cSock.close();
						}
						break;
					case rdall:
						Collection<DepTuple> list=null;
						if(commStr.length()>3){
							template = DepTuple.createTuple(commStr.split(","));
							list = accessor.rdAll(template, 0);
						}else{
							if(DEBUG)System.out.println("1Invalid number of args. ");
							continue;
						}
						response = "";
						for(DepTuple tup : list){
							if(DEBUG)System.out.println(tup.toStringTuple());
							response = tup.toStringTuple() + ";";
							data = response.getBytes("UTF-8");
							outFromDep.write(data);
						}
						cSock.close();
						break;
					/*case replace:
						if(commStr.length()<3){
							if(DEBUG)System.out.println("Invalid number of args. ");
							continue;
						}
						template = DepTuple.createTuple(commStr.split(","));
						tuple = DepTuple.createTuple(strFromC.split(" ")[2].split(","));
						DepTuple r = accessor.replace(template, tuple);
						if(r==null){
							if(DEBUG)System.out.println("null");
							data = "null\0".getBytes("UTF-8");
							outFromDep.write(data);
						}
						else{
							if(DEBUG)System.out.println(r.toStringTuple());
							response = r.toStringTuple() + "\0";
							data = response.getBytes("UTF-8");
							outFromDep.write(data);
						}
						break;
					case exit:
						continue;*/
					}
				}
			}
		} catch (IOException e1) {
			e1.printStackTrace();
		} 
	}
}

enum Command{
	rdp, cas, inp, rdall, inall, out, replace, exit;
}
