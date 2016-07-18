package pacman.controllers.osc;

import java.net.InetAddress;

import com.illposed.osc.*;

import edu.utexas.cs.nn.tasks.mspacman.facades.GameFacade;
import edu.utexas.cs.nn.tasks.mspacman.sensors.mediators.IICheckEachDirectionMediator;

import java.util.Arrays;
import java.util.Date;
import pacman.controllers.Controller;
import pacman.game.Game;
import pacman.game.Constants.MOVE;

public class OSCPacMan extends Controller<MOVE> {

	int seed;

	OSCPortIn serverPort;
	OSCPortOut outgoingPort;
	int game_server_port;
	InetAddress agent_IP; 
	int agent_port;

	int stepDelay;

	boolean moveReady;
	MOVE nextMove;
	boolean gameReady;
	boolean sleep;
	boolean usePoints;
	boolean visual;

	Sensors sensors;
	int prevGhostScore;
	int numEatenGhosts;

	public static IICheckEachDirectionMediator mediator;

	public OSCPacMan(){
		mediator = new IICheckEachDirectionMediator();
		sensors = new Sensors();
		moveReady = false;
		gameReady = false;
		prevGhostScore = 200; //score for ghosts eaten is 200 400 800 1600
		numEatenGhosts = 0;
		usePoints = false;
		visual = false;
	}

	public void reset(){
		mediator.reset();
		numEatenGhosts = 0;
		prevGhostScore = 200; //score for ghosts eaten is 200 400 800 1600
		gameReady = false;
	}

	public void seed(int s){ 
		seed = s;
		game_server_port = seed;
		agent_port = game_server_port + 1;
		System.out.println("Seed: " + seed);
	}

	public int delay(){
		return stepDelay;
	}

	public void delay(int d){
		stepDelay = d;
	}

	public boolean visual(){
		return visual;
	}

	public int seed(){ 
		return seed;
	}

	public boolean getGameReady(){
		return gameReady;
	}

	public void setGameReady(boolean b){
		gameReady = b;
	}

	public void usePoints(boolean p){
		usePoints = p;
	}

