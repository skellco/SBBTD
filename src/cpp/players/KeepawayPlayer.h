/*
Copyright (c) 2004 Gregory Kuhlmann, Peter Stone
University of Texas at Austin
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the University of Amsterdam nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef KEEPAWAY_PLAYER
#define KEEPAWAY_PLAYER

#include "BasicPlayer.h"
#include "SarsaAgent.h"

#define ACTION_KWY_DRIBBLE_NORMAL 0
#define ACTION_KWY_PASS_TO_K_2 1
#define ACTION_KWY_PASS_TO_K_3 2
#define ACTION_KWY_PASS_TO_K_4 3
#define ACTION_KWY_PASS_TO_K_5 4

#define NUM_KEEPAWAY_FEATURES 11
#define NUM_KEEPAWAY_ACTIONS 4


/*! This class is a superclass from BasicPlayer and contains a more
    sophisticated decision procedure to determine the next action. */
class KeepawayPlayer:public BasicPlayer
{
 int m_stopAfter;
 int m_episodeCount;
 protected:
  bool          bContLoop;               /*!< is server is alive             */

  Time          m_timeLastSay;           /*!< last time communicated         */
  Time          m_timeStartEpisode;
  PolicyAgent     *PA;

  int m_episodeNumber;
  
  double lastState[MAX_STATE_VARS];
  int lastAction; 
  
  // methods associated with saying (defined in KeepawayPlayer.cc)
  bool          shallISaySomething        (                                  );
  void          makeSayMessage            ( SoccerCommand  soc,
					    char *         str               );

 public:
  int m_policy;
  KeepawayPlayer                          ( PolicyAgent     *pa,
					    ActHandler     *a,
                                            WorldModel     *wm,
                                            ServerSettings *ss,
                                            PlayerSettings *cs,
                                            char           *strTeamName,
					    int            iNumKeepers,
					    int            iNumTakers,
					    bool           isGoalie,
                                            double         dVersion,
                                            int            startEpisode, 
					    int            iReconnect = -1,
                                            int            iStopAfter = 1,
                                            int            iPolicy = 0 );

  void          mainLoop                  (                                  );

  // behaviours
  virtual SoccerCommand player(int &m_episodeCount) = 0;
  SoccerCommand keeperSupport( ObjectT fastest );
  SoccerCommand interpretKeeperAction( int action );

  ObjectT chooseLookObject( double ballThr );

  ///////KEEPAWAY UTILITIES///////
  VecPosition KWYbestPointToMove();
  VecPosition KWYoptimiseBestPointToMoveTo(VecPosition point, double dis);
  double KWYgetOpenGoodness(VecPosition point);
  
  SoccerCommand KWYgetOpen();
  
  ///////KEEPAWAY ACTIONS///////
  SoccerCommand KWYpassToTeammate(int teammate);
  SoccerCommand KWYdribbleToGoal();
  SoccerCommand KWYdribbleAwayFromTakers();
  SoccerCommand KWYdribbleNormal();

};

#endif

