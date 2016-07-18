package dal.experiment;

import edu.utexas.cs.nn.mmneat.MMNEAT;
import java.io.FileNotFoundException;

public class RunSocketServer {
    public static boolean useSocket = false;
    
    public static void main(String[] args)  throws FileNotFoundException, NoSuchMethodException {
        String socket_option[] = args[0].split(":",2);
        String newArgs[];
        
        //System.out.println("\nREMOVED AVOID WALL in NNCheckEachDirectionPacManController");
        //try {Thread.sleep(5000);} catch (Exception e){}
        
        if (socket_option[0].toLowerCase().equals("socket")) {
            
            //Remove socket arguement
            newArgs = new String[args.length-1];
            for (int i = 0; i < args.length-1; i++) {
                newArgs[i] = args[i+1];
            }
            
            useSocket = socket_option[1].toLowerCase().equals("true");
            MMNEAT.main(newArgs);
        }
        else { //Normal MM-Neat Run
            useSocket = false;  
            MMNEAT.main(args);
        }
        
        
    }
}

