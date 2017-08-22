package doom64;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import w.CacheableDoomObject;

public class MapThing implements CacheableDoomObject{

	public short x;
    public short y;    
    public short z;
    public short angle;
    public short type;
    public short flags;    
    public short id;
	
	public MapThing() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public void unpack(ByteBuffer buf) throws IOException {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        this.x = buf.getShort();
        this.y = buf.getShort();
        this.z = buf.getShort();
        this.angle = buf.getShort();
        this.type = buf.getShort();
        this.flags = buf.getShort();
        this.id = buf.getShort();		
	}

    public static int sizeOf() {
        return 14;
    }

	
}
