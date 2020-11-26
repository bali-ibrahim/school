import java.io.*;
import java.net.*;

public class UDPClient {

	public static void main(String args[]) throws Exception {

		System.out.println("Client");

		var inFromUser = new BufferedReader(new InputStreamReader(System.in));

		var clientSocket = new DatagramSocket();

		var IPAddress = InetAddress.getByName("localhost");
		
		byte[] sendData = null;
		var receiveData = new byte[100];

		while (true) {

			var sentence = inFromUser.readLine();

			sendData = sentence.getBytes();
			var sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, 9876);
			clientSocket.send(sendPacket);

			var receivePacket = new DatagramPacket(receiveData, receiveData.length);
			clientSocket.receive(receivePacket);
			var modifiedSentence = new String(receivePacket.getData(), 0, receivePacket.getLength());
			System.out.println("FROM SERVER: " + modifiedSentence);
			
		}
		
	}

}