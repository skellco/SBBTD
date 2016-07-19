import java.io.*;
import java.util.*;
import java.awt.Color;

public class ShootGoalTask implements Task, SoccerTypes
{
   final int ANIMATION_DELAY = 1000;
   final String trainingMsg = "fo ";
   final int TURNOVER_TIME = 5;
   final int GOALIE_CATCH_TIME= 2;
   final int KEEPER_POS_TIME = 2;
   final int TAKER_POS_TIME = 2;

   final double halfLength = 52.5;
   final double halfWidth = 34;

   ServerParams SP;
   WorldState WS;
   TrainerCommandHandler CMD;
   int numKeepers;
   int numTakers;
   BufferedWriter bw;
   int epoch;
   boolean freshStart;
   Random rand;
   Rectangle region;

   int timeWithGoalie;
   int[] timeWithKeeper;
   int[] timeWithTaker;    

   int taskType;// = 3;

   int catchTime = 2;

   int takeTime;
   int startTime;
   double startX;
   double endX;
   double taskAdjust;
   Monitor monitor;
   Vector shapes;
   Set<Integer> keepers;
   Set<Integer> takers;
   Set<Integer> players;

   public ShootGoalTask( ServerParams sp,
         WorldState ws,
         TrainerCommandHandler cmd,
         boolean launchMonitor,
         int numKeepers,
         int numTakers,
         String kwyFile, int taskType, int startEpisode, double ta )
   {
      SP = sp;
      WS = ws;
      CMD = cmd;
      taskAdjust = ta;

      this.taskType = taskType;

      this.numKeepers = numKeepers;
      this.numTakers = numTakers;

      timeWithKeeper = new int[numKeepers];
      timeWithTaker = new int[numTakers - 1];


      if ( kwyFile != null ) {
         try {
            bw = new BufferedWriter( new FileWriter( kwyFile, true ) );//True says append
         }
         catch ( Exception e ) {
            System.err.println( "Unable to create .kwy file: " + e );
            bw = null;
         }
      }

      WS.newPlayerMessageHeard = false;

      epoch = startEpisode;
      freshStart = true;

      rand = new Random();
      region = new Rectangle(PITCH_WIDTH, PITCH_LENGTH / 2.0, new VecPosition(PITCH_LENGTH / 4.0, 0));

      takeTime = 0;

      keepers = new HashSet<Integer>();
      for ( int i = 1; i <= numKeepers; i++ )
         keepers.add( Utils.getPlayerID( SIDE_LEFT, i ) );

      takers = new HashSet<Integer>();
      for ( int i = 1; i <= numTakers; i++ )
         takers.add( Utils.getPlayerID( SIDE_RIGHT, i ) );

      players = new HashSet<Integer>();
      players.addAll( keepers );
      players.addAll( takers );

   }

   public void init()
   {
      if (( bw != null ) && (epoch == 0))
         kwyHeader();
      CMD.changeMode( PM_PlayOn );
   }

   private boolean isGoal()
   {
      VecPosition ballPosition = WS.getBallPosition();

      boolean value = ((ballPosition.getX() > (PITCH_LENGTH  / 2.0)) && (Math.abs(ballPosition.getY()) < (GOAL_WIDTH / 2.0 - GOAL_POST_RADIUS)));

      return value;

   }

   private boolean isBallOutOfBounds()
   {
      VecPosition ballPos = WS.getBallPosition();
      boolean outsideLine1, outsideLine2, outsideLine3, outsideLine4;

      outsideLine1 = (ballPos.getX() < 0);
      outsideLine2 = (ballPos.getY() < -PITCH_WIDTH / 2.0);
      outsideLine3 = (ballPos.getY() > PITCH_WIDTH / 2.0);
      outsideLine4 = (ballPos.getX() > PITCH_LENGTH / 2.0);

      return (outsideLine1 || outsideLine2 || outsideLine3 || outsideLine4);
   }

   private boolean isBallWithKeeper(int index)//index 1..numKeepers
   {
      VecPosition keeperPos = WS.getPlayerPosition(Utils.getLeftPlayerID(index));
      VecPosition ballPos = WS.getBallPosition();

      return (keeperPos.getDistanceTo(ballPos) <= SP.kickable_margin + SP.player_size + SP.ball_size);
   }

   private boolean isBallWithTaker(int index)//index 1..numTakers - 1;
   {
      VecPosition takerPos = WS.getPlayerPosition(Utils.getRightPlayerID(index));
      VecPosition ballPos = WS.getBallPosition();

      return (takerPos.getDistanceTo(ballPos) <= SP.kickable_margin + SP.player_size + SP.ball_size);
   }

