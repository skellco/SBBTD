package pacman;

import pacman.game.Constants.GHOST;

public class GhostSensorReading implements Comparable<GhostSensorReading>{
	public GHOST ghost;
	public Double distance;
	public Double approaching;
	public Double pathToGhostContainsJunction;
	public Double edible;
	
	public GhostSensorReading(GHOST g, double d, double ga, double p, double ge){
		ghost = g;
		distance = d;
		approaching = ga;
		pathToGhostContainsJunction = p;
		edible = ge;
	}
	
	public int compareTo(GhostSensorReading other)
	  {
	    return distance.compareTo(other.distance);
	  }
}
