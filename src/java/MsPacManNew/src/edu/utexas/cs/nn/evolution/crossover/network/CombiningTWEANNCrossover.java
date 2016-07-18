package edu.utexas.cs.nn.evolution.crossover.network;

import edu.utexas.cs.nn.evolution.EvolutionaryHistory;
import edu.utexas.cs.nn.evolution.genotypes.Genotype;
import edu.utexas.cs.nn.evolution.genotypes.TWEANNGenotype;
import edu.utexas.cs.nn.evolution.genotypes.TWEANNGenotype.LinkGene;
import edu.utexas.cs.nn.evolution.genotypes.TWEANNGenotype.NodeGene;
import edu.utexas.cs.nn.graphics.DrawingPanel;
import edu.utexas.cs.nn.mmneat.MMNEAT;
import edu.utexas.cs.nn.networks.TWEANN;
import edu.utexas.cs.nn.parameters.Parameters;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Scanner;

/**
 * Creates a new network via crossover in which the component networks are two
 * separate modes.
 *
 * @author Jacob Schrum
 */
public class CombiningTWEANNCrossover extends TWEANNCrossover {

    /**
     * Saved across all such crossovers so that the reassignment of innovation
     * numbers to the network on the right side maintains evolutionary history
     */
    public static HashMap<Long, Long> oldToNew = new HashMap<Long, Long>();

    /**
     * Save contents of oldToNew to file
     *
     * @param filename filename to save to
     */
    public static void saveOldToNew(String filename) {
        assert !oldToNew.isEmpty();
        try {
            PrintStream ps = new PrintStream(new FileOutputStream(new File(filename)));
            for (Long key : oldToNew.keySet()) {
                ps.println(key + " " + oldToNew.get(key));
            }
            ps.close();
            System.out.println("Done saving " + filename);
        } catch (FileNotFoundException ex) {
            System.out.println("Could not save file: " + filename);
            System.out.println(oldToNew);
            System.exit(1);
        }
    }

    /**
     * Load contents of file into oldToNew
     *
     * @param filename file containing pairs
     */
    public static void loadOldToNew(String filename) {
        try {
            oldToNew = new HashMap<Long, Long>();
            Scanner s = new Scanner(new File(filename));
            while (s.hasNext()) {
                Scanner line = new Scanner(s.nextLine());
                long key = line.nextLong();
                long value = line.nextLong();
                oldToNew.put(key, value);
                line.close();
            }
            s.close();
            System.out.println("Done loading " + filename);
        } catch (FileNotFoundException ex) {
            System.out.println("Could not load file " + filename);
            System.exit(1);
        }
    }

    public static long getAdjustedInnovationNumber(long oldInnovation) {
        if (oldToNew.containsKey(oldInnovation)) {
            return oldToNew.get(oldInnovation);
        } else {
            long newInnovation = EvolutionaryHistory.nextInnovation();
            //System.out.print("Map("+oldInnovation +" to "+ newInnovation+"):");
            oldToNew.put(oldInnovation, newInnovation); // Remember the changes to match up the links
            return newInnovation; // Change innovation to prevent weird overlaps
        }
    }
    private final boolean multitask;
    private final boolean splitInputs;

    public CombiningTWEANNCrossover() {
        this(Parameters.parameters.booleanParameter("multitaskCombiningCrossover"), false);
    }

    public CombiningTWEANNCrossover(boolean multitask, boolean splitInputs) {
        super();
        this.multitask = multitask;
        this.splitInputs = splitInputs;
    }

