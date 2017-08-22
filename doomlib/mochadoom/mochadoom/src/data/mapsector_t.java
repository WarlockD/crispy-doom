package data;

import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import rr.sector_t;
import w.CacheableDoomObject;
import w.DoomBuffer;
import w.IPackableDoomObject;
import w.IWritableDoomObject;

/** Sector definition, from editing. */ 
public class mapsector_t implements CacheableDoomObject,IWritableDoomObject, IPackableDoomObject {
    
        public mapsector_t(){

        }
    
      public short     floorheight;
      public  short     ceilingheight;
      public  String floorpic;
      public  String        ceilingpic;
      public  short     lightlevel;
      public  short     special;
      public  short     tag;
      
    @Override
    public void unpack(ByteBuffer buf)
            throws IOException {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        this.floorheight = buf.getShort();
        this.ceilingheight = buf.getShort();
        this.floorpic=DoomBuffer.getNullTerminatedString(buf,8).toUpperCase();
        this.ceilingpic=DoomBuffer.getNullTerminatedString(buf,8).toUpperCase();
        this.lightlevel= buf.getShort();
        this.special= buf.getShort();
        this.tag= buf.getShort();
    }
    
    public static int sizeOf() {
        return 26;
    }
    
    @Override
    public void write(DataOutputStream dos)
            throws IOException {    	
    	
        // More efficient, avoids duplicating code and
        // handles little endian better.
        iobuffer.position(0);
        iobuffer.order(ByteOrder.LITTLE_ENDIAN);
        this.pack(iobuffer);
        dos.write(iobuffer.array());
        
    }

    public void pack(ByteBuffer b) throws IOException {
        b.order(ByteOrder.LITTLE_ENDIAN);
        b.putShort(this.floorheight);
        b.putShort(this.ceilingheight);
        DoomBuffer.putNullTerminatedString(b, this.floorpic,8);
        DoomBuffer.putNullTerminatedString(b, this.ceilingpic,8);
        b.putShort(this.lightlevel);
        b.putShort(this.special);
        b.putShort(this.tag);
    }
    
    private static final ByteBuffer iobuffer=ByteBuffer.allocate(mapsector_t.sizeOf());
      
}
