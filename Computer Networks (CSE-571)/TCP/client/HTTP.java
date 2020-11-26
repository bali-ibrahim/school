package client;

import java.io.*;
import java.net.*;

public class HTTP {

    public static void main(String args[]) throws Exception {

        BufferedReader inFromUser = new BufferedReader(new InputStreamReader(System.in));
        System.out.print("Please Enter the URL address of the web page: ");
        String address = inFromUser.readLine();

        if (!address.startsWith("http://")) {
            address = "http://" + address;
        }

        URL url = new URL(address);
        String hostName = url.getHost();
        String path = url.getPath().isEmpty() ? "/" : url.getPath();

        Socket clientSocket = new Socket("172.31.16.1", 8080);
        connect(hostName, path, clientSocket);
        printStream(clientSocket);

        clientSocket.close();

    }

    public static void connect(String hostName, String path, Socket clientSocket)
            throws UnknownHostException, IOException {
        var request = "GET " + path + " HTTP/1.1\r\n" + "Host: " + hostName + "\r\n" + "Connection: close\r\n\r\n";

        System.out.println("Connection established successfully!!\n");

        var outToServer = new DataOutputStream(clientSocket.getOutputStream());

        System.out.println("----Request----\n" + request);
        outToServer.writeBytes(request);

        System.out.println("HTTP request sent to: " + hostName);

    }

    private static void printStream(Socket clientSocket) throws IOException {
        BufferedReader inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        String a;

        while ((a = inFromServer.readLine()) != null) {
            System.out.println(a);
        }
    }

}