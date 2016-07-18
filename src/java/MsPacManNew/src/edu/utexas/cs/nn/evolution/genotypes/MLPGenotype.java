package edu.utexas.cs.nn.evolution.genotypes;

import edu.utexas.cs.nn.evolution.EvolutionaryHistory;
import edu.utexas.cs.nn.mmneat.MMNEAT;
import edu.utexas.cs.nn.networks.MLP;
import edu.utexas.cs.nn.parameters.Parameters;
import edu.utexas.cs.nn.util.random.RandomNumbers;

/**
 *
 * @author Jacob Schrum
 */
public class MLPGenotype implements Genotype<MLP> {

    private long id = EvolutionaryHistory.nextGenotypeId();
    public double[][] firstConnectionLayer;
    public double[][] secondConnectionLayer;

    public MLPGenotype() {
        this(MMNEAT.networkInputs, Parameters.parameters.integerParameter("hiddenMLPNeurons"), MMNEAT.networkOutputs);
    }

    public MLPGenotype(int numberOfInputs, int numberOfHidden, int numberOfOutputs) {
        this(new MLP(numberOfInputs, numberOfHidden, numberOfOutputs));
    }

    public MLPGenotype(MLP mlp) {
        MLP mlpCopy = mlp.copy();
        firstConnectionLayer = mlpCopy.firstConnectionLayer;
        secondConnectionLayer = mlpCopy.secondConnectionLayer;
    }

    public Genotype<MLP> copy() {
        return new MLPGenotype(this.getPhenotype().copy());
    }

    /*
     * Directly from Togelius' code
     */
    public void mutate() {
        mutate(firstConnectionLayer);
        mutate(secondConnectionLayer);
    }

    protected void mutate(double[][] array) {
        for (int i = 0; i < array.length; i++) {
            mutate(array[i]);
        }
    }

    protected void mutate(double[] array) {
        for (int i = 0; i < array.length; i++) {
            array[i] += MMNEAT.weightPerturber.randomOutput() * Parameters.parameters.doubleParameter("mlpMutationRate");
        }
    }

    public Genotype<MLP> crossover(Genotype<MLP> g) {
        return MMNEAT.crossoverOperator.crossover(this, g);
    }

    public MLP getPhenotype() {
        return new MLP(this.firstConnectionLayer, this.secondConnectionLayer);
    }

    public Genotype<MLP> newInstance() {
        return new MLPGenotype(new MLP(this.firstConnectionLayer.length, this.secondConnectionLayer.length, this.secondConnectionLayer[0].length));
    }

    public long getId() {
        return id;
    }
}
