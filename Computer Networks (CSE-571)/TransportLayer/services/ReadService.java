package services;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class ReadService extends Thread {

  private Socket socket;

  public ReadService(Socket c) {
    socket = c;
  }

  public void run() {
    try {
      final var incomingStream = (new BufferedReader(new InputStreamReader(socket.getInputStream()))).lines();
      while (true) {
        incomingStream.forEach(System.out::println);
      }
    } catch (IOException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
  }
}
