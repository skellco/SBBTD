package edu.utexas.cs.nn.tasks.mspacman;

import dal.experiment.RunTrainedNetworks;
import edu.utexas.cs.nn.evolution.Organism;
import edu.utexas.cs.nn.evolution.genotypes.Genotype;
import edu.utexas.cs.nn.evolution.genotypes.TWEANNGenotype;
import edu.utexas.cs.nn.evolution.nsga2.tug.TUGTask;
import edu.utexas.cs.nn.mmneat.MMNEAT;
import edu.utexas.cs.nn.networks.Network;
import edu.utexas.cs.nn.networks.NetworkTask;
import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.parameters.Parameters;
import edu.utexas.cs.nn.scores.Score;
import edu.utexas.cs.nn.tasks.NoisyLonerTask;
import edu.utexas.cs.nn.tasks.mspacman.agentcontroller.ghosts.SharedNNGhosts;
import edu.utexas.cs.nn.tasks.mspacman.agentcontroller.pacman.MultinetworkMsPacManController;
import edu.utexas.cs.nn.tasks.mspacman.agentcontroller.pacman.NNMsPacMan;
import edu.utexas.cs.nn.tasks.mspacman.data.TrainingCampManager;
import edu.utexas.cs.nn.tasks.mspacman.facades.ExecutorFacade;
import edu.utexas.cs.nn.tasks.mspacman.facades.GameFacade;
import edu.utexas.cs.nn.tasks.mspacman.facades.GhostControllerFacade;
import edu.utexas.cs.nn.tasks.mspacman.facades.PacManControllerFacade;
import edu.utexas.cs.nn.tasks.mspacman.multitask.MsPacManModeSelector;
import edu.utexas.cs.nn.tasks.mspacman.objectives.*;
import edu.utexas.cs.nn.util.ClassCreation;
import edu.utexas.cs.nn.util.datastructures.Pair;
import edu.utexas.cs.nn.util.random.RandomNumbers;
import edu.utexas.cs.nn.util.stats.Average;
import edu.utexas.cs.nn.util.stats.Max;
import edu.utexas.cs.nn.util.stats.Mode;
import edu.utexas.cs.nn.util.stats.Statistic;
import java.util.ArrayList;
import java.util.Arrays;
import pacman.ExecutorNew;
import pacman.controllers.NewGhostController;
import pacman.controllers.NewPacManController;
import pacman.game.Constants;
import pacman.game.Game;

/**
 *
 * @author Jacob Schrum
 * @param <T> phenotype of evolved agent
 */
public class MsPacManTask<T extends Network> extends NoisyLonerTask<T> implements TUGTask, NetworkTask {

    protected boolean deterministic;
    protected boolean ignorePillScore;
    protected boolean noPills;
    protected boolean noPowerPills;
    protected boolean endAfterGhostEatingChances;
    protected boolean luringTask;
    protected boolean exitLairEdible;
    protected boolean endOnlyOnTimeLimit;
    protected boolean randomLairExit;
    protected boolean simultaneousLairExit;
    protected boolean evolveGhosts;
    protected ArrayList<MsPacManObjective<T>> objectives;
    protected ArrayList<MsPacManObjective<T>> otherScores;
    protected GhostControllerFacade ghosts;
    protected PacManControllerFacade mspacman;
    protected ExecutorFacade exec;
    protected GameFacade game;
    private final int scoreIndexInOtherScores;
    private final int pillScoreIndexInOtherScores;
    private final int ghostRewardIndexInOtherScores;
    private final int maxScoreIndexInOtherScores;
    private final int maxPillScoreIndexInOtherScores;
    private final int maxGhostRewardIndexInOtherScores;
    private final int properPowerPillIndexInOtherScores;
    private final int improperPowerPillIndexInOtherScores;
    private final int luringScoreIndexInOtherScores;
    private final int specificLevelScoreFirstIndexInOtherScores;
    private final int powerPillEatenWhenGhostFarIndexInOtherScores;
    private final int avgLevelIndexInOtherScores;
    private final int maxLevelIndexInOtherScores;
    private final int ghostRegretScoreInOtherScores;
    private final int ghostsEatenIndexInOtherScores;
    protected int rawTimeScoreIndex = -1;
    public int[] pillTimeFrameIndices;
    public int[] ghostTimeFrameIndices;
    private final boolean eachComponentTracksScoreToo;
    private final boolean plainGhostScore;
    private final TrainingCampManager tcManager;

