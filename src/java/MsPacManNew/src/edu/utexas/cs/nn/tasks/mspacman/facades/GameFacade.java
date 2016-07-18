package edu.utexas.cs.nn.tasks.mspacman.facades;

import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.tasks.mspacman.ghosts.GhostComparator;
import edu.utexas.cs.nn.util.datastructures.ArrayUtil;
import edu.utexas.cs.nn.util.datastructures.Pair;
import java.awt.Color;
import java.util.*;
import pacman.game.Constants;
import pacman.game.Constants.DM;
import pacman.game.Constants.GHOST;
import pacman.game.Constants.MOVE;
import pacman.game.Game;

/**
 *
 * @author Jacob Schrum
 */
public class GameFacade {

    public static final int MAX_DISTANCE = 200;
    public static final int NUM_DIRS = 4;
    public static final int DANGEROUS_TIME = 5;
    public Game newG = null;

    public static MOVE indexToMove(int index) {
        switch (index) {
            case 0:
                return MOVE.UP;
            case 1:
                return MOVE.RIGHT;
            case 2:
                return MOVE.DOWN;
            case 3:
                return MOVE.LEFT;
            default:
                return null;
        }
    }

    public static int moveToIndex(MOVE m) {
        switch (m) {
            case UP:
                return 0;
            case RIGHT:
                return 1;
            case DOWN:
                return 2;
            case LEFT:
                return 3;
            default:
                return -1;
        }
    }

    public static GHOST indexToGhost(int index) {
        assert index >= 0 && index <= 3 : "Must be a valid ghost index: " + index;
        switch (index) {
            case 0:
                return GHOST.BLINKY;
            case 1:
                return GHOST.PINKY;
            case 2:
                return GHOST.INKY;
            case 3:
                return GHOST.SUE;
            default:
                System.out.println("Index " + index + " is a NULL ghost!");
                return null;
        }
    }

    public static int ghostToIndex(GHOST ghost) {
        switch (ghost) {
            case BLINKY:
                return 0;
            case PINKY:
                return 1;
            case INKY:
                return 2;
            case SUE:
                return 3;
            default:
                return -1;
        }
    }

