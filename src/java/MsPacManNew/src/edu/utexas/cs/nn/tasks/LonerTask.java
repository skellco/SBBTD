package edu.utexas.cs.nn.tasks;

import edu.utexas.cs.nn.evolution.genotypes.Genotype;
import edu.utexas.cs.nn.evolution.genotypes.TWEANNGenotype;
import edu.utexas.cs.nn.evolution.lineage.Offspring;
import edu.utexas.cs.nn.evolution.metaheuristics.Metaheuristic;
import edu.utexas.cs.nn.evolution.mulambda.MuLambda;
import edu.utexas.cs.nn.evolution.ucb.UCB1Comparator;
import edu.utexas.cs.nn.graphics.DrawingPanel;
import edu.utexas.cs.nn.graphics.Plot;
import edu.utexas.cs.nn.log.EvalLog;
import edu.utexas.cs.nn.mmneat.MMNEAT;
import edu.utexas.cs.nn.networks.TWEANN;
import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.parameters.Parameters;
import edu.utexas.cs.nn.scores.Score;
import edu.utexas.cs.nn.tasks.mspacman.MsPacManTask;
import edu.utexas.cs.nn.util.file.FileUtilities;
import java.io.File;
import java.io.IOException;
import java.nio.file.CopyOption;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.concurrent.*;
import wox.serial.Easy;

/**
 * A task for which an individual's fitness depends only on itself. In other
 * words, the genotype is evaluated in isolation, without interacting with any
 * other members of the population.
 *
 * @author Jacob Schrum
 */
public abstract class LonerTask<T> implements SinglePopulationTask<T> {

    public static final int NETWORK_WINDOW_OFFSET = 0;

    /**
     * Since agents are evaluated in isolation, it is possible to parallelize
     * their evaluation. This thread class enables parallel evaluation, and
     * returns the results of evaluation.
     *
     * @param <T> the phenotype of the agent to be evaluated
     */
    public class EvaluationThread<T> implements Callable<Score<T>> {

        private Genotype<T> genotype;
        private LonerTask<T> task;

        public EvaluationThread(LonerTask<T> task, Genotype<T> g) {
            this.genotype = g;
            this.task = task;
        }

        public Score<T> call() { 
            DrawingPanel panel = null;
            DrawingPanel[] subPanels = null;
            if (genotype instanceof TWEANNGenotype) {
                if (CommonConstants.showNetworks) {
                    panel = new DrawingPanel(TWEANN.NETWORK_VIEW_DIM, TWEANN.NETWORK_VIEW_DIM, "Evolving Network");
                    panel.setLocation(NETWORK_WINDOW_OFFSET, 0);
                    ((TWEANNGenotype) genotype).getPhenotype().draw(panel);
                    //                if(genotype instanceof HierarchicalTWEANNGenotype){
                    //                    HierarchicalTWEANNGenotype htg = (HierarchicalTWEANNGenotype) genotype;
                    //                    ArrayList<Integer> subnetIds = htg.subNetIds.getPhenotype();
                    //                    subPanels = new DrawingPanel[subnetIds.size()];
                    //                    for(int i = 0; i < subPanels.length; i++){
                    //                        subPanels[i] = new DrawingPanel(TWEANN.NETWORK_VIEW_DIM, TWEANN.NETWORK_VIEW_DIM, "Subnet " + i);
                    //                        subPanels[i].setLocation(NETWORK_WINDOW_OFFSET + (i * TWEANN.NETWORK_VIEW_DIM), TWEANN.NETWORK_VIEW_DIM);
                    //                        ((TWEANNGenotype) htg.getSubNetGenotype(i)).getPhenotype().draw(subPanels[i]);
                    //                    }
                    //                }
                }
                if (CommonConstants.viewModePreference && TWEANN.preferenceNeuronPanel == null && TWEANN.preferenceNeuron()) {
                    TWEANN.preferenceNeuronPanel = new DrawingPanel(Plot.BROWSE_DIM, Plot.BROWSE_DIM, "Preference Neuron Activation");
                    TWEANN.preferenceNeuronPanel.setLocation(Plot.BROWSE_DIM + Plot.EDGE, Plot.BROWSE_DIM + Plot.TOP);
                }
                if (CommonConstants.monitorInputs) {
                    Offspring.fillInputs((TWEANNGenotype) genotype);
                }
            }
            // Output a report about the specific evals
            if (CommonConstants.evalReport) {
                MMNEAT.evalReport = new EvalLog("Eval-Net" + genotype.getId());
            }
            long before = System.currentTimeMillis();
            Score<T> score = task.evaluate(genotype); 
            long after = System.currentTimeMillis();
            if (MMNEAT.evalReport != null) {
                if (CommonConstants.recordPacman) {
                    // Copy the eval report
                    CopyOption[] options = new CopyOption[]{
                        StandardCopyOption.REPLACE_EXISTING,
                        StandardCopyOption.COPY_ATTRIBUTES
                    };
                    try {
                        Files.copy(MMNEAT.evalReport.getFile().toPath(), Paths.get(Parameters.parameters.stringParameter("pacmanSaveFile") + ".eval"), options);
                    } catch (IOException ex) {
                        System.out.println("Could not save eval report");
                        System.exit(1);
                    }
                }
                MMNEAT.evalReport.close();
            }
            score.totalEvalTime = (after - before); 
            // May need a Reentrant lock on this, if it is still used
            for (Metaheuristic m : MMNEAT.metaheuristics) {
                m.augmentScore(score);
            }
            if (panel != null) {
                System.out.println("Mode Usage: " + Arrays.toString(((TWEANNGenotype) score.individual).modeUsage));
                System.out.println("Fitness: " + score.toString());
                panel.dispose();
                if (subPanels != null) {
                    for (int i = 0; i < subPanels.length; i++) {
                        subPanels[i].dispose();
                    }
                    subPanels = null;
                }
            }
            return score;
        }
    }
    private boolean parallel;
    private int threads;

