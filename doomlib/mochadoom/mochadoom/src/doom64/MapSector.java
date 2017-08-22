package doom64;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import w.CacheableDoomObject;

public class MapSector implements CacheableDoomObject{

	
	public short floorheight;
	public short ceilingheight;
	public char floorpic;
	public char ceilingpic;
	public short floorcolor;
	public short ceilingcolor;
	public short thingcolor;
	public short walltopcolor;
	public short wallbottomcolor;
	public short special;
	public short tag;
	public short flags;
	
	public MapSector() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public void unpack(ByteBuffer buf) throws IOException {
        // ACHTUNG: the only situation where we'd
        // like to read memory-format sector_t's is from
        // savegames, and in vanilla savegames, not all info
        // is saved (or read) from disk.
		buf.order(ByteOrder.LITTLE_ENDIAN);
        this.floorheight = buf.getShort();
        this.ceilingheight = buf.getShort();
        this.floorpic = (char) buf.getShort();
        this.ceilingpic = (char) buf.getShort();
    	this.floorcolor= buf.getShort();
    	this.ceilingcolor= buf.getShort();
    	this.thingcolor= buf.getShort();
    	this.walltopcolor= buf.getShort();
    	this.wallbottomcolor= buf.getShort();
        
        this.special = buf.getShort();
        this.tag = buf.getShort();
        this.flags= buf.getShort();
    }
	
    public static int sizeOf() {
        return 24;
    }

	private static final ByteBuffer iobuffer=ByteBuffer.allocate(MapSector.sizeOf());

}