    /**
     * Return indices for certain types of ghosts
     * @param edibleVsThreatOnly true for edible only, false for threat only, unless "all" is true.
     * @param all get all the ghosts, except the ones in the lair
     * @return list of ghost indices
     */
    public ArrayList<Integer> getGhostIndices(boolean edibleVsThreatOnly, boolean all) {
        ArrayList<Integer> ghosts = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (!ghostInLair(i)
                    && (all
                    || (edibleVsThreatOnly && isGhostEdible(i))
                    || (!edibleVsThreatOnly && isGhostThreat(i)))) {
                ghosts.add(i);
            }
        }
        return ghosts;
    }
    
    /**
     * return the node index of the node that is the neoghbor of current in the
     * direction of move
     *
     * @param g pacman game
     * @param current current node index
     * @param move direction to move from current
     * @return neighboring node index
     */
    public static int getNeighbourInDirection(Game g, int current, MOVE move) {
        return neighbors(g, current)[moveToIndex(move)];
    }

    /**
     * Given pacman game and node index, an array of size 4 is returned
     * containing the neighboring node indices. Each slot in the array
     * corresponds to a specific direction, and if there is no neighbor in the
     * given direction, then slot is filled with a -1
     *
     * @param gs Pacman game
     * @param currentNodeIndex node to get neighbors of
     * @return contents of four neighboring positions
     */
    public static int[] neighbors(Game gs, int currentNodeIndex) {
        assert currentNodeIndex != -1 : "-1 is not a valid node index";
        MOVE[] possible = gs.getPossibleMoves(currentNodeIndex);
        int[] neighbors = gs.getNeighbouringNodes(currentNodeIndex);
        int[] result = new int[NUM_DIRS];
        Arrays.fill(result, -1);
        int cx = gs.getNodeXCood(currentNodeIndex);
        int cy = gs.getNodeYCood(currentNodeIndex);
        for (int i = 0; i < possible.length; i++) {
            int nx = gs.getNodeXCood(neighbors[i]);
            int ny = gs.getNodeYCood(neighbors[i]);
            assert !((possible[i].equals(MOVE.UP) && (cx != nx || ny >= cy))
                    || (possible[i].equals(MOVE.DOWN) && (cx != nx || ny <= cy))
                    || (possible[i].equals(MOVE.LEFT) && (cy != ny || (nx >= cx && nx != 108 & cx != 0)))
                    || (possible[i].equals(MOVE.RIGHT) && (cy != ny || (nx <= cx && nx != 0 & cx != 108)))) :
                    "Error in neighbor calculation: move:" + possible[i] + ":current:" + cx + "," + cy + ":neighbor:" + nx + "," + ny;
            result[moveToIndex(possible[i])] = neighbors[i];
        } 
        return result;
    }

    public GameFacade(Game g) {
        newG = g;
    }

    public List<Integer> getPillEatTimes() {
        return newG.getPillEatTimes();
    }

    /**
     * How long the ghosts will be edible for the next time a power pill is
     * eaten.
     *
     * @return time
     */
    public int getNextEdibleTime() {
        //return (int) (Constants.EDIBLE_TIME * (Math.pow(Constants.EDIBLE_TIME_REDUCTION, this.getCurrentLevel())));
        return newG.newEdibleTime();
    }

    public int getSpecificGhostEatenCount(int ghostIndex) {
        return newG.getEatenGhosts(ghostIndex);
    }

    /**
     * Return number of eaten ghosts across all levels
     *
     * @return
     */
    public int getNumEatenGhosts() {
        return newG.getEatenGhosts();
    }

    /**
     * Return current score
     *
     * @return
     */
    public double getScore() {
        return newG.getScore();
    }

    /**
     * Return reward for ghosts eaten across all levels, with those eaten in a
     * single power pill run worth more.
     *
     * @return
     */
    public double getGhostReward() {
        return newG.getGhostReward();
    }

    public int getCurrentLevel() {
        return newG.getCurrentLevel();
    }

    public int getEatenPills() {
        return newG.getEatenPills();
    }

    public int getTotalTime() {
        return newG.getTotalTime();
    }

    public int getPacmanNumberOfLivesRemaining() {
        return newG.getPacmanNumberOfLivesRemaining();
    }

    public int getCurrentLevelTime() {
        return newG.getCurrentLevelTime();
    }

    public int getPacmanCurrentNodeIndex() {
        return newG.getPacmanCurrentNodeIndex();
    }

    public int getPacmanLastMoveMade() {
        return moveToIndex(newG.getPacmanLastMoveMade());
    }

    public boolean anyRequiresAction() {
        return anyRequiresAction(newG);
    }

    public boolean anyIsEdible() {
        return anyIsEdible(newG);
    }

    /**
     * Given node index, an array of size 4 is returned containing the
     * neighboring node indices. Each slot in the array corresponds to a
     * specific direction, and if there is no neighbor in the given direction,
     * then slot is filled with a -1
     *
     * @param current start node
     * @return array of neighbor nodes with -1 for walls
     */
    public int[] neighbors(int current) {
        assert current != -1 : "-1 is not a valid node index";
        return neighbors(newG, current);
    }

    public int neighborInDir(int current, int dir) {
        return neighbors(current)[dir];
    }

    /**
     * The index for the direction pacman came from is -1.
     *
     * @param current = position to get neighbors of
     * @param lastMove = exclude opposite of this direction
     * @return neighbors without source node
     */
    public int[] restrictedNeighbors(int current, int lastMove) {
        int[] neighbors = neighbors(current);
        assert neighbors[0] != current : "The upward neighbor of " + current + " is " + neighbors[0] + ":" + Arrays.toString(neighbors);
        assert neighbors[1] != current : "The right neighbor of " + current + " is " + neighbors[1] + ":" + Arrays.toString(neighbors);
        assert neighbors[2] != current : "The downward neighbor of " + current + " is " + neighbors[2] + ":" + Arrays.toString(neighbors);
        assert neighbors[3] != current : "The left neighbor of " + current + " is " + neighbors[3] + ":" + Arrays.toString(neighbors);
        if (lastMove == -1) {
            return neighbors;
        }
        neighbors[getReverse(lastMove)] = -1;
        return neighbors;
    }

    /**
     * Whether or not a ghost reversal occurred on the current time step
     *
     * @return true or false
     */
    public boolean ghostReversal() {
        return newG.getTimeOfLastGlobalReversal() == newG.getTotalTime();
    }

    public int getTimeOfLastGlobalReversal() {
        return newG.getTimeOfLastGlobalReversal();
    }

    public int timeSinceLastGlobalReversal() {
        int timeOfReversal = newG.getTimeOfLastGlobalReversal();
        //System.out.print("timeOfLastGlobalReversal:" + timeOfReversal+":");
        return timeOfReversal == -1 ? -1 : newG.getTotalTime() - timeOfReversal;
    }

    public int maxEdibleTime() {
        return maxEdibleTime(newG);
    }

    public int getGhostCurrentNodeIndex(int ghostIndex) {
        return newG.getGhostCurrentNodeIndex(indexToGhost(ghostIndex));
    }

    public int getGhostLastMoveMade(int ghostIndex) {
        return moveToIndex(newG.getGhostLastMoveMade(indexToGhost(ghostIndex)));
    }

    public int[] getShortestPath(int from, int to) {
        int[] result = newG.getShortestPath(from, to);
        assert (validPath(result)) : "Invalid path! " + Arrays.toString(result) + ":" + ("new");
        assert (result.length == 0 || result[result.length - 1] == to) : "Last element of path should be the to location! " + ("new");
        assert (result.length == 0 || result[0] != from) : "Path should NOT start at  location! " + ("new");
        return result;
    }

    public boolean isGhostEdible(int ghostIndex) {
        return newG.isGhostEdible(indexToGhost(ghostIndex));
    }

    /**
     * Returns node index for next junction along current path. If current
     * direction dead-ends into a corner, then there are only two junctions on
     * either side of pacman: it is assumed the "next" junction is the one in
     * the alternate direction to the one pacman came from, which would be the
     * opposite of currentDir
     *
     * @param current = node index to start from
     * @param currentDir = last direction moved in
     * @return node index of first junction encountered in given direction.
     */
    public int nextJunctionInDirection(int current, int currentDir) {
        return nextJunctionInDirection(current, currentDir, false);
    }

    public int nextJunctionInDirection(int current, int currentDir, boolean powerPillsToo) {
        int[] neighbors = restrictedNeighbors(current, currentDir);
        int numBlocked = ArrayUtil.countOccurrences(-1, neighbors);
        int pos = -1;
        int move = -1;
        switch (numBlocked) {
            case 2:
                if (neighbors[currentDir] == -1) {
                    // Facing a T-junction, so there is no junction "ahead", only to the sides
                    return -1;
                }
            case 1:
                pos = neighbors[currentDir];
                move = currentDir;
                break;
            case 3:
                // One option
                pos = ArrayUtil.filter(neighbors, -1)[0];
                move = ArrayUtil.position(neighbors, pos);
                break;
            default:
                // No option
                System.out.println("Problem in nextJunctionInDirection(" + current + "," + currentDir + ") : " + Arrays.toString(neighbors));
                System.exit(1);
        }
//        int pillsLeft = newG.getNumberOfActivePills() + newG.getNumberOfActivePowerPills();
//        int pillsEaten = 0;
        while (!isJunction(pos) && (!powerPillsToo || !isPowerPillIndex(pos))) { // Go until a junction is reached
            // Special case: eating the last pill ends the level
//            if(newG.getPillIndex(pos) != -1 || newG.getPowerPillIndex(pos) != -1) {
//                pillsEaten++;
//                if(pillsLeft == pillsEaten){
//                    return pos;
//                }
//            }
            neighbors = restrictedNeighbors(pos, move);
            pos = ArrayUtil.filter(neighbors, -1)[0];
            move = ArrayUtil.position(neighbors, pos);
        }
        return pos;
    }

    /**
     * Return next direction to go to get around corner
     *
     * @param current = current location, MUST be an elbow/corner
     * @param currentDir = current pacman dir
     * @return direction to get through elbow.
     */
    public int nextMoveAtElbow(int current, int currentDir) {
        int[] neighbors = restrictedNeighbors(current, currentDir);
        if (neighbors[currentDir] != -1) {
            return currentDir;
        }
        int numBlocked = ArrayUtil.countOccurrences(-1, neighbors);
        assert (numBlocked == 3) :
                "Asked for elbow move, but not at elbow!\n"
                + current + ":" + currentDir + ":" + Arrays.toString(neighbors) + ":" + Arrays.toString(neighbors(current));

        int pos = ArrayUtil.filter(neighbors, -1)[0];
        int move = ArrayUtil.position(neighbors, pos);

        return move;
    }

    public boolean isPowerPillIndex(int index) {
        return ArrayUtil.member(index, newG.getActivePowerPillsIndices());
    }

    public boolean isJunction(int current) {
        return newG.isJunction(current);
    }

    /**
     * Returns true if node index is a corner in the maze, meaning there are two
     * routes out, but neither is directly in the opposite direction of the
     * other
     *
     * @param current = node index
     * @return whether the node is an elbow/corner
     */
    public boolean isElbow(int current) {
        int[] neighbors = neighbors(current);
        int numBlocked = ArrayUtil.countOccurrences(-1, neighbors);
        if (numBlocked == 2) { // Possible elbow
            // One open path
            int open = ArrayUtil.filter(neighbors, -1)[0];
            int move = ArrayUtil.position(neighbors, open);
            if (-1 == neighbors[getReverse(move)]) {
                return true;
            }
        }
        return false;
    }

    public double getEuclideanDistance(int from, int to) {
        return newG.getEuclideanDistance(from, to);
    }

    public double getShortestPathDistance(int from, int to) {
        return newG.getShortestPathDistance(from, to);
    }

    public int[] getJunctionIndices() {
        return newG.getJunctionIndices();
    }

    /**
     * Number of neighbors around node that are not walls
     *
     * @param node in maze
     * @return number open neighbors
     */
    public int getNumNeighbours(int node) {
        return newG.getNeighbouringNodes(node).length;
    }

    public boolean hasNeighbors(int node) {
        return this.getNumNeighbours(node) > 0;
    }

    public int getMazeIndex() {
        return newG.getMazeIndex();
    }

    /**
     * Shortest path from "from" to "to" in given "direction"
     * @param from starting point, will NOT be in final path result
     * @param to end point, will be last member of path array returned
     * @param direction first step of path is in this direction from "from"
     * @return shortest directional path
     */
    public int[] getDirectionalPath(int from, int to, int direction) {
        int[] result = getPathInDirFromNew(from, to, direction);
        assert (validPath(result)) : ("Invalid path! " + Arrays.toString(result));
        assert (result[result.length - 1] == to) : ("Last element of path should be the to location!");
        assert (result[0] != from) : ("Path should NOT start at  location!");
        return result;
    }

    public int[] getActivePowerPillsIndices() {
        return newG.getActivePowerPillsIndices();
    }

    public int getGhostLairTime(int ghostIndex) {
        return newG.getGhostLairTime(indexToGhost(ghostIndex));
    }

    public int[] getActivePillsIndices() {
        return newG.getActivePillsIndices();
    }

    public int getClosestNodeIndexFromNodeIndex(int current, int[] targets) {
        return newG.getClosestNodeIndexFromNodeIndex(current, targets, DM.PATH);
    }

    public void addPoints(Color c, Set<Integer> nodes) {
        addPoints(c, ArrayUtil.integerSetToArray(nodes));
    }

    public void addPoints(Color c, int[] nodes) {
        if (nodes.length > 0) {
            //System.out.println(c +":" + Arrays.toString(nodes));
            pacman.game.GameView.addPoints(newG, c, ArrayUtil.filter(nodes, -1));
        }
    }

    public double getPathDistance(int from, int to) {
        return newG.getDistance(from, to, DM.PATH);
    }

    public boolean pacmanHittingWall() {
        return newG.getPacmanLastMoveMade().equals(MOVE.NEUTRAL);
    }

    /**
     * This is the lair exit
     * @return node of the lair exit
     */
    public int getGhostInitialNodeIndex() {
        return newG.getGhostInitialNodeIndex();
    }

    public void addLines(Color c, int from, int to) {
        pacman.game.GameView.addLines(newG, c, from, to);

    }

    public int getGhostEdibleTime(int ghostIndex) {
        return newG.getGhostEdibleTime(indexToGhost(ghostIndex));
    }

    public int getNextGhostDirTowards(int whichGhost, int to) {
        return moveToIndex(newG.getApproximateNextMoveTowardsTarget(getGhostCurrentNodeIndex(whichGhost), to, newG.getGhostLastMoveMade(indexToGhost(whichGhost)), DM.PATH));
    }

    public int getNextGhostDirAway(int whichGhost, int to) {
        return moveToIndex(newG.getApproximateNextMoveAwayFromTarget(getGhostCurrentNodeIndex(whichGhost), to, newG.getGhostLastMoveMade(indexToGhost(whichGhost)), DM.PATH));
    }

    /**
     * Returns direction pacman should move in to reach "to" given that pacman
     * cannot go in reverse from the lastDir direction it came from.
     *
     * @param from starting point
     * @param to destination
     * @param lastDir last move made (prevent reversing)
     * @return move
     */
    public int getRestrictedNextDir(int from, int to, int lastDir) {
        return moveToIndex(newG.getApproximateNextMoveTowardsTarget(from, to, indexToMove(lastDir), DM.PATH));
    }

    public int getNextPacManDirTowardsTarget(int to) {
        return moveToIndex(newG.getNextMoveTowardsTarget(newG.getPacmanCurrentNodeIndex(), to, DM.PATH));
    }

    public int getNextPacManDirAwayFromTarget(int to) {
        return moveToIndex(newG.getNextMoveAwayFromTarget(newG.getPacmanCurrentNodeIndex(), to, DM.PATH));
    }

    public int getNumberOfPills() {
        return newG.getNumberOfPills();
    }

    public int getNumberOfPowerPills() {
        return newG.getNumberOfPowerPills();
    }

    public int getGhostCurrentEdibleScore() {
        return newG.getGhostCurrentEdibleScore();
    }

    public int getNodeXCoord(int current) {
        return newG.getNodeXCood(current);
    }

    public int getNodeYCoord(int current) {
        return newG.getNodeYCood(current);
    }

    public int[] getPillIndices() {
        return newG.getPillIndices();
    }

    public int[] getPowerPillIndices() {
        return newG.getPowerPillIndices();
    }

    public boolean nodeInMaze(int index) {
        return index < newG.getCurrentMaze().graph.length;
    }

    public boolean allNodesInMaze(int[] indices) {
        for (int i = 0; i < indices.length; i++) {
            if (indices[i] != -1 && !nodeInMaze(indices[i])) {
                return false;
            }
        }
        return true;
    }

    /**
     * From fromNodeIndex heading in direction, find the closest node within
     * targetNodeIndices and return a pair of both the target and the path to
     * it.
     *
     * No "from" at start, but "to" is at the end.
     *
     * @param fromNodeIndex
     * @param targetNodeIndices
     * @param direction
     * @return
     */
    public Pair<Integer, int[]> getTargetInDir(int fromNodeIndex, int[] targetNodeIndices, int direction) {
        return getTargetInDir(fromNodeIndex, targetNodeIndices, direction, true); // default to shortest
    }
    
    public Pair<Integer, int[]> getTargetInDir(int fromNodeIndex, int[] targetNodeIndices, int direction, boolean shortest) {
        assert fromNodeIndex != -1 : "Invalid from node: " + fromNodeIndex;
        assert direction >= 0 && direction <= 3 : "Not a valid direction: " + direction;
        Pair<Integer, int[]> result = getTargetInDirFromNew(fromNodeIndex, targetNodeIndices, direction, shortest);
        assert (result != null && result.t2 != null) : ("Why is pair null? " + result);
        assert (validPath(result.t2)) : ("Invalid path! " + Arrays.toString(result.t2));
        assert (result.t2.length == 0 || result.t2[0] != fromNodeIndex) : ("Path should NOT start at  location!");
        return result;
    }

    /**
     * Can return either the shortest or longest path in a given direction to any one
     * of several available targets. The chosen target is returned as well, in a pair.
     * @param fromNodeIndex start point
     * @param targetNodeIndices potential targets
     * @param direction direction pacman must go in
     * @param shortest true for shortest path, longest path otherwise
     * @return path and target pair
     */
    private Pair<Integer, int[]> getTargetInDirFromNew(int fromNodeIndex, int[] targetNodeIndices, int direction, boolean shortest) {
        assert targetNodeIndices.length > 0 : "targetNodeIndices empty:" + Arrays.toString(targetNodeIndices);
        int[] neighbors = neighbors(newG, fromNodeIndex);
        assert (neighbors[direction] != -1) : ("Picked invalid direction " + direction + " given neighbors " + Arrays.toString(neighbors) + " in level " + newG.getCurrentLevel());
        double extremeDistance = shortest ? Integer.MAX_VALUE : -Integer.MAX_VALUE;
        int target = -1;
        int[] extremePath = null;
        for (int i = 0; i < targetNodeIndices.length; i++) {
            if (targetNodeIndices[i] == -1) {
                continue;
            }
            assert targetNodeIndices[i] < newG.getCurrentMaze().graph.length : targetNodeIndices[i] + " is not an index in the maze " + newG.getCurrentLevel() + "/" + newG.getCurrentMaze().name + " : " + Arrays.toString(targetNodeIndices) + ":" + targetNodeIndices.length;
            int[] path = getPathInDirFromNew(fromNodeIndex, targetNodeIndices[i], direction);
            assert (path.length == 0 || path[path.length - 1] == targetNodeIndices[i]) : ("Last element of path should be the to location! " + ("new"));
            assert (path.length == 0 || path[0] != fromNodeIndex) : ("Path should NOT start at  location! " + ("new"));
            // Shortest distance lower bound on direction distance
            if (shortest ? path.length < extremeDistance : path.length > extremeDistance) {
                extremeDistance = path.length;
                target = targetNodeIndices[i];
                extremePath = path;
            }
        }
        assert extremePath != null : "Extreme path is null: targetNodeIndices:" + Arrays.toString(targetNodeIndices) + ":extremeDistance:" + extremeDistance;
        return new Pair<Integer, int[]>(target, extremePath);
    }

    public double getGhostPathDistance(int ghostIndex, int toNodeIndex) {
        return newG.getDistance(getGhostCurrentNodeIndex(ghostIndex), toNodeIndex, newG.getGhostLastMoveMade(indexToGhost(ghostIndex)), DM.PATH);
    }
    
    /**
     * Determines how long it will take a ghost to reach a given destination,
     * factoring int speed reduction from being edible.
     * @param ghostIndex
     * @param toNodeIndex
     * @return 
     */
    public int getGhostTravelTime(int ghostIndex, int toNodeIndex) {
        int distance = (int) getGhostPathDistance(ghostIndex, toNodeIndex);
        int edibleTime = this.getGhostEdibleTime(ghostIndex);
        int effectiveEdibleTime = Math.min(edibleTime, distance*Constants.GHOST_SPEED_REDUCTION);
        return effectiveEdibleTime + distance - ((int)Math.ceil(effectiveEdibleTime / Constants.GHOST_SPEED_REDUCTION));
    }
    
    public boolean isGhostEdibleAfterTravel(int ghostIndex, int toNodeIndex) {
        return this.getGhostEdibleTime(ghostIndex) > getGhostTravelTime(ghostIndex, toNodeIndex);
    }

    /**
     * Get the shortest path that a specific ghost could possibly take to reach
     * the target location. The ghost is restricted in that it cannot reverse
     * the last move it made. If non-empty, the last index of the array will be
     * the same as the target. The ghost's position is not included in the
     * array.
     *
     * @param ghostIndex ghost id
     * @param target target ghost is approaching
     * @return shortest path ghost can take as array of int
     */
    public int[] getGhostPath(int ghostIndex, int target) {
        int[] result = newG.getShortestPath(getGhostCurrentNodeIndex(ghostIndex), target, newG.getGhostLastMoveMade(indexToGhost(ghostIndex)));
        //this.addPoints(CombinatoricUtilities.colorFromInt(ghostIndex), result);
        assert (result.length == 0 || result[result.length - 1] == target) : ("Last element of path should be the to location! " + ("new"));
        assert (result.length == 0 || result[0] != this.getGhostCurrentNodeIndex(ghostIndex)) : ("Path should NOT start at  location! " + ("new"));
        return result;
    }

    public int getNumberOfEdibleGhosts() {
        int total = 0;
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (newG.isGhostEdible(indexToGhost(i))) {
                total++;
            }

        }
        return total;
    }

    public boolean isGhostThreat(int ghostIndex) {
        return !isGhostEdible(ghostIndex) && getNumNeighbours(getGhostCurrentNodeIndex(ghostIndex)) > 0;
    }

    /**
     * Lair time of each active ghost, including those not in lair (value of 0)
     * @return 
     */
    public int[] getGhostLairTimes() {
        int[] times = new int[CommonConstants.numActiveGhosts];
        for (int i = 0; i < times.length; i++) {
            times[i] = this.getGhostLairTime(i);
        }
        return times;
    }

    public int[] getGhostEdibleTimes() {
        int[] times = new int[CommonConstants.numActiveGhosts];
        for (int i = 0; i < times.length; i++) {
            times[i] = this.getGhostEdibleTime(i);
        }
        return times;
    }

    public int getFarthestNodeIndexFromNodeIndex(int current, int[] targets) {
        return newG.getFarthestNodeIndexFromNodeIndex(current, targets, DM.PATH);
    }

    public int[] getEdibleGhostLocations() {
        return getEdibleGhostLocations(new boolean[]{true, true, true, true});
    }

    public int[] getEdibleGhostLocations(boolean[] include) {
        ArrayList<Integer> ghostPositions = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (include[i] && isGhostEdible(i)) {
                ghostPositions.add(getGhostCurrentNodeIndex(i));
            }
        }
        return ArrayUtil.intArrayFromArrayList(ghostPositions);
    }

    /**
     * True if threat is coming at pacman along direction
     *
     * @param pacmanDir direction relative to pacman
     * @return
     */
    public boolean isThreatIncoming(int pacmanDir) {
        return isAnyGhostIncoming(pacmanDir, true);
    }

    public boolean isEdibleIncoming(int pacmanDir) {
        return isAnyGhostIncoming(pacmanDir, false);
    }

    public boolean isAnyGhostIncoming(int pacmanDir, boolean threatNotEdible) {
        for (int i = 0; i < this.getNumActiveGhosts(); i++) {
            if ((threatNotEdible && isGhostThreat(i))
                    || (!threatNotEdible && isGhostEdible(i))) {
                if (isGhostIncoming(pacmanDir, i)) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * True if ghost is coming at pacman along a path that goes through the
     * neighbor of pacman in direction "pacmanDir"
     *
     * @param pacmanDir direction from pacman of neighbor
     * @param ghostIndex ghost that may be approaching along that neighbor
     * @return true if ghost is approaching through that neighbor
     */
    public boolean isGhostIncoming(int pacmanDir, int ghostIndex) {
        int current = this.getPacmanCurrentNodeIndex();
        int[] neighbors = this.neighbors(current);
        assert neighbors[pacmanDir] != -1 : "Pacman dir is a wall: " + pacmanDir + "; " + Arrays.toString(neighbors);
        int[] ghostPath = getGhostPath(ghostIndex, current);
        return ArrayUtil.member(neighbors[pacmanDir], ghostPath);
    }

    /**
     * Returns true if there are no junctions between the specified ghost
     * and pacman facing in the given direction
     * @param pacmanDir Direction pacman could face
     * @param ghostIndex specific ghost
     * @return True if there are no junctions from pacman to ghost in direction
     */
    public boolean isGhostTrapped(int pacmanDir, int ghostIndex) {
        int current = this.getPacmanCurrentNodeIndex();
        int[] neighbors = this.neighbors(current);
        assert neighbors[pacmanDir] != -1 : "Pacman dir is a wall: " + pacmanDir + "; " + Arrays.toString(neighbors);
        int[] pacmanPath = this.getDirectionalPath(current, this.getGhostCurrentNodeIndex(ghostIndex), pacmanDir);
        int[] junctions = this.getJunctionIndices();
        return ArrayUtil.intersection(pacmanPath, junctions).length == 0;
    }
    
    /**
     * Array containing locations of as many ghosts that are threats. Edible
     * ghosts and ghosts in the lair are not present at all.
     *
     * @return
     */
    public int[] getThreatGhostLocations() {
        return getThreatGhostLocations(new boolean[]{true, true, true, true});
    }

    public int[] getThreatGhostLocations(boolean[] include) {
        ArrayList<Integer> ghostPositions = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (include[i] && isGhostThreat(i)) {
                ghostPositions.add(getGhostCurrentNodeIndex(i));
            }
        }
        return ArrayUtil.intArrayFromArrayList(ghostPositions);
    }

    /**
     * Get the locations of ghosts that are directly pursuing pacman along the
     * shortest possible path.
     *
     * @return
     */
    public int[] getApproachingThreatGhostLocations() {
        return getApproachingThreatGhostLocations(new boolean[]{true, true, true, true});
    }

    public int[] getApproachingThreatGhostLocations(boolean[] include) {
        ArrayList<Integer> ghostPositions = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (include[i] && isGhostThreat(i) && ghostApproachingPacman(i)) {
                ghostPositions.add(getGhostCurrentNodeIndex(i));
            }
        }
        return ArrayUtil.intArrayFromArrayList(ghostPositions);
    }

    /**
     * "Incoming" is different from "approaching". A ghost is approaching pacman
     * if it is taking the shortest possible path to reach pacman. The concept
     * of incoming is defined with respect to a specific direction away from
     * where pacman currently is. A ghost is "incoming" from this direction if a
     * direct path from it to pacman comes in along this direction.
     *
     * @param pacmanDir
     * @return
     */
    public int[] getIncomingThreatGhostLocations(int pacmanDir) {
        return getIncomingThreatGhostLocations(pacmanDir, new boolean[]{true, true, true, true});
    }

    public int[] getIncomingThreatGhostLocations(int pacmanDir, boolean[] include) {
        ArrayList<Integer> ghostPositions = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (include[i] && isGhostThreat(i) && isGhostIncoming(pacmanDir, i)) {
                ghostPositions.add(getGhostCurrentNodeIndex(i));
            }
        }
        return ArrayUtil.intArrayFromArrayList(ghostPositions);
    }

    public int[] getApproachingOrIncomingThreatGhostLocations(int pacmanDir) {
        return getApproachingOrIncomingThreatGhostLocations(pacmanDir, new boolean[]{true, true, true, true});
    }

    public int[] getApproachingOrIncomingThreatGhostLocations(int pacmanDir, boolean[] include) {
        ArrayList<Integer> ghostPositions = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (include[i] && isGhostThreat(i)
                    && (isGhostIncoming(pacmanDir, i) || ghostApproachingPacman(i))) {
                ghostPositions.add(getGhostCurrentNodeIndex(i));
            }
        }
        return ArrayUtil.intArrayFromArrayList(ghostPositions);
    }

    public int[] getApproachingOrIncomingEdibleGhostLocations(int pacmanDir) {
        return getApproachingOrIncomingEdibleGhostLocations(pacmanDir, new boolean[]{true, true, true, true});
    }

    public int[] getApproachingOrIncomingEdibleGhostLocations(int pacmanDir, boolean[] include) {
        ArrayList<Integer> ghostPositions = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (include[i] && isGhostEdible(i)
                    && (isGhostIncoming(pacmanDir, i) || ghostApproachingPacman(i))) {
                ghostPositions.add(getGhostCurrentNodeIndex(i));
            }
        }
        return ArrayUtil.intArrayFromArrayList(ghostPositions);
    }

    /**
     * Get the locations of edible ghosts that are directly approaching pacman
     * along the shortest possible path.
     *
     * @return
     */
    public int[] getApproachingEdibleGhostLocations() {
        return getApproachingEdibleGhostLocations(new boolean[]{true, true, true, true});
    }

    public int[] getApproachingEdibleGhostLocations(boolean[] include) {
        ArrayList<Integer> ghostPositions = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (include[i] && isGhostEdible(i) && ghostApproachingPacman(i)) {
                ghostPositions.add(getGhostCurrentNodeIndex(i));
            }
        }
        return ArrayUtil.intArrayFromArrayList(ghostPositions);
    }

    /**
     * Return locations of edible ghosts that are "incoming" along the given
     * direction relative to pacman's location.
     *
     * @param pacmanDir
     * @return
     */
    public int[] getIncomingEdibleGhostLocations(int pacmanDir) {
        return getIncomingEdibleGhostLocations(pacmanDir, new boolean[]{true, true, true, true});
    }

    public int[] getIncomingEdibleGhostLocations(int pacmanDir, boolean[] include) {
        ArrayList<Integer> ghostPositions = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (include[i] && isGhostEdible(i) && isGhostIncoming(pacmanDir, i)) {
                ghostPositions.add(getGhostCurrentNodeIndex(i));
            }
        }
        return ArrayUtil.intArrayFromArrayList(ghostPositions);
    }

    /**
     * Return true if the current shortest path to pacman that the ghost can
     * possibly take (keeping in mind no reversal restrictions) is the same as
     * the direction for the absolute shortest path
     *
     * @param ghostIndex which ghost to check
     * @return true if directly approaching pacman
     */
    public boolean ghostApproachingPacman(int ghostIndex) {
        final int current = this.getPacmanCurrentNodeIndex();
        int[] ghostPath = getGhostPath(ghostIndex, current);
        int[] shortestPath = getShortestPath(getGhostCurrentNodeIndex(ghostIndex), current);
        // Paths could be different if two equal-length paths exist
        return (ghostPath.length == shortestPath.length);
    }

    /**
     * Returns positions of all ghosts that are not currently confined to the
     * lair, whether they are edible or threats.
     *
     * @return array with node indices of active ghosts
     */
    public int[] getActiveGhostLocations() {
        ArrayList<Integer> ghostPositions = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (this.getNumNeighbours(getGhostCurrentNodeIndex(i)) > 0) {
                ghostPositions.add(getGhostCurrentNodeIndex(i));
            }
        }
        return ArrayUtil.intArrayFromArrayList(ghostPositions);
    }

    /**
     * If a ghost is located at the ghostLocation, then the index for that ghost
     * is returned. Otherwise, -1 is returned.
     *
     * @param ghostLocation node index where ghost may be
     * @return index of ghost, or -1 if not found
     */
    public int[] getGhostIndexOfGhostAt(int ghostLocation) {
        ArrayList<Integer> locs = new ArrayList<Integer>();
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (getGhostCurrentNodeIndex(i) == ghostLocation) {
                locs.add(i);
            }
        }
        return ArrayUtil.intArrayFromArrayList(locs);
    }

    public int getNextMoveTowardsTarget(int from, int to) {
        return moveToIndex(newG.getNextMoveTowardsTarget(from, to, DM.PATH));
    }

    public int getNumActiveGhosts() {
        return CommonConstants.numActiveGhosts;
    }

    public void advanceGame(int pacManDir, int[] ghostDirs) {
        EnumMap<GHOST, MOVE> myMoves = new EnumMap<GHOST, MOVE>(GHOST.class);
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            myMoves.put(indexToGhost(i), indexToMove(ghostDirs[i]));
        }
        //System.out.println("Advance new: " + indexToMove(pacManDir) + ":" + myMoves);
        newG.advanceGame(indexToMove(pacManDir), myMoves);
    }

    /**
     * Simulate forward from pacman's current location to any node along the
     * most direct path.
     *
     * @param destination node to move towards
     * @param ghostModel how to model the movement of the ghosts
     * @return
     */
    public GameFacade simulateTowardsLocation(int destination, GhostControllerFacade ghostModel) {
        int startLevel = this.getCurrentLevel();
        int previousLives = getPacmanNumberOfLivesRemaining();
        GameFacade copy = this;
        //int steps = 0;
        //LinkedList<Integer> pathTraversed = new LinkedList<Integer>();
        while (copy.getPacmanCurrentNodeIndex() != destination
                && copy.getCurrentLevel() == startLevel
                && !copy.gameOver()) {
            int simCurrent = copy.getPacmanCurrentNodeIndex();
            //pathTraversed.add(simCurrent);
            int dir = copy.getNextMoveTowardsTarget(simCurrent, destination);
            copy = copy.simulateInDir(dir, ghostModel);
            //steps++;
            if (previousLives > copy.getPacmanNumberOfLivesRemaining()) {
                return null;
            }
            // must be updated in case of a life gain
            previousLives = copy.getPacmanNumberOfLivesRemaining();
        }
        return copy;
    }

    /**
     * Given a ghost team (a model of how the ghosts will behave), and a
     * direction for pacman to move, simulate pacman's movement along that
     * direction, including turning at elbows, until the target is reached
     * (targets will usually be junctions and power pill locations), or new
     * level, or game over. Return the resulting game state within a new
     * GameFacade.
     *
     * Returns null if pacman died. If pacman reached the target, then the
     * current location of pacman in the returned facade will equal the target
     *
     * pre: number of current pacman lives is greater than 0
     *
     * @param dir direction to go
     * @param ghostModel how the ghosts will behave
     * @param target stop when reached
     * @return resulting game state, or null in case of death
     */
    public GameFacade simulateToNextTarget(int dir, GhostControllerFacade ghostModel, int target) {
        int startLevel = this.getCurrentLevel();
        int previousLives = getPacmanNumberOfLivesRemaining();
        GameFacade copy = this;
        int steps = 0;
        //LinkedList<Integer> pathTraversed = new LinkedList<Integer>();
        //LinkedList<Integer> movesMade = new LinkedList<Integer>();
        while (copy.getPacmanCurrentNodeIndex() != target
                && copy.getCurrentLevel() == startLevel
                && !copy.gameOver()) {
            int simCurrent = copy.getPacmanCurrentNodeIndex();
            //movesMade.add(dir);
            //pathTraversed.add(simCurrent);
            dir = steps == 0 ? dir : copy.getRestrictedNextDir(simCurrent, target, dir);
            copy = copy.simulateInDir(dir, ghostModel);
            steps++;
            if (previousLives > copy.getPacmanNumberOfLivesRemaining()) {
                return null;
            }
            // must be updated in case of a life gain
            previousLives = copy.getPacmanNumberOfLivesRemaining();
        }
        return copy;
    }

    /**
     * Simulate one step in direction, given model of how to move ghosts. Don't
     * allow reversals.
     *
     * @param dir direction to move
     * @param ghostModel how ghosts move
     * @return new game state
     */
    public GameFacade simulateInDir(int dir, GhostControllerFacade ghostModel) {
        GameFacade copy = this.copy();
        int[] ghostDirs = ghostModel.getActions(copy, 0);

        GameFacade backup = copy.copy();
        // Loop prevents reversals
        do {
            copy = backup.copy();
            copy.advanceGame(dir, ghostDirs);
        } while (copy.ghostReversal() && copy.getNumActivePowerPills() == backup.getNumActivePowerPills());
        return copy;
    }

    /**
     * Return true if a proposed path contains the location of a threatening
     * ghost
     *
     * @param path path for pacman to traverse
     * @return is threat on path?
     */
    public boolean pathGoesThroughThreateningGhost(int[] path) {
        int[] ghostLocs = this.getThreatGhostLocations();
        for (int i = 0; i < path.length; i++) {
            for (int g = 0; g < ghostLocs.length; g++) {
                if (path[i] == ghostLocs[g]) {
                    return true;
                }
            }
        }
        return false;
    }

    public double getScore(int level) {
        return newG.getScore(level);
    }

    public GameFacade copy() {
        return new GameFacade(newG.copy());
    }

    public static int getReverse(int move) {
        return moveToIndex(indexToMove(move).opposite());
    }

    public static int getLeftOf(int move) {
        return (move + 3) % 4;
    }

    public static int getRightOf(int move) {
        return (move + 1) % 4;
    }

    public int getNumActivePills() {
        return newG.getNumberOfActivePills();
    }

    public boolean gameOver() {
        return newG.gameOver();
    }

    public int getNumActivePowerPills() {
        return newG.getNumberOfActivePowerPills();
    }

    /**
     * To any given location, there may be multiple paths of equal length. The
     * default path calculation functions break ties, but this method considers
     * all equal length paths, and returns a collection that contains all nodes
     * along all such paths (no particular order).
     *
     * @param ghostIndex
     * @param to
     * @return
     */
    public int[] getAllGhostPathNodes(int ghostIndex, int to) {
        assert this.nodeInMaze(to) : "Node (to) " + to + " not in maze " + this.getMazeIndex();
        int from = this.getGhostCurrentNodeIndex(ghostIndex);
        assert this.nodeInMaze(from) : "Node (from) " + from + " not in maze " + this.getMazeIndex();
        int[] tempPath = this.getGhostPath(ghostIndex, to);
        int[] model = new int[tempPath.length + 1];
        System.arraycopy(tempPath, 0, model, 1, tempPath.length);
        model[0] = from;
//        if (model.length <= 1) {
//            return model;
//        }
        int[] options = this.restrictedNeighbors(from, this.getGhostLastMoveMade(ghostIndex));
        return sameDistancePathNodes(to, model, options);
    }

    private int[] sameDistancePathNodes(int to, int[] model, int[] options) {
        HashSet<Integer> set = new HashSet<Integer>();
        Queue<Pair<Integer, Integer>> junctionDistancePairs = new LinkedList<Pair<Integer, Integer>>();
        for (int i = 0; i < model.length; i++) {
            int node = model[i];
            if (isJunction(node)) {
                junctionDistancePairs.add(new Pair<Integer, Integer>(node, i));
            }
            set.add(node);
        }
        // Neighbors of start point
        for (int i = 0; i < options.length; i++) {
            if (options[i] != -1) {
                assert this.nodeInMaze(options[i]) : "Option " + options[i] + " not in maze " + this.getMazeIndex();
                assert this.nodeInMaze(model[0]) : "Model start " + model[0] + " not in maze " + this.getMazeIndex();
                assert this.nodeInMaze(to) : "To " + to + " not in maze " + this.getMazeIndex();
                assert options[i] == this.neighbors(model[0])[i] : "(using neighbors) The option " + options[i] + " in dir " + i + " does not correspond to the neighbor " + this.neighbors(model[0])[i] + " of " + model[0] + ":\nmodel = " + Arrays.toString(model) + ":\nmodel[0] neighbors = " + Arrays.toString(this.neighbors(model[0]));
                assert options[i] == this.neighborInDir(model[0], i) : "(using neighborInDir) The option " + options[i] + " in dir " + i + " does not correspond to the neighbor " + this.neighborInDir(model[0], i) + " of " + model[0] + ":\nmodel = " + Arrays.toString(model) + ":\nmodel[0] neighbors = " + Arrays.toString(this.neighbors(model[0]));

                int[] branch = this.getDirectionalPath(model[0], to, i);
                if (branch.length == model.length) {
                    //System.out.println("Immediate neighbor possible:" + options[i]);
                    // Alternate path is same length
                    for (int p = 1; p < branch.length; p++) {
                        Pair<Integer, Integer> pair = new Pair<Integer, Integer>(branch[p], p + 1);
                        if (isJunction(branch[p]) && !junctionDistancePairs.contains(pair)) {
                            junctionDistancePairs.add(pair);
                        }
                        if (set.contains(branch[p])) {
                            // One repeated node means rest of path can be ignored
                            break;
                        } else {
                            set.add(branch[p]);
                        }
                    }
                }
            }
        }
        // Branches at junctions
        while (!junctionDistancePairs.isEmpty()) {
            Pair<Integer, Integer> pair = junctionDistancePairs.poll();
            int junction = pair.t1;
            int soFar = pair.t2;
            int[] neighbors = neighbors(junction);
            for (int i = 0; i < neighbors.length; i++) {
                if (neighbors[i] != -1 && !set.contains(neighbors[i])) {
                    // Now check to see if path is right distance
                    int[] branch = this.getDirectionalPath(junction, to, i);
                    if (branch.length + soFar == model.length) {
                        //this.addPoints(Color.GRAY, branch);
                        // Alternate path is same length
                        for (int p = 1; p < branch.length; p++) {
                            Pair<Integer, Integer> pair2 = new Pair<Integer, Integer>(branch[p], p + soFar + 1);
                            if (isJunction(branch[p]) && !junctionDistancePairs.contains(pair2)) {
                                junctionDistancePairs.add(pair2);
                            }
                            if (set.contains(branch[p])) {
                                // One repeated node means rest of path can be ignored
                                break;
                            } else {
                                set.add(branch[p]);
                            }
                        }
                    }
                }
            }
        }
        // Prepare results
        int[] result = new int[set.size()];
        int in = 0;
        for (Integer node : set) {
            result[in++] = node;
        }
        return result;
    }

    public void addPoints(Color c, ArrayList<Integer> list) {
        this.addPoints(c, ArrayUtil.intArrayFromArrayList(list));
    }

    /**
     * Assumes agent can move in the given direction, i.e. the neighbor exists.
     * Calculates path is newG != null (using new pacman version).
     *
     * @param from from node (will not be in path)
     * @param to to node (will be at end of path)
     * @param direction direction to move in [0/UP, 1/RIGHT, 2/DOEN, 3/LEFT]
     * @return path from -> to in direction
     */
    public int[] getPathInDirFromNew(int from, int to, int direction) {
        /**
         * This method depends on the newG method getShortestPath, which
         * excludes the opposite of "direction". The other neighbors need to be
         * checked.
         */
        int[] neighbors = neighbors(newG, from);
        assert (neighbors[direction] != -1) : ("Picked invalid direction " + direction + " given neighbors " + Arrays.toString(neighbors));
        int[] finalPath;
        if (neighbors[getLeftOf(direction)] == -1 && neighbors[getRightOf(direction)] == -1) {
            // Can't go left or right, and getShortestPath prevents reverse, so getShortestPath
            // will return the desired directional path
            assert neighbors[direction] != -1 : from + "'s neighbor in dir " + direction + " not available towards " + to;
            finalPath = newG.getShortestPath(from, to, indexToMove(direction));
        } else {
            // Left and right are neighbors, so getShortestPath won't necessarily go in
            // "direction" first, so let's take one step
            int oneStepNode = neighbors[direction];
            // Path after first step
            int[] pathAfterStep = newG.getShortestPath(oneStepNode, to, indexToMove(direction));
            //put back the first step
            int[] resultPath = new int[pathAfterStep.length + 1];
            resultPath[0] = oneStepNode;
            System.arraycopy(pathAfterStep, 0, resultPath, 1, pathAfterStep.length);
            finalPath = resultPath;
        }
        assert (finalPath.length == 0 || finalPath[finalPath.length - 1] == to) : ("Last element of path should be the to location!");
        assert (finalPath.length == 0 || finalPath[0] != from) : ("Path should NOT start at  location!");
        assert (validPath(finalPath)) : "Invalid path! " + Arrays.toString(finalPath);
        return finalPath;
    }

    /**
     * Too expensive to actually run this on every path
     *
     * @param path
     * @return
     */
    private boolean validPath(int[] path) {
    	System.out.print("validPath!!");
        if (path == null) {
            return false;
        }
        for (int i = 1; i < path.length; i++) {
            //System.out.println(path.length +":" + i);
            if (!ArrayUtil.member(path[i], neighbors(path[i - 1]))) {
                return false;
            }
        }
        return true;
    }

    /**
     * Return true if any ghost currently requires an action
     *
     * @param newG instance of new pacman Game
     * @return true if ghost requires action
     */
    private static boolean anyRequiresAction(Game newG) {
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (newG.doesGhostRequireAction(indexToGhost(i))) {
                return true;
            }
        }
        return false;
    }
    
    public boolean doesGhostRequireAction(int ghostIndex) {
        return newG.doesGhostRequireAction(indexToGhost(ghostIndex));
    }

    /**
     * Return max edible time across all ghosts
     *
     * @param newG new pacman Game instance
     * @return max edible time
     */
    private static int maxEdibleTime(Game newG) {
        int max = -1;
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            max = Math.max(max, newG.getGhostEdibleTime(indexToGhost(i)));
        }
        return max;
    }

    /**
     * Return true if any ghost is edible
     *
     * @param newG new pacman Game instance
     * @return any ghost edible?
     */
    private boolean anyIsEdible(Game newG) {
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (newG.isGhostEdible(indexToGhost(i))) {
                return true;
            }
        }
        return false;
    }

    public boolean justAtePowerPill() {
        return newG.wasPowerPillEaten();
    }

    /**
     * Given some path that pacman wants to follow (must be short, to the next
     * junction or power pill), return a pair: first is pacman's effective
     * distance to destination, and then nearest threat's effective distance to
     * destination. "Effective" because movement restrictions on ghosts are
     * considered, as well as lair times. The "distance" may be 0 if a ghost can
     * reach it first.
     *
     * @param path
     * @param target
     * @return
     */
    public Pair<Double, Double> closestThreatToPacmanPath(int[] path, int target) {
        double closestThreatDistance = Double.MAX_VALUE;
        double pacManDistance = path.length;
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            if (isGhostThreat(i)) { // Ghost is a threat
                double distanceToNode;

                int[] gPath = getGhostPath(i, target);
                /*
                 * If ghost path is subset of pacman path on way to same
                 * location, then ghost must be moving away from pacman
                 */
                if (pathGoesThroughThreateningGhost(path)) {
                    if (ArrayUtil.subset(gPath, path)) {
                        //System.out.println("Follow behind ghost");
                        distanceToNode = pacManDistance + GameFacade.MAX_DISTANCE; // Ghost will go through node, making it safe to follow
                    } else {
                        distanceToNode = 0; // Really bad
                    }
                } else {
                    distanceToNode = gPath.length;
                }

                closestThreatDistance = Math.min(closestThreatDistance, distanceToNode);
            } else if (getGhostLairTime(i) > 0) {
                // Ghost may pop out when pacman passes
                int ghostStart = getGhostInitialNodeIndex();
                if (ArrayUtil.member(ghostStart, path)) {
                    pacManDistance = getShortestPathDistance(getPacmanCurrentNodeIndex(), ghostStart);
                    int lairTime = getGhostLairTime(i);
                    closestThreatDistance = Math.min(closestThreatDistance, lairTime);
                }
            }
        }
        return new Pair<Double, Double>(pacManDistance, closestThreatDistance);
    }

    public int getTimeGhostReward() {
        return newG.getTimeGhostReward();
    }

    public double getTimePillReward() {
        return newG.getTimePillReward();
    }

    public List<Integer> getGhostEatTimes() {
        return newG.getGhostEatTimes();
    }

    public void setEndAfterGhostEatingChances(boolean endAfterGhostEatingChances) {
        newG.setEndAfterGhostEatingChances(endAfterGhostEatingChances);
    }

    public void playWithoutPills(boolean noPills) {
        if (noPills) {
            newG.playWithoutPills();
        } else {
            newG.playWithPills();
        }
    }

    public void playWithoutPowerPills(boolean noPowerPills) {
        if (noPowerPills) {
            newG.playWithoutPowerPills();
        } else {
            newG.playWithPowerPills();
        }
    }

    public double getLureDistanceSum() {
        return newG.getLureDistanceSum();
    }

    public void setEndAfterPowerPillsEaten(boolean luringTask) {
        newG.setEndAfterPowerPillsEaten(luringTask);
    }

    public double getTimeInDeadSpace() {
        return newG.getTimeInDeadSpace();
    }

    /**
     * Given a collection of targets, pair-wise shortest paths are computed
     * between them all, and the nodes contained in all such paths are collected
     * in a Set that is returned.
     *
     * @param targets
     * @return
     */
    public Set<Integer> clusterTreeNodes(int[] targets) {
        HashSet<Integer> result = new HashSet<Integer>();
        for (int i = 0; i < targets.length; i++) {
            for (int j = 0; j < targets.length; j++) {
                if (i != j && hasNeighbors(targets[i]) && hasNeighbors(targets[j])) {
                    int[] path = this.getShortestPath(targets[i], targets[j]);
                    for (Integer x : path) {
                        result.add(x);
                    }
                }
            }
        }
        return result;
    }

    public int threatGhostClusterTreeSize() {
        return clusterTreeNodes(this.getThreatGhostLocations()).size();
    }

    public int edibleGhostClusterTreeSize() {
        return clusterTreeNodes(this.getEdibleGhostLocations()).size();
    }

    public int getNumMazeNodes() {
        return newG.getCurrentMaze().graph.length;
    }

    public int ghostLocationByProximity(int order) {
        ArrayList<Integer> ghosts = new ArrayList<Integer>(CommonConstants.numActiveGhosts);
        for (int i = 0; i < getNumActiveGhosts(); i++) {
            ghosts.add(i); // Put indices of ghosts in array
        }
        Collections.sort(ghosts, new GhostComparator(this, true, true));
        return getGhostCurrentNodeIndex(ghosts.get(order));
    }

    public double getProperlyEatenPowerPills() {
        return newG.getProperlyEatenPowerPills();
    }

    public double getImproperlyEatenPowerPills() {
        return newG.getImproperlyEatenPowerPills();
    }

    public double getPowerPillsEatenWhenGhostFar() {
        return newG.getPowerPillsEatenWhenGhostFar();
    }

    /**
     * Return true if any ghost is outside of the lair and not edible.
     *
     * @return
     */
    public boolean anyIsThreat() {
        return this.getThreatGhostLocations().length > 0;
    }

    public int getGhostRegret() {
        return newG.getGhostRegret();
    }

    /**
     * From a given start index, search in each direction until junctions are
     * found. If includePowerPills is true, then they are included in this
     * check. The indices found are considered to be at depth 1 away from the
     * start. If the process is repeated with each of these nodes as start
     * points, then the nodes found are at depth 2 and so on. All such nodes are
     * returned in an array list organized to contain a set corresponding to
     * each depth, up to the original depth parameter. A depth of 0 contains
     * just the start point.
     *
     * @param startIndex starting point of search
     * @param depth depth of search tree from starting point, branching at
     * junctions and maybe power pills
     * @param includePowerPills whether or not to junction at power pills
     * @return collection of leaves in search tree
     */
    public ArrayList<Set<Integer>> junctionsAtDepth(int startIndex, int depth, boolean includePowerPills, int lastNodeVisited) {
        Set<Integer> start = new HashSet<Integer>();
        // eliminate most recently visited node as option
        if (lastNodeVisited != -1 && !isJunction(startIndex)) {
            startIndex = lastNodeVisited;
        }
        start.add(startIndex);
        ArrayList<Set<Integer>> result = new ArrayList<Set<Integer>>();
        junctionsAtDepth(start, depth, includePowerPills, result);
        return result;
    }

    /**
     * Recursive helper to above, that can have multiple candidate start points
     *
     * @param startPoints
     * @param depth
     * @param includePowerPills
     * @return
     */
    public void junctionsAtDepth(Set<Integer> startPoints, int depth, boolean includePowerPills, ArrayList<Set<Integer>> result) {
        result.add(startPoints);
        if (depth > 0) {
            Set<Integer> nextJunctions = new HashSet<Integer>();
            for (Integer start : startPoints) {
                assert start != -1 : "Cannot start at -1";
                int[] neighbors = neighbors(start);
                for (int i = 0; i < neighbors.length; i++) {
                    if (neighbors[i] != -1) {
                        int nextJunction = nextJunctionInDirection(start, i, includePowerPills);
                        assert nextJunction != -1 : "Why can't a junction be reached from " + start + " in direction " + i + "? neighbors = " + Arrays.toString(neighbors);
                        nextJunctions.add(nextJunction);
                    }
                }
            }
            junctionsAtDepth(nextJunctions, depth - 1, includePowerPills, result);
        }
    }

    /**
     * Return true if ghost with given index is in the lair
     *
     * @param ghostIndex
     * @return
     */
    public boolean ghostInLair(int ghostIndex) {
        return getGhostLairTime(ghostIndex) > 0;
    }
    
    /**
     * If active ghosts are in lair, return the time remaining until
     * one exits. Else return -1.
     * @return time of next lair exit, -1 if none are in lair
     */
    public int timeUntilNextLairExit(){
        int[] lairTimes = this.getGhostLairTimes();
        Arrays.sort(lairTimes);
        int nextExitIndex = 0;
        while(nextExitIndex < lairTimes.length && lairTimes[nextExitIndex] == 0){
            nextExitIndex++;
        }
        if(nextExitIndex == lairTimes.length){
            return -1;
        }
        return lairTimes[nextExitIndex];
    }
    
    public boolean anyActiveGhostInLair(){
        int num = this.getNumActiveGhosts();
        for(int i = 0; i < num; i++) {
            if(ghostInLair(i)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Return true if starting a new level, which is the case if the level time
     * is zero
     *
     * @return
     */
    public boolean levelJustChanged() {
        return this.getCurrentLevelTime() == 0;
    }

    public int getNumberOfLairGhosts() {
        int count = 0;
        for(int i = 0; i < Constants.NUM_GHOSTS; i++) {
            if(this.getGhostLairTime(i) > 0){
                count++;
            }
        }
        return count;
    }

    /**
     * AVG number of ghosts eaten per each power pill
     * @param punishUneatenPowerPills include scores of 0 for each power pill that
     * wasn't eaten at all (even in levels that were not reached).
     * @return 
     */
    public double averageGhostsEatenPerPowerPill(boolean punishUneatenPowerPills) {
        return newG.averageGhostsEatenPerPowerPill(punishUneatenPowerPills);
    }

    public double averageTimeToEatAllGhostsAfterPowerPill() {
        return newG.averageTimeToEatAllGhostsAfterPowerPill();
    }

    public void setExitLairEdible(boolean exitLairEdible) {
        newG.setExitLairEdible(exitLairEdible);
    }

    public void setEndOnlyOnTimeLimit(boolean endOnlyOnTimeLimit) {
        newG.setEndOnlyOnTimeLimit(endOnlyOnTimeLimit);
    }

    public void setRandomLairExit(boolean randomLairExit) {
        newG.setRandomLairExit(randomLairExit);
    }

    public void setSimultaneousLairExit(boolean simultaneousLairExit) {
        newG.setSimultaneousLairExit(simultaneousLairExit);
    }
}
