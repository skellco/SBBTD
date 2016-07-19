public class ShortShowinfoT2 extends Struct
{
    public BallT ball;
    public PlayerT pos[];
    public short time ;

    public ShortShowinfoT2()
    {
	int MAX_PLAYER = 11;

	ball = new BallT();
	pos = new PlayerT[ MAX_PLAYER * 2 ];
	for ( int i = 0; i < MAX_PLAYER * 2; i++ ) {
	    pos[ i ] = new PlayerT();
	}
    }
}
