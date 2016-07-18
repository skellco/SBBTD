package dal.experiment;

import edu.utexas.cs.nn.experiment.Experiment;
import java.io.FileNotFoundException;
import edu.utexas.cs.nn.evolution.genotypes.Genotype;
import edu.utexas.cs.nn.mmneat.MMNEAT;
import edu.utexas.cs.nn.parameters.Parameters;
import edu.utexas.cs.nn.scores.Score;
import edu.utexas.cs.nn.tasks.LonerTask;
import edu.utexas.cs.nn.util.file.FileUtilities;
import edu.utexas.cs.nn.util.random.RandomNumbers;
import wox.serial.Easy;

import java.io.PrintWriter;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.File;

/**
 * Edited copy of edu.utexas.cs.nn.experiment.BestNetworkExperiment.java
 * @author jnip
 */

public class RunTrainedNetworks implements Experiment {

    private static final String baseFolder = "onelifesplit";
    private static final String gameType = "OneLifeSplit";
    private static final String moduleType = "TwoModules";
    private static int runNumber;
    private static int numTrials;
    
    //private static int populationSize;
    private static int currGen;
    private static int ind;
    private Genotype net;
    
    public static boolean saveActions = false;
    public static boolean randomActions = false;
    public static PrintWriter writer;
    public static PrintWriter seed_writer;
    private static String writer_filename;
    private static String seed_filename;
            
    public static void main(String[] args) throws FileNotFoundException, NoSuchMethodException {
        
        runNumber = 0; currGen = 200; ind = 80; numTrials = 100;
        Boolean visual = true; 
        //saveActions = true; randomActions = false;
        
        if (!saveActions) getSingleFitness(visual);
        else {
            try {
                writer_filename = "actionslist/actionString-" + runNumber + "_" + currGen + "_" + ind + "-" + "games" + numTrials + ".txt";
                seed_filename = "actionslist/seedNumber-" + runNumber + "_" + currGen + "_" + ind + "-" + "games" + numTrials + ".txt";
                
                if (randomActions) {
                    writer_filename = "actionslist/actionString-random.txt";
                    seed_filename = "actionslist/seedNumber-random.txt";
                }
                
                if ( (new File(writer_filename)).exists() ) {
                    System.out.println("ERROR: \nCannot save to "+ writer_filename + ", file already exists."); 
                    System.exit(1);
                }
                
                writer = new PrintWriter(writer_filename, "UTF-8");
                seed_writer = new PrintWriter(seed_filename, "UTF-8");
                
                //Write each game to one line
                getSingleFitness(visual);
                
                writer.close();
                seed_writer.close();
                
                //Take X number of lines and combine into 1
                int X = 10; createActionStrings(numTrials, X);
            } 
            catch(Exception e) {
                System.out.println("FAILED to create writer(A): " + e);
                e.printStackTrace();
            }
        }
        
        //int startGen = 2; int endGen = 2; /*populationSize = 100;*/ 
        //getFitnessOfAllIndividuals(startGen, endGen);
    }
    
    private static void getFitnessOfAllIndividuals(int startGeneration, int endGeneration) throws FileNotFoundException, NoSuchMethodException {
        for (currGen = startGeneration; currGen <= endGeneration; currGen++) {
            //for (int ind = 0; ind < populationSize; ind++) {
            for (ind = 0; ind < 100; ind++) {    
                System.out.println("Gen:" + currGen + " Ind:" + ind + "\n");
                MMNEAT.main(customArgs1()); 
                System.out.print("\n----------------------------------------------------------------------------------------------");
                System.out.println("----------------------------------------------------------------------------------------------\n");
            }
        }
    }
    
    private static void getSingleFitness(Boolean visual) throws FileNotFoundException, NoSuchMethodException {
        System.out.println("Gen:" + currGen + " Ind:" + ind + "\n");
        if (saveActions) MMNEAT.main(customArgs3());
        else { if (visual) MMNEAT.main(customArgs2()); else MMNEAT.main(customArgs1()); }
        System.out.print("\n----------------------------------------------------------------------------------------------");
        System.out.println("----------------------------------------------------------------------------------------------\n");
    }
    
