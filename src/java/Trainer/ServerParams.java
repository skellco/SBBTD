import java.lang.reflect.Field;
import java.io.Serializable;

public class ServerParams implements Serializable
{
    /* server_params */
    public double audio_cut_dist = 50;
    public int back_passes = 1;
    public double ball_accel_max = 2.7;
    public double ball_decay = 0.94;
    public double ball_rand = 0.05;
    public double ball_size = 0.085;
    public double ball_speed_max = 2.7;
    public double ball_weight = 0.2;
    public int catch_ban_cycle = 5;
    public double catch_probability = 1;
    public double catchable_area_l = 2;
    public double catchable_area_w = 1;
    public double ckick_margin = 1;
    public int clang_advice_win = 1;
    public int clang_define_win = 1;
    public int clang_del_win = 1;
    public int clang_info_win = 1;
    public int clang_mess_delay = 50;
    public int clang_mess_per_cycle = 1;
    public int clang_meta_win = 1;
    public int clang_rule_win = 1;
    public int clang_win_size = 300;
    public int coach = 0;
    public int coach_port = 6001;
    public int coach_w_referee = 0;
    public double control_radius = 2;
    public double dash_power_rate = 0.006;
    public double drop_ball_time = 200;
    public double effort_dec = 0.005;
    public double effort_dec_thr = 0.3;
    public double effort_inc = 0.01;
    public double effort_inc_thr = 0.6;
    public double effort_init = 1;
    public double effort_min = 0.6;
    public int forbid_kick_off_offside = 1;
    public int free_kick_faults = 1;
    public int freeform_send_period = 20;
    public int freeform_wait_period = 600;
    public int fullstate_l = 0;
    public int fullstate_r = 0;
    public int game_log_compression = 0;
    public int game_log_dated = 1;
    public String game_log_dir = "./";
    public int game_log_fixed = 0;
    public String game_log_fixed_name = "rcssserver";
    public int game_log_version = 3;
    public int game_logging = 1;
    public double goal_width = 14.02;
    public int goalie_max_moves = 2;
    public int half_time = 300;
    public int hear_decay = 1;
    public int hear_inc = 1;
    public int hear_max = 1;
    public double inertia_moment = 5;
    public double kick_power_rate = 0.027;
    public double kick_rand = 0;
    public double kick_rand_factor_l = 1;
    public double kick_rand_factor_r = 1;
    public double kickable_margin = 0.7;
    public String landmark_file = "~/.rcssserver-landmark.xml";
    public String log_date_format = "%Y%m%d%H%M-";
    public int log_times = 0;
    public int max_goal_kicks = 3;
    public double maxmoment = 180;
    public double maxneckang = 90;
    public double maxneckmoment = 180;
    public double maxpower = 100;
    public double minmoment = -180;
    public double minneckang = -90;
    public double minneckmoment = -180;
    public double minpower = -100;
    public double offside_active_area_size = 2.5;
    public double offside_kick_margin = 9.15;
    public double olcoach_port = 6002;
    public int old_coach_hear = 0;
    public double player_accel_max = 1;
    public double player_decay = 0.4;
    public double player_rand = 0.1;
    public double player_size = 0.3;
    public double player_speed_max = 1.2;
    public double player_weight = 60;
    public double point_to_ban = 5;
    public double point_to_duration = 20;
    public int port = 6000;
    public double prand_factor_l = 1;
    public double prand_factor_r = 1;
    public int profile = 0;
    public int proper_goal_kicks = 0;
    public double quantize_step = 0.1;
    public double quantize_step_l = 0.01;
    public int record_messages = 0;
    public double recover_dec = 0.002;
    public double recover_dec_thr = 0.3;
    public double recover_min = 0.5;
    public int recv_step = 10;
    public int say_coach_cnt_max = 128;
    public int say_coach_msg_size = 128;
    public int say_msg_size = 10;
    public int send_comms = 0;
    public int send_step = 150;
    public int send_vi_step = 100;
    public int sense_body_step = 100;
    public int simulator_step = 100;
    public double slow_down_factor = 1;
    public int slowness_on_top_for_left_team = 1;
    public int slowness_on_top_for_right_team = 1;
    public double stamina_inc_max = 45;
    public double stamina_max = 4000;
    public int start_goal_l = 0;
    public int start_goal_r = 0;
    public double stopped_ball_vel = 0.01;
    public double synch_micro_sleep = 1;
    public int synch_mode = 0;
    public double synch_offset = 60;
    public double tackle_back_dist = 0.5;
    public double tackle_cycles = 10;
    public double tackle_dist = 2;
    public double tackle_exponent = 6;
    public double tackle_power_rate = 0.027;
    public double tackle_width = 1;
    public double team_actuator_noise = 0;
    public int text_log_compression = 0;
    public int text_log_dated = 1;
    public String text_log_dir = "./";
    public int text_log_fixed = 0;
    public String text_log_fixed_name = "rcssserver";
    public int text_logging = 1;
    public int use_offside = 1;
    public int verbose = 0;
    public double visible_angle = 90;
    public double visible_distance = 3;
    public double wind_ang = 0;
    public double wind_dir = 0;
    public double wind_force = 0;
    public double wind_none = 0;
    public double wind_rand = 0;
    public double wind_random = 0;

