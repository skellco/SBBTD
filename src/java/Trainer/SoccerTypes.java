public interface SoccerTypes
{
    /* Constants */
    final int NUM_TEAMS = 2;
    final int MAX_PLAYERS_TEAM = 11;
    final int MAX_PLAYERS = NUM_TEAMS * MAX_PLAYERS_TEAM;
    final int MAX_HETERO_PLAYERS = 7;
    final int MAX_SAY_MSG = 10;
    final int MAX_TEAMNAME_LENGTH = 64;

    final double PITCH_LENGTH = 105.0;
    final double PITCH_WIDTH = 68.0;
    final double PITCH_MARGIN = 5.0;
    final double PENALTY_AREA_LENGTH = 16.5;
    final double PENALTY_AREA_WIDTH = 40.32;
    final double PENALTY_X = ( PITCH_LENGTH / 2.0 - PENALTY_AREA_LENGTH );
    final double PENALTY_SPOT_DIST = 11.0;
    final double CENTER_CIRCLE_RADIUS = 9.15;
    final double GOAL_AREA_LENGTH = 5.5;
    final double GOAL_AREA_WIDTH = 18.32;
    final double GOAL_WIDTH = 14.02;
    final double GOAL_DEPTH = 2.44;
    final double GOAL_POST_RADIUS = 0.06;
    final double SIDELINES_Y = -37;

    final double UnknownDouble = -1000.0;
    final double UnknownAngle = -1000.0;
    final int UnknownInt = -1000;
    final int UnknownTime = -20;
    final long UnknownMessageNr = -30;

    /* Play modes */
    final int PM_Null = 0;
    final int PM_BeforeKickOff = 1;
    final int PM_TimeOver = 2;
    final int PM_PlayOn = 3;
    final int PM_KickOff_Left = 4;
    final int PM_KickOff_Right = 5;
    final int PM_KickIn_Left = 6;
    final int PM_KickIn_Right = 7;
    final int PM_FreeKick_Left = 8;
    final int PM_FreeKick_Right = 9;
    final int PM_CornerKick_Left = 10;
    final int PM_CornerKick_Right = 11;
    final int PM_GoalKick_Left = 12;
    final int PM_GoalKick_Right = 13;
    final int PM_AfterGoal_Left = 14;
    final int PM_AfterGoal_Right = 15;
    final int PM_Drop_Ball = 16;
    final int PM_OffSide_Left = 17;
    final int PM_OffSide_Right = 18;
    final int PM_PK_Left = 19;
    final int PM_PK_Right = 20;
    final int PM_FirstHalfOver = 21;
    final int PM_Pause = 22;
    final int PM_Human = 23;
    final int PM_Foul_Charge_Left = 24;
    final int PM_Foul_Charge_Right = 25;
    final int PM_Foul_Push_Left = 26;
    final int PM_Foul_Push_Right = 27;
    final int PM_Foul_MultipleAttacker_Left = 28;
    final int PM_Foul_MultipleAttacker_Right = 29;
    final int PM_Foul_BallOut_Left = 30;
    final int PM_Foul_BallOut_Right = 31;
    final int PM_Back_Pass_Left = 32;
    final int PM_Back_Pass_Right = 33;
    final int PM_Free_Kick_Fault_Left = 34;
    final int PM_Free_Kick_Fault_Right = 35;
    final int PM_CatchFault_Left = 36;
    final int PM_CatchFault_Right = 37;
    final int PM_IndFreeKick_Left = 38;
    final int PM_IndFreeKick_Right = 39;
    final int PM_PenaltySetup_Left = 40;
    final int PM_PenaltySetup_Right = 41;
    final int PM_PenaltyReady_Left = 42;
    final int PM_PenaltyReady_Right = 43;
    final int PM_PenaltyTaken_Left = 44;
    final int PM_PenaltyTaken_Right = 45;
    final int PM_PenaltyMiss_Left = 46;
    final int PM_PenaltyMiss_Right = 47;
    final int PM_PenaltyScore_Left = 48;
    final int PM_PenaltyScore_Right = 49;
    final int PM_MAX = 50;

    /* Play mode strings */
    final String[] PLAYMODE_STRINGS
	= {"",
	   "before_kick_off",
	   "time_over",
	   "play_on",
	   "kick_off_l",
	   "kick_off_r",
	   "kick_in_l",
	   "kick_in_r",
	   "free_kick_l",
	   "free_kick_r",
	   "corner_kick_l",
	   "corner_kick_r",
	   "goal_kick_l",
	   "goal_kick_r",
	   "goal_l",
	   "goal_r",
	   "drop_ball",
	   "offside_l",
	   "offside_r",
	   "penalty_kick_l",
	   "penalty_kick_r",
	   "first_half_over",
	   "pause",
	   "human_judge",
	   "foul_charge_l",
	   "foul_charge_r",
	   "foul_push_l",
	   "foul_push_r",
	   "foul_multiple_attack_l",
	   "foul_multiple_attack_r",
	   "foul_ballout_l",
	   "foul_ballout_r",
	   "back_pass_l", 
	   "back_pass_r", 
	   "free_kick_fault_l", 
	   "free_kick_fault_r", 
	   "catch_fault_l", 
	   "catch_fault_r", 
	   "indirect_free_kick_l", 
	   "indirect_free_kick_r",
	   "penalty_setup_l", 
	   "penalty_setup_r",
	   "penalty_ready_l",
	   "penalty_ready_r", 
	   "penalty_taken_l", 
	   "penalty_taken_r", 
	   "penalty_miss_l", 
	   "penalty_miss_r", 
	   "penalty_score_l", 
	   "penalty_score_r"    
	} ;

    /* Side types -- DO NOT CHANGE ORDER -- */
    final int SIDE_ILLEGAL = -1;
    final int SIDE_LEFT = 0;
    final int SIDE_RIGHT = 1;


    /* Object ID types -- DO NOT CHANGE ORDER -- */
    final int ID_ILLEGAL = -1;
    final int ID_PLAYER_L_1 = 0;
    final int ID_PLAYER_L_2 = 1;
    final int ID_PLAYER_L_3 = 2;
    final int ID_PLAYER_L_4 = 3;
    final int ID_PLAYER_L_5 = 4;
    final int ID_PLAYER_L_6 = 5;
    final int ID_PLAYER_L_7 = 6;
    final int ID_PLAYER_L_8 = 7;
    final int ID_PLAYER_L_9 = 8;
    final int ID_PLAYER_L_10 = 9;
    final int ID_PLAYER_L_11 = 10;
    final int ID_PLAYER_R_1 = 11;
    final int ID_PLAYER_R_2 = 12;
    final int ID_PLAYER_R_3 = 13;
    final int ID_PLAYER_R_4 = 14;
    final int ID_PLAYER_R_5 = 15;
    final int ID_PLAYER_R_6 = 16;
    final int ID_PLAYER_R_7 = 17;
    final int ID_PLAYER_R_8 = 18;
    final int ID_PLAYER_R_9 = 19;
    final int ID_PLAYER_R_10 = 20;
    final int ID_PLAYER_R_11 = 21;
    final int ID_BALL = 22;
    final int ID_GOAL_L = 23;
    final int ID_GOAL_R = 24;
    final int ID_MAX = 25;
}