    private static void createActionStrings(int numTrials, int numLinesToCombine) {
         String final_filename = "actionslist/final/" + runNumber + "_" + currGen + "_" + ind + "-" + "games" + numTrials;
         if (randomActions) final_filename = "actionslist/final/random";
         int letterOffset = 'A'; char letter;
         try {
            PrintWriter final_writer;
            BufferedReader reader = new BufferedReader(new FileReader(new File(writer_filename)));
            for (int i = 0; i < numTrials/numLinesToCombine; i++) {
                
                letter = (char)(letterOffset + i);
                final_writer = new PrintWriter(final_filename + letter + ".txt", "UTF-8"); 
                
                for (int j = 0; j < numLinesToCombine; j++) {
                    final_writer.print(reader.readLine());
                }
                
                final_writer.println();
                final_writer.close();
                        
            }
            reader.close();
         } 
         catch (Exception e) {System.out.println("FAILED to create writer(B): " + e);e.printStackTrace();}
    }
    
    private static String[] customArgs1() {
        String[] customArgs = new String[16];
        customArgs[0] = "runNumber:" + runNumber;
        customArgs[1] = "parallelEvaluations:false";
        customArgs[2] = "experiment:dal.experiment.RunTrainedNetworks";
        customArgs[3] = "base:" + baseFolder;
        customArgs[4] = "log:" + gameType + "-" + moduleType;
        customArgs[5] = "saveTo:" + moduleType;
        customArgs[6] = "trials:" + numTrials;
        customArgs[7] = "watch:false";
        customArgs[8] = "showNetworks:false";
        customArgs[9] = "io:false";
        customArgs[10] = "netio:false";
        customArgs[11] = "onlyWatchPareto:true";
        customArgs[12] = "printFitness:true";
        customArgs[13] = "animateNetwork:false";
        customArgs[14] = "monitorInputs:false";
        customArgs[15] = "modePheremone:false";
        return customArgs;
    }
    
    private static String[] customArgs2() {
        String[] customArgs = new String[17];
        customArgs[0] = "runNumber:" + runNumber;
        customArgs[1] = "parallelEvaluations:false";
        customArgs[2] = "experiment:dal.experiment.RunTrainedNetworks";
        customArgs[3] = "base:" + baseFolder;
        customArgs[4] = "log:" + gameType + "-" + moduleType;
        customArgs[5] = "saveTo:" + moduleType;
        customArgs[6] = "trials:" + numTrials;
        customArgs[7] = "watch:false";
        customArgs[8] = "showNetworks:false";
        customArgs[9] = "io:false";
        customArgs[10] = "netio:false";
        customArgs[11] = "onlyWatchPareto:true";
        customArgs[12] = "printFitness:true";
        customArgs[13] = "animateNetwork:false";
        customArgs[14] = "monitorInputs:false";
        customArgs[15] = "modePheremone:false";
        customArgs[16] = "watch:true";
        return customArgs;
    }
    
    private static String[] customArgs3() {
        String[] customArgs = new String[16];
        customArgs[0] = "runNumber:" + runNumber;
        customArgs[1] = "parallelEvaluations:false";
        customArgs[2] = "experiment:dal.experiment.RunTrainedNetworks";
        customArgs[3] = "base:" + baseFolder;
        customArgs[4] = "log:" + gameType + "-" + moduleType;
        customArgs[5] = "saveTo:" + moduleType;
        customArgs[6] = "trials:" + numTrials;
        customArgs[7] = "watch:false";
        customArgs[8] = "showNetworks:false";
        customArgs[9] = "io:false";
        customArgs[10] = "netio:false";
        customArgs[11] = "onlyWatchPareto:true";
        customArgs[12] = "printFitness:false";
        customArgs[13] = "animateNetwork:false";
        customArgs[14] = "monitorInputs:false";
        customArgs[15] = "modePheremone:false";
        return customArgs;
    }
    
    public void init() {
        String dir = FileUtilities.getSaveDirectory() + "/gen" + currGen; 
        net = (Genotype) Easy.load(dir + "/" + gameType + "-" + moduleType + runNumber + "_gen" + currGen + "_" + ind + ".xml");
    }

    public void run() {
        RandomNumbers.reset();
        Score s = ((LonerTask) MMNEAT.task).evaluateOne(net);
        if (!saveActions) System.out.println(s);
    }

    public boolean shouldStop() {
        // Will never be called
        return true;
    }
}
