package edu.utexas.cs.nn.evolution.mutation.tweann;

import edu.utexas.cs.nn.evolution.genotypes.Genotype;
import edu.utexas.cs.nn.evolution.genotypes.TWEANNGenotype;
import edu.utexas.cs.nn.networks.TWEANN;
import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.parameters.Parameters;

/**
 * @author Jacob Schrum
 */
public abstract class ModeMutation extends TWEANNMutation {

    public ModeMutation(String rate) {
        super(rate);
    }

    public void mutate(Genotype<TWEANN> genotype) {
        if (CommonConstants.weakenBeforeModeMutation) {
            // 0.5 is weakening proportion ... make param
            ((TWEANNGenotype) genotype).weakenAllModes(Parameters.parameters.doubleParameter("weakenPortion"));
            if(infoTracking != null) {
                infoTracking.append("WEAKEN ");
            }
        }
        // Option to freeze existing network before adding new mode
        if (CommonConstants.freezeBeforeModeMutation) {
            ((TWEANNGenotype) genotype).freezeNetwork();
            if(infoTracking != null) {
                infoTracking.append("FREEZE ");
            }
        }
        addMode((TWEANNGenotype) genotype);
    }

    abstract public void addMode(TWEANNGenotype genotype);
}
