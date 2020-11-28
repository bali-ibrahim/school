package server;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ConcurrentLinkedQueue;
import services.SocketConnectionService;

final public class Server {

  final public static void main(String args[]) throws Exception {
    final ServerSocket welcomeSocket = new ServerSocket(6789);

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
