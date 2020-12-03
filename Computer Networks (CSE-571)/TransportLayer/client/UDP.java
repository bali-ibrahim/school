package server;

import java.net.*;
import java.util.*;

public class UDP {
	public static void main(String args[]) {
		byte[] buf = new byte[1024];
		InetAddress IPAddress = null;
		try {
			IPAddress = InetAddress.getByName("127.0.0.1");
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}
		try {
			DatagramSocket ds = new DatagramSocket();
			for (int i = 0; i < 25;) {
				byte[] sbuf = String.valueOf(i).getBytes();
				DatagramPacket sPacket = new DatagramPacket(sbuf, sbuf.length, IPAddress, 8888);
				ds.send(sPacket);
				System.out.println("Sent packet #" + i);

				DatagramPacket rPacket = new DatagramPacket(buf, buf.length);

				ds.receive(rPacket);
				String str = new String(rPacket.getData(), rPacket.getOffset(), rPacket.getLength());
				if (str.equals(String.valueOf(i))) {
					System.out.println("ACK #" + str + " received");
					i++;
				} else {
					System.out.println("ACK does not match. Retrying...");
				}

			}
			ds.close();
		} catch (Exception ex) {
			System.out.println("Error" + ex);
		}
	}
}