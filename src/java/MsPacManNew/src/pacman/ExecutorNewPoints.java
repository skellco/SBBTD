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
import pacman.game.Constants;
import pacman.game.Constants.GHOST;
import pacman.game.Constants.MOVE;
import pacman.game.GameView;
import pacman.game.Game;
import com.illposed.osc.*;
import java.util.Date;
import java.net.InetAddress;

@SuppressWarnings("unused")
public class ExecutorNewPoints {

	OSCPortIn serverPort;
	private static boolean pointReady = false;

	//point data
	static float[] point = new float[8];
	//	0 initialMaze;
	//	1 initialPacmanLocation;
	//	2 initialBlinkyLoc;
	//	3 initialPinkyLoc;
	//	4 initialInkyLoc;
	//	5 initialSueLoc;
	//	6 proportionOfPillsPresent;
	//	7 double powerPillsPresent?;

	public static DeathLocationsLog deaths = null;
	private static Random rnd;

	public ExecutorNewPoints() {

	}

	public void setupSockets(int port){
		try{
			//Set up sockets
			serverPort = new OSCPortIn(port);

			//Set up listener - POINT
			OSCListener act = new OSCListener(){
				public void acceptMessage(Date time, OSCMessage message) {
					for (int i = 0; i < point.length; i++)
						point [i] = (float)message.getArguments()[i];
					pointReady = true;
				}
			}; serverPort.addListener("point", act);	

			serverPort.startListening();

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void main(String[] args) {

		Parameters.initializeParameterCollections(args);

		//Setup Ms. Pac-Man simulator parameters
		Constants.NUM_LIVES = Parameters.parameters.integerParameter("pacmanLives");

		rnd = new Random(Parameters.parameters.integerParameter("randomSeed"));

		ExecutorNewPoints exec = new ExecutorNewPoints();

		OSCPacMan pacman = new OSCPacMan();
		pacman.seed(Parameters.parameters.integerParameter("randomSeed"));
		pacman.usePoints(Parameters.parameters.booleanParameter("usePoints"));
		pacman.setupSockets();	

		exec.setupSockets(Parameters.parameters.integerParameter("randomSeed")+2);
		while (true){
			if (pacman.getGameReady() && 
					(Parameters.parameters.booleanParameter("usePoints") && pointReady || 
							!Parameters.parameters.booleanParameter("usePoints"))){
				pointReady = false;
				pacman.reset();
				if (Parameters.parameters.booleanParameter("timedPacman"))
					exec.runGameTimed(pacman, new Legacy(), pacman.visual(), pacman.delay());
				else
					exec.runGame(pacman, new Legacy(), pacman.visual(), pacman.delay());
				pacman.sendEnd();
			}
			try{Thread.sleep(1);}catch(Exception e){e.printStackTrace();}
		}
	}

	/**
	 * This version of runGame can initialize games from an SBB point.
	 * Run a game in asynchronous mode: the game waits until a move is returned.
	 * In order to slow thing down in case the controllers return very quickly,
	 * a time limit can be used. If fastest gameplay is required, this delay
	 * should be put as 0.
	 *
	 * @param pacManController The Pac-Man controller
	 * @param ghostController The Ghosts controller
	 * @param p The SBB point as double array
	 * @param visual Indicates whether or not to use visuals
	 * @param delay The delay between time-steps
	 */
	public void runGame(Controller<MOVE> pacManController, Controller<EnumMap<GHOST, MOVE>> ghostController, boolean visual, int delay) {
		Game game;	
		if (Parameters.parameters.booleanParameter("usePoints")){
			float[] fullPoint = {point[0],//initialMaze
					point[1],//initialPacmanLocation
					point[2],//initialBlinkyLoc		
					point[3],//initialPinkyLoc
					point[4],//initialInkyLoc
					point[5],//initialSueLoc		
					point[6],//proportionOfPillsPresent
					point[7]//powerPillsPresent?		
			};
			game = new Game(rnd.nextLong(), fullPoint);
		}
		else
			game = new Game(rnd.nextLong());

		GameView gv = null;

		if (visual) {
			gv = new GameView(game).showGame();
		}

		int levelTime = 0;

		while (!game.gameOver() && ((CommonConstants.pacmanFatalTimeLimit && levelTime <= CommonConstants.pacManLevelTimeLimit)||
				!CommonConstants.pacmanFatalTimeLimit)) {

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

	/**
	 * This version of runGame can initialize games from an SBB point.
	 * Run a game in asynchronous mode: the game waits until a move is returned.
	 * In order to slow thing down in case the controllers return very quickly,
	 * a time limit can be used. If fastest gameplay is required, this delay
	 * should be put as 0.
	 *
	 * @param pacManController The Pac-Man controller
	 * @param ghostController The Ghosts controller
	 * @param p The SBB point as double array
	 * @param visual Indicates whether or not to use visuals
	 * @param delay The delay between time-steps
	 */
	public void runGameTimed(Controller<MOVE> pacManController, Controller<EnumMap<GHOST, MOVE>> ghostController, boolean visual, int delay) {
		Game game;	
		long timeSteps = 0;
		long onTimeSteps = 0;
		int DELAY = 40;
		boolean fixedTime = false;
		if (Parameters.parameters.booleanParameter("usePoints")){
			float[] fullPoint = {point[0],//initialMaze
					point[1],//initialPacmanLocation
					point[2],//initialBlinkyLoc		
					point[3],//initialPinkyLoc
					point[4],//initialInkyLoc
					point[5],//initialSueLoc		
					point[6],//proportionOfPillsPresent
					point[7]//powerPillsPresent?		
			};
			game = new Game(rnd.nextLong(), fullPoint);
		}
		else
			game = new Game(rnd.nextLong());

		GameView gv = null;

		if (visual) {
			gv = new GameView(game).showGame();
		}

		//        if (pacManController instanceof HumanController) {
		//            gv.getFrame().addKeyListener(((HumanController) pacManController).getKeyboardInput());
		//        }

		new Thread(pacManController).start();
		new Thread(ghostController).start();

		while (!game.gameOver()) {
			pacManController.update(game.copy(), System.currentTimeMillis() + DELAY);
			ghostController.update(game.copy(), System.currentTimeMillis() + DELAY);

			try {
				int waited = DELAY / Constants.INTERVAL_WAIT;

				for (int j = 0; j < DELAY / Constants.INTERVAL_WAIT; j++) {
					Thread.sleep(Constants.INTERVAL_WAIT);

					if (pacManController.hasComputed() && ghostController.hasComputed()) {
						waited = j;
						onTimeSteps++;
						break;
					}
				}

				if (fixedTime) {
					Thread.sleep(((DELAY / Constants.INTERVAL_WAIT) - waited) * Constants.INTERVAL_WAIT);
				}

				game.advanceGame(pacManController.getMove(), ghostController.getMove());
				timeSteps++; DELAY = 4;
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
            
			if (visual && !game.gameOver()) {
				gv.repaint();
			}
		}
		System.out.println("onTimeSteps " + (float)onTimeSteps/timeSteps);
//		pacManController.terminate();
//		ghostController.terminate();
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