    @Override
    public Genotype<TWEANN> crossover(Genotype<TWEANN> toModify, Genotype<TWEANN> toReturn) {
        TWEANNGenotype tToModify = (TWEANNGenotype) toModify;
        TWEANNGenotype tToReturn = (TWEANNGenotype) toReturn;

//        System.out.println("---------------------------------------------------------");
//        System.out.println("Crossover between " + tToModify.getId() + " (out "+ tToModify.numOut+") and " + tToReturn.getId() + " (out " + tToReturn.numOut + ")");

        if (tToModify.numModes > 1 || tToReturn.numModes > 1) {
            // Can only combine single mode networks. Else, do regular crossover
            //System.out.println("Regular crossover between " + toModify.getId() + " (out "+ tToModify.numOut+") and " + toReturn.getId() + " (out " + tToReturn.numOut + ")");
            //System.out.println("Initial modes: " + ((TWEANNGenotype)toModify).numModes + " and " + ((TWEANNGenotype)toReturn).numModes);
            Genotype<TWEANN> result = super.crossover(toModify, toReturn);
            TWEANNGenotype tgResult = (TWEANNGenotype) result;
//            if(tgResult.multitask){
//                System.out.println("tgResuilt was multitask before");
//            }
            tgResult.multitask = multitask && tgResult.numModes > 1;
//            if(tgResult.multitask){
//                System.out.println("tgResuilt is still multitask");
//            } else {
//                System.out.println("tgResuilt is NOT multitask anymore");
//            }
            removeDeadLinks(tgResult);
//            if (tToModify.multitask) {
//                System.out.println("toModify was multitask");
//            }
            tToModify.multitask = multitask && tToModify.numModes > 1;
//            if (tToModify.multitask) {
//                System.out.println("toModify is still multitask");
//            } else {
//                System.out.println("toModify is NOT multitask now");
//            }
            removeDeadLinks(tToModify);
            //System.out.println("End modes: " + ((TWEANNGenotype) toModify).numModes + " and " + ((TWEANNGenotype) result).numModes);
            //System.out.println("Crossover done: mod " + tToModify.getId() + " (out "+ tToModify.numOut+") and result " + tgResult.getId() + " (out "+ tgResult.numOut+")");
            return result;
        }

        // Preference neurons are missing, and need to be added
        if (tToModify.numOut == tToModify.neuronsPerMode && TWEANN.preferenceNeuron()) {
            tToModify.addRandomPreferenceNeuron(1);
            tToReturn.addRandomPreferenceNeuron(1);
            assert tToModify.numOut == tToModify.neuronsPerMode + 1 : "Did not add a preference neuron to tToModify";
            assert tToReturn.numOut == tToReturn.neuronsPerMode + 1 : "Did not add a preference neuron to tToReturn";
        }

        assert (tToModify.numOut == tToReturn.numOut) : "Networks to combine have different number of outputs";
        int neuronsPerMode = tToModify.neuronsPerMode;
        assert (tToModify.numIn == tToReturn.numIn || splitInputs) : "Networks to combine have different number of inputs";
        ArrayList<TWEANNGenotype.NodeGene> combinedNodes = new ArrayList<TWEANNGenotype.NodeGene>(tToModify.nodes.size() + tToReturn.nodes.size());

        int modifyNumIn = tToModify.numIn;
        int returnNumIn = tToReturn.numIn;
        int modifyNumOut = tToModify.numOut;
        int numOut = tToModify.numOut;
        // INPUTS
        // Only one set of inputs
        for (int i = 0; i < modifyNumIn; i++) {
            NodeGene nodeGene = tToModify.nodes.get(i).clone();
            combinedNodes.add(nodeGene);
            if (!splitInputs) {
                long innovation = nodeGene.innovation;
                oldToNew.put(innovation, innovation); // Input mapping remains same
            }
        }
        if (splitInputs) {
            // Add second set of inputs
            for (int i = 0; i < returnNumIn; i++) {
                NodeGene nodeGene = tToReturn.nodes.get(i).clone();
                long newInnovation = -(modifyNumIn + modifyNumOut + 1) - i;
                oldToNew.put(nodeGene.innovation, newInnovation);
                nodeGene.innovation = newInnovation;
                nodeGene.fromCombiningCrossover = true;
                // index will be -1 if innovation does not exist
                int index = EvolutionaryHistory.indexOfArchetypeInnovation(tToModify.archetypeIndex, newInnovation);
                if (index == -1) {
                    EvolutionaryHistory.archetypeAdd(tToModify.archetypeIndex, combinedNodes.size(), nodeGene, false, "combined input");
                }
                combinedNodes.add(nodeGene);
            }
        }
        //System.out.println("\tINPUTS DONE");
        // HIDDEN
        // Nodes from tToModify keep their innovation numbers
        int modifyNodes = tToModify.nodes.size();
        int modifyFirstOutput = modifyNodes - numOut;
        for (int i = modifyNumIn; i < modifyFirstOutput; i++) {
            combinedNodes.add(tToModify.nodes.get(i).clone());
        }
        // Nodes from tToReturn need new innovation numbers
        int returnNodes = tToReturn.nodes.size();
        int returnFirstOutput = returnNodes - numOut;
        for (int i = returnNumIn; i < returnFirstOutput; i++) {
            NodeGene nodeGene = tToReturn.nodes.get(i).clone();
            long oldInnovation = nodeGene.innovation;
            nodeGene.innovation = getAdjustedInnovationNumber(oldInnovation); // Change innovation to prevent weird overlaps
            assert (oldToNew.containsKey(oldInnovation)) :
                    "Archetype should contain two copies of all spliced nodes!\n"
                    + "No mapping for innovation: " + oldInnovation + "\n"
                    + "Archetype: " + EvolutionaryHistory.archetypes[tToModify.archetypeIndex] + "\n"
                    + "tToReturn:" + tToReturn;
//            int index = EvolutionaryHistory.indexOfArchetypeInnovation(tToModify.archetypeIndex, nodeGene.innovation);
//            if (index == -1) {
//                newArchetypeNodes.add(nodeGene);
//                //EvolutionaryHistory.archetypeAdd(tToModify.archetypeIndex, EvolutionaryHistory.indexOfArchetypeInnovation(tToModify.archetypeIndex, oldInnovation)+1, nodeGene, false, "copied combined hidden");
//            }
            combinedNodes.add(nodeGene);
        }
        //System.out.println("\tABOUT TO ADD HIDDEN TO ARCHETYPE");
        // Copy all hidden nodes in the archetype
        ArrayList<NodeGene> newArchetypeNodes = new ArrayList<NodeGene>();
        for(NodeGene ng : EvolutionaryHistory.archetypes[tToModify.archetypeIndex]){
            if(ng.ntype == TWEANN.Node.NTYPE_HIDDEN){
                long oldInnovation = ng.innovation;
                long newInnovation = getAdjustedInnovationNumber(oldInnovation);
                if(oldInnovation != newInnovation && EvolutionaryHistory.indexOfArchetypeInnovation(tToModify.archetypeIndex, newInnovation) == -1){
                    NodeGene copy = ng.clone();
                    copy.innovation = newInnovation;
                    newArchetypeNodes.add(copy);
                }
            }
        }
        // Add new nodes to archetype
        for (NodeGene ng : newArchetypeNodes) {
            EvolutionaryHistory.archetypeAdd(tToModify.archetypeIndex, EvolutionaryHistory.firstArchetypeOutputIndex(tToModify.archetypeIndex), ng, false, "copied combined hidden");
        }
        //System.out.println("\tHIDDEN DONE");
        // OUTPUTS
        // Copy outputs from first network directly
        int net1Outputs = 0;
        for (int i = modifyFirstOutput; i < modifyNodes; i++) {
            combinedNodes.add(tToModify.nodes.get(i).clone());
            net1Outputs++;
        }
        //System.out.println("\t\tnet1Outputs:"+net1Outputs+"\t\t");
        assert net1Outputs == tToModify.neuronsPerMode + (TWEANN.preferenceNeuron() ? 1 : 0) : "Number of outputs added is wrong";
        // Outputs from second network have innovation numbers changed
        for (int i = returnFirstOutput; i < returnNodes; i++) {
            NodeGene nodeGene = tToReturn.nodes.get(i).clone();
            long oldInnovation = nodeGene.innovation;
            // Uniform innovation numbers for output layer
            long newInnovation = oldInnovation - net1Outputs;
            //System.out.println("\t\tnewInnovation:"+newInnovation+"\t\t");
            oldToNew.put(oldInnovation, newInnovation);
            // The newly inserted innovation number will be used
            nodeGene.innovation = getAdjustedInnovationNumber(oldInnovation); // Change innovation to prevent weird overlaps
            nodeGene.fromCombiningCrossover = true;
            int index = EvolutionaryHistory.indexOfArchetypeInnovation(tToModify.archetypeIndex, nodeGene.innovation);
            if (index == -1) {
                //System.out.print("ArchOut:"+nodeGene.innovation+":");
                EvolutionaryHistory.archetypeAdd(tToModify.archetypeIndex, nodeGene, "combined out");
            }
            combinedNodes.add(nodeGene);
        }
        //System.out.println();
        //System.out.println("\tOUTPUTS DONE");

        // Now deal with the links
        ArrayList<TWEANNGenotype.LinkGene> combinedLinks = new ArrayList<TWEANNGenotype.LinkGene>(tToModify.links.size() + tToReturn.links.size());
        // Order for links doesn't matter, so copy all from first network
        for (LinkGene l : tToModify.links) {
            combinedLinks.add(l.clone());
        }
        // Links from second network need new innovations
        for (LinkGene l : tToReturn.links) {
            LinkGene linkGene = l.clone();
            linkGene.innovation = getAdjustedInnovationNumber(linkGene.innovation); // Link itself gets new number
            linkGene.sourceInnovation = oldToNew.get(linkGene.sourceInnovation); // Connections lead to existing structure
            linkGene.targetInnovation = oldToNew.get(linkGene.targetInnovation);
            combinedLinks.add(linkGene);
        }
        //System.out.println("\tLINKS DONE");

        // The network to modify has the combined1 network
        tToModify.nodes = combinedNodes;
        tToModify.links = combinedLinks;
        tToModify.multitask = multitask;
        tToModify.calculateNumModes();

        ArrayList<TWEANNGenotype.NodeGene> combinedNodesCopy = new ArrayList<TWEANNGenotype.NodeGene>(combinedNodes.size());
        for (int i = 0; i < combinedNodes.size(); i++) {
            combinedNodesCopy.add(combinedNodes.get(i).clone());
        }
        ArrayList<TWEANNGenotype.LinkGene> combinedLinksCopy = new ArrayList<TWEANNGenotype.LinkGene>(combinedLinks.size());
        for (int i = 0; i < combinedLinks.size(); i++) {
            combinedLinksCopy.add(combinedLinks.get(i).clone());
        }
        // The TWEANN returned is the same as the one modified
        TWEANNGenotype combinedCopy = new TWEANNGenotype(combinedNodesCopy, combinedLinksCopy, neuronsPerMode, multitask, 0);
        combinedCopy.calculateNumModes();
        //System.out.println("Combining Crossover done: mod " + tToModify.getId() + " and copy " + combinedCopy.getId());
        return combinedCopy;
    }

