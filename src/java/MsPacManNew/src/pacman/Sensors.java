package pacman;

import java.awt.Color;

import java.util.*;

import pacman.game.Constants;
import pacman.game.Game;
import pacman.game.GameView;
import pacman.game.Constants.*;
import edu.utexas.cs.nn.tasks.mspacman.facades.*;
import edu.utexas.cs.nn.tasks.mspacman.sensors.directional.VariableDirectionCountJunctionOptionsBlock;
import edu.utexas.cs.nn.util.datastructures.ArrayUtil;
import edu.utexas.cs.nn.tasks.mspacman.sensors.directional.counts.*;


public class Sensors {

	/**
	 * The current setup will return a close approximation of the sensors used in: 
	 * Jacob Schrum and Risto Miikkulainen, "Discovering Multimodal Behavior in 
	 * Ms. Pac-Man through Evolution of Modular Neural Networks", IEEE Transactions 
	 * on Computational Intelligence and AI in Games (2015). 
	 *
	 * There are 7 non-directed sensors plus 22 directed sensors for each of 4 directions,
	 * for a total of (7 + (4 * 22)) = 95 sensors.
	 */


	final int NUM_SENSORS = 95;
	final int MAX_DISTANCE = 200;
	final int CLOSE_TO_TARGET_THRESHOLD = 10;
	final double DOUBLE_DYNAMIC_RANGE = 50.0;
	final double BOOL_INPUT_HIGH = 5.0;
	final double BOOL_INPUT_LOW = -5.0;

	long startTime;

//	private void timeIn(){
//		//startTime = System.nanoTime();
//	}

//	private void timeOut(String label){
//		//		long endTime   = System.nanoTime();
//		//		long totalTime = endTime - startTime;
//		//		System.out.println("Time " + label + " " + totalTime/1000);
//	}

