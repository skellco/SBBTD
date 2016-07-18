package edu.utexas.cs.nn.breve2D.dynamics;

import edu.utexas.cs.nn.breve2D.agent.Agent;
import edu.utexas.cs.nn.breve2D.agent.Breve2DAction;

/**
 *
 * @author He_Deceives
 */
public abstract class Breve2DDynamics {

    public int task = 0;

    /*
     * For multitask domains
     */
    public int numTasks() {
        return 1;
    }

    public void advanceTask() {
        task++;
        task %= numTasks();
    }

    public abstract void reset();

    public abstract int numInputSensors();

    /*
     * May change in multitask domains
     */
    public boolean playerRespondsToMonster() {
        return false;
    }

    /*
     * Should never change within a domain
     */
    public boolean sensePlayerResponseToMonster() {
        return false;
    }

    public Breve2DAction playerInitialResponseToMonster(Agent player, Agent monster, int time) {
        return null;
    }

    public Breve2DAction playerContinuedResponseToMonster(Agent player, Agent monster, int time) {
        return null;
    }

    /*
     * May change in multitask domains
     */
    public boolean monsterRespondsToPlayer() {
        return false;
    }

    /*
     * Should never change within a domain
     */
    public boolean senseMonsterResponseToPlayer() {
        return false;
    }

    public Breve2DAction monsterInitialResponseToPlayer(Agent player, Agent monster, int time) {
        return null;
    }

    public Breve2DAction monsterContinuedResponseToPlayer(Agent player, Agent monster, int time) {
        return null;
    }

    public abstract double[] fitnessScores();

    public abstract int numFitnessFunctions();

    public abstract void registerFitnessFunctions();

    public abstract double[] minScores();
}
