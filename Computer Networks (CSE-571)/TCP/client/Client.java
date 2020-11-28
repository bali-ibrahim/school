package client;

import java.io.*;
import java.net.*;
import services.SocketConnectionService;

final public class Client {

  final public static void main(String args[]) throws Exception {
    final var inFromUser = new BufferedReader(new InputStreamReader(System.in));
    final var socket = new Socket("172.22.0.3", 6789);

    final var connection = new SocketConnectionService(socket);
    connection.start();

    while (true) {
      final var sentence = inFromUser.readLine();
      connection.Send(sentence + '\n');
    }
  }
}