	public void setupSockets(){
		try{
			agent_IP = InetAddress.getLocalHost();

			//Set up sockets
			outgoingPort = new OSCPortOut(agent_IP, agent_port);
			serverPort = new OSCPortIn(game_server_port);

			//Set up listener - START
			OSCListener start = new OSCListener() {
				public void acceptMessage(Date time, OSCMessage message) {
					gameReady = true;
					moveReady = true;
				}
			}; serverPort.addListener("start", start); 

			//Set up listener - ACT
			OSCListener act = new OSCListener(){
				public void acceptMessage(Date time, OSCMessage message) {
					switch ((Integer)message.getArguments()[0]) {
					case 0: nextMove = MOVE.UP; break;
					case 1: nextMove = MOVE.RIGHT; break;
					case 2: nextMove = MOVE.DOWN; break;
					case 3: nextMove = MOVE.LEFT; break;
					default: nextMove = MOVE.NEUTRAL; 
					System.out.println("ERROR: Invalid MOVE: " + (Integer)message.getArguments()[0]);
					}
					moveReady = true;
				}
			}; serverPort.addListener("act", act);	

			//Set up listener - EXIT
			OSCListener exit = new OSCListener(){
				public void acceptMessage(Date time, OSCMessage message) {
					serverPort.stopListening();
					serverPort.close();
					try {Thread.sleep(500);}catch(Exception e){}
					System.exit(0);
				}
			}; serverPort.addListener("exit", exit);

			//Set up listener - DELAY
			OSCListener delay = new OSCListener() {
				public void acceptMessage(Date time, OSCMessage message) {
					stepDelay = (Integer)message.getArguments()[0];

				}
			}; serverPort.addListener("delay", delay);

			//Set up listener - VISUAL
			OSCListener setvisual = new OSCListener(){
				public void acceptMessage(Date time, OSCMessage message) {
					visual = true;
				}
			}; serverPort.addListener("visual", setvisual);

			//Set up listener - SLEEP
			OSCListener setsleep = new OSCListener() {
				public void acceptMessage(Date time, OSCMessage message) {
					sleep = true;
				}
			}; serverPort.addListener("sleep", setsleep);

			//Set up listener - WAKE
			OSCListener setwake = new OSCListener() {
				public void acceptMessage(Date time, OSCMessage message) {
					sleep = false;
				}
			}; serverPort.addListener("wake", setwake);

			serverPort.startListening();

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	public void sendEnd() {
		OSCMessage msg = new OSCMessage("end");
		try{
			outgoingPort.send(msg);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	public MOVE getMove(Game game, long timeDue) {

		OSCMessage msg = new OSCMessage("state");

		msg.addArgument((float)0.0);//place holder for episodeEnd not used here
		msg.addArgument((float)game.getScore());
		msg.addArgument((float)game.getEatenPills());
		msg.addArgument((float)game.getGhostReward());
		msg.addArgument((float)game.getNodeXCood(game.getPacmanCurrentNodeIndex()));
		msg.addArgument((float)game.getNodeYCood(game.getPacmanCurrentNodeIndex()));

		GameFacade gs = new GameFacade(game);

		int[] neighbours = gs.neighbors(gs.getPacmanCurrentNodeIndex());

		//		//skelly sensors
		//		for (int i = 0; i < neighbours.length; i++) 
		//			msg.addArgument((float)neighbours[i]);
		//		//Save neighbours - Pacman cannot go to direction of -1 (a wall)
		//		
		//		double[] sensorReading = sensors.read(game);
		//
		//		//System.out.print("state:");
		//		for (int i = 0; i < sensorReading.length; i++){
		//			//System.out.print(" " + sensorReading[i]);
		//			msg.addArgument((float)sensorReading[i]);
		//		} 

		//jschrum sensors (faster?)
		int aNonWall = -1;
		for (int i = 0; i < neighbours.length; i++) {
			msg.addArgument((float)neighbours[i]);
			if (neighbours[i] != -1) aNonWall = i;
		}

		mediator.mediatorStateUpdate(gs);
		boolean first = true; 
		double[][] full_inputs = new double[4][1]; 
		for (int i = 0; i < 4; i++) Arrays.fill(full_inputs[i], -1); 

		for (int i = 0; i < neighbours.length; i++) {
			mediator.setDirection(i);
			double[] inputs = mediator.getInputs(gs, gs.getPacmanLastMoveMade());
			if (first) { first = false; full_inputs = new double[4][inputs.length]; } 
			for (int ab = 0; ab < inputs.length; ab++) full_inputs[i][ab] = inputs[ab]; 
		}

		//Save  inputs
		int nonDirectionOrientedIndex[] = {22,23,24,25,26,27,28};  // 7 non-directed       

		// ----- first save the non direction-oriented inputs
		for (int i = 0; i < nonDirectionOrientedIndex.length; i++) {
			msg.addArgument((float)full_inputs[aNonWall][nonDirectionOrientedIndex[i]]);
		}

		// ----- save the remaining direction-oriented inputs // 22 directed
		for (int i = 0; i < 4; i++) {
			for (int j = 1; j < 22; j++) 
				msg.addArgument((float)full_inputs[i][j]);
			msg.addArgument((float)full_inputs[i][29]); //OFNJ
		}	

		//Send inputs to outgoingSocket
		try{
			outgoingPort.send(msg);//outgoingPort.send(msg);
		} catch (Exception e) {
			e.printStackTrace();
		}

		//Get next MOVE from incomingSocket
		while (moveReady == false) {
			if (sleep) {try{Thread.sleep(1000);}catch(Exception e){e.printStackTrace();}}
			else {
				try{Thread.sleep(1);}catch(Exception e){e.printStackTrace();}
			}
		} moveReady = false;

		return nextMove;
	}
}