    public int auto_mode = 0;
    public int connect_wait = 300;
    public int game_over_wait = 100;
    public int keepaway = 0;
    public double keepaway_length = 20;
    public int keepaway_log_dated = 1;
    public String keepaway_log_dir = "./";
    public int keepaway_log_fixed = 0;
    public String keepaway_log_fixed_name = "rcssserver";
    public int keepaway_logging = 1;
    public int keepaway_start = -1;
    public double keepaway_width = 20;
    public int kick_off_wait = 100;
    public String module_dir = "/usr/local/share/rcssserver/modules";
    public int nr_extra_halfs = 2;
    public int nr_normal_halfs = 2;
    public int pen_allow_mult_kicks = 1;
    public int pen_before_setup_wait = 30;
    public int pen_coach_moves_players = 1;
    public double pen_dist_x = 42.5;
    public int pen_max_extra_kicks = 10;
    public double pen_max_goalie_dist_x = 14;
    public int pen_nr_kicks = 5;
    public int pen_random_winner = 0;
    public int pen_ready_wait = 50;
    public int pen_setup_wait = 100;
    public int pen_taken_wait = 200;
    public int penalty_shoot_outs = 1;
    public int recover_init = 1;
    public String team_l_start = "";
    public String team_r_start = "";

    /* player_params */
    public double dash_power_rate_delta_max = 0;
    public double dash_power_rate_delta_min = 0;
    public double effort_max_delta_factor = -0.002;
    public double effort_min_delta_factor = -0.002;
    public double extra_stamina_delta_max = 100;
    public double extra_stamina_delta_min = 0;
    public double inertia_moment_delta_factor = 25;
    public double kick_rand_delta_factor = 0.5;
    public double kickable_margin_delta_max = 0.2;
    public double kickable_margin_delta_min = 0;
    public double new_dash_power_rate_delta_max = 0.002;
    public double new_dash_power_rate_delta_min = 0;
    public double new_stamina_inc_max_delta_factor = -10000;
    public double player_decay_delta_max = 0.2;
    public double player_decay_delta_min = 0;
    public double player_size_delta_factor = -100;
    public double player_speed_max_delta_max = 0;
    public double player_speed_max_delta_min = 0;
    public int player_types = 7;
    public int pt_max = 3;
    public double random_seed = -1;
    public double stamina_inc_max_delta_factor = 0;
    public int subs_max = 3;

    public boolean setParam( String param, String value )
    {
	try {
	    Field field = getClass().getField( param );
	    Class type = field.getType();
	    if ( type.equals( double.class ) ) {
		field.setDouble( this, Double.parseDouble( value ) );
	    }
	    else if ( type.equals( int.class ) ) {
		field.setInt( this, Integer.parseInt( value ) );
	    }
	    else if ( type.equals( String.class ) ) {
		field.set( this, value.substring( 1, value.length() - 1 ) );
	    }
	    else {
		System.err.println( "Unsupported field type: " + type );
		return false;
	    }
	}
	catch ( Exception e ) {
	    System.err.println( e );
	    return false;
	}
	return true;
    }

    public Object getParam( String param )
    {
	try {
	    Field field = getClass().getField( param );
	    return field.get( this );
	}
	catch ( Exception e ) {
	    System.err.println( e );
	    return null;
	}
    }
}
