package client;

import java.io.*;
import java.net.*;
import services.SocketConnectionService;

public class Client {

  public static void main(String args[]) throws Exception {
    String sentence;
    BufferedReader inFromUser = new BufferedReader(new InputStreamReader(System.in));
    Socket socket = new Socket("172.22.0.3", 6789);

    var connection = new SocketConnectionService(socket);
    connection.start();

    while (true) {
      sentence = inFromUser.readLine();
      connection.Send(sentence + '\n');
    }
    // clientSocket.close();
  }
}