    public MsPacManTask() {
        this(Parameters.parameters.booleanParameter("deterministic"));
    }

    public MsPacManTask(boolean det) {
        super();
        exec = new ExecutorFacade(new ExecutorNew());
        this.deterministic = det;
        tcManager = new TrainingCampManager();

        evolveGhosts = Parameters.parameters.booleanParameter("evolveGhosts");
        boolean rawScorePacMan = Parameters.parameters.booleanParameter("rawScorePacMan");
        boolean clearTimeScore = Parameters.parameters.booleanParameter("clearTimeScore");
        boolean rewardFasterGhostEating = Parameters.parameters.booleanParameter("rewardFasterGhostEating");
        boolean rewardFasterPillEating = Parameters.parameters.booleanParameter("rewardFasterPillEating");
        luringTask = Parameters.parameters.booleanParameter("luringTask");
        simultaneousLairExit = Parameters.parameters.booleanParameter("simultaneousLairExit");
        exitLairEdible = Parameters.parameters.booleanParameter("exitLairEdible");
        endOnlyOnTimeLimit = Parameters.parameters.booleanParameter("endOnlyOnTimeLimit");
        endAfterGhostEatingChances = Parameters.parameters.booleanParameter("endAfterGhostEatingChances");
        noPills = Parameters.parameters.booleanParameter("noPills");
        noPowerPills = Parameters.parameters.booleanParameter("noPowerPills");
        ignorePillScore = Parameters.parameters.booleanParameter("ignorePillScore");
        eachComponentTracksScoreToo = Parameters.parameters.booleanParameter("eachComponentTracksScoreToo");
        plainGhostScore = Parameters.parameters.booleanParameter("plainGhostScore");
        boolean avgGhostsPerPowerPill = Parameters.parameters.booleanParameter("avgGhostsPerPowerPill");
        boolean punishDeadSpace = Parameters.parameters.booleanParameter("punishDeadSpace");
        boolean randomSelection = Parameters.parameters.booleanParameter("randomSelection");

        objectives = new ArrayList<MsPacManObjective<T>>(17);
        otherScores = new ArrayList<MsPacManObjective<T>>(17);
        if (randomSelection) {
            addObjective(new RandomScore<T>(), objectives, true);
        } else if (rawScorePacMan) {
            addObjective(new GameScore<T>(), objectives, true);
        } else if (luringTask) {
            addObjective(new LuringScore<T>(), objectives, true);
        } else if (Parameters.parameters.booleanParameter("individualLevelFitnesses")) {
            for (int i = 0; i < Constants.NUM_MAZES; i++) {
                addObjective(new LevelGameScore<T>(i), objectives, new Average(), true);
            }
        } else {
            if (!noPowerPills && CommonConstants.numActiveGhosts > 0) {
                if (!Parameters.parameters.booleanParameter("ignoreGhostScores")) {
                    if (avgGhostsPerPowerPill) {
                        addObjective(new GhostsPerPowerPillScore<T>(true), objectives, true);
                    } else if (rewardFasterGhostEating) {
                        addObjective(new FastGhostEatingScore<T>(), objectives, true);
                    } else if (plainGhostScore) {
                        addObjective(new EatenGhostScore<T>(), objectives, true);
                    } else {
                        addObjective(new GhostRewardScore<T>(), objectives, true);
                    }

                    if (Parameters.parameters.booleanParameter("ghostRegretFitness")) {
                        addObjective(new GhostRegretScore<T>(), objectives, true);
                    }
                    if (Parameters.parameters.booleanParameter("timeToEatAllFitness")) {
                        addObjective(new TimeToEatAllGhostsScore<T>(), objectives, true);
                    }
                }
                if (Parameters.parameters.booleanParameter("awardProperPowerPillEating")) {
                    addObjective(new ProperlyEatenPowerPillScore<T>(), objectives, true);
                }
                if (Parameters.parameters.booleanParameter("punishImproperPowerPillEating")) {
                    addObjective(new ImproperlyEatenPowerPillScore<T>(), objectives, true);
                }
            }
            if (!ignorePillScore && !noPills) {
                if (rewardFasterPillEating) {
                    addObjective(new FastPillEatingScore<T>(), objectives, true);
                } else {
                    addObjective(new PillScore<T>(), objectives, true);
                }
            }
        }
        if (Parameters.parameters.booleanParameter("rawTimeScore")) {
            rawTimeScoreIndex = objectives.size();
            addObjective(new RawTimeScore<T>(), objectives, true);
        }
        if (punishDeadSpace) {
            addObjective(new AvoidDeadSpaceScore<T>(), objectives, true);
        }
        if (clearTimeScore && CommonConstants.justMaze != -1) {
            addObjective(new ClearTimeScore<T>(), objectives, true);
        }
        if (Parameters.parameters.booleanParameter("livesObjective")) {
            addObjective(new RemainingLivesScore<T>(), objectives, true);
        }
        if (Parameters.parameters.booleanParameter("levelObjective")) {
            addObjective(new LevelScore<T>(), objectives, true);
        }
        if (Parameters.parameters.booleanParameter("consistentLevelObjective")) {
            addObjective(new LevelScore<T>(), objectives, new Mode(), true);
        }
        if (Parameters.parameters.booleanParameter("pacManTimeFitness")) {
            addObjective(new SurvivalAndSpeedTimeScore<T>(), objectives, true);
        }
        if (Parameters.parameters.booleanParameter("pacManLureFitness")) {
            addObjective(new LuringScore<T>(), objectives, true);
        }
        // Game Score
        scoreIndexInOtherScores = otherScores.size();
        addObjective(new GameScore<T>(), otherScores, new Average(), false);
        maxScoreIndexInOtherScores = otherScores.size();
        addObjective(new GameScore<T>(), otherScores, new Max(), false);
        // Pill Score
        pillScoreIndexInOtherScores = otherScores.size();
        addObjective(new PillScore<T>(), otherScores, new Average(), false);
        maxPillScoreIndexInOtherScores = otherScores.size();
        addObjective(new PillScore<T>(), otherScores, new Max(), false);
        // Ghost Reward
        addObjective(new GhostsPerPowerPillScore<T>(true), otherScores, new Average(), false);
        addObjective(new GhostsPerPowerPillScore<T>(false), otherScores, new Average(), false);
        ghostRewardIndexInOtherScores = otherScores.size();
        addObjective(new GhostRewardScore<T>(), otherScores, new Average(), false);
        maxGhostRewardIndexInOtherScores = otherScores.size();
        addObjective(new GhostRewardScore<T>(), otherScores, new Max(), false);
        // Level Scores
        avgLevelIndexInOtherScores = otherScores.size();
        addObjective(new LevelScore<T>(), otherScores, new Average(), false);
        maxLevelIndexInOtherScores = otherScores.size();
        addObjective(new LevelScore<T>(), otherScores, new Max(), false);
        addObjective(new LevelScore<T>(), otherScores, new Mode(), false);
        // Ghosts Eaten
        ghostsEatenIndexInOtherScores = otherScores.size();
        addObjective(new EatenGhostScore<T>(), otherScores, new Average(), false);
        addObjective(new EatenGhostScore<T>(), otherScores, new Max(), false);
        // Missed Ghosts
        ghostRegretScoreInOtherScores = otherScores.size();
        addObjective(new GhostRegretScore<T>(), otherScores, new Average(), false);
        // Luring
        luringScoreIndexInOtherScores = otherScores.size();
        addObjective(new LuringScore<T>(), otherScores, false);
        // How/When Power Pills Are Eaten
        properPowerPillIndexInOtherScores = otherScores.size();
        addObjective(new ProperlyEatenPowerPillScore<T>(), otherScores, false);
        improperPowerPillIndexInOtherScores = otherScores.size();
        addObjective(new ImproperlyEatenPowerPillScore<T>(), otherScores, false);
        powerPillEatenWhenGhostFarIndexInOtherScores = otherScores.size();
        addObjective(new PowerPillEatenWhenGhostFarScore<T>(), otherScores, false);

        addObjective(new SurvivalAndSpeedTimeScore<T>(), otherScores, new Average(), false);
        addObjective(new SurvivalAndSpeedTimeScore<T>(), otherScores, new Max(), false);

        if (MMNEAT.taskHasSubnetworks()) {
            boolean eligibilityOnEarnedFitness = Parameters.parameters.booleanParameter("eligibilityOnEarnedFitness");
            pillTimeFrameIndices = new int[MMNEAT.modesToTrack];
            ghostTimeFrameIndices = new int[MMNEAT.modesToTrack];
            for (int i = 0; i < MMNEAT.modesToTrack; i++) {
                pillTimeFrameIndices[i] = otherScores.size();
                addObjective(eligibilityOnEarnedFitness ? new EligibilityTimeFramesPillScore<T>(i) : new TimeFramesPillScore<T>(i), otherScores, false);
                ghostTimeFrameIndices[i] = otherScores.size();
                addObjective(eligibilityOnEarnedFitness ? new EligibilityTimeFramesGhostScore<T>(i) : new TimeFramesGhostScore<T>(i), otherScores, false);
            }
        }

        specificLevelScoreFirstIndexInOtherScores = otherScores.size();
        for (int i = 0; i < Constants.NUM_MAZES; i++) {
            addObjective(new LevelGameScore<T>(i), otherScores, new Average(), false);
        }
        // Kind of a silly way to track this, but easy
        addObjective(new EdibleTimeParameter<T>(), otherScores, new Average(), false);
        addObjective(new LairTimeParameter<T>(), otherScores, new Average(), false);
    }