    /**
     * Crossover between a multitask and non-multitask network creates an
     * unusual situation in which links may remain in a link list that point to
     * an output node that no longer exists. Such links need to be removed,
     * which is done by this method. This method also removes links that
     * originate from removed nodes.
     *
     * @param tweannGenotype
     */
    private boolean removeDeadLinks(TWEANNGenotype tweannGenotype) {
        Iterator<LinkGene> itr = tweannGenotype.links.iterator();
        boolean anyRemoved = false;
        while (itr.hasNext()) {
            LinkGene next = itr.next();
            if (!containsNodeWithInnovation(tweannGenotype.nodes, next.targetInnovation)
                    || !containsNodeWithInnovation(tweannGenotype.nodes, next.sourceInnovation)) {
                itr.remove();
                anyRemoved = true;
//                if (next.targetInnovation == 0) {
//                    System.out.println("Removed a dead-end link: " + next);
//                }
            }
        }
        tweannGenotype.calculateNumModes();
        return anyRemoved;
    }

    /**
     * Checks to see if the list of node genes contains a gene with the searched
     * for innovation number.
     *
     * @param nodes list of NodeGene objects
     * @param targetInnovation innovation to search for
     * @return true if found
     */
    private boolean containsNodeWithInnovation(ArrayList<NodeGene> nodes, long targetInnovation) {
        for (NodeGene ng : nodes) {
            if (ng.innovation == targetInnovation) {
                return true;
            }
        }
        return false;
    }

