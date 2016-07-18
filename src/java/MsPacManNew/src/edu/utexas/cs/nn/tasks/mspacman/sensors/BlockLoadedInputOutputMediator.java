package edu.utexas.cs.nn.tasks.mspacman.sensors;

import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.tasks.mspacman.facades.GameFacade;
import edu.utexas.cs.nn.tasks.mspacman.sensors.blocks.MsPacManSensorBlock;
import java.util.ArrayList;
import java.util.Arrays;

/**
 *
 * @author Jacob Schrum
 */
public class BlockLoadedInputOutputMediator extends MsPacManControllerInputOutputMediator {

    public static double[] this_is_the_inputs;
    public ArrayList<MsPacManSensorBlock> blocks;
    private int numSensors = 0;

    public BlockLoadedInputOutputMediator() {
        super();
        blocks = new ArrayList<MsPacManSensorBlock>();
    }

    @Override
    public double[] getInputs(GameFacade gs, int currentDir) {
        double[] inputs = new double[numIn()];
        int in = 0;
        for (int i = 0; i < blocks.size(); i++) {
            //System.out.println(blocks.get(i).getClass()); //Joe
            in = CommonConstants.pacManSensorCaching
                    ? blocks.get(i).retrieveSensors(inputs, in, gs, currentDir)
                    : blocks.get(i).incorporateSensors(inputs, in, gs, currentDir);
        }
        //int loop = 1; while (loop==1){} //Joe
        assert (in == numIn()) : "Improper inputs for Ms Pac-Man. Only " + in + " inputs: " + Arrays.toString(inputs);
        this_is_the_inputs = new double[inputs.length];
        for (int i = 0; i < inputs.length; i++){
            this_is_the_inputs[i] = inputs[i];
        }
        return inputs;
    }

    @Override
    public String[] sensorLabels() {
        String[] labels = new String[numIn()];
        int in = 0;
        for (int i = 0; i < blocks.size(); i++) {
            in = blocks.get(i).incorporateLabels(labels, in);
        }
        assert (in == numIn()) : "Improper inputs for Ms Pac-Man. Only " + in + " inputs: " + Arrays.toString(labels);
        return labels;
    }

    /**
     * Save result of calculation and reuse instead of repeating
     *
     * @return
     */
    @Override
    public int numIn() {
        if (numSensors == 0) {
            for (int i = 0; i < blocks.size(); i++) {
                numSensors += blocks.get(i).numberAdded();
            }
        }
        return numSensors;
    }

    @Override
    public void reset() {
        super.reset();
        for (MsPacManSensorBlock b : blocks) {
            b.reset();
        }
    }
}
