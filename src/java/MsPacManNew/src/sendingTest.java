import java.net.*;
import com.illposed.osc.*;

public class sendingTest {

   public static void main(String[] args) {
      try {
         System.out.println("Working");
      
         OSCPortOut sender = new OSCPortOut(InetAddress.getLocalHost(), 12372);
         
         OSCMessage message = new OSCMessage("start");
         message.addArgument("byebye");
         
         sender.send(message);
      } catch (Exception e) {
      
      } 
   }
}