package edu.utexas.cs.nn.tasks.mspacman;

import edu.utexas.cs.nn.evolution.genotypes.Genotype;
import edu.utexas.cs.nn.networks.Network;
import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.parameters.Parameters;
import edu.utexas.cs.nn.util.datastructures.Pair;

/**
 * One pacman eval consists of two separate evals: One in the regular game, and
 * one in the ghost eating scenario.
 *
 * @author Jacob Schrum
 */
public class MsPacManPillsVsEdibleMultitask<T extends Network> extends MsPacManTask<T> {

    public MsPacManPillsVsEdibleMultitask(){
        Parameters.parameters.setBoolean("infiniteEdibleTime", true);
        CommonConstants.infiniteEdibleTime = true;
        Parameters.parameters.setBoolean("imprisonedWhileEdible", true);
        CommonConstants.imprisonedWhileEdible = true;
    }
    
    @Override
    public Pair<double[], double[]> oneEval(Genotype<T> individual, int num) {
        // Do an eval with Threats and Pills
        noPills = false;
        noPowerPills = true;
        endOnlyOnTimeLimit = false;
        exitLairEdible = false;
        randomLairExit = false;
        simultaneousLairExit = false;
        Pair<double[], double[]> full = super.oneEval(individual, num);
        // Now do an eval with only edible ghosts and a time limit
        noPills = true;
        noPowerPills = true;
        endOnlyOnTimeLimit = true;
        exitLairEdible = true;
        randomLairExit = true;
        simultaneousLairExit = true;
        // Edible task does not need to be as long as pill task
        CommonConstants.pacManLevelTimeLimit = Parameters.parameters.integerParameter("edibleTaskTimeLimit");
        Pair<double[], double[]> ghostEating = super.oneEval(individual, num);
        // Restore eval time for next pill task eval
        CommonConstants.pacManLevelTimeLimit = Parameters.parameters.integerParameter("pacManLevelTimeLimit");
        if (Parameters.parameters.booleanParameter("rawTimeScore")) {
            // Need to subtract time alive in edible task, since it is always the max
            ghostEating.t1[rawTimeScoreIndex] = 0;
        }
        
        double[] combinedScores = new double[full.t1.length];
        for (int i = 0; i < combinedScores.length; i++) {
            combinedScores[i] = full.t1[i] + ghostEating.t1[i];
        }
        double[] combinedOthers = new double[full.t2.length];
        for (int i = 0; i < combinedOthers.length; i++) {
            combinedOthers[i] = full.t2[i] + ghostEating.t2[i];
        }

        Pair<double[], double[]> combo = new Pair<double[], double[]>(combinedScores, combinedOthers);
        return combo;
    }
}
