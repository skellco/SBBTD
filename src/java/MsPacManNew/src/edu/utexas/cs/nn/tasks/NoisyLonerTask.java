package edu.utexas.cs.nn.tasks;

import edu.utexas.cs.nn.evolution.genotypes.Genotype;
import edu.utexas.cs.nn.mmneat.MMNEAT;
import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.parameters.Parameters;
import edu.utexas.cs.nn.scores.MultiObjectiveScore;
import edu.utexas.cs.nn.scores.Score;
import edu.utexas.cs.nn.tasks.mspacman.agentcontroller.pacman.NNCheckEachDirectionPacManController;
import edu.utexas.cs.nn.util.ClassCreation;
import edu.utexas.cs.nn.util.datastructures.ArrayUtil;
import edu.utexas.cs.nn.util.datastructures.Pair;
import edu.utexas.cs.nn.util.stats.Average;
import edu.utexas.cs.nn.util.stats.Statistic;
import edu.utexas.cs.nn.util.stats.StatisticsUtilities;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Any task in which multiple trials are needed because evaluations are noisy.
 * The final fitness score of an individual is some statistic of the scores from
 * all evaluations, such as average or maximum.
 *
 * @author Jacob Schrum
 */
public abstract class NoisyLonerTask<T> extends LonerTask<T> {

    public Statistic stat;
    public final boolean printFitness;

    public NoisyLonerTask() {
        this.printFitness = Parameters.parameters.booleanParameter("printFitness");
        try {
            stat = (Statistic) ClassCreation.createObject("noisyTaskStat");
        } catch (NoSuchMethodException ex) {
            ex.printStackTrace();
            System.exit(1);
        }
    }

    /**
     * Any domain-specific prep that needs to be done before starting the
     * sequence of evals
     */
    public void prep() {
    }

    /**
     * Any domain-specific cleanup that needs to be done after evaluations
     */
    public void cleanup() {
    }

    /**
     * Return domain-specific behavior vector. Don't need to define if it won't
     * be used, hence the default definition of null. A behavior vector is a
     * collection of numbers that somehow characterizes the behavior of the
     * agent in the domain.
     *
     * @return behavior vector
     */
    public ArrayList<Double> getBehaviorVector() {
        return null;
    }

    /**
     * All actions performed in a single evaluation of an agent
     *
     * @param individual genotype of agent to be evaluated
     * @param num which evaluation is currently being performed
     * @return Combination of fitness scores (multiobjective possible), and
     * other scores (for tracking non-fitness data)
     */
    public abstract Pair<double[], double[]> oneEval(Genotype<T> individual, int num);

