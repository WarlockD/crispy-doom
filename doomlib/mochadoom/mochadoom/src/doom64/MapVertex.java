package doom64;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import w.CacheableDoomObject;

public class MapVertex implements CacheableDoomObject{
	
    public int x;
    public int y;

	public MapVertex() {
	}

	@Override
	public void unpack(ByteBuffer buf) throws IOException {
		buf.order(ByteOrder.LITTLE_ENDIAN);		
		x=buf.getInt();
		y=buf.getInt();		
	}

	public static final int sizeOf() {
		return 8;
	}

}