    /**
     * Based on a designation from the fitness mode map, return a collection of
     * fitness values.
     *
     * @param id an id from FitnessToModeMap
     * @return array of designated fitness values
     */
    public double[] fitnessArray(int id, Score<T> taskScores) {
        int scoreIndex;
        switch (id) {
            case MsPacManModeSelector.ACTIVE_GHOST_SCORE:
                // Assumes the first subnet is always the ghost network
                scoreIndex = ghostTimeFrameIndices[0];
                break;
            case MsPacManModeSelector.ACTIVE_PILL_SCORE:
                // Assumes the second subnet is always the pill network
                scoreIndex = pillTimeFrameIndices[1];
                break;
            case MsPacManModeSelector.LEVEL_SCORE:
                scoreIndex = avgLevelIndexInOtherScores;
                break;
            case MsPacManModeSelector.GAME_SCORE:
                scoreIndex = scoreIndexInOtherScores;
                break;
            case MsPacManModeSelector.GHOST_SCORE:
                scoreIndex = plainGhostScore ? ghostsEatenIndexInOtherScores : ghostRewardIndexInOtherScores;
                break;
            case MsPacManModeSelector.PILL_SCORE:
                scoreIndex = pillScoreIndexInOtherScores;
                break;
            case MsPacManModeSelector.PROPER_POWER_PILL_SCORE:
                scoreIndex = properPowerPillIndexInOtherScores;
                break;
            case MsPacManModeSelector.LURING_FITNESS:
                scoreIndex = luringScoreIndexInOtherScores;
                break;
            case MsPacManModeSelector.IMPROPER_POWER_PILL_SCORE:
                scoreIndex = improperPowerPillIndexInOtherScores;
                break;
            case MsPacManModeSelector.GHOST_AND_LEVEL_COMBO:
                return new double[]{taskScores.otherStats[plainGhostScore ? ghostsEatenIndexInOtherScores : ghostRewardIndexInOtherScores], taskScores.otherStats[avgLevelIndexInOtherScores]};
            case MsPacManModeSelector.PROPER_POWER_PILL_GHOST_COMBO:
                return new double[]{taskScores.otherStats[plainGhostScore ? ghostsEatenIndexInOtherScores : ghostRewardIndexInOtherScores], taskScores.otherStats[properPowerPillIndexInOtherScores]};
            case MsPacManModeSelector.IMPROPER_POWER_PILL_GHOST_COMBO:
                return new double[]{taskScores.otherStats[plainGhostScore ? ghostsEatenIndexInOtherScores : ghostRewardIndexInOtherScores], taskScores.otherStats[improperPowerPillIndexInOtherScores]};
            case MsPacManModeSelector.PILL_AND_NO_POWER_PILL_COMBO:
                return new double[]{taskScores.otherStats[pillScoreIndexInOtherScores], taskScores.otherStats[powerPillEatenWhenGhostFarIndexInOtherScores]};
            case MsPacManModeSelector.NO_PREFERENCE:
                // Just use all standard scores, whatever they are
                return Arrays.copyOf(taskScores.scores, taskScores.scores.length);
            default:
                scoreIndex = -100; // error value
                // Check specific ghosts
                for (int j = 0; j < MsPacManModeSelector.SPECIFIC_GHOSTS.length; j++) {
                    if (id == MsPacManModeSelector.SPECIFIC_GHOSTS[j]) {
                        scoreIndex = taskScores.otherStats.length - CommonConstants.numActiveGhosts + j;
                        break;
                    }
                }
                // Check specific levels
                for (int j = 0; j < MsPacManModeSelector.SPECIFIC_LEVELS.length; j++) {
                    if (id == MsPacManModeSelector.SPECIFIC_LEVELS[j]) {
                        scoreIndex = specificLevelScoreFirstIndexInOtherScores + j;
                        break;
                    }
                }
                // score not found
                if (scoreIndex == -100) {
                    System.out.println("Error! Fitness index does not exist: " + id);
                    System.exit(1);
                }
                break;
        }
        return (eachComponentTracksScoreToo && scoreIndex != scoreIndexInOtherScores // Game score is fitness in addition to preferred
                ? new double[]{taskScores.otherStats[scoreIndex], taskScores.otherStats[scoreIndexInOtherScores]}
                : new double[]{taskScores.otherStats[scoreIndex]});
    }

