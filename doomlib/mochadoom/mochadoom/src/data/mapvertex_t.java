package data;

import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import w.CacheableDoomObject;
import w.IPackableDoomObject;
import w.IWritableDoomObject;

/**
 * This is the structure of a map vertex ON DISK: in memory it gets shifted and
 * expanded to fixed_t. Also, on disk it only exists as part of the VERTEXES
 * lump: it is not individually cacheable, even though it implements
 * CacheableDoomObject.
 */

public class mapvertex_t implements CacheableDoomObject,IWritableDoomObject,IPackableDoomObject {

    public mapvertex_t(short x, short y) {
        this.x = x;
        this.y = y;
    }

    public mapvertex_t() {
        this((short) 0, (short) 0);
    }

    public short x;

    public short y;

    public static int sizeOf() {
        return 4;
    }

    @Override
    public void unpack(ByteBuffer buf)
            throws IOException {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        x = buf.getShort();
        y = buf.getShort();
    }

	@Override
	public void pack(ByteBuffer buf) throws IOException {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        buf.putShort(x);
        buf.putShort(y);		
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
	
	private static final ByteBuffer iobuffer=ByteBuffer.allocate(mapvertex_t.sizeOf());

}
