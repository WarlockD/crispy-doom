package doom64;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import w.CacheableDoomObject;

public class MapSidedef implements CacheableDoomObject{
	
    public short xoffset;

    public short yoffset;

    /* unsigned short hashes for DOOM64EX */
    public char toptexture;

    public char bottomtexture;

    public char midtexture;
    
    public short sector;    

	public MapSidedef() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public void unpack(ByteBuffer buf) throws IOException {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        this.xoffset= buf.getShort();
        this.yoffset= buf.getShort();
        this.toptexture= buf.getChar();
        this.bottomtexture= buf.getChar();
        this.midtexture= buf.getChar();
        this.sector = buf.getShort();
	}
	
    public static int sizeOf() {
        return 12;
    }
}
