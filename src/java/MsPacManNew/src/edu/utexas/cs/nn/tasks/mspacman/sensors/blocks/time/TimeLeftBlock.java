/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.utexas.cs.nn.tasks.mspacman.sensors.blocks.time;

import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.tasks.mspacman.facades.GameFacade;
import edu.utexas.cs.nn.tasks.mspacman.sensors.blocks.MsPacManSensorBlock;

/**
 *
 * @author Jacob
 */
public class TimeLeftBlock extends MsPacManSensorBlock {

    @Override
    public int incorporateSensors(double[] inputs, int in, GameFacade gf, int lastDirection) {
        int levelTime = gf.getCurrentLevelTime();
        inputs[in++] = (CommonConstants.pacManLevelTimeLimit - levelTime) / (CommonConstants.pacManLevelTimeLimit * 1.0);
        return in;
    }

    @Override
    public int incorporateLabels(String[] labels, int startPoint) {
        labels[startPoint++] = "Time Remaining";
        return startPoint;
    }

    @Override
    public int numberAdded() {
        return 1;
    }
}
