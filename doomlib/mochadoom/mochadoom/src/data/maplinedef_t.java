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
 * A LineDef, as used for editing, and as input to the BSP builder.
 */
public class maplinedef_t implements CacheableDoomObject,IPackableDoomObject,IWritableDoomObject{

    public maplinedef_t() {
        this.sidenum = new char[2];
    }

    public char v1;

    public char v2;

    public short flags;

    public short special;

    public short tag;

    /** sidenum[1] will be 0xFFFF if one sided */
    public char[] sidenum;

    public static int sizeOf() {
        return 14;
    }

    @Override
    public void unpack(ByteBuffer buf)
            throws IOException {
    buf.order(ByteOrder.LITTLE_ENDIAN);
    this.v1 = buf.getChar();
    this.v2 = buf.getChar();
    this.flags = buf.getShort();
    this.special = buf.getShort();
    this.tag = buf.getShort();
    DoomBuffer.readCharArray(buf, this.sidenum, 2);
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

	@Override
	public void pack(ByteBuffer buf) throws IOException {
	    buf.order(ByteOrder.LITTLE_ENDIAN);
	    buf.putChar(v1);
	    buf.putChar(v2);
	    buf.putShort(flags);
	    buf.putShort(special);
	    buf.putShort(tag);
	    buf.putChar(sidenum[0]);
	    buf.putChar(sidenum[1]);		
	}
	
	private static ByteBuffer iobuffer=ByteBuffer.allocate(maplinedef_t.sizeOf());
}
