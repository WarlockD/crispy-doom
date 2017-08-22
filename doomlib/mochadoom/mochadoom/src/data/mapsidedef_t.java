package data;

import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import w.CacheableDoomObject;
import w.DoomBuffer;
import w.IPackableDoomObject;
import w.IWritableDoomObject;

/**
 * A SideDef, defining the visual appearance of a wall, by setting textures and
 * offsets. ON-DISK.
 */

public class mapsidedef_t implements CacheableDoomObject,IWritableDoomObject,IPackableDoomObject{

    public mapsidedef_t() {

    }

    public short textureoffset;

    public short rowoffset;

    // 8-char strings.
    public String toptexture;

    public String bottomtexture;

    public String midtexture;

    /** Front sector, towards viewer. */
    public short sector;

    public static int sizeOf() {
        return 30;
    }

    @Override
    public void unpack(ByteBuffer buf)
            throws IOException {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        this.textureoffset = buf.getShort();
        this.rowoffset = buf.getShort();
        this.toptexture=DoomBuffer.getNullTerminatedString(buf,8).toUpperCase();
        this.bottomtexture=DoomBuffer.getNullTerminatedString(buf,8).toUpperCase();
        this.midtexture=DoomBuffer.getNullTerminatedString(buf,8).toUpperCase();
        this.sector = buf.getShort();
        
    }

	@Override
	public void pack(ByteBuffer buf) throws IOException {
	    buf.order(ByteOrder.LITTLE_ENDIAN);
	    buf.putShort(textureoffset);
	    buf.putShort(rowoffset);
	    DoomBuffer.putNullTerminatedString(buf,toptexture,8);
	    DoomBuffer.putNullTerminatedString(buf,bottomtexture,8);
	    DoomBuffer.putNullTerminatedString(buf,midtexture,8);
	    buf.putShort(sector);		
	}

	@Override
	public void write(DataOutputStream dos) throws IOException {
        // More efficient, avoids duplicating code and
        // handles little endian better.
        iobuffer.position(0);
        iobuffer.order(ByteOrder.LITTLE_ENDIAN);
        this.pack(iobuffer);
        dos.write(iobuffer.array());				
	}
	
	private static ByteBuffer iobuffer=ByteBuffer.allocate(mapsidedef_t.sizeOf());
}