   private boolean isBallWithGoalie()//Assume goalie's index is numTakers
   {
      VecPosition goaliePos = WS.getPlayerPosition(Utils.getRightPlayerID(numTakers));
      VecPosition ballPos = WS.getBallPosition();

      return (goaliePos.getDistanceTo(ballPos) <= SP.catchable_area_l);
   }

   String makeSayMessage(String gameCondition, double state[], int numStates, int action)
   {
      String s = "*";
      s += gameCondition;
      s += " ";

      //Sometimes state and action may be dummies
      if((numStates <= 0) || (numStates > 50))//a maximum
      {
         s += "*0 *0 *0";
         return s;
      }


      s += "*";
      for(int i = 0; i < numStates; i++)
      {
         s += (new Double(state[i])).toString();
         s += " ";
      }

      s += "*";
      s += (new Integer(action)).toString();
      s += " ";

      double reward = 1.0;
      s += "*";
      s += (new Double(reward)).toString();
      s += " ";

      return s;
   }

   public boolean processCycle()
   {
      String sayMessage;


      if ( freshStart)
      {
         sayMessage = makeSayMessage("r", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);//anyway invalid
         broadcast(sayMessage);
         resetField();

         freshStart = false;
      }
      else if(isGoal())
      {
         if(bw != null)
            kwyLogEpisode('g');
         sayMessage = makeSayMessage("g", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
         broadcast(sayMessage);
         resetField();
      }
      else if(isBallOutOfBounds()) 
      {
         if ( bw != null )
            kwyLogEpisode( 'o' );
         sayMessage = makeSayMessage("o", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
         broadcast(sayMessage);
         resetField();	
      }
      else//Ball is in play 
      {
         System.out.println("Ball in play...");
         if(isBallWithGoalie())
            timeWithGoalie++;
         else
            timeWithGoalie = 0;

         int kPos = 0;
         int tPos = 0;

         for(int i = 0; i < numKeepers; i++)
         {
            if(isBallWithKeeper(i + 1))
            {
               timeWithKeeper[i]++;
               kPos++;
            }
            else
               timeWithKeeper[i] = 0;
         }	

         for(int i = 0; i < numTakers - 1; i++)
         {
            if(isBallWithTaker(i + 1))
            {
               timeWithTaker[i]++;
               tPos++;
            }	
            else
               timeWithTaker[i] = 0;
         }	

         if(timeWithGoalie >= GOALIE_CATCH_TIME)
         {
            if(bw != null)
               kwyLogEpisode('c');

            sayMessage = makeSayMessage("c", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
            broadcast(sayMessage);
            resetField();
         }
         else if((kPos > 0) && (tPos > 0))
         {
            for(int i = 0; i < numKeepers; i++)
               timeWithKeeper[i] = 0;

            for(int i = 0; i < numTakers - 1; i++)
               timeWithTaker[i] = 0;
         }
         else if(tPos > 0)
         {
            if(timeWithTaker[0] >= TAKER_POS_TIME)
            {
               if(bw != null)
                  kwyLogEpisode('t');
               sayMessage = makeSayMessage("t0", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
               broadcast(sayMessage);
               resetField();			
            }
            else if(timeWithTaker[1] >= TAKER_POS_TIME)
            {
               if(bw != null)
                  kwyLogEpisode('t');
               sayMessage = makeSayMessage("t1", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
               broadcast(sayMessage);
               resetField();
            }
            else if(timeWithTaker[2] >= TAKER_POS_TIME)
            {
               if(bw != null)
                  kwyLogEpisode('t');
               sayMessage = makeSayMessage("t2", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
               broadcast(sayMessage);
               resetField();
            }
            //else if(timeWithTaker[3] >= TAKER_POS_TIME)
            //{
            //	if(bw != null)
            //		kwyLogEpisode('t');
            //	sayMessage = makeSayMessage("t3", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
            //	broadcast(sayMessage);
            //	resetField();			
            //}			
         }
         else
         {
            switch(taskType)
            {
               case Trainer.TASK_SHOOTGOALVOICED:
                  if(WS.newPlayerMessageHeard)//Broadcast this to the players
                  {
                     if(WS.messageSayer == 0)
                     {
                        sayMessage = makeSayMessage("k0", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
                        broadcast(sayMessage);
                     }
                     else if(WS.messageSayer == 1)
                     {
                        sayMessage = makeSayMessage("k1", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
                        broadcast(sayMessage);		
                     }
                     else if(WS.messageSayer == 2)
                     {
                        sayMessage = makeSayMessage("k2", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
                        broadcast(sayMessage);			
                     }
                     else if(WS.messageSayer == 3)
                     {
                        sayMessage = makeSayMessage("k3", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
                        broadcast(sayMessage);			
                     }

                     WS.newPlayerMessageHeard = false;
                  }
                  break;	

               case Trainer.TASK_SHOOTGOAL:
               default:
                  if(kPos > 0)
                  {
                     if(timeWithKeeper[0] >= KEEPER_POS_TIME)
                     {
                        sayMessage = makeSayMessage("k0", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
                        broadcast(sayMessage);
                     }
                     else if(timeWithKeeper[1] >= KEEPER_POS_TIME)
                     {
                        sayMessage = makeSayMessage("k1", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
                        broadcast(sayMessage);
                     }
                     else if(timeWithKeeper[2] >= KEEPER_POS_TIME)
                     {
                        sayMessage = makeSayMessage("k2", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
                        broadcast(sayMessage);
                     }
                     else if(timeWithKeeper[3] >= KEEPER_POS_TIME)
                     {
                        sayMessage = makeSayMessage("k3", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);
                        broadcast(sayMessage);
                     }	
                  }
                  break;

            }
         }
      }	


      // If I haven't seen one of the players in 500 cycles, quit.
      for ( int id : players )
         if ( WS.getTimeSinceSeenPlayer( id ) > 500 )
            return false;


      //// If an episode lasts longer than 1:45, reset.
      //if ( WS.getTime() - startTime > 1050 )
      //{
      //   sayMessage = makeSayMessage("r", WS.lastPlayerState, WS.numStates, WS.lastPlayerAction);//anyway invalid
      //   broadcast(sayMessage);
      //   CMD.move( ID_BALL, WS.getBallPosition(), new VecPosition( 0, 0 ) );
      //   resetField();
      //}

      return true;

   }

   private void broadcast(String s)
   {
      CMD.say(s);
   }

   private void resetField()//Assumed to be for 4 keepers and 4 takers (including goalie)
   {
      VecPosition pos;

      int keeperPos = rand.nextInt(numKeepers);

      double squareSide = 20.0;
      //VecPosition squareMid = new VecPosition(2.0 + squareSide / 2.0, -PITCH_WIDTH / 2.0 + squareSide / 2.0 + 2.0 + rand.nextDouble() * (PITCH_WIDTH - squareSide - 4.0));//standard
      double x = 35.0-(taskAdjust * 23.0);//USE THIS
      //double x = 12.0;
      double y = -(taskAdjust * 22.0) + (rand.nextDouble() * (taskAdjust * 44.0));//USE THIS
      //double y = 0.0; 
      VecPosition squareMid = new VecPosition(x,y);//standard

      //System.out.println("taskadj taskAdjust " + taskAdjust + " " + x + " " + y);

      //VecPosition squareMid = new VecPosition(PITCH_WIDTH-35, -PITCH_WIDTH / 2.0 + squareSide / 2.0 + 2.0 + 0.5 * (PITCH_WIDTH - squareSide - 4.0));//close to goal
      //	VecPosition squareMid = new VecPosition(squareSide / 2.0 + 2.0 + rand.nextDouble() * (PITCH_LENGTH / 2.0 - squareSide - 2.0 - GOAL_AREA_LENGTH), -PITCH_WIDTH / 2.0 + squareSide / 2.0 + 2.0 + rand.nextDouble() * (PITCH_WIDTH - squareSide - 4.0));
      for ( int i = 1; i <= numKeepers; i++ ) {
         switch( keeperPos ) {

            case 0:
               pos = new VecPosition(squareMid.getX() - squareSide / 2.0, squareMid.getY() - squareSide / 2.0);
               break;
            case 1:
               pos = new VecPosition(squareMid.getX() - squareSide / 2.0, squareMid.getY() + squareSide / 2.0);
               break;
            case 2:
               pos = new VecPosition(squareMid.getX() + squareSide / 2.0, squareMid.getY() - squareSide / 2.0);
               break;
            case 3:
               pos = new VecPosition(squareMid.getX() + squareSide / 2.0, squareMid.getY() + squareSide / 2.0);
               break;

            default:
               pos = new VecPosition(0, 0);
               break;
         }

         CMD.move( Utils.getLeftPlayerID( i ), pos );
         keeperPos = ( keeperPos + 1 ) % numKeepers;
      }

      int i;

      //Player ID 1
      i = 1;
      pos = new VecPosition(squareMid.getX() - squareSide / 2.0 + 4.0, squareMid.getY() + squareSide / 2.0);
      pos = new VecPosition(squareMid.getX() - 1.0, squareMid.getY() + 1.0);

      CMD.move( Utils.getRightPlayerID(i), pos );

      //Player ID 2
      i = 2;
      pos = new VecPosition(squareMid.getX() + squareSide / 2.0 + 4.0, squareMid.getY() - squareSide / 2.0);
      pos = new VecPosition(squareMid.getX() + 1.0, squareMid.getY() - 1.0);
      CMD.move( Utils.getRightPlayerID(i), pos );

      //Player ID 3
      i = 3;
      pos = new VecPosition(squareMid.getX() + squareSide / 2.0 + 4.0, squareMid.getY() + squareSide / 2.0);
      pos = new VecPosition(squareMid.getX() + squareSide / 2.0 + 4.0, squareMid.getY());
      CMD.move( Utils.getRightPlayerID(i), pos );

      if (taskType == 3){
         //Player ID 4
         i = 4;
         pos = new VecPosition(PITCH_LENGTH / 2.0 - GOAL_AREA_LENGTH - 0.5 + rand.nextDouble() * 1.0, -0.5 + rand.nextDouble() * 1.0);
         //pos = new VecPosition(squareMid.getX() - squareSide / 2.0 + 4.0, squareMid.getY() - squareSide / 2.0);

         CMD.move( Utils.getRightPlayerID(i), pos );
      }
      //The goalie
      pos = new VecPosition(PITCH_LENGTH / 2.0, 0);
      CMD.move( Utils.getRightPlayerID(numTakers), pos );

      //The ball
      double r = rand.nextDouble();
      //     if (taskAdjust < 1.0){
      //       if(r < 0.5)
      //           pos = new VecPosition(squareMid.getX() - squareSide / 2.0 + rand.nextDouble() * 1.0, squareMid.getY() - squareSide / 2.0 + rand.nextDouble() * 1.0);
      //        else
      //           pos = new VecPosition(squareMid.getX() - squareSide / 2.0 + rand.nextDouble() * 1.0, squareMid.getY() + squareSide / 2.0 - 2.0 +  rand.nextDouble() * 1.0);
      //     }
      //     else{
      if(r < 0.25)
         pos = new VecPosition(squareMid.getX() - squareSide / 2.0 + rand.nextDouble() * 1.0, squareMid.getY() - squareSide / 2.0 + rand.nextDouble() * 1.0);
      else if(r < 0.5)
         pos = new VecPosition(squareMid.getX() - squareSide / 2.0 + rand.nextDouble() * 1.0, squareMid.getY() + squareSide / 2.0 - 2.0 +  rand.nextDouble() * 1.0);
      else if(r < 0.75)
         pos = new VecPosition(squareMid.getX() + squareSide / 2.0 - 2.0 + rand.nextDouble() * 1.0, squareMid.getY() - squareSide / 2.0 +  rand.nextDouble() * 1.0);
      else //if(r < 1.0)
         pos = new VecPosition(squareMid.getX() + squareSide / 2.0 - 2.0 + rand.nextDouble() * 1.0, squareMid.getY() + squareSide / 2.0 - 2.0 +  rand.nextDouble() * 1.0);
      //     }

      CMD.move( ID_BALL, pos, new VecPosition( 0, 0 ) );
      
      CMD.changeMode( PM_Pause );
      try{ Thread.sleep(ANIMATION_DELAY); }
      catch (Exception e) { System.out.println(e); }
      CMD.changeMode( PM_PlayOn );

      takeTime = 0;
      startTime = WS.getTime();

      startX = pos.getX();

      epoch++;

      timeWithGoalie = 0;

      for(i = 0; i < numKeepers; i++)
         timeWithKeeper[i] = 0;

      for(i = 0; i < numTakers - 1; i++)
         timeWithTaker[i] = 0;
   }

   private void kwyHeader()
   {
      try {
         bw.write( "# Keepers: " + numKeepers + "\n" +
               "# Takers:  " + numTakers + "\n" );

         bw.write( "#\n");

         bw.write( "# Description of Fields:\n" +
               "# 1) Episode number\n" +
               "# 2) Start time in simulator steps / 100ms\n" +
               "# 3) End time in simulator steps / 100ms\n" +
               "# 4) Duration in simulator steps / 100ms\n" +
               "# 5) (g)oal scored / (o)ut of bounds \n" );

         bw.write( "#\n" );

         bw.flush();
      }
      catch ( Exception e ) {
         System.err.println( "Unable to write to kwy file: " + e );
      }
   }

   private void kwyLogEpisode( char endCond )
   {
      try {
         bw.write( epoch + "\t" + 
               startTime + "\t" +
               WS.getTime() + "\t" +
               ( WS.getTime() - startTime ) + "\t" +
               endCond + "\n" );

         bw.flush();
      }
      catch ( Exception e ) {
         System.err.println( "Unable to write to kwy file: " + e );
      }
   }
}
