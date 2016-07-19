/*
   Copyright (c) 2004, Gregory Kuhlmann, Peter Stone 
   University of Texas at Austin               
   All right reserved

   Based On:     

   Copyright (c) 2000-2003, Jelle Kok, University of Amsterdam
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

/*! \file main.cpp
  <pre>
  <b>File:</b>          main.cpp
  <b>Project:</b>       Robocup Soccer Simulation Team: UvA Trilearn
  <b>Authors:</b>       Jelle Kok
  <b>Created:</b>       28/11/2000
  <b>Last Revision:</b> $ID$
  <b>Contents:</b>      This file contains the main of the program that is used
  to start the agent. It creates all classes, starts the different
  threads and calls the mainloop. Furthermore it parses the
  specified arguments to adjust the variables.
  <hr size=2>
  <h2><b>Changes</b></h2>
  <b>Date</b>             <b>Author</b>          <b>Comment</b>
  28/11/2000       Jelle Kok       Initial version created
  </pre>
 */

#include <sys/stat.h>
#include "SenseHandler.h"
#include "ActHandler.h"
#include "SGType1Keeper.h"
#include "SGHandcodedKeeper.h"
#include "SGRandomKeeper.h"
#include "SGTaker.h"
#include "KeepawayType1Keeper.h"
#include "KeepawayType1Taker.h"
#include "CMAC.h"
#include "SarsaAgent.h"
#include "SBBAgent.h"
#include "SGHandcodedAgent.h"
#include "RandomAgent.h"

#include "Parse.h"
#include <string.h>   // needed for strcpy
#ifdef WIN32
#include <windows.h>  // needed for CreateThread
#else
#include <pthread.h>  // needed for pthread_create
#endif
#include <stdlib.h>   // needed for exit

extern Logger Log;     /*!< This is a reference to the normal Logger class   */
extern LoggerDraw LogDraw; /*!< This is a reference to the drawing Logger class  */

void printOptions( );

/*! This is the main function and creates and links all the different classes.
  First it reads in all the parameters from the command prompt
  (<program name> -help) and uses these values to create the classes. After
  all the classes are linked, the mainLoop in the Player class is called. */
