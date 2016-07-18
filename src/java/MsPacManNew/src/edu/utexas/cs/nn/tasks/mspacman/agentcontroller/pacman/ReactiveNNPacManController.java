package edu.utexas.cs.nn.tasks.mspacman.agentcontroller.pacman;

import edu.utexas.cs.nn.networks.Network;
import edu.utexas.cs.nn.parameters.CommonConstants;
import edu.utexas.cs.nn.tasks.mspacman.facades.GameFacade;

/**
 *
 * @author Jacob Schrum
 */
public class ReactiveNNPacManController extends NNDirectionalPacManController {

    public ReactiveNNPacManController(Network n) {
        super(n);
    }

    @Override
    public double[] getDirectionPreferences(GameFacade gf) {
        final int current = gf.getPacmanCurrentNodeIndex();
        double[] inputs = inputMediator.getInputs(gf, gf.getPacmanLastMoveMade());
        if (nn.isMultitask()) {
            ms.giveGame(gf);
            nn.chooseMode(ms.mode());
        }
        double[] outputs = nn.process(inputs);
        //System.out.println("OUT:"+Arrays.toString(outputs));

        // Make directions towards walls impossible to choose
        final int referenceDir = CommonConstants.relativePacmanDirections ? gf.getPacmanLastMoveMade() : 0;
        if (CommonConstants.eliminateImpossibleDirections) {
            final int[] neighbors = gf.neighbors(current); System.out.println("Blahhhh");
            for (int i = 0; i < GameFacade.NUM_DIRS; i++) {
                int dir = (referenceDir + i) % GameFacade.NUM_DIRS;
                // Disable blocked paths
                if (neighbors[dir] == -1) {
                    outputs[i] = -Double.MAX_VALUE;
                }
            }
        }

        double[] absoluteDirectionPreferences = new double[GameFacade.NUM_DIRS + (externalPreferenceNeurons ? 1 : 0)];
        for (int i = 0; i < GameFacade.NUM_DIRS; i++) {
            int dir = (referenceDir + i) % GameFacade.NUM_DIRS;
            absoluteDirectionPreferences[dir] = outputs[i];
        }
        if (externalPreferenceNeurons) {
            absoluteDirectionPreferences[absoluteDirectionPreferences.length - 1] = outputs[absoluteDirectionPreferences.length - 1];
        }

        return absoluteDirectionPreferences;
    }
}