    public LonerTask() {
        this.parallel = Parameters.parameters.booleanParameter("parallelEvaluations");
        this.threads = Parameters.parameters.integerParameter("threads");
    }

    public Score<T> evaluateOne(Genotype<T> genotype) {
        return new EvaluationThread(this, genotype).call();
    }

    public ArrayList<Score<T>> evaluateAll(ArrayList<Genotype<T>> population) {
        ArrayList<Score<T>> scores = new ArrayList<Score<T>>(population.size());

        ExecutorService poolExecutor = null;
        ArrayList<Future<Score<T>>> futures = null;
        ArrayList<EvaluationThread<T>> calls = new ArrayList<EvaluationThread<T>>(population.size());

        for (int i = 0; i < population.size(); i++) {
            Genotype<T> genotype = population.get(i);
            EvaluationThread<T> callable = new EvaluationThread<T>(this, genotype);
            calls.add(callable);
        }

        if (parallel) {
            poolExecutor = Executors.newFixedThreadPool(threads);
            futures = new ArrayList<Future<Score<T>>>(population.size());
            for (int i = 0; i < population.size(); i++) {
                Future<Score<T>> future = poolExecutor.submit(calls.get(i));
                futures.add(future);
            }
        }

        // General tracking of best in each objective
        double[] bestObjectives = minScores();
        Genotype[] bestGenotypes = new Genotype[bestObjectives.length];
        Score<T>[] bestScores = new Score[bestObjectives.length];

        int maxPacManScore = 0;
        Genotype bestPacMan = null;
        Score<T> bestScoreSet = null;
        boolean trackBestPacManScore = CommonConstants.netio
                && this instanceof MsPacManTask
                && MMNEAT.ea instanceof MuLambda
                && ((MuLambda) MMNEAT.ea).evaluatingParents;
        for (int i = 0; i < population.size(); i++) {
            try {
                Score<T> s = parallel ? futures.get(i).get() : calls.get(i).call();
                // Specific to Ms Pac-Man
                if (trackBestPacManScore) {
                    int gameScore = (int) s.otherStats[0]; // Game Score is always first
                    if (gameScore >= maxPacManScore) {
                        bestPacMan = s.individual;
                        maxPacManScore = gameScore;
                        bestScoreSet = s;
                    }
                }
                // Best in each objective
                for (int j = 0; j < bestObjectives.length; j++) {
                    double objectiveScore = s.scores[j];
                    if (i == 0 || objectiveScore >= bestObjectives[j]) {
                        bestGenotypes[j] = s.individual;
                        bestObjectives[j] = objectiveScore;
                        bestScores[j] = s;
                    }
                }

                scores.add(s);
            } catch (InterruptedException ex) {
                ex.printStackTrace();
                System.exit(1);
            } catch (ExecutionException ex) {
                ex.printStackTrace();
                System.exit(1);
            }
        }

        if (bestPacMan != null) {
            // Save best pacman
            String bestPacManDir = FileUtilities.getSaveDirectory() + "/bestPacMan";
            File bestDir = new File(bestPacManDir);
            // Delete old contents/team
            if (bestDir.exists()) {
                FileUtilities.deleteDirectoryContents(bestDir);
            } else {
                bestDir.mkdir();
            }
            Easy.save(bestPacMan, bestPacManDir + "/bestPacMan.xml");
            //System.out.println("Saved best Ms. Pac-Man agent with score of " + maxPacManScore);
            FileUtilities.simpleFileWrite(bestPacManDir + "/score.txt", bestScoreSet.toString());
        }

        if (CommonConstants.netio) {
            // Save best in each objective
            String bestDir = FileUtilities.getSaveDirectory() + "/bestObjectives";
            File dir = new File(bestDir);
            // Delete old contents/team
            if (dir.exists()) {
                FileUtilities.deleteDirectoryContents(dir);
            } else {
                dir.mkdir();
            }

            for (int j = 0; j < bestObjectives.length; j++) {
                Easy.save(bestGenotypes[j], bestDir + "/bestIn" + j + ".xml");
                FileUtilities.simpleFileWrite(bestDir + "/score" + j + ".txt", bestScores[j].toString());
            }
        }

        if (parallel) {
            poolExecutor.shutdown();
        }

        /**
         * If using UCB to decide who to give extra evals to, then by this point
         * every member of the population will have been evaluated (preferably
         * once). From here on, intelligent decision need to be made about who
         * to evaluate again.
         */
        if (CommonConstants.ucb1Evaluation) {
            int evaluationBudget = Parameters.parameters.integerParameter("evaluationBudget");
            // Do an initial sort so the individual to evaluate will always
            // be at the end of the list
            int index = 0;
            double max = 0;
            for (Score s : scores) {
                max = Math.max(max, s.scores[index]);
            }
            UCB1Comparator ucb1 = new UCB1Comparator(index, scores.size(), max);
            Collections.sort(scores, ucb1);
            int last = scores.size() - 1;

//            System.out.println("--SORTED-----------------------------------");
//            for (int i = 0; i < scores.size(); i++) {
//                System.out.println(ucb1.ucb1(scores.get(i)) + "::" + scores.get(i));
//            }
//            System.out.println("--UCB--------------------------------------");
            // Perform the budgetted number of evals
            for (int i = 0; i < evaluationBudget; i++) {
                // Highest UCB is always at end, so evaluate it
                Score<T> oldScore = scores.get(last);
                //System.out.print(ucb1.ucb1(oldScore) + "::" + oldScore + "->");
                EvaluationThread<T> callable = new EvaluationThread<T>(this, oldScore.individual);
                Score<T> newScore = oldScore.incrementalAverage(callable.call());
                // After eval, insert the individual into the correct slot in sorted list
                ucb1.increaseTotal();
                //System.out.println(ucb1.ucb1(newScore) + "::" + newScore);
                ucb1.setMax(newScore.scores[index]);

                scores.set(last, newScore);
                Collections.sort(scores, ucb1);

//                System.out.println("--SORTED-----------------------------------");
//                for (int j = 0; j < scores.size(); j++) {
//                    System.out.println(ucb1.ucb1(scores.get(j)) + "::" + scores.get(j));
//                }
//                System.out.println("--UCB--------------------------------------");
//                int pos = last - 1;
//                //while(scores.get(pos) > newScore && pos >= 0){
//                while(pos >= 0 && ucb1.compare(scores.get(pos), newScore) > 0){
//                    pos--;
//                }
//                scores.remove(last);
//                scores.add(pos+1, newScore);
            }
//            System.out.println("--END--------------------------------------");
        }

        return scores;
    }

    public abstract Score<T> evaluate(Genotype<T> individual);

    /*
     * Default objective mins of 0.
     */
    public double[] minScores() {
        return new double[this.numObjectives()];
    }

    // Stats to track besides fitness
    public int numOtherScores() {
        return 0;
    }
}
