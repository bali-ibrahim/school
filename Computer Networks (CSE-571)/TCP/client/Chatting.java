package client;

import java.io.*;
import java.net.*;
import services.SocketConnectionService;
import services.NotReallyWellKnownPort;

final public class Chatting {

  final public static void main(String args[]) throws Exception {
    final var inFromUser = new BufferedReader(new InputStreamReader(System.in));
    final var socket = new Socket("172.22.0.3", NotReallyWellKnownPort.Chatting);

    final var connection = new SocketConnectionService(socket);
    connection.start();

    while (true) {
      final var sentence = inFromUser.readLine();
      connection.Send(sentence + '\n');
    }
  }
}