    public void loadGhosts() {
        if (ghosts == null) {
            try {
                this.ghosts = new GhostControllerFacade((NewGhostController) ClassCreation.createObject("ghostTeam"));
            } catch (NoSuchMethodException ex) {
                ex.printStackTrace();
                System.exit(1);
            }
        } else {
            ghosts.reset();
        }
    }

    /**
     * If a static pacman is being used against evolving ghosts, then this
     * method loads it.
     */
    public void loadPacMan() {
        if (mspacman == null) {
            try {
                this.mspacman = new PacManControllerFacade((NewPacManController) ClassCreation.createObject("staticPacMan"));
            } catch (NoSuchMethodException ex) {
                ex.printStackTrace();
                System.exit(1);
            }
        } else {
            mspacman.reset();
        }
    }

    public final void addObjective(MsPacManObjective o, ArrayList<MsPacManObjective<T>> list, boolean affectsSelection) {
        addObjective(o, list, null, affectsSelection);
    }

    public final void addObjective(MsPacManObjective o, ArrayList<MsPacManObjective<T>> list, Statistic override, boolean affectsSelection) {
        list.add(o);
        MMNEAT.registerFitnessFunction(o.getClass().getSimpleName(), override, affectsSelection);
    }

    @Override
    public Score<T> evaluate(Genotype<T> individual) { 
        exec.log("Genotype ID: " + individual.getId());
        return super.evaluate(individual);
    }

