package edu.utexas.cs.nn.tasks.mspacman;

import edu.utexas.cs.nn.evolution.genotypes.Genotype;
import edu.utexas.cs.nn.mmneat.MMNEAT;
import edu.utexas.cs.nn.networks.Network;
import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.scores.Score;
import edu.utexas.cs.nn.tasks.mspacman.objectives.SpecificGhostScore;
import java.util.ArrayList;

/**
 *
 * @author Jacob Schrum
 */
public class CooperativeGhostMonitorNetworksMsPacManTask<T extends Network> extends CooperativeMsPacManTask<T> {

    public CooperativeGhostMonitorNetworksMsPacManTask() {
        super();
        for (int i = 0; i < CommonConstants.numActiveGhosts; i++) {
            task.addObjective(new SpecificGhostScore<T>(i), task.otherScores, false);
        }
    }

    @Override
    public ArrayList<Score> evaluate(Genotype[] team) {

        Genotype<T> topLevelNetwork = team[0];
        // Put subnets into input-output mediator
        ArrayList<Genotype<T>> subnets = new ArrayList<Genotype<T>>(team.length);
        for (int i = 1; i < team.length; i++) {
            subnets.add(team[i]);
        }
        MMNEAT.replaceSubnets(subnets);
        // Evaluate
        Score<T> taskScores = task.evaluate(topLevelNetwork);
        // Redistribute scores to each genotype
        ArrayList<Score> genotypeScores = new ArrayList<Score>(team.length);
        //Score<T> straightScore = new Score<T>(topLevelNetwork, new double[]{taskScores.otherStats[task.scoreIndexInOtherScores]}, null, taskScores.otherStats);
        //genotypeScores.add(straightScore);
        genotypeScores.add(taskScores);

        int totalOtherScores = taskScores.otherStats.length;
        for (int i = 1; i < team.length; i++) {
            genotypeScores.add(new Score<T>(team[i], new double[]{taskScores.otherStats[totalOtherScores - team.length + i]}, null));
        }

        return genotypeScores;
    }

    /**
     * One control network and one network to monitor each ghost
     *
     * @return
     */
    public int numberOfPopulations() {
        return CommonConstants.numActiveGhosts + 1;
    }

    public int[] objectivesPerPopulation() {
        int[] result = new int[numberOfPopulations()];
        result[0] = task.objectives.size();
        for (int i = 1; i < result.length; i++) {
            result[i] = 1;
        }
        return result;
    }

    public int[] otherStatsPerPopulation() {
        int[] result = new int[numberOfPopulations()];
        result[0] = task.otherScores.size();
        for (int i = 1; i < result.length; i++) {
            result[i] = 0;
        }
        return result;
    }
}
