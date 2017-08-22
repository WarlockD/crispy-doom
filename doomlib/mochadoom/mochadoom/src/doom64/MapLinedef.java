package doom64;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import w.CacheableDoomObject;

public class MapLinedef implements CacheableDoomObject{
	
	public short v1;
	public short v2;
	public int flags;
	public short special;
	public short tag;
	public short left;
	public short right;

	public MapLinedef() {
	}
	
    public void unpack(ByteBuffer buf)
            throws IOException {
    buf.order(ByteOrder.LITTLE_ENDIAN);
    this.v1 = buf.getShort();
    this.v2 = buf.getShort();
    this.flags = buf.getInt();
    this.special = buf.getShort();
    this.tag = buf.getShort();
    this.left= buf.getShort();
    this.right= buf.getShort();
    }
    
    public static int sizeOf() {
        return 16;
    }

    
    /* These are Doom 64-specific linedef flags (not all of them) */
    public static final int D64_ML_RENDERMIDDLE= 0x200;
    public static final int D64_ML_NOT_CLIPPED= 0x400;
    public static final int D64_ML_DONTPEGMIDDLE= 0x800;
    public static final int D64_ML_KILLTRIGGER= 0x1000;
    public static final int D64_ML_SWMASK1= 0x2000;
    public static final int D64_ML_SWMASK2= 0x4000;
    public static final int D64_ML_SWMASK3= 0x8000;
    
}
