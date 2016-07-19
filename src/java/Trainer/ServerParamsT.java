public class ServerParamsT extends Struct
{
    public long goal_width ;				/* goal width */
    public long inertia_moment ;			/* intertia moment for turn */
    public long player_size ;					/* player size */
    public long player_decay ;					/* player decay */
    public long player_rand ;					/* player rand */
    public long player_weight ;					/* player weight */
    public long player_speed_max ;				/* player speed max */
    // th 6.3.00
    public long player_accel_max ;				/* player acceleration max */
    //
    public long stamina_max ;				/* player stamina max */
    public long stamina_inc_max ;				/* player stamina inc */
    public long recover_init ;			/* player recovery init */
    public long recover_dec_thr ;			/* player recovery decriment threshold */
    public long recover_min ;				/* player recovery min */
    public long recover_dec ;				/* player recovery decriment */
    public long effort_init ;				/* player dash effort init */
    public long effort_dec_thr ;				/* player dash effort decriment threshold */
    public long effort_min ;				/* player dash effrot min */
    public long effort_dec ;				/* player dash effort decriment */
    public long effort_inc_thr ;				/* player dash effort incriment threshold */
    public long effort_inc ;				/* player dash effort incriment */
    // pfr 8/14/00: for RC2000 evaluation
    public long kick_rand;                                /* noise added directly to kicks */
    public short team_actuator_noise;                        /* flag whether to use team specific actuator noise */
    public long prand_factor_l;                           /* factor to multiple prand for left team */
    public long prand_factor_r;                           /* factor to multiple prand for right team */
    public long kick_rand_factor_l;                       /* factor to multiple kick_rand for left team */
    public long kick_rand_factor_r;                       /* factor to multiple kick_rand for right team */

    public long bsize ;					/* ball size */
    public long bdecay ;					/* ball decay */
    public long brand ;					/* ball rand */
    public long bweight ;					/* ball weight */
    public long bspeed_max ;				/* ball speed max */
    // th 6.3.00
    public long baccel_max;				/* ball acceleration max */
    //
    public long dprate ;					/* dash power rate */
    public long kprate ;					/* kick power rate */
    public long kmargin ;					/* kickable margin */
    public long ctlradius ;				/* control radius */
    public long ctlradius_width ;			/* (control radius) - (plyaer size) */
    public long maxp ;					/* max power */
    public long minp ;					/* min power */
    public long maxm ;					/* max moment */
    public long minm ;					/* min moment */
    public long maxnm ;					/* max neck moment */
    public long minnm ;					/* min neck moment */
    public long maxn ;					/* max neck angle */
    public long minn ;					/* min neck angle */
    public long visangle ;				/* visible angle */
    public long visdist ;					/* visible distance */
    public long windir ;					/* wind direction */
    public long winforce ;				/* wind force */
    public long winang ;					/* wind angle for rand */
    public long winrand ;					/* wind force for force */
    public long kickable_area ;			/* kickable_area */
    public long catch_area_l ;			/* goalie catchable area length */
    public long catch_area_w ;			/* goalie catchable area width */
    public long catch_prob ;				/* goalie catchable possibility */
    public short   goalie_max_moves;                 /* goalie max moves after a catch */
    public long ckmargin ;				/* corner kick margin */
    public long offside_area ;			/* offside active area size */
    public short win_no ;					/* wind factor is none */
    public short win_random ;				/* wind factor is random */
    public short say_cnt_max ;				/* max count of coach SAY */
    public short SayCoachMsgSize ;				/* max length of coach SAY */
    public short clang_win_size;
    public short clang_define_win;
    public short clang_meta_win;
    public short clang_advice_win;
    public short clang_info_win;
    public short clang_mess_delay;
    public short clang_mess_per_cycle;
    public short half_time ;					/* half time */
    public short sim_st ;					/* simulator step interval msec */
    public short send_st ;					/* udp send step interval msec */
    public short recv_st ;					/* udp recv step interval msec */
    public short sb_step ;					/* sense_body interval step msec */
    public short lcm_st ;		                        /* lcm of all the above steps msec */
    public short say_msg_size ;				/* string size of say message */
    public short hear_max ;					/* player hear_capacity_max */
    public short hear_inc ;					/* player hear_capacity_inc */
    public short hear_decay ;				/* player hear_capacity_decay */
    public short cban_cycle ;				/* goalie catch ban cycle */
    public short slow_down_factor ;                          /* factor to slow down simulator and send intervals */
    public short useoffside ;				/* flag for using off side rule */
    public short kickoffoffside ;			/* flag for permit kick off offside */
    public long offside_kick_margin ;		/* offside kick margin */
    public long audio_dist ;				/* audio cut off distance */
    public long dist_qstep ;				/* quantize step of distance */
    public long land_qstep ;				/* quantize step of distance for landmark */
    public long dir_qstep ;				/* quantize step of direction */
    public long dist_qstep_l ;			/* team right quantize step of distance */
    public long dist_qstep_r ;			/* team left quantize step of distance */
    public long land_qstep_l ;			/* team right quantize step of distance for landmark */
    public long land_qstep_r ;			/* team left quantize step of distance for landmark */
    public long dir_qstep_l ;				/* team left quantize step of direction */
    public long dir_qstep_r ;				/* team right quantize step of direction */
    public short CoachMode ;				/* coach mode */
    public short CwRMode ;					/* coach with referee mode */
    public short old_hear ;					/* old format for hear command (coach) */
    public short sv_st ;					/* online coach's look interval step */


    // spare variables which are to be used for paramenter added in the future
    //public long sparelong1;
    //public long sparelong2;
    public long slowness_on_top_for_left_team;
    public long slowness_on_top_for_right_team;
    //public long sparelong3;
    //public long sparelong4;
    public long ka_length ;                    /* keepaway region length */
    public long ka_width ;                     /* keepaway region width */
    public long sparelong5;
    public long sparelong6;
    public long sparelong7;
    public long sparelong8;
    public long sparelong9;
    public long sparelong10;
    
    public short start_goal_l;
    public short start_goal_r;
    public short fullstate_l;
    public short fullstate_r;
    public short drop_time;
    public short synch_mode;
    public short synch_offset;
    public short synch_micro_sleep;
    public short point_to_ban;
    public short point_to_duration;
    //public short kaway; /* keepaway mode on/off */ /* No room in struct */

    // no room in struct 
    //  public short pen_before_setup_wait;
    //  public short pen_setup_wait;
    //  public short pen_ready_wait;
    //  public short pen_taken_wait;
    //  public short pen_nr_kicks;
    //  public short pen_max_extra_kicks;
    //  public long pen_dist_x;
    //  public short pen_random_winner;
    //  public long pen_max_goalie_dist_x;
    //  public short pen_allow_mult_kicks;
}