    @Override
    public Pair<double[], double[]> oneEval(Genotype<T> individual, int num) {
        Organism<T> organism = evolveGhosts ? new SharedNNGhosts<T>(individual) : new NNMsPacMan<T>(individual);
        if (evolveGhosts) {
            loadPacMan();
            ghosts = new GhostControllerFacade((NewGhostController) ((SharedNNGhosts<T>) organism).controller);
        } else {
            mspacman = new PacManControllerFacade((NewPacManController) ((NNMsPacMan<T>) organism).controller);
        }
        
        // Side-effects to "game"
        agentEval(mspacman, num);
        if (mspacman.newP instanceof MultinetworkMsPacManController
                && individual instanceof TWEANNGenotype) {
            // Track subnet selections as if they were modes
            ((TWEANNGenotype) individual).modeUsage = ((MultinetworkMsPacManController) mspacman.newP).fullUsage;
        }

        double[] fitnesses = new double[this.numObjectives()];
        double[] scores = new double[this.numOtherScores()];
        // When evolving ghosts, all fitness scores are flipped to negative,
        // because the ghosts are in direct opposition to pacman
        for (int j = 0; j < objectives.size(); j++) {
            fitnesses[j] = (evolveGhosts ? -1 : 1) * objectives.get(j).score(game, organism);
        }
        for (int j = 0; j < otherScores.size(); j++) {
            scores[j] = otherScores.get(j).score(game, organism);
        } 
        
        return new Pair<double[], double[]>(fitnesses, scores);
    }