int main( int argc, char * argv[] )
{

#ifdef WIN32
	HANDLE         sense;
#else
	pthread_t      sense;
#endif

	ServerSettings ss;
	PlayerSettings cs;

	// define variables for command options and initialize with default values
	char     strTeamName[MAX_TEAM_NAME_LENGTH] = "UvA_Trilearn";
	int      iPort                             = ss.getPort();
	int      iMinLogLevel                      ;
	int      iMaxLogLevel                      ;
	char     strHost[128]                      = "127.0.0.1";
	double   dVersion                          = 9.3;
	int      iMode                             = 0;
	int      iNr                               = 2;
	int      iReconnect                        = -1;
	int      iNumKeepers                       = 3;
	int      iNumTakers                        = 2;
	bool     isGoalie			     = false;	
	char     strPolicy[128]                    = "random";
	bool     bLearn                            = false;
	char     loadWeightsFile[256]               = "";
	char     saveWeightsFile[256]               = "";
	bool     bInfo                             = false;
	bool     bSuppliedLogFile                  = false;
	bool     bSuppliedLogDrawFile              = false;
	int	     iSeed	                           = 0;
	int      iId                               = 0;
	int      t_sbb                             = 0;
	int      level_sbb                         = 0;
	int      phase_sbb                         = 0;
	int	 iPolicy                           = 0;
	int      iStopAfter                        = -1; //*met+1 8/16/05

	ofstream os;
	ofstream osDraw;

	int taskType = 1;//Default
	int keeperType = 2;//Default
	int transferType = 0;
	char *transferWeightsFile = "";
	int startEpisode = 0;
	char *dumpWeightsFile = "";  
	char *dumpLogFile = "";
	char *stateLogFile = "";

	// read in all the command options and change the associated variables
	// assume every two values supplied at prompt, form a duo
	char * str;
	for( int i = 1 ; i < argc ; i = i + 2  )
	{
		// help is only option that does not have to have an argument
		if( i + 1 >= argc && strncmp( argv[i], "-help", 3 ) != 0 )
		{
			cout << "Need argument for option: " << argv[i] << endl;
			exit( 0 );
		}
		// read a command option
		if( argv[i][0] == '-' && strlen( argv[i] ) > 1)
		{
			switch( argv[i][1] )
			{
				case '?':                                   // print help
					printOptions( );
					exit(0);
					break;
				case 'a':                                   // output file drawlog info
					osDraw.open( argv[i+1] );
					bSuppliedLogDrawFile = true;
					break;
				case 'b':
					str   = &argv[i+1][0];
					iId = Parse::parseFirstInt( &str ); // seed
					break;
				case 'c':                                   // clientconf file
					if( cs.readValues( argv[i+1], ":" ) == false )
						cerr << "Error in reading client file: " << argv[i+1] << endl;
					break;
				case 'd':
					str   = &argv[i+1][0];
					LogDraw.setActive( Parse::parseFirstInt( &str ) == 1 );
					break;
				case 'e': // enable learning 0/1
					str    = &argv[i+1][0];
					bLearn = (Parse::parseFirstInt( &str ) == 1 ) ? true : false ;
					break;
				case 'f':
					strcpy( saveWeightsFile, argv[i+1] );
					break;
				case 'g':
					str    = &argv[i+1][0];
					isGoalie = (Parse::parseFirstInt( &str ) == 1) ? true : false ;
					break;
				case 'h':                                   // host server or help
					if( strlen( argv [i]) > 2 && argv[i][2] == 'e' )
					{
						printOptions( );
						exit(0);
					}
					else
						strcpy( strHost, argv[i+1] );
					break;
				case 'i':                                   // info 1 0
					str   = &argv[i+1][0];
					bInfo = (Parse::parseFirstInt( &str ) == 1 ) ? true : false ;
					break;
				case 'j':
					str   = &argv[i+1][0];
					iNumTakers = Parse::parseFirstInt( &str );
					break;
				case 'k':
					str   = &argv[i+1][0];
					iNumKeepers = Parse::parseFirstInt( &str );
					break;
				case 'l':                                   // loglevel int[..int]
					str = &argv[i+1][0];
					iMinLogLevel = Parse::parseFirstInt( &str );
					while( iMinLogLevel != 0 )
					{
						if( *str == '.' || *str == '-') // '.' or '-' indicates range
						{
							*str += 1 ;
							iMaxLogLevel = Parse::parseFirstInt( &str );
							if( iMaxLogLevel == 0 ) iMaxLogLevel = iMinLogLevel;
							Log.addLogRange( iMinLogLevel, iMaxLogLevel );
						}
						else
							Log.addLogLevel( iMinLogLevel );
						iMinLogLevel = Parse::parseFirstInt( &str );
					}
					break;
				case 'm':                                   // mode int
					str = &argv[i+1][0];
					iMode = Parse::parseFirstInt( &str );
					break;
				case 'n':                                   // number in formation int
					str = &argv[i+1][0];
					iNr = Parse::parseFirstInt( &str );
					break;
				case 'o':                                   // output file log info
					os.open( argv[i+1] );
					bSuppliedLogFile = true;
					break;
				case 'p':                                   // port
					str = &argv[i+1][0];
					iPort = Parse::parseFirstInt( &str );
					break;
				case 'q':
					strcpy( strPolicy, argv[i+1] );
					break;	  
				case 'r':                                   // reconnect 1 0
					str = &argv[i+1][0];
					iReconnect = Parse::parseFirstInt( &str );
					break;
				case 's':                                   // serverconf file
					if( ss.readValues( argv[i+1], ":" ) == false )
						cerr << "Error in reading server file: " << argv[i+1] << endl;
					break;
				case 't':                                   // teamname name
					strcpy( strTeamName, argv[i+1] );
					break;
				case 'v':                                   // version version
					str = &argv[i+1][0];
					dVersion = Parse::parseFirstDouble( &str );
					break;
				case 'w':
					strcpy( loadWeightsFile, argv[i+1] );
                                        struct stat buffer;
					if (stat (loadWeightsFile, &buffer) != 0){
						cout << "Error: sarsa weights file doesn't exist? filename:" << loadWeightsFile << endl;
						exit(1);
					}
					break;
				case 'x':
					str   = &argv[i+1][0];
					iStopAfter = Parse::parseFirstInt( &str ); // exit after running for iStopAfter episodes
					break;
				case 'z':
					str   = &argv[i+1][0];
					iSeed = Parse::parseFirstInt( &str ); // seed
					break;

				case 'T':
					str = &argv[i + 1][0];	
					taskType = Parse::parseFirstInt( &str);
					break;
				case 'K':
					str = &argv[i + 1][0];	
					keeperType = Parse::parseFirstInt( &str);
					break;
				case 'R':
					str = &argv[i + 1][0];	
					transferType = Parse::parseFirstInt( &str);
					break;
				case 'W':
					str = &argv[i + 1][0];	
					transferWeightsFile = str;
					break;
				case 'S':
					str = &argv[i + 1][0];	
					startEpisode = Parse::parseFirstInt( &str);
					break;
				case 'D':
					str = &argv[i + 1][0];	
					dumpWeightsFile = str;
					break;
				case 'L':
					str = &argv[i + 1][0];	
					dumpLogFile = str;
					break;
				case 'A':
					str = &argv[i + 1][0];	
					stateLogFile = str;
					break;
				case 'X':
					str   = &argv[i+1][0];
					phase_sbb = Parse::parseFirstInt( &str );
					break;

				case 'Y':
					str   = &argv[i+1][0];
					level_sbb = Parse::parseFirstInt( &str );
					break;
				case 'Z':
					str   = &argv[i+1][0];
					t_sbb = Parse::parseFirstInt( &str ); 
					break;
				case 'P':
					str   = &argv[i+1][0];
					iPolicy = Parse::parseFirstInt( &str ); 
					break;
				default:
					cerr << "(main) Unknown command option: " << argv[i] << endl;
			}
		}
	}
	if( bInfo == true )
	{
		cout << "team         : "  << strTeamName    << endl <<
			"port         : "  << iPort          << endl <<
			"host         : "  << strHost        << endl <<
			"version      : "  << dVersion       << endl <<
			"mode         : "  << iMode          << endl <<
			"playernr     : "  << iNr            << endl <<
			"reconnect    : "  << iReconnect     << endl ;
		Log.showLogLevels( cout );
	}
	if( bSuppliedLogFile == true )
		Log.setOutputStream( os );                   // initialize logger
	else
		Log.setOutputStream( cout );
	if( bSuppliedLogDrawFile == true )
		LogDraw.setOutputStream( osDraw );          // initialize drawing logger
	else
		LogDraw.setOutputStream( cout );

	Log.restartTimer( );

	//Formations fs( strFormations, (FormationT)cs.getInitialFormation(), iNr-1 );
	// read formations file
	WorldModel wm( &ss, &cs, NULL );              // create worldmodel
	Connection c( strHost, iPort, MAX_MSG );     // make connection with server
	ActHandler a( &c, &wm, &ss );                // link actHandler and worldmodel
	SenseHandler s( &c, &wm, &ss, &cs );         // link senseHandler with wm

	SarsaAgent *sa;
	SBBAgent *sbba; 
	SGHandcodedAgent *ha;
	RandomAgent *ra;

	// Greg: move this somewhere else
	double ranges[ MAX_STATE_VARS ];
	double minValues[ MAX_STATE_VARS ];
	double resolutions[ MAX_STATE_VARS ];
	int numFeatures;
	int numActions;

	FunctionApproximator *FA;
	int dummyNum;

	switch(taskType)
	{
		case TASK_SHOOTGOAL:
			numFeatures = NUM_SHOOTGOAL_FEATURES;
			numActions = NUM_SHOOTGOAL_ACTIONS;

			dummyNum = wm.SGkeeperStateRangesAndResolutions(ranges, minValues, resolutions, iNumKeepers, iNumTakers);
			if(dummyNum!= numFeatures)
			{
				cout << "Mismatch in numFeatures: " << dummyNum << ":" << numFeatures << " Aborting." << endl;
				return 1;
			}

			FA = new CMAC_RBF(numFeatures, numActions, ranges, minValues, resolutions );
			//FA = new OneLayerNeuralNet(numFeatures, numActions, ranges, minValues, resolutions );
			//FA = new OneLayerNeuralNetIterate(numFeatures, numActions, ranges, minValues, resolutions );
			sa = new SarsaAgent(numFeatures, numActions, bLearn, FA, loadWeightsFile, saveWeightsFile, iId );
			ha = new SGHandcodedAgent(numFeatures, numActions, iId);
			ra = new RandomAgent(numFeatures, numActions, iId);  

			ShootgoalPlayer *sp;

			if(*strTeamName == 'D')//taker
			{
				sp = new SGTaker(sa, &a, &wm, &ss, &cs, strTeamName, iId, iNumKeepers, iNumTakers, isGoalie, dVersion, startEpisode, stateLogFile, iReconnect, iStopAfter);
			}
			else if (*strTeamName == 'O')//keeper
			{
				sbba = new SBBAgent( numFeatures, numActions, bLearn, FA, loadWeightsFile, saveWeightsFile, phase_sbb, t_sbb, level_sbb, iNumKeepers, iId, iSeed, iStopAfter, taskType);
				PolicyAgent *transferAgent;

				sp = new SGType1Keeper( sa,sbba, &a, &wm, &ss, &cs, strTeamName, iId, iNumKeepers, iNumTakers, isGoalie, dVersion, startEpisode, transferType, transferAgent, dumpWeightsFile, dumpLogFile, stateLogFile, iReconnect, iStopAfter,iPolicy );

			}
			else
			{
				cout << "Invalid team name. Aborting." << endl;
				return 1;
			}

			pthread_create( &sense, NULL, sense_callback  , &s); // start listening

			if( iMode == 0 )
				sp->mainLoop();
			break;

		case TASK_KEEPAWAY:

			numFeatures = NUM_KEEPAWAY_FEATURES;
			numActions = NUM_KEEPAWAY_ACTIONS;

			dummyNum = wm.KWYkeeperStateRangesAndResolutions(ranges, minValues, resolutions, iNumKeepers, iNumTakers);
			if(dummyNum!= numFeatures)
			{
				cout << "Mismatch in numFeatures: << " << dummyNum << ":" << numFeatures << " Aborting." << endl;
				return 1;
			}
			FA = new CMAC_RBF(numFeatures, numActions, ranges, minValues, resolutions );
			sa = new SarsaAgent(numFeatures, numActions, bLearn, FA, loadWeightsFile, saveWeightsFile, iId ); 

			KeepawayPlayer *kp;

			if (*strTeamName == 't')//taker
			{
				kp = new KeepawayType1Taker(sa, &a, &wm, &ss, &cs, strTeamName, iNumKeepers, iNumTakers, isGoalie, dVersion, startEpisode, stateLogFile, iReconnect, iStopAfter);
			}
			else if (*strTeamName == 'k')//keeper
			{
				sbba = new SBBAgent( numFeatures, numActions, bLearn, FA, loadWeightsFile, saveWeightsFile, phase_sbb, t_sbb, level_sbb, iNumKeepers, iId, iSeed, iStopAfter, taskType);
				PolicyAgent *transferAgent;

				kp = new KeepawayType1Keeper( sa, sbba, &a, &wm, &ss, &cs, strTeamName, iNumKeepers, iNumTakers, isGoalie, dVersion, startEpisode, transferType, transferWeightsFile, dumpWeightsFile, dumpLogFile, stateLogFile, iReconnect, iStopAfter, iPolicy );

			}
			else
			{
				cout << "Invalid team name. Aborting." << endl;
				return 1;
			}

			pthread_create( &sense, NULL, sense_callback  , &s); // start listening

			if( iMode == 0 )
				kp->mainLoop();

			break;

	}

	c.disconnect();
	os.close();
}

