package services;

import java.io.IOException;
import java.net.Socket;

public class RelayService extends Thread {

  private Socket inSocket;
  private Socket outSocket;

  public RelayService(Socket i, Socket o) {
    inSocket = i;
    outSocket = o;
  }

  public void run() {
    try {
      while (true) {
        inSocket.getInputStream().transferTo(outSocket.getOutputStream());
        outSocket.getInputStream().transferTo(inSocket.getOutputStream());
      }
    } catch (IOException e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
  }
}