    public GameFacade agentEval(PacManControllerFacade mspacman, int num) {
        //System.out.println("Agent Eval");
        if (!evolveGhosts) {
            loadGhosts();
        } 
        tcManager.preEval(); 
        long nextSeed = deterministic ? num : RandomNumbers.randomGenerator.nextLong();
        if (RunTrainedNetworks.saveActions) {RunTrainedNetworks.seed_writer.println(nextSeed);}
        game = new GameFacade(new Game(deterministic ? num : nextSeed));
        game.setExitLairEdible(exitLairEdible);
        game.setEndOnlyOnTimeLimit(endOnlyOnTimeLimit);
        game.setRandomLairExit(randomLairExit);
        game.setSimultaneousLairExit(simultaneousLairExit);
        game.setEndAfterGhostEatingChances(endAfterGhostEatingChances);
        game.playWithoutPills(noPills);
        game.playWithoutPowerPills(noPowerPills);
        game.setEndAfterPowerPillsEaten(luringTask);
        int campNum = tcManager.campSetup(game, num);
        int startingLevel = game.getCurrentLevel();
        mspacman.reset(); 
        if (CommonConstants.recordPacman) {
            exec.runGameTimedRecorded(game, mspacman, ghosts, CommonConstants.watch, Parameters.parameters.stringParameter("pacmanSaveFile")); 
//        } else if(CommonConstants.replayPacman){
//            exec.replayGame(Parameters.parameters.stringParameter("pacmanSaveFile"), CommonConstants.watch);
        } else if (CommonConstants.watch) {
            //System.out.println("Watch game: " + mspacman);
            exec.runGameTimed(mspacman, ghosts, game);
            //exec.runGameTimed(new HumanController(new KeyBoardInput()), ghosts, true, game);
        } else if (CommonConstants.timedPacman){
            exec.runGameTimedNonVisual(game, mspacman, ghosts);
        } else {
            exec.runExperiment(mspacman, ghosts, game);
        }
        tcManager.postEval(game, campNum, startingLevel); 
        if (MMNEAT.evalReport != null) { 
            mspacman.logEvaluationDetails();
        }

        return game;
    }

    @Override
    public int numOtherScores() {
        return otherScores.size();
    }

    public int numObjectives() {
        return objectives.size();
    }

    /**
     * All zeroes, since objectives are positive
     *
     * @return
     */
    @Override
    public double[] minScores() {
        double[] result = new double[numObjectives()];
        for (int i = 0; i < result.length; i++) {
            result[i] = objectives.get(i).minScore();
        }
        return result;
    }

    public double[] startingGoals() {
        return minScores();
    }

    public String[] sensorLabels() {
        return MMNEAT.pacmanInputOutputMediator.sensorLabels();
    }

    public String[] outputLabels() {
        return MMNEAT.pacmanInputOutputMediator.outputLabels();
    }

    public double getTimeStamp() {
        return game.getTotalTime();
    }
}
