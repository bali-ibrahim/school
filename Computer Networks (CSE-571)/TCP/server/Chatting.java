package server;

import java.net.ServerSocket;
import java.util.concurrent.ConcurrentLinkedQueue;
import services.SocketConnectionService;
import services.NotReallyWellKnownPort;

final public class Chatting {

  final public static void main(String args[]) throws Exception {
    final var welcomeSocket = new ServerSocket(NotReallyWellKnownPort.Chatting);

    final var connectionPool = new ConcurrentLinkedQueue<SocketConnectionService>();

    while (true) {
      System.out.println("Started accepting connections!");
      final var connectionSocket = welcomeSocket.accept();
      final var connection = new SocketConnectionService(connectionSocket);
      connection.start();
      connectionPool.add(connection);
      System.out.println("Accepted a connection.");
      connection.Send("Hello client!");
    }
  }
}
