package edu.utexas.cs.nn.tasks.mspacman;

import edu.utexas.cs.nn.evolution.genotypes.Genotype;
import edu.utexas.cs.nn.networks.Network;
import edu.utexas.cs.nn.util.datastructures.Pair;

/**
 * One pacman eval consists of two separate evals: One in the regular game, and
 * one in the ghost eating scenario.
 *
 * @author Jacob Schrum
 */
public class MsPacManGhostsVsPillsMultitask<T extends Network> extends MsPacManTask<T> {

    @Override
    public Pair<double[], double[]> oneEval(Genotype<T> individual, int num) {
        // Do an eval in the Ghost Only version
        noPills = true;
        endAfterGhostEatingChances = false;
        noPowerPills = false;
        Pair<double[], double[]> full = super.oneEval(individual, num);
        // Now do an eval in the Pill Only version
        noPills = false;
        endAfterGhostEatingChances = false;
        noPowerPills = true;
        Pair<double[], double[]> ghostEating = super.oneEval(individual, num);

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
