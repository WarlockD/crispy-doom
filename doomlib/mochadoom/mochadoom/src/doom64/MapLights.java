package doom64;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import w.CacheableDoomObject;
import w.DoomBuffer;

public class MapLights implements CacheableDoomObject {	
	
	/** N.B.: read those as unsigned bytes */
	public short red,green,blue;
	public byte pad;
	public short tag;
	

	public MapLights() {
		// TODO Auto-generated constructor stub
	}

	public MapLights(short i) {
		this.red=this.green=this.blue= i;
	}

	@Override
	public void unpack(ByteBuffer buf) throws IOException {
		buf.order(ByteOrder.LITTLE_ENDIAN);
		this.red=(short) (0x00FF&buf.get());
		this.green=(short) (0x00FF&buf.get());
		this.blue=(short) (0x00FF&buf.get());
		this.pad=buf.get();
		this.tag=buf.getShort();
	}
	
	public final static int sizeOf(){
		return 6;
	}
	
	public short greyscaleLightLevel(){
		return (short) (0.2989 * red + 0.5870 * green + 0.1140 * blue); 
	}

}