    /**
     * Evaluate an agent by subjecting it to several separate evaluations/trials
     * in the domain. Return the fitness score(s)
     *
     * @param individual Genotype of individual to be evaluated
     * @return score instance containing the fitness scores, and other necessary
     * data
     */
    @Override
    public Score<T> evaluate(Genotype<T> individual) {
        prep();
        double[][] objectiveScores = new double[CommonConstants.trials][this.numObjectives()];
        double[][] otherScores = new double[CommonConstants.trials][this.numOtherScores()];
        double evalTimeSum = 0;
        
        for (int i = 0; i < CommonConstants.trials; i++) {
            long before = System.currentTimeMillis();
            if (MMNEAT.evalReport != null) {
                MMNEAT.evalReport.log("Eval " + i + ":");
            }
            Pair<double[], double[]> result = oneEval(individual, i);
            if (printFitness) {
                System.out.println(Arrays.toString(result.t1) + Arrays.toString(result.t2)); 
            }
            long after = System.currentTimeMillis();
            evalTimeSum += (after - before);
            objectiveScores[i] = result.t1; // fitness scores
            //ScoreHistory.add(individual.getId(), result.t1);
            otherScores[i] = result.t2; // other scores
        }
        double averageEvalTime = evalTimeSum / CommonConstants.trials;
        double[] fitness = new double[this.numObjectives()];
        for (int i = 0; i < fitness.length; i++) {
            if (MMNEAT.aggregationOverrides.get(i) == null) {
                fitness[i] = stat.stat(ArrayUtil.column(objectiveScores, i));
            } else {
                fitness[i] = MMNEAT.aggregationOverrides.get(i).stat(ArrayUtil.column(objectiveScores, i));
            }
        }
        double[] other = new double[this.numOtherScores()];
        for (int i = 0; i < other.length; i++) {
            if (MMNEAT.aggregationOverrides.get(fitness.length + i) == null) {
                other[i] = stat.stat(ArrayUtil.column(otherScores, i));
            } else {
                other[i] = MMNEAT.aggregationOverrides.get(fitness.length + i).stat(ArrayUtil.column(otherScores, i));
            }
        }
        if (printFitness) {
            System.out.println("Individual: " + individual.getId());
            System.out.println("\t" + scoreSummary(objectiveScores, otherScores, fitness, other));
        }
        if (MMNEAT.evalReport != null) {
            MMNEAT.evalReport.log(scoreSummary(objectiveScores, otherScores, fitness, other));

            if (NNCheckEachDirectionPacManController.totalChosenDirectionModeUsageCounts != null) {
                MMNEAT.evalReport.log("Usage: "
                        + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionModeUsageCounts))
                        + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionJunctionModeUsageCounts))
                        + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionEdibleModeUsageCounts))
                        + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionThreatModeUsageCounts))
                        + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionJunctionEdibleModeUsageCounts))
                        + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionJunctionThreatModeUsageCounts)));
                MMNEAT.evalReport.log("Total Mode Usage Across Evals");
                MMNEAT.evalReport.log("\tMode Usage For Chosen Direction Networks: " + Arrays.toString(NNCheckEachDirectionPacManController.totalChosenDirectionModeUsageCounts) + ":" + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionModeUsageCounts)));
                NNCheckEachDirectionPacManController.totalChosenDirectionModeUsageCounts = null;                
                MMNEAT.evalReport.log("\tMode Usage At Junctions For Chosen Direction Networks: " + Arrays.toString(NNCheckEachDirectionPacManController.totalChosenDirectionJunctionModeUsageCounts) + ":" + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionJunctionModeUsageCounts)));
                NNCheckEachDirectionPacManController.totalChosenDirectionJunctionModeUsageCounts = null;
                MMNEAT.evalReport.log("\tEdible Mode Usage For Chosen Direction Networks: " + Arrays.toString(NNCheckEachDirectionPacManController.totalChosenDirectionEdibleModeUsageCounts) + ":" + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionEdibleModeUsageCounts)));
                NNCheckEachDirectionPacManController.totalChosenDirectionEdibleModeUsageCounts = null;
                MMNEAT.evalReport.log("\tThreat Mode Usage For Chosen Direction Networks: " + Arrays.toString(NNCheckEachDirectionPacManController.totalChosenDirectionThreatModeUsageCounts) + ":" + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionThreatModeUsageCounts)));
                NNCheckEachDirectionPacManController.totalChosenDirectionThreatModeUsageCounts = null;
                MMNEAT.evalReport.log("\tEdible Mode Usage At Junctions For Chosen Direction Networks: " + Arrays.toString(NNCheckEachDirectionPacManController.totalChosenDirectionJunctionEdibleModeUsageCounts) + ":" + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionJunctionEdibleModeUsageCounts)));
                NNCheckEachDirectionPacManController.totalChosenDirectionJunctionEdibleModeUsageCounts = null;
                MMNEAT.evalReport.log("\tThreat Mode Usage At Junctions For Chosen Direction Networks: " + Arrays.toString(NNCheckEachDirectionPacManController.totalChosenDirectionJunctionThreatModeUsageCounts) + ":" + Arrays.toString(StatisticsUtilities.distribution(NNCheckEachDirectionPacManController.totalChosenDirectionJunctionThreatModeUsageCounts)));
                NNCheckEachDirectionPacManController.totalChosenDirectionJunctionThreatModeUsageCounts = null;
            }
        }
        cleanup();
        Score<T> s = new MultiObjectiveScore<T>(individual, fitness, getBehaviorVector(), other);
        s.averageEvalTime = averageEvalTime;
        return s;
    }

    public String scoreSummary(double[][] objectiveScores, double[][] otherScores, double[] fitness, double[] other) {
        String nl = System.getProperty("line.separator");
        String result = "";
        result += "Fitness scores:" + nl;
        int globalFitnessFunctionIndex = 0;
        for (int i = 0; i < fitness.length; i++) {
            Statistic fitnessStat = MMNEAT.aggregationOverrides.get(globalFitnessFunctionIndex);
            boolean includeStdev = fitnessStat == null || fitnessStat instanceof Average;
            String fitnessFunctionName = MMNEAT.fitnessFunctions.get(globalFitnessFunctionIndex) + (includeStdev ? "" : "[" + fitnessStat.getClass().getSimpleName() + "]");
            globalFitnessFunctionIndex++;
            double[] xs = ArrayUtil.column(objectiveScores, i);
            double stdev = StatisticsUtilities.sampleStandardDeviation(xs);
            result += "\t" + fitnessFunctionName + ":\t" + Arrays.toString(xs) + ":" + fitness[i] + (includeStdev ? " +/- " + stdev : "") + nl;
        }
        result += "\tOther scores:" + nl;
        for (int i = 0; i < other.length; i++) {
            Statistic fitnessStat = MMNEAT.aggregationOverrides.get(globalFitnessFunctionIndex);
            boolean includeStdev = fitnessStat == null || fitnessStat instanceof Average;
            String otherScoreName = MMNEAT.fitnessFunctions.get(globalFitnessFunctionIndex) + (includeStdev ? "" : "[" + fitnessStat.getClass().getSimpleName() + "]");
            globalFitnessFunctionIndex++;
            double[] xs = ArrayUtil.column(otherScores, i);
            double stdev = StatisticsUtilities.sampleStandardDeviation(xs);
            result += "\t" + otherScoreName + ":\t" + Arrays.toString(xs) + ":" + other[i] + (includeStdev ? " +/- " + stdev : "") + nl;
        }
        result += "Fitness:" + Arrays.toString(fitness) + nl;
        result += "OtherScores:" + Arrays.toString(other) + nl;
        return result;
    }
}