/*! This function prints the command prompt options that can be supplied to the
  program. */
void printOptions( )
{
	cout << "Command options:"                                         << endl <<
		" a file                - write drawing log info to "             << endl <<


		" c(lientconf) file     - use file as client conf file"           << endl <<
		" d(rawloglevel) int[..int] - level(s) of drawing debug info"     << endl <<
		" e(nable) learning 0/1  - turn learning on/off"                  << endl << 
		" f save weights file   - use file to save weights"               << endl <<
		" he(lp)                - print this information"                 << endl <<
		" h(ost) hostname       - host to connect with"                   << endl <<
		" i(nfo) 0/1            - print variables used to start"          << endl <<
		" j takers  int         - number of takers"                       << endl <<
		" k(eepers) int         - number of keepers"                      << endl <<
		" l(oglevel) int[..int] - level of debug info"                    << endl <<
		" m(ode) int            - which mode to start up with"            << endl <<
		" n(umber) int          - player number in formation"             << endl <<
		" o(utput) file         - write log info to (screen is default)"  << endl <<
		" p(ort)                - port number to connect with"            << endl <<
		" q policy name         - policy to play with"                    << endl <<
		" r(econnect) int       - reconnect as player nr"                 << endl <<
		" s(erverconf) file     - use file as server conf file"           << endl <<
		" t(eamname) name       - name of your team"                      << endl <<
		" w(eights) file        - use file to load weights"               << endl;
}
