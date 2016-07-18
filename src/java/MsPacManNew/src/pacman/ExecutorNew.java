package pacman;

import edu.utexas.cs.nn.log.DeathLocationsLog;
import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.parameters.Parameters;
import edu.utexas.cs.nn.tasks.mspacman.facades.GameFacade;
import edu.utexas.cs.nn.tasks.mspacman.sensors.VariableDirectionBlockLoadedInputOutputMediator;
import edu.utexas.cs.nn.tasks.mspacman.sensors.mediators.IICheckEachDirectionMediator;
import pacman.controllers.osc.OSCPacMan;

import java.io.*;
import java.util.Arrays;
import java.util.EnumMap;
import java.util.Random;
import pacman.controllers.Controller;
import pacman.controllers.examples.DeterministicLegacy;
import pacman.controllers.examples.Legacy;
import pacman.controllers.examples.PansyGhosts;
import pacman.game.Constants.GHOST;
import pacman.game.Constants.MOVE;
import pacman.game.GameView;
import pacman.game.Game;
import com.illposed.osc.*;
import java.util.Date;
import java.net.InetAddress;

@SuppressWarnings("unused")
public class ExecutorNew {

	public static DeathLocationsLog deaths = null;
    private static Random rnd;
    
	public ExecutorNew() {
		
	}

	public static void main(String[] args) {
		
		Parameters.initializeParameterCollections(args);
		
		rnd = new Random(Parameters.parameters.integerParameter("randomSeed"));

		ExecutorNew exec = new ExecutorNew();
			
		OSCPacMan pacman = new OSCPacMan();
		pacman.seed(Parameters.parameters.integerParameter("randomSeed"));
		pacman.setupSockets();	

		int delay = 0;
		
		while (true){
			if (pacman.getGameReady()){
				pacman.reset();
				if (pacman.visual()) delay = 40;
				exec.runGame(pacman, new Legacy(), pacman.visual(), delay);
				pacman.sendEnd();
			}else {
				try{Thread.sleep(1);}catch(Exception e){e.printStackTrace();}
			}
		}
	}
	
	/**
     * Run a game in asynchronous mode: the game waits until a move is returned.
     * In order to slow thing down in case the controllers return very quickly,
     * a time limit can be used. If fastest gameplay is required, this delay
     * should be put as 0.
     *
     * @param pacManController The Pac-Man controller
     * @param ghostController The Ghosts controller
     * @param visual Indicates whether or not to use visuals
     * @param delay The delay between time-steps
     */
    public void runGame(Controller<MOVE> pacManController, Controller<EnumMap<GHOST, MOVE>> ghostController, boolean visual, int delay) {
        Game game = new Game(rnd.nextLong());

        GameView gv = null;

        if (visual) {
            gv = new GameView(game).showGame();
        }

        int levelTime = 0;
        
        while (!game.gameOver() && (CommonConstants.pacmanFatalTimeLimit && levelTime <= CommonConstants.pacManLevelTimeLimit)) {
            game.advanceGame(pacManController.getMove(game.copy(), -1), ghostController.getMove(game.copy(), -1));
            
            try {
                Thread.sleep(delay);
            } catch (Exception e) {
            }

            if (visual) {
                gv.repaint();
            }
            levelTime = game.getCurrentLevelTime();
        }
    }
	
	/*********************************************************************************************/  
	int timeBetweenRepaint = 40;
	long timeDiff;
	long timeStart;
	long sleepTime;
	public void repaintGame(GameView gv) {
		timeDiff = System.currentTimeMillis() - timeStart;
		sleepTime = timeBetweenRepaint - timeDiff;
		if (sleepTime > 0) {
			try {
				Thread.sleep(sleepTime);
			} catch (Exception e) { e.printStackTrace(); } 
		}
		gv.repaint();
		timeStart = System.currentTimeMillis();
	}
}