	public double[] read(Game game){
		GameFacade gf = new GameFacade(game);
		int currentLocation = gf.getPacmanCurrentNodeIndex();
		int[] neighbours = gf.neighbors(currentLocation);
		int numReadings = 0;
		double[] sensorReadings = new double[NUM_SENSORS];

		/* Common Undirected ***************************************/

		//timeIn();
		//Proportion Power Pills
		sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE*(double)game.getNumberOfActivePowerPills() / (double)game.getNumberOfPowerPills(); 
		//timeOut("1");

		//timeIn();
		//Proportion Pills
		sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE*(double)game.getNumberOfActivePills() / (double)game.getNumberOfPills();
		//timeOut("2");

		//timeIn();
		//Number of Edible Ghosts
		int numEdibleGhosts = 0;
		int totalLairTime = 0;
		double remainingGhostEdibleTime = 0;
		for (GHOST ghost : GHOST.values()) {
			if (game.isGhostEdible(ghost))
				remainingGhostEdibleTime += game.getGhostEdibleTime(ghost);
			totalLairTime += game.getGhostLairTime(ghost);
		}
		sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE*(double)game.getNumEdibleGhosts() / Constants.NUM_GHOSTS;
		//timeOut("3");

		//timeIn();
		//Remaining Ghost Edible Time
		sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE*((double)remainingGhostEdibleTime / Constants.NUM_GHOSTS)/Constants.EDIBLE_TIME;
		//timeOut("4");

		//timeIn();
		//Any Ghosts Edible?
		sensorReadings[numReadings++] = game.getNumEdibleGhosts() > 0 ? BOOL_INPUT_HIGH : BOOL_INPUT_LOW;   
		//timeOut("5");

		//timeIn();
		//All Threat Ghosts Present?
		sensorReadings[numReadings++] = !(totalLairTime > 0) && (numEdibleGhosts == 0) ? BOOL_INPUT_HIGH : BOOL_INPUT_LOW; 
		//timeOut("6");

		//timeIn();
		//Close to Power Pill?
		int closestPowerPillDistance = Integer.MAX_VALUE;
		int[] activePowerPills=game.getActivePowerPillsIndices();
		for(int i = 0; i < activePowerPills.length; i++)
			closestPowerPillDistance = Math.min(closestPowerPillDistance, game.getShortestPathDistance(currentLocation,activePowerPills[i]));
		sensorReadings[numReadings++] = closestPowerPillDistance < CLOSE_TO_TARGET_THRESHOLD ? BOOL_INPUT_HIGH : BOOL_INPUT_LOW;
		//timeOut("7");

		/* Directed **********************************************/

		for (int dir = 0; dir < 4; dir++){

			//			Color colour;
			//			switch(dir){
			//			case 0: colour = Color.RED; break;
			//			case 1: colour = Color.GREEN; break;
			//			case 2: colour = Color.BLUE; break;
			//			case 3: colour = Color.YELLOW; break;
			//			default: colour = Color.RED; break;
			//			}

			/* Common Directed ***************************************/

			//timeIn();
			//Distance to closest regular pill in given direction
			if (neighbours[dir] != -1 && gf.getActivePillsIndices().length > 1){
				sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE*(double)gf.getTargetInDir(gf.getPacmanCurrentNodeIndex(), gf.getActivePillsIndices(), dir).t2.length/MAX_DISTANCE;	
				//GameView.addPoints(game, Color.YELLOW,gf.getTargetInDir(gf.getPacmanCurrentNodeIndex(), gf.getActivePillsIndices(), dir).t2);
			} else sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE;
			//timeOut("8");

			//timeIn();
			//Distance to closest power pill in given direction
			if (neighbours[dir] != -1 && gf.getActivePowerPillsIndices().length > 0){
				sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE*(double)gf.getTargetInDir(gf.getPacmanCurrentNodeIndex(), gf.getActivePowerPillsIndices(), dir).t2.length/MAX_DISTANCE;
				//GameView.addPoints(game, Color.YELLOW,gf.getTargetInDir(gf.getPacmanCurrentNodeIndex(), gf.getActivePowerPillsIndices(), dir).t2);
			} else sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE;
			//timeOut("9");

			//timeIn();
			//Distance to closest maze junction in given direction
			if (neighbours[dir] != -1){
				sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE*(double)gf.getTargetInDir(gf.getPacmanCurrentNodeIndex(), gf.getJunctionIndices(), dir).t2.length/MAX_DISTANCE;
				//GameView.addPoints(game, Color.YELLOW,gf.getTargetInDir(gf.getPacmanCurrentNodeIndex(), gf.getJunctionIndices(), dir).t2);
			} else sensorReadings[numReadings++] = DOUBLE_DYNAMIC_RANGE;
			//timeOut("10");


			//Conflict Ghost Sensors
			int[] junctions = game.getJunctionIndices();
			ArrayList<GhostSensorReading> ghostSensors = new ArrayList<GhostSensorReading>();

			for (GHOST ghost : GHOST.values()) {
				int[] path;
				double distToGhost = DOUBLE_DYNAMIC_RANGE;
				double ghostApproachingPacman = BOOL_INPUT_LOW;
				double pathToGhostContainsJunction = BOOL_INPUT_HIGH;
				double ghostEdible = BOOL_INPUT_LOW;

				if (neighbours[dir] != -1 && game.getGhostLairTime(ghost) == 0){
					path = gf.getDirectionalPath(currentLocation, game.getGhostCurrentNodeIndex(ghost),dir);

					//timeIn();
					distToGhost = DOUBLE_DYNAMIC_RANGE*(double)path.length/MAX_DISTANCE;
					//timeOut("11");

					//timeIn();
					ghostApproachingPacman = gf.ghostApproachingPacman(GameFacade.ghostToIndex(ghost))? BOOL_INPUT_HIGH : BOOL_INPUT_LOW;
					//timeOut("12");

					//timeIn();
					pathToGhostContainsJunction = BOOL_INPUT_LOW; boolean foundJunction = false;
					for (int j = 0; j < junctions.length && !foundJunction; j++)
						if (ArrayUtil.countOccurrences(junctions[j], path) > 0){
							pathToGhostContainsJunction = BOOL_INPUT_HIGH;
							foundJunction = true;
						}
					//timeOut("13");

					//timeIn();
					ghostEdible = game.isGhostEdible(ghost) ? BOOL_INPUT_HIGH : BOOL_INPUT_LOW;
					//timeOut("14");
				}

				ghostSensors.add(new GhostSensorReading(ghost, distToGhost, ghostApproachingPacman, pathToGhostContainsJunction, ghostEdible));
			}
			//timeIn();
			Collections.sort(ghostSensors);
			//timeOut("15");

			//			if (neighbours[dir] != -1 && game.getGhostLairTime(ghostSensors.get(0).ghost) == 0){
			//				//if ( ghostSensors.get(0).ghostApproaching > 0) colour = Color.CYAN;
			//				//if (ghostSensors.get(0).pathToGhostContainsJunction > 0) colour = Color.ORANGE;
			//				//if (ghostSensors.get(0).edible > 0) colour = Color.PINK;
			//				GameView.addPoints(game, colour,gf.getDirectionalPath(currentLocation, game.getGhostCurrentNodeIndex(ghostSensors.get(0).ghost),dir));
			//			}

			for (int i = 0; i < ghostSensors.size(); i++){
				sensorReadings[numReadings++] = ghostSensors.get(i).distance;
				sensorReadings[numReadings++] = ghostSensors.get(i).approaching;
				sensorReadings[numReadings++] = ghostSensors.get(i).pathToGhostContainsJunction;
				sensorReadings[numReadings++] = ghostSensors.get(i).edible;
			}
			//timeIn();
			//Number of pills on the path in the given direction that has the most pills
			VariableDirectionCountAllPillsInKStepsBlock pill_block = new VariableDirectionCountAllPillsInKStepsBlock(dir,30);
			sensorReadings[numReadings++] = neighbours[dir] != -1 ? DOUBLE_DYNAMIC_RANGE*pill_block.getValue(gf) : 0;
			//timeOut("16");

			//timeIn();
			//Number of junctions on the path in the given direction that has the most junctions
			VariableDirectionKStepJunctionCountBlock j_block = new VariableDirectionKStepJunctionCountBlock(dir);
			sensorReadings[numReadings++] = neighbours[dir] != -1 ? DOUBLE_DYNAMIC_RANGE*j_block.getValue(gf) : 0;
			//timeOut("17");

			//timeIn();
			//OFNJ
			VariableDirectionCountJunctionOptionsBlock ofnj_block = new VariableDirectionCountJunctionOptionsBlock(dir);
			sensorReadings[numReadings++] = neighbours[dir] != -1 ? DOUBLE_DYNAMIC_RANGE*ofnj_block.getValue(gf) : 0;	
			//timeOut("18");
		}
		return sensorReadings;
	}
}
