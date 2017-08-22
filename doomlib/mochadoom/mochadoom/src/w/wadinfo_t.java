package w;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class wadinfo_t implements IReadableDoomObject,IWritableDoomObject{
        // Should be "IWAD" or "PWAD".
        String       identification;      
        long         numlumps;
        long         infotableofs;
             
        /** Reads the wadinfo_t from the file.*/
        public void read(DataInputStream f) throws IOException {
            identification = DoomIO.readString(f,4);
            numlumps=DoomIO.readUnsignedLEInt(f);
            infotableofs=DoomIO.readUnsignedLEInt(f);
        }

		@Override
		public void write(DataOutputStream dos) throws IOException {
			DoomIO.writeString(dos, identification, 4);
			DoomIO.writeLEInt(dos, (int)numlumps);
			DoomIO.writeLEInt(dos, (int)infotableofs);
		}
        
    }