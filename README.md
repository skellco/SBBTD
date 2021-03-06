This code distribution is a work in progress and is intended for research purposes only. 
Detailed tutorial coming soon.

Symbiotic Bid-Based GP (SBB) is a hierarchical framework for coevolving teams 
of simple programs. SBBTD is an extension of the original c++ code for 
reinforcement learning tasks, with additional support for transfer learning and task-agnostic diversity mechanisms.

This code was developed primarily to evolve agent behaviours for Half Field Offense (Robocup 2D Soccer) and Ms. Pac-Man. Animations of behaviours in each domain, which illustrate how the modular GP strategies support decision making during gameplay, can be viewed at the following links:

Half Field Offense: https://youtu.be/ZPqa-m0x4so

Ms. Pac-Man: https://youtu.be/xRt0AVn5HlI

(Note that you can change the speed of video playback by clicking on settings in the lower-right corner.)

BACKGROUND

Full description of SBB is available in:

Lichodzijewski, P. (2011) A Symbiotic Bid-Based (SBB) framework for problem decomposition using Genetic Programming, PhD Thesis

Further details on SBBTD can be found in:

S. Kelly and M. I. Heywood, “Knowledge transfer from keepaway soccer to half-field offensethrough program symbiosis: Building simple programs for a complex task,” in Proceedings of the ACM Genetic and Evolutionary Computation Conference, 2015, pp. 1143–1150.

S. Kelly and M. I. Heywood, “Genotypic versus behavioural diversity for teams of programs under the 4-v-3 keepaway soccer task,” in Proceedings of the AAAI
Conference on Artificial Intelligence, 2014, pp. 3110–3111.

S. Kelly, P. Lichodzijewski, and M. I. Heywood, “On run time libraries and hierarchical symbiosis,” in IEEE Congress on Evolutionary Computation, 2012, pp. 3245–3252.

TASK ENVIRONMENTS

Code for the Half Field Offense task (src/cpp/players) is derived from earlier work by Peter Stone and Shivaram Kalyanakrishnan, now published here: https://www.cs.utexas.edu/~AustinVilla/sim/halffieldoffense/

The Ms. Pac-Man environment included herein (src/java/MsPacManNew) is a modified version of the code released by Jacob Schrum, available here: http://www.cs.utexas.edu/users/ai-lab/?mm-neat

FOR MORE INFORMATION CONTACT

stephen.kelly@dal.ca
