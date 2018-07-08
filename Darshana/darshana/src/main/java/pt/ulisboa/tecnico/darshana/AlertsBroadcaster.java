package pt.ulisboa.tecnico.darshana;

import pt.ulisboa.tecnico.darshana.structures.Structures;
import pt.ulisboa.tecnico.darshana.utils.LogManager;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.*;

/**
 * Created by isabelcosta on 06-Nov-17.
 *
 * DAR stands for Darshana Alerts Receiver
 *
 * This class is responsible to connect to DAR remote machine to send alert messages
 * indicating that a path was possibly hijacked
 * The protocol to be used is UDP
 */
public class AlertsBroadcaster {

    private String _darIp;
    private Integer _darPort;
    private DatagramSocket _clientSocket;

    public AlertsBroadcaster (String darIp, Integer darPort) {
        this._darIp = darIp;
        this._darPort = darPort;

        try {
            this._clientSocket = new DatagramSocket();
        } catch (SocketException e) {
            e.printStackTrace();
        }
    }

    public static String getAlertFormatted(String alert, String sourceIp, String destinationIp, String metric) {
        return alert + "," + sourceIp + "," + destinationIp + "," + metric;
    }

    public void sendAlertToRemoteDAR(String alertMessage){

        BufferedReader inFromUser = new BufferedReader(new InputStreamReader(System.in));

        try {
            byte[] sendData = new byte[1024];
//        byte[] receiveData = new byte[1024];

            sendData = alertMessage.getBytes();

            InetAddress ipAddress = null;
            ipAddress = InetAddress.getByName(_darIp);
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, ipAddress, _darPort);
            _clientSocket.send(sendPacket);

        } catch (UnknownHostException e) {
            e.printStackTrace();
            LogManager.print("|ERROR| Can't recognize source Ip !!!!");

        } catch (IOException e) {
            e.printStackTrace();
            LogManager.print("|ERROR| Can't send message to Darshana Alert Receiver !!!!");
        }

//        DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
//        clientSocket.receive(receivePacket);
//        String modifiedSentence = new String(receivePacket.getData());
//        System.out.println("FROM SERVER:" + modifiedSentence);
    }
}
