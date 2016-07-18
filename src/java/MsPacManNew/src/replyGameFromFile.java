import com.illposed.osc.*;
import java.util.Date;
import java.net.InetAddress;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;


public class replyGameFromFile {
    public static int random = 1;
    public static float gameover = 0;
    public static float score = 0;
    
    public static void main(String[] args) {
//------------------------------------------------------------------------------        
     
        String input;
        String sMove;
        String invalidMove = "ERROR";
        boolean invalid = false;
        String seed = "ERROR";
        LinkedList moves = new LinkedList();
        BufferedReader br = null;
 
        System.out.println("Reading file...");
         
        try {
            br = new BufferedReader(new FileReader("game.txt"));
            
            //Get game seed number
            input = br.readLine();
            seed = input.split(":", 2)[1];
            seed = seed.replaceAll("\\s+","");
            seed = seed.substring(0,seed.length()-1);
            System.out.println("Seed: " + seed);

            //Get agent moves from file
            while ((input = br.readLine()) != null) {
                sMove = input.split(":", 2)[1];
                sMove = sMove.replaceAll("\\s+","");
                
                if (sMove.equals("UP")) moves.add(0);
                else if (sMove.equals("RIGHT")) moves.add(1);
                else if (sMove.equals("DOWN")) moves.add(2);
                else if (sMove.equals("LEFT")) moves.add(3);
                else {
                  if(invalid)
                     System.out.println("INVALID MOVE: "+invalidMove); 
                  else
                     invalid = true;
                  moves.add(100); 
                  invalidMove = sMove;
                } 
            }
        } catch (IOException e) { e.printStackTrace(); }
       
        //Obtained all moves, close file
        try { if (br != null)br.close(); } 
        catch (IOException ex) { ex.printStackTrace(); }
        
        System.out.println("Done reading from file.");
        
//-----------------------------------------------------------------------------        
        int nextmove; int timestep = 0;
        try {
            //Socket variables
            int game_server_port = 45;
            InetAddress game_server_IP = InetAddress.getLocalHost();
            int agent_port = game_server_port + 1;

            //Set-up outgoing socket
            OSCPortOut outgoingPort = new OSCPortOut(game_server_IP, game_server_port);

            //Set-up incoming socket
            OSCPortIn incomingPort = new OSCPortIn(agent_port);
            OSCListener listener1 = new OSCListener(){
               public void acceptMessage(Date time, OSCMessage message) {
                  /*
                     //Joe, show what was sent through socket and check that it is the same values as NN_inputs
                      int matchingIndex[] = {0,30,31,32,33,34,35,36};
                      int base = 7; //First argument that is input
                      float current_value = -1;
                      
                      for (int loop = 0; loop < 4; loop++) {
                          
                          current_value = (Float)msg.getArguments()[0+base]; //Constant
                          if (is_same(current_value, NNCheckEachDirectionPacManController.full_inputs[loop][0])) System.out.print(current_value); else { System.out.println("\n Mismatch at full_inputs["+loop+"][0]: " + current_value + ", " + NNCheckEachDirectionPacManController.full_inputs[loop][0]); System.exit(1);}
                          
                          for (int i = 8; i < 37; i++) {
                              current_value = (Float)msg.getArguments()[i + 30*loop + base];
                              if (is_same(current_value, NNCheckEachDirectionPacManController.full_inputs[loop][i-7])) System.out.print(", " + current_value); else { System.out.println("\n Mismatch at full_inputs["+loop+"]["+(i-7)+"]: " + current_value + ", " + NNCheckEachDirectionPacManController.full_inputs[loop][i-7]); System.exit(1);}
                          }
                          for (int i = 1; i < 8; i++) {
                              current_value = (Float)msg.getArguments()[i+base]; //Constant
                              if (is_same(current_value, NNCheckEachDirectionPacManController.full_inputs[loop][i+29])) System.out.print(", " + current_value); else { System.out.println("\n Mismatch at full_inputs["+loop+"]["+(i+29)+"]:" + current_value + ", " + NNCheckEachDirectionPacManController.full_inputs[loop][i+29]); System.exit(1);}
                          }
                          current_value = (Float)msg.getArguments()[37 + 30*loop + base];
                          if (is_same(current_value, NNCheckEachDirectionPacManController.full_inputs[loop][37])) System.out.println(", " + current_value); else { System.out.println("\n Mismatch at full_inputs["+loop+"][37]: " + current_value + ", " + NNCheckEachDirectionPacManController.full_inputs[loop][37]); System.exit(1);}
                          
                      } System.out.println();
                      
                      //--------------------------------------------------------------
               
                     int matchingIndex[] = {0,30,31,32,33,34,35,36};
                      int base = 7; //First argument that is input
                      float current_value = -1;
                      
                      for (int loop = 0; loop < 4; loop++) {
                          
                          System.out.print((Float)message.getArguments()[0+base]); //Constant
                          
                          for (int i = 8; i < 37; i++) {
                              System.out.print(", " + (Float)message.getArguments()[i + 30*loop + base]);
                          }
                          for (int i = 1; i < 8; i++) {
                              System.out.print(", " + (Float)message.getArguments()[i+base]); //Constant
                          }
                          System.out.println(", " + (Float)message.getArguments()[37 + 30*loop + base]);
                      } System.out.println();
                      
                  */
                  
                    score = (Float) message.getArguments()[2]; 
                    gameover = (Float) message.getArguments()[1];                    
                    random = 2;
               }
            }; incomingPort.addListener("state", listener1);
            incomingPort.startListening();
            
            //Send start message................................................
            OSCMessage msg = new OSCMessage("visual");
            outgoingPort.send(msg);
            try{Thread.sleep(200);}catch(Exception e){}
            msg = new OSCMessage("startwithseed");
            msg.addArgument(seed);
            outgoingPort.send(msg);
            System.out.println("Waiting...");
            
            
            //Replay game
            while (true) {
               //Wait for reply from server
               while(random == 1){
                  System.out.print("");
               } random = 1;              
               
               //Base-case, if no more moves, end processes
               if (moves.size() == 1) { //only End remains
                  try{Thread.sleep(2000);}catch(Exception e){}
                  msg = new OSCMessage("exit");
                  outgoingPort.send(msg);
                  incomingPort.stopListening();
                  incomingPort.close();
                  try{Thread.sleep(100);}catch(Exception e){}
                  if (gameover < 1) System.out.println("Error: out of moves but game is not over");
                  else System.out.println("All moves sent");
                  System.out.println("Time Step: " + timestep + " | Score: " + (int)score);
                  break;
               }
               
               //Send next move to server
               msg = new OSCMessage("act"); nextmove=(Integer)moves.removeFirst();
               msg.addArgument(nextmove); 
               msg.addArgument(++timestep); //timestep
               outgoingPort.send(msg);
            }
      }
      
      catch (Exception e) {
         e.printStackTrace();
      }    
    }
}