    public static void main(String[] args) {
        Parameters.initializeParameterCollections(new String[]{"io:false", "multitaskCombiningCrossover:false"});
        MMNEAT.loadClasses();
        TWEANNGenotype tg1 = new TWEANNGenotype(5, 2, 0);
        MMNEAT.genotype = tg1.copy();
        EvolutionaryHistory.initArchetype(0);
        CombiningTWEANNCrossover cross = new CombiningTWEANNCrossover(true, false);
        TWEANNGenotype tg2 = new TWEANNGenotype(5, 2, 0);

        final int MUTATIONS1 = 20;
        final int MUTATIONS2 = 10;

        for (int i = 0; i < MUTATIONS1; i++) {
            tg1.mutate();
            tg2.mutate();
        }

        DrawingPanel p1 = new DrawingPanel(TWEANN.NETWORK_VIEW_DIM, TWEANN.NETWORK_VIEW_DIM, "Left");
        DrawingPanel p2 = new DrawingPanel(TWEANN.NETWORK_VIEW_DIM, TWEANN.NETWORK_VIEW_DIM, "Right");
        p2.setLocation(TWEANN.NETWORK_VIEW_DIM + 10, 0);
        tg1.getPhenotype().draw(p1, true);
        tg2.getPhenotype().draw(p2, true);

        TWEANNGenotype combined1 = (TWEANNGenotype) cross.crossover(tg1, tg2);
        DrawingPanel p3 = new DrawingPanel(TWEANN.NETWORK_VIEW_DIM, TWEANN.NETWORK_VIEW_DIM, "Combined 1");
        p3.setLocation(0, TWEANN.NETWORK_VIEW_DIM + 10);

        combined1.getPhenotype().draw(p3, true);

        //System.out.println("Archetype:" + EvolutionaryHistory.archetypes[0]);

        tg2 = (TWEANNGenotype) tg2.copy();
        for (int i = 0; i < MUTATIONS2; i++) {
            tg2.mutate();
        }

        DrawingPanel p4 = new DrawingPanel(TWEANN.NETWORK_VIEW_DIM, TWEANN.NETWORK_VIEW_DIM, "Net 2 Modified");
        p4.setLocation(TWEANN.NETWORK_VIEW_DIM + 10, TWEANN.NETWORK_VIEW_DIM + 10);

        tg2.getPhenotype().draw(p4, true);

        TWEANNGenotype combined2 = (TWEANNGenotype) cross.crossover(tg2, combined1);
        DrawingPanel p5 = new DrawingPanel(TWEANN.NETWORK_VIEW_DIM, TWEANN.NETWORK_VIEW_DIM, "Combined 2 Return");
        p5.setLocation(2 * TWEANN.NETWORK_VIEW_DIM + 20, TWEANN.NETWORK_VIEW_DIM + 10);

        combined2.getPhenotype().draw(p5, true);

        DrawingPanel p6 = new DrawingPanel(TWEANN.NETWORK_VIEW_DIM, TWEANN.NETWORK_VIEW_DIM, "Combined 2 Mod");
        p6.setLocation(2 * TWEANN.NETWORK_VIEW_DIM + 20, 0);

        tg2.getPhenotype().draw(p6, true);
    }
}
